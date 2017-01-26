#include "DistanceSensor.h"

int cmp(const void * a, const void * b) {
  double aa = *(double*)a;
  double bb = *(double*)b;

  return aa > bb ? 1 : (aa < bb ? -1 : 0);
}

void DistanceSensor::setupPins() {
  pinMode(this->trigger, OUTPUT);
  pinMode(this->echo, INPUT);
}

DistanceSensor::DistanceSensor(uint8_t triggerPin, uint8_t echoPin)
: dataSampleSize(DistanceSensor::DEFAULT_SAMPLE_SIZE), vcc(-1), trigger(triggerPin), echo(echoPin) {
  this->setupPins();
}

DistanceSensor::DistanceSensor(uint8_t sampleSize, uint8_t triggerPin, uint8_t echoPin)
: dataSampleSize(sampleSize), vcc(-1), trigger(triggerPin), echo(echoPin) {
  this->setupPins();
}

double DistanceSensor::sampleDistance() {
  digitalWrite(this->trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(this->trigger, LOW);

  double durationMicroSec = pulseIn(this->echo, HIGH, 100000);

  if (durationMicroSec == 0) {
    this->reset();
  }

  return durationMicroSec;
}

void DistanceSensor::reset() {
  pinMode(this->echo, OUTPUT);
  pinMode(this->trigger, INPUT);
  digitalWrite(this->echo, LOW);
  delay(10);
  pinMode(this->echo, INPUT);
  pinMode(this->trigger, OUTPUT);
}

double DistanceSensor::getPulseReading() {
  uint8_t sampleSize = this->dataSampleSize;
  if (sampleSize % 2 == 0) {
    sampleSize += 1;
  }

  double readings[sampleSize];

  this->powerOn();
  for (uint8_t i = 0; i < sampleSize; i++) {
    delay(10);
    readings[i] = this->sampleDistance();
  }
  this->powerOff();

  qsort(readings, sampleSize, sizeof(double), cmp);
  return readings[sampleSize / 2];
}

void DistanceSensor::powerOn() {
  if (this->vcc >= 0) {
    digitalWrite(this->vcc, HIGH);
  }
}

void DistanceSensor::powerOff() {
  if (this->vcc >= 0) {
    digitalWrite(this->vcc, LOW);
  }
}

double DistanceSensor::convertDurationToCM(double roundTripMicroSeconds) {
  // speed of sound: 340.29m/s
  return roundTripMicroSeconds / 2 * .034029;
}

double DistanceSensor::convertDurationToInches(double roundTripMicroSeconds) {
  // speed of sound: 13,397in/s
  return roundTripMicroSeconds / 2 * .013397;
}

int DistanceSensor::getInches() {
  double pulseTime = this->getPulseReading();
  return (int)(this->convertDurationToInches(pulseTime) + 0.5);
}

int DistanceSensor::getCM() {
  double pulseTime = this->getPulseReading();
  return (int)(this->convertDurationToCM(pulseTime) + 0.5);
}

void DistanceSensor::setVCCPin(uint8_t vcc) {
  this->vcc = vcc;
  pinMode(this->vcc, OUTPUT);
  digitalWrite(this->vcc, LOW);
}
