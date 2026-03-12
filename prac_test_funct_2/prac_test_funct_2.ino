void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

}

void loop() {
  // put your main code here, to run repeatedly:
 myFunction(12,23);
 delay(1000);
}

void myFunction(int a, int b) {
  int c = a + b;
  Serial.print("the sum of ");
  Serial.print(a);
  Serial.print(" + ");
  Serial.println(b);
  Serial.println(c);

}

