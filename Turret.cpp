#include "WPILib.h"

class Turret {
	
	Relay spike;
	
public:

	Turret(void):
		spike(1)
	{
	}
	void cycle_linear_actuator(bool state) {

		printf("testing: class Turret, function cycle");
		int foo = spike.Get();
		printf("Turret: spike status: %d\n", foo);
		spike.Set(Relay::kForward);
		printf("Turret: wait 5 sec...\n");
		Wait(5);
		spike.Set(Relay::kReverse);
		printf("Turret: Wait 5 more sec...\n");
		Wait(5);
		printf("Turret: Exiting!\n");
	}
};
