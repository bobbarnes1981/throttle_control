
#define app0 A0 // accelerator pedal position #0
#define app1 A1 // accelerator pedal position #1
#define tps0 A2 // throttle position sensor #0
#define pwm 9   // motor speed output
#define dir 10  // motor direction control

#define app0min 0
#define app0max 1023
#define app1min 0
#define app1max 1023
#define tps0min 0
#define tps0max 1023
#define pwmmin 0
#define pwmmax 1999

#define LED_PIN 6
#define BTN_PIN 2

int cal_app0min = -1;
int cal_app0max = -1;
int cal_app1min = -1;
int cal_app1max = -1;
int cal_tps0min = -1;
int cal_tps0max = -1;

#define DEBUG

bool calibration = false;

void setup() {
  timerInitialisation();
    
  pinMode(app0, INPUT);
  pinMode(app1, INPUT);
  pinMode(tps0, INPUT);
  pinMode(pwm, OUTPUT);
  pinMode(dir, OUTPUT);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BTN_PIN, INPUT);

  if(digitalRead(BTN_PIN)) {
    calibration = true;
  }

  #ifdef DEBUG
  Serial.begin(115200);
  #endif
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
    
    // TODO: write calibration to flash
    delay(1000);
    
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
