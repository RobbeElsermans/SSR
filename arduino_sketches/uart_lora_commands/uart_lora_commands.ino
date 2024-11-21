#include<SoftwareSerial.h>

SoftwareSerial SUART(2, 3); //SRX = 2, STX = 3

void setup() {
  Serial.begin(9600);
  SUART.begin(9600);

  // Send connection check
  Serial.println("Send AT-commands");
  SUART.print("AT");

  // Read response
  Serial.println("Receive AT-response");
  if (SUART.available() > 0) {
    char ch = SUART.read();
    Serial.print(ch);
  }
  Serial.println();
  Serial.println("Received AT-response");
}

void loop() { }
