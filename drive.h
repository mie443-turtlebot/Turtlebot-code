#define _USE_MATH_DEFINES
#include <math.h>
	
int DriveStraight(speed, distance)
{
	#initialize x y and yaw position from odom
	x = posX;
	y = posY;
	theta = yaw;
	#variable stop -- 1 to move
	#int stop = 1;
	#tolerance
	double tol = 0.1;
	
	#define x and y goal location
	xgoal = x + distance * cos(theta);
	ygoal = y + distance * sin(theta);
	
	#define starting position from x and y
	startx = x; starty = y; 
	
	#define current x y location 
	localx = 0; localy = 0;
	
	#xdistance = distance * cos(theta);
	#ydistance = distance * sin(theta);
	
	while((x < xgoal - tol or x > xGoal + tol) or (y < yGoal - tol or y > yGoal + tol))
	{
		publishTwist(speed, 0);
		time.sleep(.1);
		localx = x - startx;
		localy = y - starty;
	}
	ROS_INFO("The robot drove straight");
	publishTwist(0, 0);
}				
	
int Rotate(angleToRotate)
{
	#yaw is + for CCW and - for CW
	#recieves angle information from yaw
	theta = yaw;
	#defines tolerance to be 5 degrees
	tol = M_PI/36;
	closertol = M_PI/180;
	
	#define angular goal of movement
	anglegoal = theta + angleToRotate;
	
	if (angleGoal > M_PI){
	anglegoal -= (M_PI * 2);}
	else if (angleGoal < (-1 * M_PI)){
	anglegoal += (M_PI * 2);}
	
	while (theta < anglegoal - tol or theta > anglegoal + tol){
		if (angleOfRotation < 0){
		publishTwist(0, M_PI / -4);}
		else{
		publishTwist(0, M_PI / 4);}
		ros::Duration(0.1).sleep(); // sleep for 0.1 sec/100 milliseconds
	}
	
	while(theta < anglegoal - closertol or theta > anglegoal + closertol){
		if (angleOfRotation < 0){
		publishTwist(0, M_PI / -18);
			if (theta < anglegoal + closertol){
			break;}
		}
		else{
		publishTwist(0, math.pi / 18)
			if (theta > angleGoal - closertol){
			break;}
		}
		ros::Duration(0.05).sleep(); // sleep for 0.05 sec/50 milliseconds
	}
	
	ROS_INFO("Rotated");
	publishTwist(0, 0);
}
	

int publishTwist(linear_vel, angular_vel)
{
	vel.linear.x = linear_vel;
	vel.linear.y = 0;
	vel.linear.z = 0;
	vel.angular.x = 0;
	vel.angular.y = 0;
	vel.angular.z = angular_vel;

	twistPublisher.publish(vel);
}

		

	
	
	
	
