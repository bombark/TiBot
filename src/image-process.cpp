#include <stdio.h>
#include <iostream>
 
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <tiobj.hpp>


using namespace std;
using namespace cv;






int main(){
	Mat depth, image, output, map;
	depth.create(480,640,CV_16S);
	image.create(480,640,CV_8UC1);


	map.create(1024,1024,CV_8UC1);
	map.setTo(0);


	int minHessian = 1600;
	SurfFeatureDetector detector( minHessian );
	std::vector<KeyPoint> keypoints;


	FILE* depth_fd = fopen("../sensors/depth","r");
	FILE* image_fd = fopen("../sensors/image","r");
	while(1){
		fseek(depth_fd, SEEK_SET, 0);
		fread(depth.data, sizeof(short), 640*480, depth_fd);
		fseek(image_fd, SEEK_SET, 0);
		fread(image.data, sizeof(char), 640*480, image_fd);

		TiObj pose;
		pose.loadFile("../sensors/pose");


		int rx = round(pose.atDbl("x")/125.0);
		int ry = round(pose.atDbl("y")/125.0);
		Point robot(512-ry,rx+512);
		//cout << robot << endl;

		circle(map, robot, 2, Scalar(128) );

		double th = pose.atDbl("th")/180*3.141592;
		int dx = robot.x+5*cos(th);
		int dy = robot.y-5*sin(th);

		line(map, robot, Point(dx,dy), Scalar(128) );


		detector.detect( image, keypoints );
		drawKeypoints(image, keypoints, output);

		for (int i=0; i<keypoints.size(); i++){
			if ( depth.at<short>(keypoints[i].pt) == 2047 )
				continue;


			int de = depth.at<short>(keypoints[i].pt)/4;
			int ax = keypoints[i].pt.x - 320;

			cout << ax << " " << de << " " << (acos(ax/de)*180.0)/3.141592 << endl;


			int px = robot.x;// + ax;
			int py = robot.y; //- ay;

			//int px = robot.x + ax + de*cos(th);
			//int py = robot.y + ay - de*sin(th);
			//cout << depth.at<short>(keypoints[i].pt) << endl;
//		cout << px << " " << py << endl;

			//map.at<uchar>(py,px) = 255;
		}








		imshow("ddd",map);
		imshow("aaa",output);
		waitKey(200);
	}


	fclose(depth_fd);
	fclose(image_fd);


	return 0;
}
