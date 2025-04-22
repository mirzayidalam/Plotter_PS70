// ESP32 FIRMWARE
// This runs on your ESP32 to control the pen plotter with A4988 drivers

#include <WiFi.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <AccelStepper.h>
#include <ESP32Servo.h>


// Network credentials
const char* ssid = "MAKERSPACE";
const char* password = "12345678";

// Define stepper motor connections and motor interface type
#define X_STEP_PIN 14
#define X_DIR_PIN 12
#define X_ENABLE_PIN 13
#define Y_STEP_PIN 26
#define Y_DIR_PIN 27
#define Y_ENABLE_PIN 25
#define PEN_SERVO_PIN 33

// Define motor steps per revolution and microstepping
#define STEPS_PER_REV 200  // Change based on your specific stepper
#define MICROSTEPS 16      // Based on A4988 configuration
#define MM_PER_REV 8       // Belt pitch * pulley teeth or lead screw pitch

// Calculate steps per mm
#define STEPS_PER_MM (STEPS_PER_REV * MICROSTEPS / MM_PER_REV)

// Define motor speeds
#define MAX_SPEED 1000
#define ACCELERATION 500

// Define drawing area (A4 paper in mm)
#define MAX_X 297
#define MAX_Y 210

// Define pen positions
#define PEN_UP_ANGLE 80
#define PEN_DOWN_ANGLE 100

// Initialize steppers and servo
AccelStepper stepperX(AccelStepper::DRIVER, X_STEP_PIN, X_DIR_PIN);
AccelStepper stepperY(AccelStepper::DRIVER, Y_STEP_PIN, Y_DIR_PIN);
Servo penServo;

// WebSocket server
WebSocketsServer webSocket = WebSocketsServer(81);

bool isPenDown = false;

void setup() {
  Serial.begin(115200);
  
  // Configure pin modes
  pinMode(X_ENABLE_PIN, OUTPUT);
  pinMode(Y_ENABLE_PIN, OUTPUT);
  digitalWrite(X_ENABLE_PIN, LOW); // Enable X stepper (LOW = enabled for A4988)
  digitalWrite(Y_ENABLE_PIN, LOW); // Enable Y stepper
  
  // Configure servos
  penServo.attach(PEN_SERVO_PIN);
  penServo.write(PEN_UP_ANGLE); // Pen up to start
  
  // Configure steppers
  stepperX.setMaxSpeed(MAX_SPEED);
  stepperX.setAcceleration(ACCELERATION);
  stepperY.setMaxSpeed(MAX_SPEED);
  stepperY.setAcceleration(ACCELERATION);
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  // Start WebSocket server
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  Serial.println("WebSocket server started");
  
  // Home the machine
  homeAxes();
}

void loop() {
  webSocket.loop();
  
  // Run the stepper motors
  stepperX.run();
  stepperY.run();
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
      
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d\n", num, ip[0], ip[1], ip[2], ip[3]);
      }
      break;
      
    case WStype_TEXT:
      {
        // Parse JSON
        StaticJsonDocument<200> doc;
        DeserializationError error = deserializeJson(doc, payload);
        
        if (error) {
          Serial.print("deserializeJson() failed: ");
          Serial.println(error.c_str());
          return;
        }
        
        // Process command
        const char* command = doc["command"];
        
        if (strcmp(command, "moveTo") == 0) {
          float x = doc["x"];
          float y = doc["y"];
          moveToPosition(x, y);
        }
        else if (strcmp(command, "penUp") == 0) {
          setPenUp();
        }
        else if (strcmp(command, "penDown") == 0) {
          setPenDown();
        }
        else if (strcmp(command, "home") == 0) {
          homeAxes();
        }
      }
      break;
  }
}

void moveToPosition(float x, float y) {
  // Convert mm to steps
  long xSteps = x * STEPS_PER_MM;
  long ySteps = y * STEPS_PER_MM;
  
  // Constrain to valid range
  xSteps = constrain(xSteps, 0, MAX_X * STEPS_PER_MM);
  ySteps = constrain(ySteps, 0, MAX_Y * STEPS_PER_MM);
  
  // Move to position
  stepperX.moveTo(xSteps);
  stepperY.moveTo(ySteps);
}

void setPenUp() {
  if (isPenDown) {
    penServo.write(PEN_UP_ANGLE);
    isPenDown = false;
    delay(200); // Give time for pen to lift
  }
}

void setPenDown() {
  if (!isPenDown) {
    penServo.write(PEN_DOWN_ANGLE);
    isPenDown = true;
    delay(200); // Give time for pen to lower
  }
}

void homeAxes() {
  // Implement homing depending on your hardware setup
  // This is a simple version that just moves to 0,0
  setPenUp();
  stepperX.setCurrentPosition(0);
  stepperY.setCurrentPosition(0);
  stepperX.moveTo(0);
  stepperY.moveTo(0);
}