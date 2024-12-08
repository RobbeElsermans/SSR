#define UP 2
#define DOWN 6
#define LEFT 3
#define RIGHT 5
#define PRESS 4

void setup(void) {
  pinMode(UP, OUTPUT);
  pinMode(DOWN, OUTPUT);
  pinMode(LEFT, OUTPUT);
  pinMode(RIGHT, OUTPUT);
  pinMode(PRESS, OUTPUT);

  digitalWrite(UP, HIGH);
  digitalWrite(DOWN, HIGH);
  digitalWrite(LEFT, HIGH);
  digitalWrite(RIGHT, HIGH);
  digitalWrite(PRESS, HIGH);

  digitalWrite(UP, LOW);     delay(1000); digitalWrite(UP, HIGH);     delay(1000);
  digitalWrite(DOWN, LOW);   delay(1000); digitalWrite(DOWN, HIGH);   delay(1000);
  digitalWrite(LEFT, LOW);   delay(1000); digitalWrite(LEFT, HIGH);   delay(1000);
  digitalWrite(RIGHT, LOW);  delay(1000); digitalWrite(RIGHT, HIGH);  delay(1000);
  digitalWrite(PRESS, LOW);  delay(1000); digitalWrite(PRESS, HIGH);  delay(1000);
}

void loop() {
  // Empty loop
}
