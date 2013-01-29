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
		extend();			// Extend
		printf("Turret: wait 5 sec...\n");
		Wait(5);
		retract();			// Retract
		printf("Turret: Wait 5 more sec...\n");
		Wait(5);
		off();
		printf("Turret: Exiting!\n");
	}
	void extend(void){
		printf("extending the relay");
		spike.Set(Relay::kForward);
	}
	void retract(void){
		printf("retracting the relay");
		spike.Set(Relay::kReverse);
	}
	void off(void){
		printf("stopping the spike");
		spike.Set(Relay::kOff);
	}
};
