void setup() {
  // put your setup code here, to run once:
 Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
programOne();
programTwo();

}

void programOne(){
 int a = 23;
 a ++;
 Serial.println(a);
 delay(500);

}

void programTwo(){
for(int i = 20; i >1; i --){
  Serial.println(i);
  delay(500);
}

}