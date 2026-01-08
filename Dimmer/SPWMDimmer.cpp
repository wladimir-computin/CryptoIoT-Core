/*
  CryptoGarage - PWMSPWMDimmer

  (implementation)
*/

#include "SPWMDimmer.h"

SPWMDimmer::SPWMDimmer(const char * name, int pin) : IDimmer(name), curve(MIN_BRIGHTNESS, MAX_BRIGHTNESS) {
	pwmPin = pin;
}

void SPWMDimmer::loop() {
}

void SPWMDimmer::setup() {
	PersistentMemory pmem(appname, true);
	pwmPin = pmem.readInt(KEY_PWM_PIN, pwmPin);
	#ifdef ARDUINO_ARCH_ESP32
		String pointsstr = pmem.readString(KEY_CURVE_POINTS, "0,0 1000,16382 -1000,16382");
	#else
		String pointsstr = pmem.readString(KEY_CURVE_POINTS, "0,0 1000,1923 -1000,1023");
	#endif
	pmem.commit();
  
	printDebug("[SPWMDimmer] Generating polynom...");
	const char * pointsarr = pointsstr.c_str();
	double points[10][2];
	int x;
	int y;
	int offset = 0;
	int len = pointsstr.length();
	int i = 0;

	for (; i < 10; i++) {
		if (offset <= len && sscanf(pointsarr, "%d,%d%n", &x, &y, &offset) == 2) {
			points[i][0] = x;
			points[i][1] = y;
			pointsarr += offset + 1;
		} else {
			break;
		}
	}

	curve.generatePolynomFromPoints(points, i);
	if(pwmPin >= 0){
		pinMode(pwmPin, OUTPUT);
		analogWrite(pwmPin, 0);
		#ifdef ARDUINO_ARCH_ESP32
			#ifdef ARDUINO_ESP32_CORE_V3
				analogWriteResolution(pwmPin, 14);
			#else
				analogWriteResolution(14);
			#endif
		#endif
	}

}

void SPWMDimmer::setVal(double val) {
	curBrightness = val;
	if(pwmPin >= 0){
		int out = curve.calc(curBrightness);
		analogWrite(pwmPin, constrain(out, MIN_BRIGHTNESS, MAX_BRIGHTNESS));
	}
}

double SPWMDimmer::getVal() {
  return curBrightness;
}
