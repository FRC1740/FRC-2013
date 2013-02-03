#ifndef CODRIVER_H_
#define CODRIVER_H_

#define codriverStickPort 3

#define raiseButton 11
#define lowerButton 10
#define conveyorButton 9
class coDriver {
	Joystick codriverStick; // stick for the co-driver
	bool conveyorToggleState;
public:
	coDriver():
		codriverStick(codriverStickPort)
	{	
		conveyorToggleState = false;
	}
/*  old method of checking if the codriver wanted to move the lifter
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
	*/
	void lifterCheck(Lifter *spikeLifter){
		if(codriverStick.GetRawButton(lowerButton) == true && spikeLifter->currentState() != Relay::kReverse){
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
				printf("stopping the relay");
			}
		}
	}

	void conveyorCheck(Conveyor *conveyorBelt) {
		bool buttonPress = codriverStick.GetRawButton(conveyorButton);
		if(buttonPress && buttonPress != conveyorToggleState){
			if (conveyorBelt->conveyorState() != 0){
				conveyorBelt->activateConveyor();
			}
			else {
				conveyorBelt->stopConveyor();
			}
			
			conveyorToggleState = true;
		}
		else{
			conveyorToggleState =  false;
		}
	}
//	bool alreadyDone(){

//	}

	

};
#endif
