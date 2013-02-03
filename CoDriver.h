#ifndef CODRIVER_H_
#define CODRIVER_H_

#define codriverStickPort 3
class coDriver {
	Joystick codriverStick; // stick for the co-driver

public:
	coDriver():
		codriverStick(codriverStickPort)
	{
		// spikeLifter = new Lifter;
		// frisbeeShooter = new Shooter;
		// Loader = new Limiter;
		
	}

	bool raiseCheck(Lifter *spikeLifter){
		if (codriverStick.GetRawButton(11)){
			spikeLifter->raise();
			printf("extending the relay\n");
			return true;
		}
		else{
			return false;
		}
	}
	bool lowerCheck(Lifter *spikeLifter){
		if(codriverStick.GetRawButton(10) == true){
			spikeLifter->lower();
			printf("retracting the relay\n");
			return true;
			}
		else{
			return false;
		}
	}
	void stopCheck(Lifter *spikeLifter){
		spikeLifter->off();
	}
	
};
#endif
