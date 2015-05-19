all: ../robot/bin/driver-kinect ../robot/bin/view-kinect ../robot/bin/driver-aria

../robot/bin/driver-kinect: driver-kinect.c
	gcc driver-kinect.c -o ../robot/bin/driver-kinect -lfreenect -lpthread -lm

../robot/bin/view-kinect: view-kinect.cpp
	g++ view-kinect.cpp -o ../robot/bin/view-kinect `pkg-config --cflags --libs opencv`

../robot/bin/driver-aria: driver-aria.cpp
	g++ driver-aria.cpp -o ../robot/bin/driver-aria -lAria -ltiobj
