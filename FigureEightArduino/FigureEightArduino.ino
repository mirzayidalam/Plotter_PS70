#include <AccelStepper.h>
#include <ESP32Servo.h>

// Motor 1
#define stepPin1 14
#define dirPin1 12

// Motor 2
#define stepPin2 26
#define dirPin2 27

// Pen Switch
#define penSwitch 5

// Servo Pin
Servo servo;
const int pin = 33;

// Limit switch y-axis
#define Y_AXIS_LIMIT_SWITCH_PIN 18

// x-axis limit switch
#define X_AXIS_LIMIT_SWITCH_PIN 19

// Use driver interface (step + dir)
#define motorInterfaceType 1

// Create stepper instances
AccelStepper stepper1(motorInterfaceType, stepPin1, dirPin1);
AccelStepper stepper2(motorInterfaceType, stepPin2, dirPin2);

// Global target position for both motors
long target1 = 5000;
long target2 = 5000;

void setup() {
  Serial.begin(9600);
  // Pen Btn
  pinMode(penSwitch, INPUT_PULLUP);

  // Servo
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  servo.setPeriodHertz(50);
  servo.attach(pin, 500, 2400);

  // Set speed and acceleration for motor 1
  stepper1.setMaxSpeed(3000);      // You can tweak this
  stepper1.setAcceleration(1500);  // And this too
  stepper1.moveTo(target1);

  // Set speed and acceleration for motor 2
  stepper2.setMaxSpeed(1000);
  stepper2.setAcceleration(500);
  stepper2.moveTo(target2);

  // Homing sequence
  pinMode(X_AXIS_LIMIT_SWITCH_PIN, INPUT_PULLUP);
  pinMode(Y_AXIS_LIMIT_SWITCH_PIN, INPUT_PULLUP);
  stepper1.setCurrentPosition(0);
  stepper2.setCurrentPosition(0);
}

bool y_axis_isHome = false;
bool x_axis_isHome = false;

void loop() {
  // Ensure that we are home!
  if (!y_axis_isHome || !x_axis_isHome) {
    // Home the y-axis
    if (!y_axis_isHome) {
      if (digitalRead(Y_AXIS_LIMIT_SWITCH_PIN) == LOW)
      {
        // IT HAS REACHED Y HOME
        Serial.println("Y Home!");
        stepper2.setCurrentPosition(0);
        y_axis_isHome = true;
      } else {
        Serial.println("Not activated.");
        stepper2.move(200);
        stepper2.runToPosition();
      }
    } else {
      if (!x_axis_isHome) {
        if (digitalRead(X_AXIS_LIMIT_SWITCH_PIN) == LOW)
        {
          // IT HAS REACHED Y HOME
          Serial.println("X Home!");
          stepper1.setCurrentPosition(0);
          x_axis_isHome = true;
        } else {
          Serial.println("Not activated.");
          stepper1.move(-200);
          stepper1.runToPosition();
        }
      }
    }
    // Home the x-axis
  } else {
    Serial.println("READY TO GO!");
    // We are home... so do the other stuff!
    // int pen = digitalRead(penSwitch);

    // // servo.write(100);

    // Servo
    // if (pen == 1) {
    //   servo.write(108);
    // } else {
    //   servo.write(90);
    // }

    // Motor 1: reverse direction at end
    // if (stepper1.distanceToGo() == 0) {
    //   target1 = -target1;
    //   stepper1.moveTo(target1);
    // }

    // // Motor 2: same logic
    // if (stepper2.distanceToGo() == 0) {
    //   target2 = -target2;
    //   stepper2.moveTo(target2);
    // }
  
    // // Run both motors
    // stepper1.runToPosition();
    // stepper2.runToPosition();
  }
  
}