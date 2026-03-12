void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

}

void loop() {
  // put your main code here, to run repeatedly:
  for(int i = 0; i < 10; i++){
    Serial.println(i);
    delay(500);
  }
  
}
