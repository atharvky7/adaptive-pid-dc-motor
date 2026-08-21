#include <Wire.h>
#include <MPU6050.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>

// Pins
#define ENA_PIN 4
#define IN1_PIN 2
#define IN2_PIN 5
#define ENCODER_A_PIN 15
#define ENCODER_B_PIN 38
#define SCL_PIN 9
#define SDA_PIN 8

// Wi-Fi credentials
// Replace with local credentials before use.
const char *ssid = "YOUR_WIFI_SSID";
const char *password = "YOUR_WIFI_PASSWORD";

// Global variables
MPU6050 mpu;
volatile int encoderPos = 0;
float motorRPM = 0;
float targetRPM = 0;
unsigned long lastEncoderTime = 0;
unsigned long lastLoopTime = 0;
double pidInput, pidOutput, pidSetpoint;
double Kp = 1.0, Ki = 0.5, Kd = 0.2;
int lastError = 0;
int integral = 0;
bool motorRunning = false;

// Web server
AsyncWebServer server(80);

// Encoder ISR
void IRAM_ATTR encoderISR()
{
  encoderPos++;
}

// PID calculation
double calculatePID(double input)
{
  unsigned long currentTime = millis();
  unsigned long timeChange = currentTime - lastLoopTime;

  if (timeChange >= 100) {
    double error = pidSetpoint - input;
    integral += error * timeChange;
    int derivative = (error - lastError) / timeChange;

    pidOutput = Kp * error + Ki * integral + Kd * derivative;
    lastError = error;
    lastLoopTime = currentTime;
  }

  return pidOutput;
}

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Initialize MPU6050
  Wire.begin(SDA_PIN, SCL_PIN);
  mpu.initialize();

  // Encoder
  pinMode(ENCODER_A_PIN, INPUT_PULLUP);
  pinMode(ENCODER_B_PIN, INPUT_PULLUP);
  attachInterrupt(
    digitalPinToInterrupt(ENCODER_A_PIN),
    encoderISR,
    RISING
  );

  // Motor driver
  pinMode(ENA_PIN, OUTPUT);
  pinMode(IN1_PIN, OUTPUT);
  pinMode(IN2_PIN, OUTPUT);

  // Web dashboard
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String html = "<html><body><h1>Motor Control Dashboard</h1>";
    html += "<p>Current RPM: <span id='currentRPM'>" + String(motorRPM) + "</span></p>";
    html += "<p>Target RPM: <input type='number' id='targetRPM' value='" + String(targetRPM) + "'></p>";
    html += "<button onclick='updateRPM()'>Set RPM</button><br><br>";

    if (motorRunning) {
      html += "<button onclick='stopMotor()'>Stop Motor</button>";
    } else {
      html += "<button onclick='startMotor()'>Start Motor</button>";
    }

    html += "<h3>Real-Time PID Parameters</h3>";
    html += "<p>Kp: <span id='Kp'>" + String(Kp) + "</span></p>";
    html += "<p>Ki: <span id='Ki'>" + String(Ki) + "</span></p>";
    html += "<p>Kd: <span id='Kd'>" + String(Kd) + "</span></p>";

    html += "<script>";
    html += "function updateRPM(){var rpm = document.getElementById('targetRPM').value;fetch('/setRPM?rpm='+rpm);}";
    html += "function startMotor(){fetch('/startMotor');}";
    html += "function stopMotor(){fetch('/stopMotor');}";
    html += "function fetchRPM(){fetch('/getRPM').then(response => response.json()).then(data => {";
    html += "document.getElementById('currentRPM').innerText = data.rpm;});}";
    html += "function fetchPID(){fetch('/getPID').then(response => response.json()).then(data => {";
    html += "document.getElementById('Kp').innerText = data.Kp;";
    html += "document.getElementById('Ki').innerText = data.Ki;";
    html += "document.getElementById('Kd').innerText = data.Kd;});}";
    html += "setInterval(fetchRPM, 1000);";
    html += "setInterval(fetchPID, 1000);";
    html += "</script>";
    html += "</body></html>";

    request->send(200, "text/html", html);
  });

  server.on("/setRPM", HTTP_GET, [](AsyncWebServerRequest *request){
    String rpm = request->getParam("rpm")->value();
    targetRPM = rpm.toFloat();
    pidSetpoint = targetRPM;
    request->send(200, "text/plain", "RPM set to: " + rpm);
  });

  server.on("/startMotor", HTTP_GET, [](AsyncWebServerRequest *request){
    motorRunning = true;
    targetRPM = 175;
    pidSetpoint = targetRPM;
    request->send(200, "text/plain", "Motor started");
  });

  server.on("/stopMotor", HTTP_GET, [](AsyncWebServerRequest *request){
    motorRunning = false;
    targetRPM = 0;
    pidSetpoint = targetRPM;
    request->send(200, "text/plain", "Motor stopped");
  });

  server.on("/getRPM", HTTP_GET, [](AsyncWebServerRequest *request){
    String json = "{"rpm": " + String(motorRPM) + "}";
    request->send(200, "application/json", json);
  });

  server.on("/getPID", HTTP_GET, [](AsyncWebServerRequest *request){
    String json = "{"Kp": " + String(Kp) + ", "Ki": " + String(Ki) + ", "Kd": " + String(Kd) + "}";
    request->send(200, "application/json", json);
  });

  server.begin();
}

void loop() {
  unsigned long currentMillis = millis();

  // Calculate motor RPM every 1 second
  if (currentMillis - lastEncoderTime >= 1000) {
    motorRPM = (encoderPos / 2.0);
    encoderPos = 0;
    lastEncoderTime = currentMillis;

    pidInput = motorRPM;
    int motorSpeed = calculatePID(pidInput);

    // Adaptive gain scheduling
    if (motorRPM < targetRPM) {
      Kp = 1.5;
      Ki = 0.8;
      Kd = 0.3;
    } else {
      Kp = 1.0;
      Ki = 0.5;
      Kd = 0.2;
    }

    motorSpeed = constrain(motorSpeed, 0, 255);
    analogWrite(ENA_PIN, motorSpeed);
    digitalWrite(IN1_PIN, HIGH);
    digitalWrite(IN2_PIN, LOW);

    Serial.print("Motor RPM: ");
    Serial.print(motorRPM);
    Serial.print(" | Target RPM: ");
    Serial.print(targetRPM);
    Serial.print(" | Kp: ");
    Serial.print(Kp);
    Serial.print(" | Ki: ");
    Serial.print(Ki);
    Serial.print(" | Kd: ");
    Serial.println(Kd);
  }

  delay(100);
}
