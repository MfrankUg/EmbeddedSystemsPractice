#include <WiFi.h>
#include <WiFiMulti.h>
#include <SPI.h>
#include <MFRC522.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>

// WiFi Credentials
WiFiMulti WiFiMulti;

const char* ssid = "Era";
const char* password = "Conrad2024";

//const char* ssid = "CORE BYTES LINK";
//const char* password = "corebytes!";

// Google Sheets Webhook URL
const char* googleSheetUrl = "https://script.google.com/macros/s/AKfycbzjCU2Liv_xKUleKgCURZa7bza7-V_a5IZ2RhOjGmoWkqpcB9cT1WHv4B6r7KtqAoGYyw/exec";

// Pins
#define SS_PIN         5
#define RST_PIN        27
#define BUZZER_PIN     26
#define IR_SENSOR_PIN  34
#define BUTTON_PIN     33     // Push button with external pull-up
#define SERVO_PIN      25     // PWM-capable pin for servo

MFRC522 rfid(SS_PIN, RST_PIN);
WiFiClient client;
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo gateServo;

String studentID = "";
String bookID = "";
bool isStudentScanned = false;
bool isBorrowing = true; // Toggle
String lastBorrower = "";
bool hasPendingReturn = false;
bool isGateOpen = false;

// --- Student & Book Records ---
struct Student {
  String id;
  String name;
};

Student students[] = {
  {"1BC835F", "CONRAD GUMISIRIZA"},
  {"3647E2AD", "FLORENE NAMYALO"},
  {"D931314", "PETER KALULU"},
  {"M3N4O5P6", "BOB JOHNSON"}
};

struct Book {
  String id;
  String title;
};

Book books[] = {
  {"7390DEE", "Intro to Robotics"},
  {"B5C6D7E8", "Microcontrollers"},
  {"F9G0H1I2", "Machine Learning"},
  {"93B84E", "Art of Electronics"}
};

bool doorOpen = false;
unsigned long doorOpenedTime = 0;



void setup() {
  Serial.begin(115200);
  delay(1000);  // Allow time for Serial and peripherals to stabilize

  SPI.begin();
  rfid.PCD_Init();
  lcd.init();
  lcd.backlight();

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(IR_SENSOR_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP); // External pull-up assumed
  digitalWrite(BUZZER_PIN, LOW);

  gateServo.setPeriodHertz(50);   // Standard frequency for servos
  gateServo.attach(SERVO_PIN);
  gateServo.write(0); // Initially closed

  WiFiMulti.addAP("Era", "Conrad2024");

  lcd.setCursor(0, 0);
  lcd.print("Connecting WiFi...");

  //WiFi.begin(ssid, password);

  // Print progress while connecting
  Serial.print("Connecting to WiFi");

  while (WiFiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  delay(500);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Scan ID Card...");

  /* gateServo.write(90);
    delay(2000);
    gateServo.write(0); */

}


void loop() {
  // IR sensor check
  if (digitalRead(IR_SENSOR_PIN) == LOW) {
    buzz();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Alert: Movement!");
    lcd.setCursor(0, 1);
    lcd.print("Check Entry/Exit");
    delay(2000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Scan ID Card...");
  }

  // --- Push Button Gate Toggle Logic with Debouncing ---
  static bool lastStableState = HIGH;
  static bool lastReadState = HIGH;
  static unsigned long lastDebounceTime = 0;
  const unsigned long debounceDelay = 50;

  bool currentRead = digitalRead(BUTTON_PIN);

  if (currentRead != lastReadState) {
    lastDebounceTime = millis();
    lastReadState = currentRead;
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (lastStableState != currentRead) {
      lastStableState = currentRead;

      if (currentRead == LOW) {
        Serial.println("Button Press Detected");
        toggleDoor(); // Toggle the servo
      }
    }
  }

  // --- Auto-close logic (after 10 seconds) ---
  if (doorOpen && (millis() - doorOpenedTime >= 10000)) {
    gateServo.write(0); // Close the door
    doorOpen = false;
    Serial.println("Auto-closed door after timeout");

    Serial.println("Door Closed");

    lcd.setCursor(0, 0);
    lcd.print("Door Closed...");
    delay(2000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Scan ID Card...");

    return;
  }


  // --- RFID Scanning ---
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;
  }

  String tagID = getTagID();
  Serial.println("Scanned Tag: " + tagID);
  buzz();

  if (!isStudentScanned) {
    studentID = tagID;
    String studentName = getStudentName(studentID);

    if (studentName == "Unknown Student") {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("ID Not Registered");
      lcd.setCursor(0, 1);
      lcd.print("Add to System");
      delay(3000);
      lcd.clear();
      lcd.print("Scan ID Card...");
      return;
    }

    // Restrict borrowing if not returned
    if (hasPendingReturn && studentID == lastBorrower) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Return Book First");
      lcd.setCursor(0, 1);
      lcd.print("Cannot Borrow");
      delay(3000);
      lcd.clear();
      lcd.print("Scan ID Card...");
      return;
    }

    isStudentScanned = true;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Student: ");
    lcd.setCursor(0, 1);
    lcd.print(studentName);

    delay(2000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Scan Book...");
  } else {
    bookID = tagID;
    String bookTitle = getBookTitle(bookID);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Book: ");
    lcd.setCursor(0, 1);
    lcd.print(bookTitle);

    sendToGoogleSheets(studentID, bookID);
    isStudentScanned = false;

    delay(3000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Scan ID Card...");
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}





// --- Helper Functions ---

String getTagID() {
  String tagID = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    tagID += String(rfid.uid.uidByte[i], HEX);
  }
  tagID.toUpperCase();
  return tagID;
}

String getStudentName(String id) {
  for (auto& student : students) {
    if (student.id == id) return student.name;
  }
  return "Unknown Student";
}

String getBookTitle(String id) {
  for (auto& book : books) {
    if (book.id == id) return book.title;
  }
  return "Unknown Book";
}

void sendToGoogleSheets(String student, String book) {
  String studentName = getStudentName(student);
  String bookTitle = getBookTitle(book);
  String status = isBorrowing ? "Borrowed" : "Returned";

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(googleSheetUrl);
    http.addHeader("Content-Type", "application/json");

    StaticJsonDocument<256> jsonDoc;
    jsonDoc["studentID"] = student;
    jsonDoc["studentName"] = studentName;
    jsonDoc["bookID"] = book;
    jsonDoc["bookTitle"] = bookTitle;
    jsonDoc["timestamp"] = getTime();
    jsonDoc["status"] = status;

    String jsonData;
    serializeJson(jsonDoc, jsonData);

    Serial.println("Sending JSON to Google Sheets: " + jsonData);

    int httpResponseCode = http.POST(jsonData);
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Google Sheets Response: " + response);
    } else {
      Serial.println("Failed to send data to Google Sheets");
    }
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }

  // Update borrow status
  if (isBorrowing) {
    lastBorrower = student;
    hasPendingReturn = true;
  } else {
    hasPendingReturn = false;
    lastBorrower = "";
  }

  isBorrowing = !isBorrowing;
}

String getTime() {
  return "2025-04-15 13:00:00"; // Update with RTC or NTP if needed
}

void buzz() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(1000);
  digitalWrite(BUZZER_PIN, LOW);
}

void toggleDoor() {
  if (!doorOpen) {
    gateServo.write(90); // Open the door
    doorOpenedTime = millis(); // Start the timer
    doorOpen = true;
    Serial.println("Door Opened");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Door Opened...");
  } else {
    gateServo.write(0); // Close the door
    doorOpen = false;
    Serial.println("Door Closed");
    /*  lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Door Closed...");   */
  }
}
