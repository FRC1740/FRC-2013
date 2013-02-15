#include "WPILIB.h"
#include "turret.h"

Lifter::Lifter(void):
spikeLifter(lifterSpikePort),
lifterState(1)
{
	minOut = 0.0;   // initalize to reasonable values
	maxOut = 5.0;
}

void Lifter::cycle_linear_actuator(void) {
	printf("Calibrating Lifter\n");
	raise();			// Extend
	printf("Maximus Outpodius (maximum output)\n");
	Wait(5);
	maxOut = getInches();
	lower();			// Retract
	printf("Minimus Outpodius (minimum output)\n");
	Wait(5);
	minOut = getInches();
	off();
	printf("Lifter Tests Complete\n");
}

void Lifter::raise(void){
	spikeLifter.Set(Relay::kForward);
}
void Lifter::lower(void){
	spikeLifter.Set(Relay::kReverse);
}
void Lifter::off(void){
	spikeLifter.Set(Relay::kOff);
}
float Lifter::currentState(void){
	return spikeLifter.Get();
}
float Lifter::currentVoltage(void){
	return lifterState.GetVoltage();
}
float Lifter::getPercent(){
	if (currentVoltage() < minOut){
		return 0;
	}
	else if (currentVoltage() > maxOut){
		return 100;
	}
	else{
		return currentVoltage() / maxOut;
	}
}
float Lifter::getInches(){
	return getPercent() * 6.0;
}


Shooter::Shooter(void):
				Torquey(torqueyMotorPort),
				Quickly(quickMotorPort)
{
}
void Shooter::Fire(void){ // fire
	Wait(.5);
	Torquey.Set(1);
	Quickly.Set(1);

}
void Shooter::stopFiring(void){ // Ok we are stopping firing now
	Torquey.Set(0);
	Quickly.Set(0);
}

