#ifndef LOADER_H_
#define LOADER_H_

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
#endif
