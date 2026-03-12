int ledPin = 23;
void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);
pinMode(ledPin, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(ledPin, HIGH);
  Serial.println("Lights ON");
  delay(1000);
  
  digitalWrite(ledPin, LOW);
  Serial.println("Lights OFF");
  delay(1000);

}
