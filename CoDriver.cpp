#include "CoDriver.h"

coDriver::coDriver(void):
	codriverStick(codriverStickPort)
{	
	previousButtonStateConveyor = false;
	isShooting = false;
}

void coDriver::lifterCheck(Lifter *spikeLifter){
	SmartDashboard::PutNumber("Actuator Distance", spikeLifter->getInches());
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
		printf("he hit the button!\n");
		spikeLifter->goToInch(inchesTest);
	}
}

