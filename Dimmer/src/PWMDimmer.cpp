/*
  CryptoGarage - PWMPWMDimmer

  (implementation)
*/

#include "PWMDimmer.h"

static int curve_lookup[1023 + 1];

PWMDimmer::PWMDimmer(const char * name, int pin) : IDimmer(name) {
	pwmPin = pin;
}

void PWMDimmer::loop() {
}

void PWMDimmer::setup() {
	PersistentMemory pmem(appname, true);
	pwmPin = pmem.readInt(KEY_PWM_PIN, pwmPin);
	String pointsstr = pmem.readString(KEY_CURVE_POINTS, "0,0 1000,1023");
	pmem.commit();
  
	printDebug("[PWMDimmer] Generating lookup table...");
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

	curve.generateTableFromPoints(points, i, curve_lookup, sizeof(curve_lookup) / sizeof(curve_lookup[0]));
	if(pwmPin > 0){
		pinMode(pwmPin, OUTPUT);
	}
}

void PWMDimmer::setVal(double val) {
	curBrightness = toAbsolute(val);
	if(pwmPin > 0){
		analogWrite(pwmPin, curve_lookup[curBrightness]);
	}
}

double PWMDimmer::getVal() {
  return toRelative(curBrightness);
}

double PWMDimmer::toRelative(int absolute) {
  return (double)absolute / MAX_BRIGHTNESS;
}

int PWMDimmer::toAbsolute(double relative) {
  return round(relative * MAX_BRIGHTNESS);
}
