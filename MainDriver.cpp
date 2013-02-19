#include "WPILib.h"
#include "MainDriver.h"

mainDriver::mainDriver(void):
myRobot(rightDrivePort, leftDrivePort),	// these must be initialized in the same order
leftStick(leftStickPort),		// as they are declared above.
rightStick(rightStickPort)
{
	myRobot.SetExpiration(0.1);
	myRobot.SetSafetyEnabled(false);
}
void mainDriver::teleopDrive(void){
	myRobot.TankDrive(leftStick, rightStick, true); // Third argument squares the inputs, which is better for percise control
}
void mainDriver::disableSafety(void){
	myRobot.SetSafetyEnabled(false);
}
void mainDriver::killDrive(void){
	myRobot.StopMotor();
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
