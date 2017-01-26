// Enable debug prints
// #define MY_DEBUG
#define MY_RADIO_NRF24

#include <SPI.h>
#include <MySensors.h>
#include <DistanceSensor.h>

#define BATTERY_SENSE_PIN  A0  // select the input pin for the battery sense point
#define PING_ID      1
#define VOLT_ID      2

#define BATTERY_VCC  6 // Voltage source for battery voltage reader
#define PING_VCC     5 // Voltage source for hc-sr04

#define TRIGGER_PIN  3 // Trigger pin for hc-sr04
#define ECHO_PIN     4 // Trigger pin for hc-sr04
unsigned long SLEEP_TIME = 1800000; // Sleep time between reads (in milliseconds)

MyMessage msg, batteryMsg;
float lastBattery;
int lastDistance = 0;
DistanceSensor ping(TRIGGER_PIN, ECHO_PIN);

void setup()
{
  analogReference(INTERNAL);
  ping.setVCCPin(PING_VCC);

  pinMode(BATTERY_VCC, OUTPUT);
  digitalWrite(BATTERY_VCC, LOW);

  msg = MyMessage(PING_ID, V_DISTANCE);
  batteryMsg = MyMessage(VOLT_ID, V_VOLTAGE);
}

void presentation() {
  sendSketchInfo("Soft Water Salt Level", "1.0");
  present(PING_ID, S_DISTANCE);
  present(VOLT_ID, S_MULTIMETER);
}

float getBatteryVoltage() {
  digitalWrite(BATTERY_VCC, HIGH);

  for (uint8_t i = 0; i < 8; i++) {
    analogRead(BATTERY_SENSE_PIN); // burn some readings
  }

  // 1M, 270K divider across battery and using internal ADC ref of 1.1V
  // ((1e6+270e3)/270e3)*1.1 = Vmax = 5.17 Volts
  // 5.17/1024 = Volts per bit = 0.005048828125
  float v = analogRead(BATTERY_SENSE_PIN);
  digitalWrite(BATTERY_VCC, LOW);

  return v * 0.005048828125;
}

void loop()
{
  int dist = ping.getInches();
  int reportedDist = dist;

  if (lastDistance > 0 && dist > 0 && abs(dist - lastDistance) < 2) {
    reportedDist = lastDistance;
  } else {
    lastDistance = dist;
  }

  #if defined MY_DEBUG
  Serial.print("Ping: ");
  Serial.print(dist);
  Serial.println(" in");
  Serial.print("Reported: ");
  Serial.print(reportedDist);
  Serial.println(" in");
  #endif

  send(msg.set(reportedDist));

  float batteryV = getBatteryVoltage();

  #ifdef MY_DEBUG
  Serial.print("Battery Voltage: ");
  Serial.println(batteryV);
  #endif

  if (batteryV != lastBattery) {
    send(batteryMsg.set((int)(batteryV*100)));
    lastBattery = batteryV;
  }

  sleep(SLEEP_TIME);
}
