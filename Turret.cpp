#include "WPILib.h"

class Turret {
	
	Relay spike;
	
public:

	Turret(void):
		spike(1)
	{
	}
	void powerChange(bool state) {

		printf("testing: class spike, function powerChange");
		spike.Set(Relay.Value.kForward);
//		Wait(5);
		spike.Set(Relay.Value.kReverse);
//		Wait(5);
	}
};
