#ifndef CODRIVER_H_
#define CODRIVER_H_

#define codriverStickPort 3


#define Trigger 1
#define raiseButton 11
#define lowerButton 10
#define conveyorButton 9
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
			}
		}
		else if (codriverStick.GetRawButton(raiseButton)){
			if (spikeLifter->currentState() != Relay::kForward){
				spikeLifter->raise();
				printf("extending the relay\n");
			}
		}
		else{
			if (spikeLifter->currentState() != Relay::kOff){
				spikeLifter->off();
				printf("stopping the relay\n");
			}
		}
	}

	void conveyorCheck(Loader *frisbeeLoader) {
		bool buttonPress = codriverStick.GetRawButton(conveyorButton);
		if(buttonPress && (buttonPress != previousButtonStateConveyor)){
			if (not frisbeeLoader->conveyorState()){
				frisbeeLoader->activateConveyor();
				printf("starting the conveyor\n");
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
			printf("EMERGENCY!!! \rCLEAR THE AREA!!!!!!\r GUNS ARE ACTIVE!!!!!!!!!!!!!\n");
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
};
#endif
