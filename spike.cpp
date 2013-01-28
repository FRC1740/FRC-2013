#include "WPILib.h"


class mySpike : public Relay {

	static const int motorStatus = 0;

	public:
	mySpike(void) {}
	

	void powerChange(bool state) {
		printf("testing: class spike, function powerChange");
	}
};
/* */
