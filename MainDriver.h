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
	mainDriver(void);
	void teleopDrive(void);
	void disableSafety(void);
	void killDrive(void);
	double Lefty(void);
	double Righty(void);
	double Leftx(void);
	double Rightx(void);
	double leftThrottle(void);
	double rightThrottle(void);
	float returnLeftJoystick(int port);
	float returnRightJoystick(int port);

};
#endif
