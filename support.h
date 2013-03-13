#ifndef SUPPORT_H_
#define SUPPORT_H_


#define DEBUG_MODE true
#define EMERGENCY 0
#define NORMAL 1
#define DEBUG 2
#define SAME_LINE_DEBUG 3
#define SAME_LINE_NORMAL 4

class robotOut {
public:
	void printDebug(char *output, int message_type);
};

#endif
