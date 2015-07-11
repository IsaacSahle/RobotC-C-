#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, in1,    LEFT_IR1,       sensorReflection)
#pragma config(Sensor, in2,    RIGHT_IR2,      sensorReflection)
#pragma config(Sensor, dgtl2,  Left_Bumper,    sensorTouch)
#pragma config(Sensor, dgtl3,  Right_Bumper,   sensorTouch)
#pragma config(Sensor, dgtl4,  sonar1,         sensorSONAR_inch)
#pragma config(Sensor, dgtl9,  MAG,            sensorDigitalOut)
#pragma config(Sensor, dgtl10, LED,            sensorDigitalOut)
#pragma config(Sensor, dgtl11, LED2,           sensorDigitalOut)
#pragma config(Sensor, I2C_1,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign)
#pragma config(Motor,  port1,           motor1,        tmotorVex393_HBridge, openLoop, driveLeft)
#pragma config(Motor,  port6,           motor3,        tmotorVex393_MC29, openLoop, encoderPort, I2C_1)
#pragma config(Motor,  port10,          motor2,        tmotorVex393_HBridge, openLoop, reversed, driveRight)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

/*** GLOBAL VARIABLES ***/
bool condition = true;
int leftValue; 	//Left IR Value
int rightValue;	//Right IR Value
const int SHORTRANGE = 3800; // Short range IR threshold value
const int LONGRANGE = 500; //Long range IR threshold value
const int Motor_On = 50;	//Motor speed
const int Motor_On_Fast = 60; //Fast Motor Speed
const int Motor_On_Slow = 30; //Slow Motor Speed
const int Motor_On_Medium = 40; //Medium Motor Speed
const int Arm_Speed = 20; //Speed of arm
const int ON = 1; //ON
const int OFF = 0; //OFF
const int Drop_Distance = 3; //Drop off distance
const int NoSonVal = -1; //No sonar value read
/***                	***/


/***	Calculates Right IR Values	***/
void Right_IR(){

		int x;
		int maxValueR = 0;	//set initial max reading
		int minValueR = 3950;	//set initial min reading

		//read 15 values and store if comparisons are true

		for(x=0;x<15;x++){
				if(SensorValue(RIGHT_IR2) > minValueR){
					minValueR = SensorValue(RIGHT_IR2);
				}
				else{
				maxValueR = SensorValue(RIGHT_IR2);
			}
			//calculate the maximum difference
			rightValue = minValueR - maxValueR;
		}//for

}//Right_IR

/***	Calculates Left IR Values	***/
void Left_IR(){

		int y;
		int maxValueL = 0; //set initial max reading
		int minValueL = 3950;	//set initial min reading

		//read 15 values and store if comparisons are true

		for(y=0;y<15;y++){
				if(SensorValue(LEFT_IR1) > minValueL){
					minValueL = SensorValue(LEFT_IR1);
				}
				else{
				maxValueL = SensorValue(LEFT_IR1);
			}
			//calculate the maximum difference
			leftValue = minValueL - maxValueL;
		}//for

}//Left_IR


/***	Locates the Beacon ***/
void Locate_Beacon(){

	while(condition != false){

	Right_IR();
	Left_IR();
	clearTimer(T1);

	//rotate until Shortrange IR values are found
	while(leftValue < SHORTRANGE && rightValue < SHORTRANGE){

	Right_IR();
	Left_IR();

	//Once timer exceeds 5s, rotate opposite direction for longrange IR Values
	while(time1[T1] > 5000){

		Right_IR();
		Left_IR();

		motor[motor1] = - Motor_On;
		motor[motor2] =  Motor_On;

		if(leftValue > LONGRANGE || rightValue > LONGRANGE && SensorValue(sonar1) > 20){

				//if long range values are found, approach for 2 seconds and continue short range search
				motor[motor1] =  Motor_On;
				motor[motor2] =  Motor_On;

				wait1Msec(2000);

				motor[motor1] = OFF;
				motor[motor2] = OFF;

				clearTimer(T1);


				if(SensorValue(Left_Bumper) == ON || SensorValue(Right_Bumper) == ON){

						//failsafe rearrange if obstruction is hit and continue search

						motor[motor1] =  - Motor_On;
						motor[motor2] =  - Motor_On;

						wait1Msec(1500);

						motor[motor1] =  Motor_On;
						motor[motor2] =  - Motor_On;

						wait1Msec(2500);

						motor[motor1] =  OFF;
						motor[motor2] =  OFF;

						SensorValue(Left_Bumper) = OFF;
						SensorValue(Right_Bumper) = OFF;
			}

	}

}
		motor[motor1] = Motor_On;
		motor[motor2] = - Motor_On;

		condition = false;

}//while

}//while

}//Locate_Beacon


/** Aprroaches Beacon while correcting positioning with IR values **/
void Approach_Beacon(){


				//approach beacon while sonar is greater than 5 inches
		while(SensorValue(sonar1) > 5 && SensorValue(sonar1) != NoSonVal){

				Right_IR();
				Left_IR();
			motor[motor1] = Motor_On;
			motor[motor2] = Motor_On;

				//if IR values get decrease, find beacon again
			if(leftValue < LONGRANGE || rightValue < LONGRANGE){
				Right_IR();
				Left_IR();
			Locate_Beacon();

		}

		//if right IR difference greater than left difference correct positioning
		if(rightValue > leftValue){
				Right_IR();
				Left_IR();
			motor[motor1] = Motor_On;
			motor[motor2] = OFF;
			wait1Msec(50);
	}
		//if left IR difference greater than right difference correct positioning
		if(leftValue > rightValue + 300){
				Right_IR();
				Left_IR();
				motor[motor1] = OFF;
				motor[motor2] = Motor_On;
				wait1Msec(50);
	}

	}//while

	motor[motor1] = OFF;
	motor[motor2] = OFF;

}//Approach_Beacon



/** Disposes of Object by approaching a wall and dropping it off **/
void Discard(){

	delay(1000);
	resetMotorEncoder(motor3);
	SensorValue(MAG) = ON;

	clearTimer(T1);

	//Lower Arm
	while(time1[T1] < 50){
			motor[motor3] = Arm_Speed;
		}
			motor[motor3] = OFF;

			wait1Msec(1500);

			clearTimer(T2);
	//lift object
	while(time1[T2] < 2500){
			motor[motor3] = -Arm_Speed;
 }

 motor[motor3] = OFF;

 wait1Msec(1500);

 //backup
 motor[motor1] = - Motor_On_Slow;
 motor[motor2] = - Motor_On_Slow;

 wait1Msec(1500);

//turn roughly 90 degrees
 motor[motor1] = Motor_On_Slow;
 motor[motor2] = -Motor_On_Slow;

 wait1Msec(4000);

 motor[motor1] = OFF;
 motor[motor2] = OFF;

 clearTimer(T1);

 //move to a wall, stop 3 inches from wall, and drop object
 while(SensorValue(sonar1) > Drop_Distance || SensorValue(sonar1) == NoSonVal){

		motor[motor1] = Motor_On_Medium;
		motor[motor2] = Motor_On_Medium;

		//if left bumper hit readjust and proceed
	if(SensorValue(Left_Bumper) == ON){

		motor[motor1] = -Motor_On_Slow;
		motor[motor2] = -Motor_On_Slow;

		wait1Msec(750);

		motor[motor1] = OFF;
		motor[motor2] = OFF;


		motor[motor2] = Motor_On_Slow;
		motor[motor1] = OFF;
		wait1Msec(1000);
		SensorValue(Left_Bumper) = OFF;
}

//if right bumper hit, rearrange and proceed
if(SensorValue(Right_Bumper) == ON){

		motor[motor1] = - Motor_On_Slow;
		motor[motor2] = - Motor_On_Slow;

		wait1Msec(500);

		motor[motor1] = OFF;
		motor[motor2] = OFF;

		motor[motor1] = Motor_On_Slow;
		motor[motor2] = OFF;
		wait1Msec(1000);
		SensorValue(Right_Bumper) = OFF;
}

}//while

SensorValue(MAG) = OFF;

}//Discard

/*** Robot Celebration Once Task is Complete ***/
void Celebrate(){

		motor[motor1] = -Motor_On_Fast;
		motor[motor2] = -Motor_On_Fast;

		wait1Msec(1500);

		motor[motor1] = -Motor_On_Fast;
		motor[motor1] =  Motor_On_Fast;

		wait1Msec(2000);

	  motor[motor1] = OFF;
		motor[motor2] = OFF;

}//Celebrate


task main()
{
	Locate_Beacon();
	Approach_Beacon();
	Discard();
	Celebrate();
}
