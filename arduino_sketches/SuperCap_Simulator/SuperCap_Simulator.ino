#define VOLTAGE_MAX_HIGH 2.72
#define VOLTAGE_MAX_LOW 2.4

#define VOLTAGE_SEM_MAX_HIGH 2.4
#define VOLTAGE_SEM_MAX_LOW 2.2

#define VOLTAGE_MED_HIGH 2.2
#define VOLTAGE_MED_LOW 2.0

#define VOLTAGE_LOW_HIGH 2.0
#define VOLTAGE_LOW_LOW 1.7

int sensorPin = A4;    // select the input pin for the potentiometer
const byte amount_of_leds = 4;
int ledPins[amount_of_leds] = {9, 3, 6, 5};      // select the pin for the LED
int led_en[amount_of_leds] = {0, 0, 0, 0};      // enable or disable a LED

int sensorValue = 0;  // variable to store the value coming from the sensor

void write_leds();
float determine_voltage(int digital_value);
void check_led_state(float voltage);

void setup() {

  Serial.begin(9600);
  
  for(byte i = 0; i < amount_of_leds; i++)
  {
    pinMode(ledPins[i], OUTPUT);
  }
  pinMode(sensorPin, INPUT);
}

void loop() {
  sensorValue = analogRead(sensorPin);

  //Determine leds to be on
  
  float voltage =  determine_voltage(sensorValue);
  Serial.print("analog: ");
  Serial.print(sensorValue);
  Serial.print(", v: ");
  Serial.println(voltage);
  check_led_state(voltage);
  write_leds();
  
  delay(100);
}

float determine_voltage(int digital_value)
{
  float conversion_factor = 0.003222656; //V/div
  return digital_value*conversion_factor;
}

void write_leds()
{
  for(byte i = 0; i < amount_of_leds; i++)
  {
    digitalWrite(ledPins[i], led_en[i]);
  } 
}

void check_led_state(float voltage){
  if(voltage >= VOLTAGE_MAX_LOW)
  {
    led_en[0] = 0x01;
    led_en[1] = 0x01;
    led_en[2] = 0x01;
    led_en[3] = 0x01;
  }
  else if(voltage < VOLTAGE_SEM_MAX_HIGH && voltage >= VOLTAGE_SEM_MAX_LOW)
  {
    led_en[0] = 0x01;
    led_en[1] = 0x01;
    led_en[2] = 0x01;
    led_en[3] = 0x00;
  }
  else if(voltage < VOLTAGE_MED_HIGH && voltage >= VOLTAGE_MED_LOW)
  {
    led_en[0] = 0x01;
    led_en[1] = 0x01;
    led_en[2] = 0x00;
    led_en[3] = 0x00;
  }
    else if(voltage < VOLTAGE_LOW_HIGH)
  {
    led_en[0] = 0x01;
    led_en[1] = 0x00;
    led_en[2] = 0x00;
    led_en[3] = 0x00;
  }
}
