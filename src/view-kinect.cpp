#include <stdio.h>
#include <iostream>
 
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace std;
using namespace cv;


int main(){
	Mat depth, image;
	depth.create(480,640,CV_16S);
	image.create(480,640,CV_8UC1);


	FILE* depth_fd = fopen("../sensors/depth","r");
	FILE* image_fd = fopen("../sensors/image","r");


	while(1){

		fseek(depth_fd, SEEK_SET, 0);
		fread(depth.data, sizeof(short), 640*480, depth_fd);

		cout << depth.at<short>(320,240) << endl;

		fseek(image_fd, SEEK_SET, 0);
		fread(image.data, sizeof(char), 640*480, image_fd);

		imshow("ddd",depth);
		imshow("aaa",image);
		waitKey(200);
	}


	fclose(depth_fd);
	fclose(image_fd);


	return 0;
}
