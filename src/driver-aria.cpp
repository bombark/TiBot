#include <stdio.h>
#include <iostream>
#include <tiobj.hpp>
#include <Aria/Aria.h>


using namespace std;


int   G_id;
FILE* G_SONAR_FD;
FILE* G_pose_fd;


void readSonars(ArRobot& robot, int numSonar){
	char angle[64], value[64];
	G_id += 1;
	ArSensorReading* sonarReading;
	string res;
	sprintf(value,"id=%d;",G_id);
	res += value;
	for (int i=0; i < numSonar; i++){
		sonarReading = robot.getSonarReading(i);
		sprintf(value,"v%d=%05d;", i, sonarReading->getRange());
		res += value;
		//cout << "Sonar reading " << i << " = " << sonarReading->getRange() << " Angle " << i << " = " << sonarReading->getSensorTh() << "\n";
	}
	res += "\n";
	fseek(G_SONAR_FD, SEEK_SET, 0);
	fwrite(res.c_str(), sizeof(char), res.size(), G_SONAR_FD);
}


void readPosition(ArRobot& robot){
	ArPose pose = robot.getPose();
	fseek(G_pose_fd, SEEK_SET, 0);
	fprintf(G_pose_fd, "x=%0.6f;y=%0.6f;th=%0.6f;\n", pose.getX(), pose.getY(), pose.getTh());
}




void setMotors(ArRobot& robot){
	TiObj G_motor;
	G_motor.loadFile("../motors/motors");

	if ( G_motor.has("speed") || G_motor.has("rotation") ){
		cout << G_motor;

		robot.lock();
		if ( G_motor.has("speed") )
			robot.setVel( G_motor.atInt("speed") );
		if ( G_motor.has("rotation") ) 
			robot.setRotVel( G_motor.atInt("rotation") );
		robot.unlock();

		FILE* fd = fopen("../motors/motors","w");
		fclose(fd);
	}
	//
}




int main(int argc, char **argv){
	Aria::init();
	ArRobot robot;
	ArArgumentParser parser(&argc, argv);
	ArSimpleConnector connector(& parser);

	parser.loadDefaultArguments();
	Aria::logOptions();
	if (!connector.parseArgs()){
		cout << "Unknown settings\n";
		Aria::exit(0);
		exit(1);
	}

	if (!connector.connectRobot(&robot)){
		cout << "Unable to connect\n";
		Aria::exit(0);
		exit(1);
	}

	robot.runAsync(true);
	robot.lock();
	robot.comInt(ArCommands::ENABLE, 1);
	robot.unlock();



	ArSonarDevice sonar;
	robot.addRangeDevice(&sonar);

	G_id = 0;
	G_SONAR_FD = fopen("../sensors/sonars","w");
	G_pose_fd  = fopen("../sensors/pose","w");
	int numSonar = robot.getNumSonar();
	while(1){
		readPosition(robot);
		readSonars(robot, 8);
		setMotors(robot);
		usleep(20000);
	}

	fclose(G_SONAR_FD);
	fclose(G_pose_fd);
	Aria::exit(0);
}
