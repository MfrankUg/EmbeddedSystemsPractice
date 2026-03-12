void setup() {
  // put your setup code here, to run once:
 Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
 //String S = "Hello_world";
 int a = 24;
 int b = 30;
 /*if(S == "Hello_worlds") {
  Serial.println("correct match.....");
 }
 else{
  Serial.println(" error!, incorrect match");
 }
 delay(500);
 */

if (b > a) {
  Serial.print(b);
  Serial.print(" is greater than ");
  Serial.println(a);
}
else {
  Serial.print(a);
  Serial.print(" is greater than");
  Serial.println(b);
}

 delay(500);
}
