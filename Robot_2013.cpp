#include "WPILib.h"
#include "Math.h"
#include "CameraCode.h"
#include "Turret.h"
#include "Loader.h"
#include "CoDriver.h"
#include "MainDriver.h"
#include "support.h"

/**
 *	Team 1740
 *	Ultimate Ascent Code
 *	Programming Staff:
 *	Brian Healy: Captain and Labview / TI84 guy
 *	Kevin Konrad: Not-Captain and Python / C++ guy
 *	Charles Estabooks: Programming Mentor and C/C++ Guy
 *	
 */


class Robot_2013 : public SimpleRobot
{
	
	Scores *scores; //defined in CameraCode.h, needed for vision tracking 
	CameraCode *cameraFunctions; // Moved all of the original camera processing code to here
	mainDriver *Driver1; // The Functions that deal with the main driver and his two joysticks
	Lifter *spikeLifter; // Controls our spike for the acutator, probably could make a more flexable one 
	Shooter *frisbeeShooter; // Controls our two shooter motors
	Loader *Sweeper; // Is responsible for the conveyor and and frisbee elevator
	coDriver *Driver2; // The functions that deal with the co driver and he one joystick
	robotOut *mainOut; // The function that we are using to process output
	DriverStationLCD *DsLCD; // Used to put values in the User Messages box on the driver station
	Task *notificationTask;
	Task *loaderShooterTask;
	Task *lifterTask;
	Victor *leftDrive;
	Victor *rightDrive;

	
// we disabled the camera for now
public:
	Robot_2013(void) {
		// These lines create a new object of the pointers that we created above
//		cameraFunctions = new CameraCode("amber");
		spikeLifter = new Lifter;
		frisbeeShooter = new Shooter;
		Sweeper = new Loader;
		mainOut = new robotOut;
		leftDrive = new Victor(leftDrivePort);
		rightDrive = new Victor(rightDrivePort);
		DsLCD = DriverStationLCD::GetInstance();
	}
	// ok the constructor and destructor for driver1 driver2 are moved 
	~Robot_2013(void) {
		// and these lines delete the functions for a nice cleanup 
//		delete cameraFunctions;
		delete spikeLifter;
		delete frisbeeShooter;
		delete Sweeper;
		delete mainOut;
		delete DsLCD;
	}
	static int notifierTask(Robot_2013 *robot){
		// This task manages putting joystick values on to the dashboard
		while (true){  //  Being a task, we loop forever (or until Task::Stop() is used)
			for (int differentJoysticks = 1; differentJoysticks < 4; differentJoysticks++){ // for each of our three joysticks
				for (int count = 1; count < 12; count++){ // for each button on a joystick
					char location [5]; // create a sting that has a buffer 5 characters long for brian cryptic button names
					bool buttonValue; // create a bool that we will feed the current value of the button in to
					// next we check what joystick we are on and feed that value into buttonValue
					if (differentJoysticks == 1){ 
						buttonValue = robot->Driver1->returnLeftJoystick(count);
					}
					if (differentJoysticks == 2){
						buttonValue = robot->Driver1->returnRightJoystick(count);
					}
					if (differentJoysticks == 3){
						buttonValue = robot->Driver2->returnJoystick(count);
					}
					sprintf(location, "%d_%d", differentJoysticks, count); // make a "joystick#_button#" string to appease brians dashboard
					SmartDashboard::PutBoolean(location, buttonValue); // put the finished value on the dashboard
				}
			}
			// the following lines cannot be put on the dashboard by my loop, so i threw them here
			SmartDashboard::PutNumber("Y-Axis1", robot->Driver1->Lefty());
			SmartDashboard::PutNumber("X-Axis1", robot->Driver1->Leftx());
			SmartDashboard::PutNumber("Y-Axis2", robot->Driver1->Righty());
			SmartDashboard::PutNumber("X-Axis2", robot->Driver1->Rightx());
			SmartDashboard::PutNumber("Right Drive Motor", robot->Driver1->Lefty());
			SmartDashboard::PutNumber("Left Drive Motor", robot->Driver1->Righty());
			SmartDashboard::PutNumber("Throttle2", robot->Driver1->rightThrottle());
			SmartDashboard::PutNumber("Throttle1", robot->Driver1->leftThrottle());
			robot->spikeLifter->updateDashboard();
			Wait(.02);  // lets not starve the crio doing tasks
		}
		return 0;
	}
	static int lifterThread(Robot_2013 *robot){
		while (true){
			while (robot->IsOperatorControl()){
				// this doesnt have to be in its own task anymore, however it isnt hurting anything
				robot->Driver2->lifterCheck(robot->spikeLifter); // check if codriver has hit a button to control the actuator (spike)
				// do not uncomment follow lines, it will hit the hard limit and damage the robot
				//robot->Driver2->goToInchCheck(robot->spikeLifter);
				//robot->Driver2->initalizeLifter(robot->spikeLifter);
				Wait(.05);
			}
			Wait(.1);
		}
		return false;
	}
	static int loaderThread(Robot_2013 *robot){
		while (true){
			while (robot->IsOperatorControl()){
				robot->Driver2->conveyorCheck(robot->Sweeper); // does the driver want to start a conveyor
				robot->Sweeper->loaderSequence();  // did we hit the limit switch to grab a frisbee?
				robot->Driver2->fireAtWill(robot->frisbeeShooter, robot->Sweeper); // does the codriver want to fire?
				Wait(.05);
			}
		Wait(.1);
		}
		return 0;
	}

	/**
	 * Image processing code to identify 2013 Vision targets
	 */
	void Autonomous(void)
	{	
		startTasks();
		mainOut->printDebug("Entering autonomous...\n", 1);
		DsLCD->PrintfLine(DsLCD->kUser_Line1, "Entering autonomous");  // print to the dashboard and console
		DsLCD->UpdateLCD();
		leftDrive->Set(1);
		rightDrive->Set(-1); // fix the speeds later
		Wait(1);
		leftDrive->Set(0);
		rightDrive->Set(0);
		Wait(.5);
		frisbeeShooter->Fire();
		Wait(5);
		frisbeeShooter->stopFiring();
		
		while (IsAutonomous() && IsEnabled()) {
//			cameraFunctions->targetImage();  // find all of the targets
			Wait(.01);
		}

		stopTasks();
	}

	/**
	 * Runs the motors with tank drive steering. 
	 */
	void OperatorControl(void)
	{
		// Lets see if i can do something multithreaded, this needs to be organized badly
		Driver1 = new mainDriver;
		Driver2 = new coDriver;
		mainOut->printDebug("starting Teleop\n", 1);
		DsLCD->PrintfLine(DsLCD->kUser_Line1, "Entering Teleop mode");
		DsLCD->UpdateLCD();
		Driver1->disableSafety();
		mainOut->printDebug("Teleop initalziation completed\n", 2);
		SmartDashboard::PutBoolean("In Teleop", true);
		startTasks();
		while (IsOperatorControl()){
			Driver1->teleopDrive();
			// next we do the checks to see what the codriver is trying to do
			// all commented out lines have been moved to their own tasks
//			Driver2->lifterCheck(spikeLifter);
//			Driver2->conveyorCheck(Sweeper);
//			Sweeper->loaderSequence();
//			Driver2->fireAtWill(frisbeeShooter, Sweeper);
//			Driver2->initalizeLifter(spikeLifter);
//			Driver2->goToInchCheck(spikeLifter);
			Wait(0.005);
		}
		stopTasks();
		delete Driver1;
		delete Driver2;
	}

	void Disabled(void)
	{
		// this code is run once whenever the robot is disabled, im not sure what would happen if we tried to loop
		mainOut->printDebug("I\'m Disabled!!\n", 1); // This code runs whenever the robot is disabled, even if the printf buffer sometimes forgets to flush
		DsLCD->PrintfLine(DsLCD->kUser_Line1, "Disabled");
		DsLCD->UpdateLCD();

	}
	void RobotInit(void){
		// Run once when the robot is turned on
		mainOut->printDebug("I\'m Init`ed\n", 1);
		DsLCD->PrintfLine(DsLCD->kUser_Line1, "Initalizing");
		DsLCD->UpdateLCD();
	}
	void startTasks(void){
		char name[30];
		sprintf(name, "notificationThread-%d", GetFPGATime());
		notificationTask = new Task(name, (FUNCPTR)this->notifierTask);
		notificationTask->Start((INT32)this);	
		sprintf(name, "lifterThread-%d", GetFPGATime());
		lifterTask = new Task(name, (FUNCPTR)this->lifterThread);
		lifterTask->Start((INT32)this);
		sprintf(name, "loaderShooterThread-%d", GetFPGATime());
		loaderShooterTask = new Task(name, (FUNCPTR)this->loaderThread);
		loaderShooterTask->Start((INT32)this);	
	}
	void stopTasks(void){
		notificationTask->Stop();
		lifterTask->Stop();
		loaderShooterTask->Stop();
	}

};

// This apparently tells the FRC thread to create an instance of our class and start calling its functions
START_ROBOT_CLASS(Robot_2013);

