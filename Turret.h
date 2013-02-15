#ifndef TURRET_H_
#define TURRET_H_

// This is the digital output port for the spike that raises and lowers the turret
#define lifterSpikePort 1
// This is the stronger slower motor for the first wheel on the shooter
#define torqueyMotorPort 3
// This is the weaker faster motor for the second wheel on the shooter
#define quickMotorPort 4

class Lifter {
	float maxOut;
	float minOut;
	Relay spikeLifter;
	AnalogChannel lifterState;
	
public:

	Lifter(void);
	void cycle_linear_actuator(void);
	void raise(void);
	void lower(void);
	void off(void);
	float currentState(void);
	float currentVoltage(void);
	float getPercent();
	float getInches();
};

class Shooter {
	Victor Torquey;
	Victor Quickly;
public:	
	Shooter(void);
	void Fire(void); // fire
	void stopFiring(void); // Ok we are stopping firing now
};

#endif
