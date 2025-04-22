#include <AccelStepper.h>

// Motor 1
#define stepPin1 14
#define dirPin1 12

// Motor 2
#define stepPin2 26
#define dirPin2 27

// Use driver interface (step + dir)
#define motorInterfaceType 1

// Create stepper instances
AccelStepper stepper1(motorInterfaceType, stepPin1, dirPin1);
AccelStepper stepper2(motorInterfaceType, stepPin2, dirPin2);

// Global target position for both motors
long target1 = 10000;
long target2 = 10000;

void setup() {
  // Set speed and acceleration for motor 1
  stepper1.setMaxSpeed(3000);      // You can tweak this
  stepper1.setAcceleration(1500);  // And this too
  stepper1.moveTo(target1);

  // Set speed and acceleration for motor 2
  stepper2.setMaxSpeed(1000);
  stepper2.setAcceleration(500);
  stepper2.moveTo(target2);
}

void loop() {
  // Motor 1: reverse direction at end
  if (stepper1.distanceToGo() == 0) {
    target1 = -target1;
    stepper1.moveTo(target1);
  }

  // Motor 2: same logic
  if (stepper2.distanceToGo() == 0) {
    target2 = -target2;
    stepper2.moveTo(target2);
  }

  // Run both motors
  stepper1.run();
  stepper2.run();
}