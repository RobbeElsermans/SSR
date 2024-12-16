#define LED_G 5
#define LED_Y 18
#define LED_R 19

#define POT   4
uint16_t IN_VOL = 0; // 175 - 3815
float   BAT_VOL = 0; // 0V0 - 3V3

void setup() {
  Serial.begin(115200);
  
  pinMode(LED_G, OUTPUT);
  pinMode(LED_Y, OUTPUT);
  pinMode(LED_R, OUTPUT);
  pinMode(POT, INPUT);
  
  delay(1000);
}

void loop() {
  IN_VOL = analogRead(POT);
  IN_VOL = map(IN_VOL, 175, 3815, 0, 3300);
  BAT_VOL = IN_VOL/1000.0;
  Serial.print("Battery level:\t");
  Serial.print(BAT_VOL);
  Serial.println("V");

  if (BAT_VOL > 2.8) {
    digitalWrite(LED_G, HIGH);
    digitalWrite(LED_Y, HIGH);
    digitalWrite(LED_R, HIGH);
  } else if (BAT_VOL > 1.8) {
    digitalWrite(LED_G, LOW);
    digitalWrite(LED_Y, HIGH);
    digitalWrite(LED_R, HIGH);
  }  else if (BAT_VOL > 0.8) {
    digitalWrite(LED_G, LOW);
    digitalWrite(LED_Y, LOW);
    digitalWrite(LED_R, HIGH);
  }  else {
    digitalWrite(LED_G, LOW);
    digitalWrite(LED_Y, LOW);
    digitalWrite(LED_R, LOW);
  } 

  delay(1000);
}
