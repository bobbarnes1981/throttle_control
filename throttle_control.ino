
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

#define DEBUG

void setup() {
  timerInitialisation();
    
  pinMode(app0, INPUT);
  pinMode(app1, INPUT);
  pinMode(tps0, INPUT);
  pinMode(pwm, OUTPUT);
  pinMode(dir, OUTPUT);

  pinMode(LED_PIN, OUTPUT);

  #ifdef DEBUG
  Serial.begin(115200);
  #endif
}

void loop() {
  // TODO: calibrate app0 app1 and tps0 min/max values
  // TODO: calibrate pwm output min/max?
  
  // flash LED
  digitalWrite(LED_PIN, (millis() % 1000) < 250);

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
