void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

}

void loop() {
  // put your main code here, to run repeatedly:
  int num1 = 1;
  char val = 'a';
  float num2 = 0.25;
  String val2 = "Hello_world";
  Serial.println(num1);
  Serial.println(num2);
  Serial.println(val);
  Serial.println(val2);

  delay(500);
  


}
