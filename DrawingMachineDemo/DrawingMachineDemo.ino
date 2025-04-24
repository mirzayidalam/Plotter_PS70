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
int MAX_X = 25000;
int MAX_Y = 25000;

long x_delta = 5000;
long y_delta = 5000;
long middle_init = MAX_X/2;

class MyClass {
  private:
    // Private member variables
    int privateVar;
    
  public:
    // Constructor
    MyClass();
    
    // Constructor with parameters
    MyClass(int initialValue);
    
    // Public methods
    void setVar(int value);
    int getVar();
    
    // Other public methods
    void doSomething();
};

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

  // Set speed and acceleration for motor 2
  stepper2.setMaxSpeed(1000);
  stepper2.setAcceleration(500);

  // Homing sequence
  pinMode(X_AXIS_LIMIT_SWITCH_PIN, INPUT_PULLUP);
  pinMode(Y_AXIS_LIMIT_SWITCH_PIN, INPUT_PULLUP);
  stepper1.setCurrentPosition(0);
  stepper2.setCurrentPosition(0);
}

bool y_axis_isHome = false;
bool x_axis_isHome = false;


bool isSetUp = false;

void loop() {
  // Ensure that we are home!
  if (!y_axis_isHome || !x_axis_isHome) {
    // Home the y-axis
    if (!y_axis_isHome) {
      if (digitalRead(Y_AXIS_LIMIT_SWITCH_PIN) == LOW)
      {
        // IT HAS REACHED Y HOME
        Serial.println("Y Home!");
        // Now give a bit of space
        stepper2.move(-10);
        stepper2.setCurrentPosition(0);
        y_axis_isHome = true;

      } else {
        Serial.println("Not activated.");
        stepper2.move(400);
        stepper2.runToPosition();
      }
    } else {
      if (!x_axis_isHome) {
        if (digitalRead(X_AXIS_LIMIT_SWITCH_PIN) == LOW)
        {
          // IT HAS REACHED Y HOME
          Serial.println("X Home!");
          // Give a bit of space
          stepper1.move(10);
          stepper1.setCurrentPosition(0);
          x_axis_isHome = true;
        } else {
          Serial.println("Not activated.");
          stepper1.move(-400);
          stepper1.runToPosition();
        }
      }
    }
  } else {
    // Move to middle of the drawing machine
    if (!isSetUp) {
      stepper1.moveTo(middle_init);
      stepper2.moveTo(-1 * middle_init);

      stepper1.runToPosition();
      stepper2.runToPosition();
      isSetUp = true;
    } else {
      // DO A FIGURE 8
      // int pen = digitalRead(penSwitch);

      // // Servo
      // if (pen == 1) {
      //   servo.write(108);
      // } else {
      //   servo.write(90);
      // }

      // // Motor 1: reverse direction at end
      // if (stepper1.distanceToGo() == 0) {
      //   // target1 = -target1;
      //   x_delta = -x_delta;
      //   stepper1.moveTo(middle_init + x_delta);
      // }

      // // Motor 2: same logic
      // if (stepper2.distanceToGo() == 0) {
      //   y_delta = -y_delta;
      //   stepper2.moveTo(-middle_init + y_delta);
      // }
    
      // // Run both motors
      // stepper1.run();
      // stepper2.run();


      // Do a circle!!
      static float theta = 0.0;
      static float radius = 5000; // You can tweak this
      static float stepSize = 0.02; // Smaller = smoother circle

      int pen = digitalRead(penSwitch);
      if (pen == 1) {
        servo.write(108); // Pen down
      } else {
        servo.write(90);  // Pen up
      }

      long x = middle_init + radius * cos(theta);
      long y = -middle_init + radius * sin(theta);

      stepper1.moveTo(x);
      stepper2.moveTo(y);

      stepper1.runToPosition();
      stepper2.runToPosition();

      theta += stepSize;
      if (theta >= 2 * PI) {
        theta = 0.0;
      }
    }
  }
}