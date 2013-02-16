#ifndef CODRIVER_H_
#define CODRIVER_H_

#define codriverStickPort 3

#define Trigger 1
#define raiseButton 11
#define lowerButton 10
#define conveyorButton 9

#define inchesTest 2.6
class coDriver {
	Joystick codriverStick; // stick for the co-driver
	bool previousButtonStateConveyor;
	bool isShooting;
public:
	coDriver():
		codriverStick(codriverStickPort)
	{	
		previousButtonStateConveyor = false;
		isShooting = false;
	}

	void lifterCheck(Lifter *spikeLifter){
		if(codriverStick.GetRawButton(lowerButton)){
			if (spikeLifter->currentState() != Relay::kReverse){
				spikeLifter->lower();
				printf("retracting the relay\n");
				printf("We are starting at state %f\n", spikeLifter->getInches());
			}
		}
		else if (codriverStick.GetRawButton(raiseButton)){
			if (spikeLifter->currentState() != Relay::kForward){
				spikeLifter->raise();
				printf("extending the relay\n");
				printf("We are starting at state %f\n", spikeLifter->getInches());
			}
		}
		else{
			if (spikeLifter->currentState() != Relay::kOff){
				spikeLifter->off();
				printf("stopping the relay\n");
				printf("We are ending at state %f\n", spikeLifter->getInches());
			}
		}
	}
	

	void conveyorCheck(Loader *frisbeeLoader) {
		bool buttonPress = codriverStick.GetRawButton(conveyorButton);
		if(buttonPress && (buttonPress != previousButtonStateConveyor)){
			if (not frisbeeLoader->conveyorState()){
				frisbeeLoader->activateConveyor();
			}
			else {
				frisbeeLoader->stopConveyor();
				printf("stopping the conveyor\n");
			}
			
			previousButtonStateConveyor = true;
		}
		else if (not buttonPress){
			previousButtonStateConveyor =  false;
		}
	}
	void fireAtWill(Shooter *frisbeeShooter, Loader *frisbeeLoader){
		bool buttonPress = codriverStick.GetRawButton(Trigger);
		if (buttonPress && (isShooting != buttonPress)){
			printf("EMERGENCY!!! \rCLEAR THE AREA!!!!!!\rGUNS ARE ACTIVE!!!!!!!!!!!!!\n");
			frisbeeShooter->Fire();
			frisbeeLoader->activateElevator();
			isShooting = true;
		}
		else if (not buttonPress){
			frisbeeShooter->stopFiring();
			frisbeeLoader->stopElevator();
			isShooting = false;
		}


	}
	
	void initalizeLifter(Lifter *spikeLifter){
		if (codriverStick.GetRawButton(7)){
			spikeLifter->cycle_linear_actuator();
		}
	}
	
	void goToInchCheck(Lifter *spikeLifter){
		if (codriverStick.GetRawButton(8)){
			printf("he hit the button!\n");
			spikeLifter->goToInch(inchesTest);
		}
	}

};
#endif
