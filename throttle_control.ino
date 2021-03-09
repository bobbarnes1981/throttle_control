#include <EEPROM.h>

#define app0 A0 // accelerator pedal position #0
#define app1 A1 // accelerator pedal position #1
#define tps0 A2 // throttle position sensor #0
#define pwm 9   // motor speed output
#define dir 10  // motor direction control

#define app0min_LOWBYTE 0
#define app0min_HIGHBYTE 1
#define app0max_LOWBYTE 2
#define app0max_HIGHBYTE 3
#define app1min_LOWBYTE 4
#define app1min_HIGHBYTE 5
#define app1max_LOWBYTE 6
#define app1max_HIGHBYTE 7
#define tps0min_LOWBYTE 8
#define tps0min_HIGHBYTE 9
#define tps0max_LOWBYTE 10
#define tps0max_HIGHBYTE 11

#define pwmmin 0
#define pwmmax 1999

#define LED_PIN 6
#define BTN_PIN 2

int app0min = -1;
int app0max = -1;
int app1min = -1;
int app1max = -1;
int tps0min = -1;
int tps0max = -1;

int cal_app0min = -1;
int cal_app0max = -1;
int cal_app1min = -1;
int cal_app1max = -1;
int cal_tps0min = -1;
int cal_tps0max = -1;

#define DEBUG

bool calibration = false;

int readIntFromEEPROM(int low, int high) {
  int val = EEPROM.read(low) | (EEPROM.read(high)<<8);
  #ifdef DEBUG
//  Serial.print(low, HEX);
//  Serial.print(' ');
//  Serial.print(high, HEX);
//  Serial.print(' ');
//  Serial.println(val);
  #endif
  return val;
}

void readDataFromEEPROM() {
  app0min = readIntFromEEPROM(app0min_LOWBYTE, app0min_HIGHBYTE);
  app0max = readIntFromEEPROM(app0max_LOWBYTE, app0max_HIGHBYTE);

  app1min = readIntFromEEPROM(app1min_LOWBYTE, app1min_HIGHBYTE);
  app1max = readIntFromEEPROM(app1max_LOWBYTE, app1max_HIGHBYTE);

  tps0min = readIntFromEEPROM(tps0min_LOWBYTE, tps0min_HIGHBYTE);
  tps0max = readIntFromEEPROM(tps0max_LOWBYTE, tps0max_HIGHBYTE);

  #ifdef DEBUG
//  Serial.println(app0min);
//  Serial.println(app0max);
//  Serial.println(app1min);
//  Serial.println(app1max);
//  Serial.println(tps0min);
//  Serial.println(tps0max);
//  delay(10000);
  #endif
}

void writeIntToEEPROM(int low, int high, int value) {
  EEPROM.write(low, value & 0x00FF);
  EEPROM.write(high, (value & 0xFF00) >> 8);
}

void writeCalibrationToEEPROM() {
  writeIntToEEPROM(app0min_LOWBYTE, app0min_HIGHBYTE, cal_app0min);
  writeIntToEEPROM(app0max_LOWBYTE, app0max_HIGHBYTE, cal_app0max);

  writeIntToEEPROM(app1min_LOWBYTE, app1min_HIGHBYTE, cal_app1min);
  writeIntToEEPROM(app1max_LOWBYTE, app1max_HIGHBYTE, cal_app1max);

  writeIntToEEPROM(tps0min_LOWBYTE, tps0min_HIGHBYTE, cal_tps0min);
  writeIntToEEPROM(tps0max_LOWBYTE, tps0max_HIGHBYTE, cal_tps0max);
}

void setup() {  
  #ifdef DEBUG
  Serial.begin(115200);
  #endif
  
  pinMode(app0, INPUT);
  pinMode(app1, INPUT);
  pinMode(tps0, INPUT);
  pinMode(pwm, OUTPUT);
  pinMode(dir, OUTPUT);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BTN_PIN, INPUT);

  if(digitalRead(BTN_PIN)) {
    calibration = true;
  } else {
    readDataFromEEPROM();
  }
    
  timerInitialisation();
}

void calibrate() {
  digitalWrite(LED_PIN, (millis() % 250) < 125);

  int app0val = analogRead(app0);
  int app1val = analogRead(app1);
  int tps0val = analogRead(tps0);

  if (cal_app0min == -1 || cal_app0min > app0val) {
    cal_app0min = app0val;
  }
  if (cal_app0max == -1 || cal_app0max < app0val) {
    cal_app0max = app0val;
  }

  if (cal_app1min == -1 || cal_app1min > app1val) {
    cal_app1min = app1val;
  }
  if (cal_app1max == -1 || cal_app1max < app1val) {
    cal_app1max = app1val;
  }
  
  if (cal_tps0min == -1 || cal_tps0min > tps0val) {
    cal_tps0min = tps0val;
  }
  if (cal_tps0max == -1 || cal_tps0max < tps0val) {
    cal_tps0max = tps0val;
  }

  #ifdef DEBUG
  Serial.print("app0min:");
  Serial.print(cal_app0min);
  Serial.print("\tapp0max:");
  Serial.print(cal_app0max);
  Serial.print("\tapp1min:");
  Serial.print(cal_app1min);
  Serial.print("\tapp1max:");
  Serial.print(cal_app1max);
  Serial.print("\ttps0min:");
  Serial.print(cal_tps0min);
  Serial.print("\ttps0max:");
  Serial.print(cal_tps0max);
  Serial.println();
  #endif

  if (millis() > 5000 && digitalRead(BTN_PIN)) {
    digitalWrite(LED_PIN, HIGH);
    writeCalibrationToEEPROM();
    delay(5000);
    digitalWrite(LED_PIN, LOW);
  }
}

void throttle() {  // flash LED
  digitalWrite(LED_PIN, (millis() % 1000) < 125);

  // throttle actuator closed loop control

  int app0val = analogRead(app0);
  int app1val = analogRead(app1);
  int tps0val = analogRead(tps0);

  app0val = map(app0val, app0min, app0max, pwmmin, pwmmax); // map pedal sensor #0
  app1val = map(app1val, app1min, app1max, pwmmin, pwmmax); // map pedal sensor #1
  tps0val = map(tps0val, tps0max, tps0min, pwmmin, pwmmax); // map throttle position sensor #0

  int throttleInputAverage = (app0val + app1val) / 2;
  int difference = throttleInputAverage-tps0val;

  #ifdef DEBUG
  Serial.print("app0val:");
  Serial.print(app0val);
  Serial.print("\tapp1val:");
  Serial.print(app1val);
  Serial.print("\tappAvg:");
  Serial.print(throttleInputAverage);
  Serial.print("\ttps0val:");
  Serial.print(tps0val);
  Serial.print("\tdiff:");
  Serial.print(difference);
  Serial.println();
  #endif

  if (difference < 0) {
    digitalWrite(dir, HIGH);
  }else{
    digitalWrite(dir, LOW);
  }
  analogWrite(pwm, 1999); // full speed all the time
}

void loop() {
  if (calibration) {
    calibrate();
  } else {
    throttle();
  }
}
