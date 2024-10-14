/*
  CryptoGarage - EdgeDimmer

  (implementation)
*/

#ifdef ARDUINO_ARCH_ESP8266

#include "EdgeDimmer.h"

static int curve_lookup[1000 + 1];
volatile static int dimmer_count = 0;
const static EdgeDimmer* refs[5];
const static EdgeDimmer* next_dimmer[5];
volatile static int next = 0;
volatile static int next_len = 0;

EdgeDimmer::EdgeDimmer(const char * name) : IDimmer(name){
}

EdgeDimmer::~EdgeDimmer() {
  dimmer_count--;
}

void EdgeDimmer::loop() {
}

void EdgeDimmer::setup() {
  PersistentMemory pmem(appname, true);

  LongestDutyPauseUs = pmem.readInt(KEY_LONGEST_DUTY_PAUSE, LongestDutyPauseUs);
  flickerfix = pmem.readBool(KEY_FLICKER_FIX, false);
  pwmPin = pmem.readInt(KEY_PWM_PIN, pwmPin);
  zcPin = pmem.readInt(KEY_ZC_PIN, zcPin);
  String pointsstr = pmem.readString(KEY_CURVE_POINTS, "0,0 1000,1000");
  pmem.commit();

  const char * pointsarr = pointsstr.c_str();

  double points[10][2];
  int x;
  int y;
  int offset=0;
  int len = pointsstr.length();
  int i=0;
  
  for(; i<10; i++){
    if(offset <= len && sscanf(pointsarr,"%d,%d%n",&x, &y, &offset) == 2){
      points[i][0] = x;
      points[i][1] = y;
      pointsarr += offset+1;
    } else {
      break;
    }
  }

  printDebug("[Dimmer] Generating lookup table...");
  //double points[][2] = {{0, 0}, {1000, 1000}, { -1000, 1000}, {560, 322}, { -560, 322}, {600, 330}, { -600, 330}};
  curve.generateTableFromPoints(points, i, LongestDutyPauseUs, curve_lookup, sizeof(curve_lookup) / sizeof(curve_lookup[0]));

  pinMode(pwmPin, OUTPUT);

  dimmer_index = dimmer_count++;
  refs[dimmer_index] = this;

  if (dimmer_index == 0) {
    pinMode(zcPin, INPUT_PULLUP);
    attachInterrupt(zcPin, zeroCrossISR, RISING);
	timer1_attachInterrupt(onTimerISR);
    timer1_enable(TIM_DIV16, TIM_EDGE, TIM_SINGLE);
  }
  
}

void EdgeDimmer::setVal(double val) {
  curBrightness = toAbsolute(val);
}

double EdgeDimmer::getVal() {
  return toRelative(curBrightness);
}

double EdgeDimmer::toRelative(int absolute) {
  return (double)absolute / MAX_BRIGHTNESS;
}

int EdgeDimmer::toAbsolute(double relative) {
  return round(relative * MAX_BRIGHTNESS);
}

int transform(int input) {
  return curve_lookup[input];
}

void onTimerISR() {
  digitalWrite(next_dimmer[next]->pwmPin, HIGH);
  if(next_dimmer[next]->flickerfix){
	if (next_dimmer[next]->curBrightness <= 850) {
		for (int i = 0; i < 5; i++) {
			digitalWrite(next_dimmer[next]->pwmPin, HIGH);
		}
		digitalWrite(next_dimmer[next]->pwmPin, LOW);
	}
  }
  if (next + 1 < next_len) {
    int wait = transform(next_dimmer[next + 1]->curBrightness);
    for (int i = next; i >= 0; i--) {
      wait -= transform(next_dimmer[i]->curBrightness);
    }
    next++;
    if (wait != 0) {
      timer1_write(wait);
    } else {
      onTimerISR();
    }
  }
}

int sort_dimmers(const void *cmp1, const void *cmp2) {
  EdgeDimmer * a = *((EdgeDimmer **)cmp1);
  EdgeDimmer * b = *((EdgeDimmer **)cmp2);
  return b->curBrightness - a->curBrightness;
}

volatile unsigned long lastmicros = 0;
void zeroCrossISR() {
  unsigned long now = micros();
  unsigned long delta = now - lastmicros;
  if (delta > (((1 * 1000 * 1000) / 55) / 2)) {
    lastmicros = now;
    int index = 0;
    for (int i = 0; i < dimmer_count; i++) {
      if (refs[i]->curBrightness == MAX_BRIGHTNESS) {
        digitalWrite(refs[i]->pwmPin, HIGH);
      } else if (refs[i]->curBrightness == MIN_BRIGHTNESS) {
        digitalWrite(refs[i]->pwmPin, LOW);
      } else {
        digitalWrite(refs[i]->pwmPin, LOW);
        next_dimmer[index++] = refs[i];
      }
    }
    next = 0;
    next_len = index;
    if (next_len > 0) {
      if (next_len > 1) {
        qsort(next_dimmer, next_len, sizeof(next_dimmer[0]), sort_dimmers);
      }
      timer1_write(transform(next_dimmer[next]->curBrightness));
    }
  }
}

#endif
