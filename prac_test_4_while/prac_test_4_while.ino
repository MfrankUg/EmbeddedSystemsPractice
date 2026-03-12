void setup() {
  // put your setup code here, to run once:
  Serial.begin(115000);

}

void loop() {
  // put your main code here, to run repeatedly:
  int i = 0;
  while(i<=10){
    Serial.println(i);
    delay(1000);
    i++;
  }
  
}
