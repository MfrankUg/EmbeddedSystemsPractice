#include "DHT.h"

// Define the ESP32 pin connected to the DHT11 data pin
#define DHTPIN 4     

// Define the type of sensor we are using
#define DHTTYPE DHT11   

// Initialize the DHT sensor
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  // Start the serial communication at 115200 baud rate
  Serial.begin(115200);
  Serial.println("ESP32 + DHT22 Sensor Test");

  // Start the sensor
  dht.begin();
}

void loop() {
  // The DHT22 is a slow sensor. You must wait at least 2 seconds between readings.
  delay(2000);

  // Read humidity
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed. If so, exit the loop early and try again.
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor! Check your wiring.");
    return;
  }

  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);
  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);

  // Print the results to the Serial Monitor
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print("%  |  Temperature: ");
  Serial.print(t);
  Serial.print("°C / ");
  Serial.print(f);
  Serial.print("°F  |  Heat index: ");
  Serial.print(hic);
  Serial.print("°C / ");
  Serial.print(hif);
  Serial.println("°F");
}