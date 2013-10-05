#include "CoDriver.h"

coDriver::coDriver(void):
	codriverStick(codriverStickPort)
{	
	previousButtonStateConveyor = false;
	isShooting = false;
	coDriverOut = new robotOut;
}

void coDriver::lifterCheck(Lifter *spikeLifter){
	char outBuffer [40];
	if(codriverStick.GetRawButton(lowerButton)){
		if (spikeLifter->currentState() != Relay::kReverse){
			spikeLifter->lower();
			sprintf(outBuffer, "We are starting at state %f\n", spikeLifter->getInches());
			coDriverOut->printDebug(outBuffer, 2);
		}
	}
	else if (codriverStick.GetRawButton(raiseButton)){
		if (spikeLifter->currentState() != Relay::kForward){
			spikeLifter->raise();
			sprintf(outBuffer, "We are starting at state %f\n", spikeLifter->getInches());
			coDriverOut->printDebug(outBuffer, 2);
		}
	}
	else{
		if (spikeLifter->currentState() != Relay::kOff){
			spikeLifter->off();
			sprintf(outBuffer, "We are ending at state %f\n", spikeLifter->getInches());
			coDriverOut->printDebug(outBuffer, 2);
		}
	}
}


void coDriver::conveyorCheck(Loader *frisbeeLoader) {
	bool buttonPress = codriverStick.GetRawButton(conveyorButton);
	if(buttonPress && (buttonPress != previousButtonStateConveyor)){
		if (not frisbeeLoader->conveyorState()){
			frisbeeLoader->activateConveyor();
		}
		else {
			frisbeeLoader->stopConveyor();
		}
		
		previousButtonStateConveyor = true;
	}
	else if (not buttonPress){
		previousButtonStateConveyor =  false;
	}
}
void coDriver::fireAtWill(Shooter *frisbeeShooter, Loader *frisbeeLoader){
	bool buttonPress = codriverStick.GetRawButton(Trigger);
	if (buttonPress && (isShooting != buttonPress)){
		coDriverOut->printDebug("EMERGENCY!!! \rCLEAR THE AREA!!!!!!\rGUNS ARE ACTIVE!!!!!!!!!!!!!\n", 2);
		frisbeeShooter->Fire();
//		frisbeeLoader->activateElevator();
		isShooting = true;
	}
	else if (not buttonPress){
		frisbeeShooter->stopFiring();
//		frisbeeLoader->stopElevator();
		isShooting = false;
	}


}

void coDriver::initalizeLifter(Lifter *spikeLifter){
	if (codriverStick.GetRawButton(7)){
		spikeLifter->cycle_linear_actuator();
	}
}

float coDriver::returnJoystick(int port){
	return codriverStick.GetRawButton(port);
}

void coDriver::goToInchCheck(Lifter *spikeLifter){
	if (codriverStick.GetRawButton(8)){
		spikeLifter->goToInch(inchesTest);
	}
}

