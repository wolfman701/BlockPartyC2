#pragma config(Hubs,  S1, HTServo,  HTMotor,  HTMotor,  none)
#pragma config(Hubs,  S2, HTMotor,  none,     none,     none)
#pragma config(Sensor, S1,     ,               sensorI2CMuxController)
#pragma config(Sensor, S2,     ,               sensorI2CMuxController)
#pragma config(Sensor, S3,     irSensor,       sensorHiTechnicIRSeeker1200)
#pragma config(Motor,  mtr_S1_C2_1,     motorF,        tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C2_2,     motorG,        tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C3_1,     motorH,        tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C3_2,     motorI,        tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S2_C1_1,     LeftDrive,     tmotorTetrix, openLoop, reversed, encoder)
#pragma config(Motor,  mtr_S2_C1_2,     RightDrive,    tmotorTetrix, openLoop, encoder)
#pragma config(Servo,  srvo_S1_C1_1,    servo1,               tServoNone)
#pragma config(Servo,  srvo_S1_C1_2,    servo2,               tServoNone)
#pragma config(Servo,  srvo_S1_C1_3,    AutonomousDispenser,  tServoStandard)
#pragma config(Servo,  srvo_S1_C1_4,    servo4,               tServoNone)
#pragma config(Servo,  srvo_S1_C1_5,    servo5,               tServoNone)
#pragma config(Servo,  srvo_S1_C1_6,    servo6,               tServoNone)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#define TICKS_PER_INCH 120
#define ROTATION_TICKS_1 25
#define ROTATION_TICKS_2 35
#define ROTATION_TICKS 15
#define ROTATIONS_PER_90_DEGREES 30
#define FIELD_WIDTH 144
#define FIELD_LENGTH 144
#define DIRECTION_MARGIN_OF_ERROR 5

#include "JoystickDriver.c"

int FieldObstaclesXCoord[] = { 23, 24, 23 }; // X coordinates for grid obstacles on field (dummy values for now)
int FieldObstaclesYCoord[] = { 23, 24, 25 }; // Y coordinates for grid obstacles on field (dummy values for now)
int RouteXCoord[] = { 0, 0, 0, 0, 0, 0, -1};
int RouteYCoord[] = { 0, 1, 2, 3, 4, 5, -1};

// Y coordinates for pre-programmed route
int CurrentYCoord = 0;
int CurrentXCoord = 0;
int CurrentRouteXIndex = 0;
int CurrentRouteYIndex = 0;
int InitialDirection = 0;
int DistanceTraveled = 0;
typedef enum
{
	North,
	South,
	East,
	West
} Directions;

Directions CurrentDirection = North;

bool isEndOfRoute()
{
	// Test to see if next move is the end of the route
	if ((RouteXCoord[CurrentRouteXIndex + 1] == -1) && (RouteYCoord[CurrentRouteYIndex + 1] == -1))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool isObstacleOnCoordinates(int X, int Y)
{
	int i;

	// loop through the field's obstacle coordinates and compare them to the X and Y values passed to this subroutine.
	for (i = 0;(i < sizeof(FieldObstaclesXCoord));i++)
	{
		if ((FieldObstaclesXCoord[i] == X) && (FieldObstaclesYCoord[i] == Y))
		{
			return true;
		}
	}
	return false;
}

// before the robot moves to any coordinate, call this to see if it can actually go there
bool CanIMoveToCoordinate(int X, int Y)
{
	// is this coordinate outside the field itself?
	if ((X >= FIELD_WIDTH) || (Y >= FIELD_LENGTH))
	{
		return false;  // can't move there, outside the field.
	}
	else if (isObstacleOnCoordinates(X, Y)) // Is there an obstacle on the field at this coordinate?
	{
		return false;  // can't move there, something is sitting on that coordinate.
	}
	else
	{
		return true; // otherwise, I can move to that coordinate location.
	}
}

// this subroutine takes a compass sensor reading and sees if it's within the margin of error that's equivalent to the desired heading
bool Near(int Direction, int LastKnownDirection)
{
	// easy case, they are equal so direction hasn't changed, still on same course
	if (Direction == LastKnownDirection)
	{
		return true;
	}

	// uses a counter to check if the robot is within the margin of error before or after the current heading
	for (int i = 1;(i <= DIRECTION_MARGIN_OF_ERROR);i++)
	{
		// check for values below the margin of error
		if ((Direction - i) < 0)
		{
			if ((360 - (Direction - i)) == LastKnownDirection)
			{
				return true;
			}
		}
		else if ((Direction - i) == LastKnownDirection)
		{
			return true;
		}

		// check for values above the margin of error
		if ((Direction + i) > 359)
		{
			if (360 - (Direction + i) == LastKnownDirection)
			{
				return true;
			}
		}
		else if ((Direction + i) == LastKnownDirection)
		{
			return true;
		}
	}
	// no matches, just return false
	return false;
}

void moveRobot(int LeftPower, int RightPower)	// subroutine for moving the robot
{
	motor[LeftDrive] = LeftPower;	// assign variable power level to left motor
	motor[RightDrive] = RightPower;	// assign variable power level to right motor
}

// turn robot right 90 degrees
void turnRight()
{
	int i;
	// stop robot before turning
	moveRobot(0, 0);
	for (i = 0;(i < ROTATIONS_PER_90_DEGREES);i++)
	{
		nMotorEncoder[RightDrive] = 0;	// set encoder values to 0
		nMotorEncoder[LeftDrive] = 0;
		moveRobot(100, -100);	// turn robot
		while ((nMotorEncoder[LeftDrive] < ROTATION_TICKS) && (nMotorEncoder[RightDrive] < ROTATION_TICKS))
		{
		}
	}
	moveRobot(0, 0);
}

void turnLeft1()
{
	int i;
	// stop robot before turning
	moveRobot(0, 0);
	for (i = 0;(i < ROTATIONS_PER_90_DEGREES);i++)
	{
		nMotorEncoder[RightDrive] = 0;	// set encoder values to 0
		nMotorEncoder[LeftDrive] = 0;
		moveRobot(-100, 100);	// turn robot
		while ((nMotorEncoder[LeftDrive] < ROTATION_TICKS_1) && (nMotorEncoder[RightDrive] < ROTATION_TICKS_1))
		{
		}
	}
}
// turn robot left 90 degrees
void turnLeft2()
{
	int i;
	// stop robot before turning
	moveRobot(0, 0);
	for (i = 0;(i < ROTATIONS_PER_90_DEGREES);i++)
	{
		nMotorEncoder[RightDrive] = 0;	// set encoder values to 0
		nMotorEncoder[LeftDrive] = 0;
		moveRobot(-100, 100);	// turn robot
		while ((nMotorEncoder[LeftDrive] < ROTATION_TICKS_2) && (nMotorEncoder[RightDrive] < ROTATION_TICKS_2))
		{
		}
	}
	moveRobot(0, 0);
}

void moveRobotOneRouteCoordinate(int LeftPower, int RightPower)
{
	nMotorEncoder[RightDrive] = 0;
	nMotorEncoder[LeftDrive] = 0;
	moveRobot(LeftPower, RightPower);
	while (abs(nMotorEncoder[LeftDrive]) < TICKS_PER_INCH)
	{
	}
}

// calculate robot's heading from compass and how far it has moved since the last time this was called, update current grid location
// by calculating how many grid positions it have moved (and in what direction).
// this can and should be called frequently from the main task loop.
void getCurrentPosition()
{
	int CurrentDistanceTraveled;
	int CurrentDirection = 0;
	//CurrentDistanceTraveled = (SensorValue[RightDrive] / 720) * WHEEL_DIAMETER;
	DistanceTraveled += CurrentDistanceTraveled;
	//if (DistanceTraveled > 0) {
	//	moveRobot(0, 0);
	//	StopAllTasks();
	//}
	//CurrentDirection = SensorValue[Compass];
	// reset rotation tick counter since we've already read its value.
	SensorValue[RightDrive] = 0;
	// now, use distance travelled and heading to calculate where we are on the field.
	if (DistanceTraveled >= 5)
	{
		moveRobot(0, 0);
		StopAllTasks();
		//CurrentRouteXIndex++;
		//CurrentRouteYIndex++;
		//DistanceTraveled = 0;
		//if (Near(CurrentDirection, InitialDirection))
		//{
		//	CurrentYCoord += CurrentDistanceTraveled;
		//}
		//else if (Near(CurrentDirection, ((InitialDirection + 90) % 360)))
		//{
		//	CurrentXCoord += CurrentDistanceTraveled;
		//}
		//else if (Near(CurrentDirection, ((InitialDirection + 180) % 360)))
		//{
		//	CurrentYCoord -= CurrentDistanceTraveled;
		//}
		//else if (Near(CurrentDirection, ((InitialDirection + 270) % 360)))
		//{
		//	CurrentYCoord -= CurrentDistanceTraveled;
		//}
	}
}

bool isBeaconInRange(tSensors irSensor)	// subroutine that determines whether IR beacon is in range
{
	wait1Msec(1);
	int ir = SensorValue[irSensor];	// defines integer that returns current sensor value
	wait1Msec(1);
	if (ir == 0)	// no beacon detected
	{
		return false;
	}
	else	// beacon detected
	{
		return true;
	}
}

void DispenseBlock()	// subroutine to dispense block into bin with IR beacon, will be called when robot reaches box with beacon
{
	servo[AutonomousDispenser] = 150;	// run motor to dispense block
	wait1Msec(400);	// replace 3000 with amount of time it takes to dispense
	servo[AutonomousDispenser] = 0;	// stop motor after dispensing
}

//void initializeRobot()
//{
//	servo[AutonomousDispenser] = 0;
//}

task main()
{
	//initializeRobot();
	waitForStart();
	servo[AutonomousDispenser] = 0;
	wait1Msec(2000);
	long i;
	for (i = 0;(i < 50);i++)
	{
		moveRobotOneRouteCoordinate(100, 100);
		if (isBeaconInRange(irSensor) == true)
		{
			int sector;
			if (i < 33)
			{
				sector = 5;
			}
			else
			{
				sector = 6;
			}
			if (SensorValue[irSensor] == sector)	// robot has reached the desired sector
			{
				if (sector == 6)
				{
					wait1Msec(100);
				}
				DispenseBlock();
			}
		}
	}
	turnLeft1();
	wait1Msec(10);
	for (i = 0;(i < 35);i++)
	{
		moveRobotOneRouteCoordinate(100, 100);
	}
	turnLeft2();
	for (i = 0;(i < 50);i++)
	{
		moveRobotOneRouteCoordinate(100, 100);
	}
	moveRobot(0, 0);
}
