#define LORA_V 14

void setup() {
  Serial.begin(115200);
  
  pinMode(LORA_V, OUTPUT);
  
  delay(1000);
}

void loop() {
  Serial.println("LoRA On");
  digitalWrite(LORA_V, HIGH);
  delay(3000);
  
  Serial.println("LoRA Off");
  digitalWrite(LORA_V, LOW);
  delay(3000);
}
