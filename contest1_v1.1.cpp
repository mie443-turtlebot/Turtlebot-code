#include <ros/console.h>
#include "ros/ros.h"
#include <geometry_msgs/Twist.h>
#include <kobuki_msgs/BumperEvent.h>
#include <sensor_msgs/LaserScan.h>
#include <eStop.h>

#include <stdio.h>
#include <cmath>

#include <iostream>
#include <chrono>

#include <nav_msgs/Odometry.h>
#include <tf/transform_datatypes.h>

using namespace std;

bool bumperLeft = 0, bumperCenter = 0, bumperRight = 0;
bool bumperFlag = 0;
bool Wall_recognition = 0;
double laserRange = 10;
int laserSize = 0, laserOffset = 0, desiredAngle = 10;

double posX, posY, yaw;
double pi = 3.1416;
double Base_orient = 0.0;
double off_angle_temp;

int shortest_distance_index = 0;


void bumperCallback(const kobuki_msgs::BumperEvent::ConstPtr& msg){
	//fill with your code
	if (msg->bumper == 0)
	{
		bumperLeft = !bumperLeft;
		if (bumperLeft == 1)
			bumperFlag == 1;
	}
	else if(msg->bumper == 1)
	{
		bumperCenter = !bumperCenter;
		if (bumperCenter == 1)
			bumperFlag == 1;
	}
	else if(msg->bumper == 2)
	{
		bumperRight = !bumperRight;
		if (bumperRight == 1)
			bumperFlag == 1;
	}
}

void laserCallback(const sensor_msgs::LaserScan::ConstPtr& msg){ //Note: angle_max, angle_min and angle_increment are the parameters of the sensors and by default the unit is radian.
	//The Microsoft Kinect 360 cemera tilting range: 43 degree vertical x 57 degree horizontal (Here we will use 57 degrees as the horizontal angle limit)
	//fill with your code
	laserSize = (msg->angle_max - msg->angle_min)/msg->angle_increment; //Resolution of laser range
	laserOffset = desiredAngle*pi/(180*msg->angle_increment);
	laserRange = 11; // This laserRange is the shortest distances measured in the array. Therefore, each time when calling the function this variable need to be "reset" to a large number in order to receive new small number.

	//ROS_INFO("Size of laser scan array: %i and size of offset: %i", laserSize, laserOffset);

	if(desiredAngle*pi/180 < msg->angle_max && -desiredAngle*pi/180 > msg->angle_min){
		for(int i =laserSize/2 - laserOffset; i < laserSize/2 + laserOffset; i++){
			if (laserRange > msg->ranges[i])
			{
				laserRange = msg->ranges[i]; // This step searchs the distances between the (-laserOffset, +laserOffset) and get the shortest distance.
				shortest_distance_index = i; // Mark the element at which location the shortest distance is reached.
			}
		}
	}
	else{
		for(int i = 0; i < laserSize; i++){
			if (laserRange > msg->ranges[i])
			{
				laserRange = msg->ranges[i];
				shortest_distance_index = i;
			} // This step implements when the desired angle is outside of the parameters of the scanner.
	}

	if(laserRange ==11)
		laserRange = 0;
}

void odomCallback (const nav_msgs::Odometry::ConstPtr& msg){
	posX = msg->pose.pose.position.x;
	posY = msg->pose.pose.position.y;
	yaw = tf::getYaw(msg->pose.pose.orientation);

	ROS_INFO("Position: (%f, %f) Orientation: %f rad or %f degrees.", posX, posY, yaw, yaw*180/pi);

}

int main(int argc, char **argv)
{
	ros::init(argc, argv, "image_listener");
	ros::NodeHandle nh;
	teleController eStop;

	ros::Subscriber bumper_sub = nh.subscribe("mobile_base/events/bumper", 10, &bumperCallback);
	ros::Subscriber laser_sub = nh.subscribe("scan", 10, &laserCallback);

	ros::Subscriber odom = nh.subscribe("/odom", 1, odomCallback);

	ros::Publisher vel_pub = nh.advertise<geometry_msgs::Twist>("cmd_vel_mux/input/teleop", 1);

	double angular = 0.0;
	double linear = 0.0;
	geometry_msgs::Twist vel;

	std::chrono::time_point<std::chrono::system_clock> start;
	start = std::chrono::system_clock::now(); /* start timer */
    uint64_t secondsElapsed = 0; // the timer just started, so we know it is less than 480, no need to check.

	while(ros::ok() && secondsElapsed <= 480)
    {
		ros::spinOnce();
		//.....**E-STOP DO NOT TOUCH**.......
		eStop.block();
		//...................................

		//fill with your code
		//ROS_INFO("Position: (%f, %f) Orientation: %f degreses Range: %f", posX, posY, yaw*180/pi, laserRange);

		//&& yaw < pi/12
		/*if (posX < 0.5 && yaw < pi/12 && !bumperRight && !bumperCenter && !bumperLeft && laserRange > 0.7) //Ensure that the measured laser range is larger than 0.7 meters before moving.
		{
			ROS_INFO("forward");
			angular = 0.0;
			linear = 0.2; //To move more accurately, linear velocity should be 0.25 m/s or less
		}
		else if (yaw < pi/2 && posX > 0.5 && !bumperRight && !bumperCenter && !bumperLeft && laserRange > 0.5) //if the robot moves more than 0.5 meters and the laser scanned range is larger than 0.5 meter, turn 90 degrees
		{
			ROS_INFO("turn big");
			angular = pi/6; //To move more accurately, angular velocity should be pi/6 rad/s or less
			linear = 0.0;
		}
		else if (laserRange > 1.0 && !bumperRight && !bumperCenter && !bumperLeft)
		{
			if(yaw < 17*pi/36 || posX > 0.6)
			{
				ROS_INFO("forward + left turn");
				angular = pi/12;
				linear = 0.1;
			}
			else if (yaw > 19*pi/36 || posX < 0.4)
			{
				ROS_INFO("forward + right turn");
				angular = -pi/12;
				linear = 0.1;
			}
			else
			{
				ROS_INFO("forward small");
				angular = 0;
				linear = 0.1;
			}
		}
		else if (bumperRight || bumperCenter || bumperLeft)
		{
			ROS_INFO("DEADDDDDDDDDDDDDDDD");
			angular = pi/6;
			linear = -0.2;
		}
		*/
		if (laserRange > 2 && yaw < (Base_orient + (1 / 12)*pi) && yaw >(Base_orient - (1 / 12)*pi) && bumperFlag == 0)
		{
			ROS_INFO("Fast forward");
			angular = 0.0;
			linear = 0.2;
		}
		else if ((laserRange > 2 && yaw > (Base_orient + (1 / 12)*pi) && bumperFlag == 0) || (laserRange > 2 && yaw < (Base_orient - (1 / 12)*pi) && bumperFlag == 0))
		{
			ROS_INFO("Fine Tuning");
			off_angle_temp = yaw - Base_orient;
			linear = 0.0;
			Angular_Rotation(off_angle_temp);
			off_angle_temp = 0.0;
		}
		else if (laserRange <= 2 && laserRange > 1 && bumperFlag == 0)
		{
			ROS_INFO("I'm slowing down");
			linear = 0.1;
			angular = 0.0;
		}
		else if (laserRange <= 1 && bumperFlag == 0)
		{
			ROS_INFO("Turning direction now!");
			linear = 0.0;
			if (shortest_distance_index < laserSize / 2)
			{
				Angular_Rotation(-90); // This rotation will correspond to the direction where the obstacles are not detected. Meaning, the direction which is opposite to the shortest_distance_index side.
				Base_orient = Base_orient - 90;
			}
			else
			{
				Angular_Rotation(90);
				Base_orient = Base_orient + 90;
			}
		else if (bumperFlag == 1)
		{
			ROS_INFO("GOD DAMN IT!");
			linear = 0.0;
			if (bumperLeft)
			{
				linear_movement(-1 meter);
				off_angle_temp = yaw - Base_orient;
				Angular_Rotation(off_angle_temp);
				off_angle_temp = 0.0;
				bumperFlag = 0;
			}
			else if (bumperCenter)
			{
				linear_movement(-1 meter);
				Angular_Rotation(90); //If the robot hits right in the front (this should not happen theoretically), turn 90 degrees to the right under any circumstance.
				ROS_INFO("The sensors are not working properly! ");
				bumperFlag = 0;
			}
			else if (bumperRight)
			{
				linear_movement(-1 meter);
				off_angle_temp = yaw - Base_orient;
				Angular_Rotation(off_angle_temp);
				off_angle_temp = 0.0;
				bumperFlag = 0;
			}
		}
  		vel.angular.z = angular;
  		vel.linear.x = linear;

  		vel_pub.publish(vel);

        // The last thing to do is to update the timer.
		secondsElapsed = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now()-start).count();
	}

	return 0;
}
