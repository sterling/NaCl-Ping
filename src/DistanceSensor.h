#include <Arduino.h>

class DistanceSensor {
	private:
		static const uint8_t DEFAULT_SAMPLE_SIZE = 3;
		uint8_t dataSampleSize;
		int8_t vcc;
		uint8_t trigger;
		uint8_t echo;

		void setupPins();
		double convertDurationToCM(double roundTripMicroSeconds);
		double convertDurationToInches(double roundTripMicroSeconds);
		double sampleDistance();
		void reset();
		double getPulseReading();
		void powerOn();
		void powerOff();

	public:
		DistanceSensor(uint8_t trigger, uint8_t echo);
    DistanceSensor(uint8_t sampleSize, uint8_t trigger, uint8_t echo);

		void setVCCPin(uint8_t vcc);
		int getInches();
		int getCM();
};
