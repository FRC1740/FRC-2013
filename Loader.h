#ifndef LOADER_H_
#define LOADER_H_

#include <time.h>

#define elevatorMotorPort 5
#define oneRotationTime 3
#define stopTime .05

#define limitConveyorPort 1
#define limitElevatorPort 2

#define beltMotorPort 6

class Loader {
	Victor Belt;
	Victor Elevator;
	DigitalInput limitConveyor;
	DigitalInput limitElevator;
public:
	Loader(void):
		Belt(beltMotorPort),
		Elevator(elevatorMotorPort),
		limitConveyor(limitConveyorPort),
		limitElevator(limitElevatorPort)
	{
	}
	void activateConveyor(void) {
		if (not checkLimitConveyor()){
			Belt.Set(1);
		}
	}
	void stopConveyor(void) {
		Belt.Set(0);
	}
	float conveyorState(void){
		return Belt.Get();
	}
	
	void activateElevator(void){
		Elevator.Set(1);
	}
	
	void stopElevator(void){
		Elevator.Set(0);
	}
	
	float elevatorState(void){
		return Elevator.Get();
	}
	
	bool checkLimitConveyor(void){
		return limitConveyor.Get();
	}

	void loaderSequence(void){
		if (limitConveyor.Get()){
			Belt.Set(0);
			Elevator.Set(1);
		}
	}
	
};
#endif
