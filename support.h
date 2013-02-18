#ifndef SUPPORT_H_
#define SUPPORT_H_


#define DEBUG_MODE false
#define EMERGENCY 0
#define NORMAL 1
#define DEBUG 2

/*
#include <time.h>

class Timer {
	float startTime;
	float timeElapsed;
public:

	void startTimer(void){
		startTime = time();
		timerStarted = true;
	}
	float timePassed(void){
		return time() - startTime;
	}
	bool isItDoneYet(int seconds){
		if (timePassed() <= seconds){
			return false;
		}
		else{
			return true;
		}
	}
	void Restart(void){
		startTime = time();
		timerStarted = true;
	}
};
*/
class robotOut {
public:
	void printDebug(char *output, int message_type){
		if (message_type == NORMAL){
			printf("[NORMAL] %s", output);
		}
		else if (message_type == EMERGENCY){
			printf("[ERROR] %s", output);
		}
		else if (message_type == DEBUG && DEBUG_MODE){
			printf("[DEBUG] %s", output);
		}
	}
};

#endif
