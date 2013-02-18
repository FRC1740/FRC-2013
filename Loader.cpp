#include "Loader.h"


Loader::Loader(void):
Belt(beltMotorPort),
Elevator(elevatorMotorPort),
limitConveyor(limitConveyorPort),
limitElevator(limitElevatorPort)
{
}
void Loader::activateConveyor(void) {
	if (not checkLimitConveyor()){
		Belt.Set(-1);
		SmartDashboard::PutBoolean("Pickup Running", true);
	}
}
void Loader::stopConveyor(void) {
	Belt.Set(0);
	SmartDashboard::PutBoolean("Pickup Running", false);
}
float Loader::conveyorState(void){
	return Belt.Get();
}

void Loader::activateElevator(void){
	Elevator.Set(-1);
}

void Loader::stopElevator(void){
	Elevator.Set(0);
}

float Loader::elevatorState(void){
	return Elevator.Get();
}

bool Loader::checkLimitConveyor(void){
	return limitConveyor.Get();
}

void Loader::loaderSequence(void){
	if (limitConveyor.Get()){
		Belt.Set(0);
		SmartDashboard::PutBoolean("Pickup Running", false);
		this->activateElevator();
		Wait(2);
		this->stopConveyor();
	}
}	
