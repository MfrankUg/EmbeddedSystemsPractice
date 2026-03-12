const int pinBuzzer = 23;

void setup() {
pinMode(pinBuzzer,OUTPUT);
Serial.begin(115200);
}

void loop(){
digitalWrite(pinBuzzer,HIGH);
Serial.println("Sound On");
delay(1000);

digitalWrite(pinBuzzer,LOW);
Serial.println("Sound Off");
delay(1000);
}



