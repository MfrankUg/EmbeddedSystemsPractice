#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// --- Pin Definitions ---
#define DHTPIN 4        // DHT22 Data Pin
#define BUZZER_PIN 13   // The safe pin we chose for the Buzzer

#define DHTTYPE DHT22   
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2); 

// --- Alarm Threshold ---
const float TEMP_ALARM_LIMIT = 23.0; // Set your alarm temperature here!

void setup() {
  Serial.begin(115200); 

  // Initialize the Buzzer pin as an OUTPUT
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW); // Ensure it starts turned off

  lcd.init();
  lcd.backlight();
  dht.begin();
  
  lcd.setCursor(0, 0);
  lcd.print("System Booting..");
  delay(2000); 
  lcd.clear();
}

void loop() {
  float humidity = dht.readHumidity();
  float tempC = dht.readTemperature();

  if (isnan(humidity) || isnan(tempC)) {
    lcd.setCursor(0, 0);
    lcd.print("Sensor Error!   ");
    Serial.println("Failed to read from DHT sensor!"); 
    return; 
  }

  // --- Alarm Logic ---
  if (tempC > TEMP_ALARM_LIMIT) {
    digitalWrite(BUZZER_PIN, HIGH); // Turn buzzer ON
    Serial.println("ALARM! Temperature too high!");
  } else {
    digitalWrite(BUZZER_PIN, LOW);  // Turn buzzer OFF
  }

  // --- Print to Serial Monitor ---
  Serial.print("Temp: ");
  Serial.print(tempC);
  Serial.print(" °C | Hum: ");
  Serial.print(humidity);
  Serial.println(" %");

  // --- Print to LCD ---
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(tempC);
  lcd.print(" C  ");

  lcd.setCursor(0, 1);
  lcd.print("Hum:  ");
  lcd.print(humidity);
  lcd.print(" %  ");

  delay(2000); 
}