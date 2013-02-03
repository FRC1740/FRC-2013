#ifndef CODRIVER_H_
#define CODRIVER_H_

#define codriverStickPort 3

#define raiseButton 11
#define lowerButton 10
#define conveyorButton 9
class coDriver {
	Joystick codriverStick; // stick for the co-driver

public:
	coDriver():
		codriverStick(codriverStickPort)
	{		
	}

	bool raiseCheck(Lifter *spikeLifter){
		if (codriverStick.GetRawButton(raiseButton)){
			spikeLifter->raise();
			printf("extending the relay\n");
			return true;
		}
		else{
			return false;
		}
	}
	bool lowerCheck(Lifter *spikeLifter){
		if(codriverStick.GetRawButton(lowerButton) == true){
			spikeLifter->lower();
			printf("retracting the relay\n");
			return true;
			}
		else{
			return false;
		}
	}
	void stopCheck(Lifter *spikeLifter){
		spikeLifter->off();
	}

	bool conveyorToggle(Conveyor *conveyorBelt, bool State) {
		bool currentState = codriverStick.GetRawButton(conveyorButton);
		if(currentState != State && currentState){
			if (conveyorBelt->conveyorState() != 0){
				conveyorBelt->activateConveyor();
			}
			else {
				conveyorBelt->stopConveyor();
			}
			
			return true;
		}
		return false;
	}
//	bool alreadyDone(){

//	}

	

};
#endif
