// Code in this file is for the thing to load disks into the robot
#include "WPILib.h"

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
