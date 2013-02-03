#ifndef LOADER_H_
#define LOADER_H_

#define elevatorMotorPort 5
#define oneRotationTime 3
#define stopTime .05

#define beltMotorPort 6

class Limiter {
	
	DigitalInput Limit;
//	Victor Grab;
//	Victor Lift;
	
public:
	Limiter(void):
		Limit(1)
	{
	}
	bool checkLimit(void){
		return Limit.Get();
	}
};

class Conveyor {
	Victor Belt;
public:
	Conveyor(void):
		Belt(beltMotorPort)
	{
	}
	void activateConveyor(void) {
		Belt.Set(1);
	}
	void stopConveyor(void) {
		Belt.Set(0);
	}
	float conveyorState(void){
		return Belt.Get();
	}
	
};

class Raiser {
	Victor Elevator;
public:
	Raiser(void):
		Elevator(elevatorMotorPort)
	{
	}
	void raiseOneLevel(void){
		Elevator.Set(1);
		Wait(oneRotationTime);
		Elevator.Set(-1);
		Wait(stopTime);
		Elevator.Set(0);
	}
};
#endif
