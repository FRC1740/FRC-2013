#ifndef TURRET_H_
#define TURRET_H_

// This is the digital output port for the spike that raises and lowers the turret
#define lifterSpikePort 1
// This is the stronger slower motor for the first wheel on the shooter
#define torqueyMotor 3
// This is the weaker faster motor for the second wheel on the shooter
#define quickMotor 4

class Lifter {
	
	Relay spikeLifter;
	
public:

	Lifter(void):
		spikeLifter(lifterSpikePort)
	{
	}

/* 
	void cycle_linear_actuator(bool state) {

		printf("testing: class Lifter, function cycle");
		int foo = spikeLifter.Get();
		printf("Lifter: spikeLifter status: %d\n", foo);
		raise();			// Extend
		printf("Lifter: wait 5 sec...\n");
		Wait(5);
		lower();			// Retract
		printf("Lifter: Wait 5 more sec...\n");
		Wait(5);
		off();
		printf("Lifter: Exiting!\n");
	}
*/
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
};

class Shooter {
	Victor Torquey;
	Victor Quickly;
public:
	
	Shooter(void):
		Torquey(torqueyMotor),
		Quickly(quickMotor)
	{
	}
	void fire(void){ // Rev up and fire
		Torquey.Set(0.5);
		Wait(5);
		Torquey.Set(0);
		Quickly.Set(1);
		Wait(5);
		Quickly.Set(0);
	}
};

#endif

