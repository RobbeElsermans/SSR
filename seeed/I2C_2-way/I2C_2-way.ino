#include <Wire.h>

void receive_event(int howMany); //I2C callback
void request_event();

volatile uint8_t counter = 0;

void setup() {
  // put your setup code here, to run once:
  Wire.begin(0x12);
  Serial.begin(115200);
  while(!Serial) delay(10);

  Wire.onReceive(receive_event); // register event
  Wire.onRequest(request_event);
}

void receive_event(int howMany)
{
  while(Wire.available()){
    counter =  Wire.read();
    Serial.printf("Data recieved: %d and size%d\r\n", counter, howMany);
    
  }
  counter++;
}

void request_event()
{
  Serial.printf("Data requested:");
  Wire.write(counter);
}

void loop() {
  // put your main code here, to run repeatedly:

}
