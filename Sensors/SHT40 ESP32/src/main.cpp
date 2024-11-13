#include <Wire.h>
#include <SensirionI2cSht4x.h>

SensirionI2cSht4x sht40;

        #define LED_BUILTIN 2  // GPIO2 for Wemos D1 Mini ESP32

void setup() {
    Wire.begin();
    Serial.begin(9600);

        pinMode(LED_BUILTIN, OUTPUT);  // Set LED pin as output
    
    // Initialize the SHT40 sensor with I2C address 0x44
    sht40.begin(Wire, 0x44);

    uint16_t error;
    uint32_t serialNum;
    error = sht40.serialNumber(serialNum);
    if (error) {
        Serial.print("Error retrieving serial number, error code: ");
        Serial.println(error);
    } else {
        Serial.print("SHT40 sensor found! Serial number: ");
        Serial.println(serialNum);
    }
}

void loop() {
    float temperature, humidity;
        digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED on
        delay(500);                       // Wait for 500 milliseconds
    // Measure in high precision mode (you can replace this with measureLowPrecision for lower power consumption)
    uint16_t error = sht40.measureHighPrecision(temperature, humidity);
    if (error) {
        Serial.print("Error measuring: ");
        Serial.println(error);
    } else {
        Serial.print("Temperature: ");
        Serial.print(temperature);
        Serial.println(" °C");

        Serial.print("Humidity: ");
        Serial.print(humidity);
        Serial.println(" % RH");

        digitalWrite(LED_BUILTIN, LOW);   // Turn the LED off
        delay(500);
    }

    delay(2000);  // Wait 2 seconds before the next measurement
}
