#ifndef TURRET_H_
#define TURRET_H_

// This is the digital output port for the spike that raises and lowers the turret
#define lifterSpikePort 1
// This is the stronger slower motor for the first wheel on the shooter
#define torqueyMotorPort 3
// This is the weaker faster motor for the second wheel on the shooter
#define quickMotorPort 4

//#define maxOut 4.863
//#define minOut 0.1

class Lifter {
	float maxOut;
	float minOut;
	Relay spikeLifter;
	AnalogChannel lifterState;
	
public:

	Lifter(void):
		spikeLifter(lifterSpikePort),
		lifterState(1)
	{
		minOut = 0.0;   // initalize to reasonable values
		maxOut = 5.0;
	}


	void cycle_linear_actuator(void) {
		printf("Testing Lifter\n");
		raise();			// Extend
		printf("Maximus Outpodius (maximum output)\n");
		Wait(5);
		maxOut = getInches();
		lower();			// Retract
		printf("Minimus Outpodius (minimum output)\n");
		Wait(5);
		minOut = getInches();
		off();
		printf("Lifter Tests Complete\n");
	}

	void raise(void){
		spikeLifter.Set(Relay::kForward);
	}
	void lower(void){
		spikeLifter.Set(Relay::kReverse);
	}
	void off(void){
		spikeLifter.Set(Relay::kOff);
	}
	float currentState(void){
		return spikeLifter.Get();
	}
	float currentVoltage(void){
		return lifterState.GetVoltage();
	}
	float getPercent(){
		if (currentVoltage() < minOut){
			return 0;
		}
		else if (currentVoltage() > maxOut){
			return 100;
		}
		else{
			return currentVoltage() / maxOut;
		}
	}
	float getInches(){
		return getPercent() * 6.0;
	}
};

class Shooter {
	Victor Torquey;
	Victor Quickly;
public:
	
	Shooter(void):
		Torquey(torqueyMotorPort),
		Quickly(quickMotorPort)
	{
	}
	void Fire(void){ // fire
		Wait(.5);
		Torquey.Set(1);
		Quickly.Set(1);

	}
	void stopFiring(void){ // Ok we are stopping firing now
		Torquey.Set(0);
		Quickly.Set(0);
	}
};

#endif

