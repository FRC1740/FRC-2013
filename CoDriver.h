#ifndef CODRIVER_H_
#define CODRIVER_H_

#include "WPILib.h"
#include "Loader.h"
#include "Turret.h"
#define codriverStickPort 3

#define Trigger 1
#define raiseButton 11
#define lowerButton 10
#define conveyorButton 9
#define inchesTest 2.6

class coDriver {
	Joystick codriverStick; // stick for the co-driver
	bool previousButtonStateConveyor;
	bool isShooting;
public:
	coDriver(void);
	void lifterCheck(Lifter *spikeLifter);
	void conveyorCheck(Loader *frisbeeLoader);
	int cycle_linear_actuator(...);
	void fireAtWill(Shooter *frisbeeShooter, Loader *frisbeeLoader);
	void initalizeLifter(Lifter *spikeLifter);
	void goToInchCheck(Lifter *spikeLifter);
	float returnJoystick(int port);
};
#endif
