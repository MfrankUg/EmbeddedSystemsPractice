#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2); 

void setup() {
  // --- ADDED: Start the Serial Monitor at 115200 baud ---
  Serial.begin(115200); 
  Serial.println("Starting up...");

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
    // --- ADDED: Print error to computer ---
    Serial.println("Failed to read from DHT sensor!"); 
    return; 
  }

  // --- ADDED: Print readings to computer ---
  Serial.print("Temperature: ");
  Serial.print(tempC);
  Serial.print(" °C | Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  // LCD Updates
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