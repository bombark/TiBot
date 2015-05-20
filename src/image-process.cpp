#include <stdio.h>
#include <iostream>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <tiobj.hpp>
#include <tisys.hpp>


using namespace std;
using namespace cv;


class Robot {
  private:
	FILE* depth_fd;
	FILE* image_fd;

  public:
	Point  pt;
	double th;

	Mat image;
	Mat depth;


	Robot(){
		string url;
		string robot_url = path_context("Robot");
		cout << url << endl;

		url = robot_url+"/sensors/depth";
		depth_fd = fopen(url.c_str(),"r");
		if ( !depth_fd ){
			cout << "ERROR\n";
			exit(1);
		}
		depth.create(480,640,CV_16S);

		url = robot_url+"/sensors/image";
		image_fd = fopen(url.c_str(),"r");
		if ( !image_fd ){
			cout << "ERROR\n";
			exit(1);
		}
		image.create(480,640,CV_8UC1);


	}

	~Robot(){
		fclose(depth_fd);
		fclose(image_fd);
	}


	void update(){
		fseek(depth_fd, SEEK_SET, 0);
		fread(depth.data, sizeof(short), 640*480, depth_fd);
		fseek(image_fd, SEEK_SET, 0);
		fread(image.data, sizeof(char), 640*480, image_fd);

		TiObj pose;
		pose.loadFile("../sensors/pose");
		this->pt.x  = round(pose.atDbl("x")/10.0);
		this->pt.y  = round(pose.atDbl("y")/10.0);
		this->th    = pose.atDbl("th")*3.141592/180.0;
	}

};


class Map{
  public:
	Mat map;
	Robot* robot;
	Mat buffer;
	int res;
	Point center;


	Map(int rows, int cols, int res){
		map.create(rows,cols,CV_8UC1);
		map.setTo(0);
		buffer.create(map.size(),CV_8UC3);
		this->res = res;

		center.x = 512;
		center.y = 512;
	}

	void setRobot(Robot& robot){
		this->robot = &robot;
	}

	void render(){
		Point pt, li;
		pt.x = robot->pt.x + center.x;
		pt.y = robot->pt.y + center.y;
		li.x = pt.x + 10*cos(robot->th);
		li.y = pt.y - 10*sin(robot->th);

		circle(buffer, pt, 5, Scalar(255,0,0) );
		line  (buffer, pt, li, Scalar(255,0,0) );

		for (int iy=0;iy<map.rows; iy++){
			for (int ix=0;ix<map.rows; ix++){
				if ( map.at<uchar>(iy,ix) == 255 )
					buffer.at<Vec3b>(iy,ix) = Vec3b(0,255,0);
			}
		}

		imshow("Map", buffer);
		this->clear();
	}


	Point referenceRobot(int depth){
		Point res;
		res.x = robot->pt.x + center.x + depth*cos(robot->th);
		res.y = robot->pt.y + center.y - depth*sin(robot->th);
		return res;
	}


	Point referenceRobot(int y, int x){
		Point res;

		double th = robot->th;
		res.x = robot->pt.x + center.x + x*sin(th) + y*cos(th);
		res.y = robot->pt.y + center.y + x*cos(th) - y*sin(th);


		return res;
	}

	void setWall(Point pt){
		map.at<uchar>(pt) = 255;
	}

	void draw(Point pt){
		//circle(buffer, pt, 2, Scalar(0,255,0) );
		buffer.at<Vec3b>(pt) = Vec3b(0,255,0);
	}


	void clear(){
		buffer.setTo(0);
	}
};







int main(){
	Map map(1024,1024,5);
	Robot robot;
	map.setRobot(robot);

	int minHessian = 1600;
	SurfFeatureDetector detector( minHessian );
	std::vector<KeyPoint> keypoints;

	Mat res;


	while(1){
		robot.update();
		


		detector.detect( robot.image, keypoints );
		drawKeypoints(robot.image, keypoints, res);

		for (int i=0; i<keypoints.size(); i++){
			if ( robot.depth.at<short>(keypoints[i].pt) == 2047 )
				continue;

			int px = (keypoints[i].pt.x-320)/2;
			int py = keypoints[i].pt.y;
			int pz = robot.depth.at<short>(keypoints[i].pt)/8;



			map.setWall(   map.referenceRobot(pz,px)   );
		}


		//map.draw( map.referenceRobot(10) );
		map.render();
		imshow("res",res);
		waitKey(20);
	}



	return 0;
}
