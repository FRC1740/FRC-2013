#ifndef MAINDRIVER_H_
#define MAINDRIVER_H_
// these are the pwn ports for the left and right motors respectively
#define rightDrivePort 1
#define leftDrivePort 2
// these are the ports for the joysticks on the PC
#define rightStickPort 2
#define leftStickPort 1

class mainDriver {
	RobotDrive myRobot; // robot drive system
	Joystick leftStick; // left joystick
	Joystick rightStick; // right joystick
public:
	mainDriver(void):
		myRobot(rightDrivePort, leftDrivePort),	// these must be initialized in the same order
		leftStick(leftStickPort),		// as they are declared above.
		rightStick(rightStickPort)
	{
		myRobot.SetExpiration(0.1);
		myRobot.SetSafetyEnabled(false);
	}
	void teleopDrive(void){
		myRobot.TankDrive(leftStick, rightStick, true); // Third argument squares the inputs, which is better for percise control
	}
	void disableSafety(void){
		myRobot.SetSafetyEnabled(false);
	}
	void killDrive(void){
		myRobot.StopMotor();
	}
};
#endif
