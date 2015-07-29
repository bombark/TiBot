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
#include <signal.h> 



using namespace std;
using namespace cv;


VideoCapture G_capture;

void sig_hnd(int sig){
	G_capture.release();
	exit(0);
}



int main(){
	Mat img;
	G_capture.open(1);
	signal( SIGINT, sig_hnd );
	while(1){
		G_capture >> img;
//		imshow("asa",img);
		imwrite("../sensors/video1.jpg",img);
		waitKey(200);
	}

	return 0;
}
