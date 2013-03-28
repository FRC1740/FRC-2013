#include "WPILIB.h"
#include "turret.h"

Lifter::Lifter(void):
spikeLifter(lifterSpikePort),
lifterState(1)
{
	minOut = 0.0;   // initalize to reasonable values
	maxOut = 6.5;
}

int Lifter::cycle_linear_actuator(...) {
	printf("Calibrating Lifter\n");
	this->raise();			// Extend
	printf("Maximus Outpodius (maximum output)\n");
	Wait(5);
	maxOut = this->currentVoltage();
	this->lower();			// Retract
	printf("Minimus Outpodius (minimum output)\n");
	Wait(5);
	minOut = this->currentVoltage();
	this->off();
	printf("Lifter Tests Complete\n");
	return 0;
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
	if (Lifter::currentVoltage() < minOut){
		return 0;
	}
	else if (Lifter::currentVoltage() > maxOut){
		return 1;
	}
	else{
		return Lifter::currentVoltage() / (maxOut - minOut);
	}
}
float Lifter::getInches(){
	return Lifter::getPercent() * 6.375;
}

void Lifter::goToInch(float Inches){
	while (Lifter::getInches() > Inches){
		Lifter::lower();
	}
	while (Lifter::getInches() < Inches){
		Lifter::raise();
	}
	Lifter::off();
	printf("done\n!");
}

void Lifter::updateDashboard(void){
	SmartDashboard::PutNumber("Actuator Distance", this->getInches());
}

Shooter::Shooter(void)
{
	Torquey = new Victor(torqueyMotorPort);
	Quickly = new Victor(quickMotorPort);
}
void Shooter::Fire(void){ // fire
	SmartDashboard::PutBoolean("Shooter Motors", true);
	Torquey->Set(-1);
	Quickly->Set(1);

}
void Shooter::stopFiring(void){ // Ok we are stopping firing now
	Torquey->Set(0);
	Quickly->Set(0);
	SmartDashboard::PutBoolean("Shooter Motors", false);
}

