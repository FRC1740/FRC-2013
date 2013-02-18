#include "WPILib.h"
#include "Math.h"
#include "CameraCode.h"
#include "Turret.h"
#include "Loader.h"
#include "CoDriver.h"
#include "MainDriver.h"

int lifterThread(...);

// Note: Currently the code regarding to dashboard is commented out line #s are: 56 154

/**
 * Sample program to use NIVision to find rectangles in the scene that are illuminated
 * by a LED ring light (similar to the model from FIRSTChoice). The camera sensitivity
 * is set very low so as to only show light sources and remove any distracting parts
 * of the image.
 * 
 * The CriteriaCollection is the set of criteria that is used to filter the set of
 * rectangles that are detected. In this example we're looking for rectangles with
 * a minimum width of 30 pixels and maximum of 400 pixels.
 * 
 * The algorithm first does a color threshold operation that only takes objects in the
 * scene that have a bright green color component. Then a convex hull operation fills 
 * all the rectangle outlines (even the partially occluded ones). Then a small object filter
 * removes small particles that might be caused by green reflection scattered from other 
 * parts of the scene. Finally all particles are scored on rectangularity, aspect ratio,
 * and hollowness to determine if they match the target.
 *
 * Look in the VisionImages directory inside the project that is created for the sample
 * images as well as the NI Vision Assistant file that contains the vision command
 * chain (open it with the Vision Assistant)
 */


class Robot_2013 : public SimpleRobot
{
	
	Scores *scores; //defined in CameraCode.h
	CameraCode *cameraFunctions;
	mainDriver *Driver1;
	Lifter *spikeLifter;
	Shooter *frisbeeShooter;
	Loader *Sweeper;
	coDriver *Driver2;


	DriverStationLCD *DsLCD;	
	
//	dashboardControl *DBControl;

public:
	Robot_2013(void) {
		cameraFunctions = new CameraCode("amber");
		Driver1 = new mainDriver;
		spikeLifter = new Lifter;
		frisbeeShooter = new Shooter;
		Sweeper = new Loader;
		Driver2 = new coDriver;
		DsLCD = DriverStationLCD::GetInstance();
//		DBControl = new dashboardControl;

	}
	static int lifterThread(Robot_2013 *robot){
		Timer *lifterTimer;
		lifterTimer = new Timer;
		while (true){
			robot->Driver2->initalizeLifter(robot->spikeLifter);
			robot->Driver2->lifterCheck(robot->spikeLifter);
			Wait(.01);
		}
		return false;
	}
	
	static int printy(Robot_2013 *robot){
		while (true){
			printf("testing testing 123\n");
		}
		return 0;
	}
	/**
	 * Image processing code to identify 2013 Vision targets
	 */
	void Autonomous(void)
	{	
		printf("Entering autonomous...\n");
		DsLCD->PrintfLine(DsLCD->kUser_Line1, "Entering autonomous mode");
		DsLCD->UpdateLCD();
//		spikeLifter->cycle_linear_actuator();
		cameraFunctions->Test();
		cameraFunctions->targetImage();
		
		while (IsAutonomous() && IsEnabled()) {

		}
		// delete instances of other classes that we utilized
		delete spikeLifter;
		delete frisbeeShooter;
	}

	/**
	 * Runs the motors with arcade steering. 
	 */
	void OperatorControl(void)
	{

		
		// Lets see if i can do something multithreaded
		Task myTask("thread_test", (FUNCPTR)this->lifterThread);
		myTask.Start((INT32)this);
//		DBControl->dashboardOut(1);
		printf("starting Teleop\n");
		DsLCD->PrintfLine(DsLCD->kUser_Line1, "Entering Teleop mode");
		DsLCD->UpdateLCD();
//		spikeLifter->cycle_linear_actuator(true);
		Driver1->disableSafety();
		printf("we are in teleop, accepting joystick input now\n");
		SmartDashboard::PutBoolean("In Teleop", true);
		
		while (IsOperatorControl()){
			Driver1->teleopDrive();
			// next we do the checks to see what the codriver is trying to do
			// all commented out lines have been moved to their own tasks
//			Driver2->lifterCheck(spikeLifter);
			Driver2->conveyorCheck(Sweeper);
			Sweeper->loaderSequence();
			Driver2->fireAtWill(frisbeeShooter, Sweeper);
//			Driver2->initalizeLifter(spikeLifter);
			Driver2->goToInchCheck(spikeLifter);
			Wait(0.005);				// wait for a motor update time changing from 5ms to .1 second
		}
		delete frisbeeShooter;
		delete spikeLifter;
		delete Driver1;
		delete Driver2;
		delete Sweeper;
	}

	void Disabled(void)
	{
		printf("I\'m Disabled!!\n"); // This code runs whenever the robot is disabled, even if the printf buffer sometimes forgets to flush
		DsLCD->PrintfLine(DsLCD->kUser_Line1, "Entering disabled mode");
		DsLCD->UpdateLCD();
	}
	void RobotInit(void){
		printf("I\'m Init`ed\n");
		DsLCD->PrintfLine(DsLCD->kUser_Line1, "Entering Initalization mode");
		DsLCD->UpdateLCD();
	}


};


START_ROBOT_CLASS(Robot_2013);

