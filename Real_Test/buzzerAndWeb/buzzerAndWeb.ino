#include <WiFi.h>
#include <WebServer.h>

// Your specific network credentials
const char* ssid = "CODEX_PRO_2";
const char* password = "$Frank1122";

// Define the buzzer pin
const int buzzerPin = 23;

// Create a web server running on port 80
WebServer server(80);

// HTML & CSS for the web page
const char* htmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>ESP32 Alarm System</title>
  <style>
    body { font-family: Arial; text-align: center; margin-top: 50px; }
    button { padding: 15px 30px; font-size: 24px; margin: 10px; cursor: pointer; border-radius: 8px;}
    .on { background-color: #d9534f; color: white; border: none; }
    .off { background-color: #5cb85c; color: white; border: none; }
    .on:active { background-color: #c9302c; }
    .off:active { background-color: #449d44; }
  </style>
</head>
<body>
  <h1>Remote IoT Alarm</h1>
  <p><a href="/alarm/on"><button class="on">TURN ALARM ON</button></a></p>
  <p><a href="/alarm/off"><button class="off">TURN ALARM OFF</button></a></p>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW); // Ensure it's off initially

  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  // Check connection status
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected successfully.");
  Serial.print("Go to this IP address in your browser: ");
  Serial.println(WiFi.localIP()); 

  // Define routing: What happens when you visit specific URLs
  server.on("/", []() {
    server.send(200, "text/html", htmlPage);
  });

  server.on("/alarm/on", []() {
    digitalWrite(buzzerPin, HIGH);
    server.send(200, "text/html", htmlPage); 
  });

  server.on("/alarm/off", []() {
    digitalWrite(buzzerPin, LOW);
    server.send(200, "text/html", htmlPage); 
  });

  // Start the server
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  // Listen for incoming clients (like your web browser)
  server.handleClient();
}