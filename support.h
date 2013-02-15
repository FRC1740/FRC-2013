#ifndef SUPPORT_H_
#define SUPPORT_H_

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
};

#endif
