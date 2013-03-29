#include "WPILib.h"
#include "MainDriver.h"

mainDriver::mainDriver(void):
myRobot(leftDrivePort, rightDrivePort),	// these must be initialized in the same order
leftStick(leftStickPort),		// as they are declared above.
rightStick(rightStickPort)
{
	myRobot.SetExpiration(0.1);
	myRobot.SetSafetyEnabled(false);
}
void mainDriver::teleopDrive(void){
	myRobot.TankDrive(-leftStick.GetY(), -rightStick.GetY(), true); // Third argument squares the inputs, which is better for percise control
}
void mainDriver::disableSafety(void){
	myRobot.SetSafetyEnabled(false);
}
void mainDriver::killDrive(void){
	myRobot.StopMotor();
}
void mainDriver::Go(float speed){
	myRobot.Drive(speed, 0.0);
}
double mainDriver::Lefty(void){
	return leftStick.GetY();
}
double mainDriver::Righty(void){
	return rightStick.GetY();
}
double mainDriver::Leftx(void){
	return leftStick.GetX();
}
double mainDriver::Rightx(void){
	return rightStick.GetX();
}
double mainDriver::rightThrottle(void){
	return rightStick.GetThrottle();
}
double mainDriver::leftThrottle(void){
	return leftStick.GetThrottle();
}
float mainDriver::returnLeftJoystick(int port){
	return leftStick.GetRawButton(port);
}
float mainDriver::returnRightJoystick(int port){
	return rightStick.GetRawButton(port);
}
