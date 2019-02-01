#include <cmath>
#include <stdlib.h>
#include <iostream>
using namespace std

bool gogogo(double frontDist, double leftDist, double rightDist){

  if (frontDist < 0.7){
    if (abs(leftDist-rightDist) < 0.2) {
      driveStraight(0.1, 0.3);
      randomPath();
    }

    else if(rightDist < leftDist){    //close to right, adjust to left
      float d = sqrt(pow(frontDist, 2)+pow(rightDist, 2)-2*frontDist*rightDist*cos(30*pi/180)); //cos law
      float angleCorrection = asin(sin(30*pi/180)*rightDist/d); //sin law to calculate angle correction
      rotateAngle(angleCorrection);
    }

    else {    //close to left, adjust to right
      float d = sqrt(pow(frontDist, 2)+pow(leftDist, 2)-2*frontDist*leftDist*cos(30*pi/180)); //cos law
      float angleCorrection = asin(sin(30*pi/180)*rightDist/d); //sin law to calculate angle correction
      rotateAngle(-angleCorrection);
    }

  }

  else if (leftDist < 0.6 || rightDist < 0.6) {     //if the robot is too close to either wall, <0.3m
    if(leftDist < 0.6) rotateAngle(-2.0);
    else rotateAngle(2.0);
  }

  else if ((leftDist - leftDist_last) > 0.3 || (rightDist - rightDist_last) > 0.3){   //if a sudden opening on either side
    driveStraight(0.1, 0.5);    //Drive straight for 0.5m
    randomPath();
  }

  else driveStraight(0.1, 0.3);

  frontDist_last = frontDist;
  leftDist_last = leftDist;
  rightDist_last = rightDist;

}

bool randomPath(){
//Spin every 30 deg to scan the whole 360 deg, determine how many paths existing and randomly pick #ifndef
  float isPath[12] = 0;   //Initialize an array storing whether it is a path in each direction
  int n_path = 0;

  for (int i = 0; i < 12; i++){
    if (frontDist > 1.5) {    //If the front distance is greater than 1.5, set the value in array to be 1
      isPath[i] = 1;
      n_path++;
    }
    rotateAnlge(30.0);
  }

  int path_chosen = rand() % n_path;  //Randomly choose the path
  int path_idx;   //Path index in the array

  for (int i = 0; i < 12; i++){
    if (isPath[i] == 1){
      n_path--;
    }
    if (n_path == 0){
      path_idx = i;     //Get the index of path in isPath array
      break;
    }
  }

  rotateAngle(30.0*path_idx);
}
