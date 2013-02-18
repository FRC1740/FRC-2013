#ifndef LOADER_H_
#define LOADER_H_


#include "WPILIB.h"
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
	Loader(void);
	void activateConveyor(void);
	void stopConveyor(void);
	float conveyorState(void);
	void activateElevator(void);
	void stopElevator(void);
	float elevatorState(void);
	bool checkLimitConveyor(void);
	void loaderSequence(void);
};
#endif
