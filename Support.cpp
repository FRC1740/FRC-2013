#include "WPILib.h"
#include "support.h"

void robotOut::printDebug(char *output, int message_type){
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
