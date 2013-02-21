#ifndef SUPPORT_H_
#define SUPPORT_H_


#define DEBUG_MODE true
#define EMERGENCY 0
#define NORMAL 1
#define DEBUG 2
#define SAME_LINE_DEBUG 3
#define SAME_LINE_NORMAL 4

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
		if (message_type == NORMAL || message_type == SAME_LINE_NORMAL){
			if (message_type == SAME_LINE_NORMAL){
				printf(output);
			}
			else{
				printf("[NORMAL] %s", output);
			}
		}
		else if (message_type == EMERGENCY){
			fprintf(stderr, "[ERROR] %s", output);
		}
		else if ((message_type == DEBUG || message_type == SAME_LINE_DEBUG) && DEBUG_MODE){
			if (message_type == SAME_LINE_DEBUG){
				fprintf(stderr, output);
			}
			else{
				fprintf(stderr, "[DEBUG] %s", output);
			}
		}
	}
};

#endif
