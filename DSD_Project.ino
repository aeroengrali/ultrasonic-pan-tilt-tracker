// Include the Servo library, which provides functions to control hobby servo motors.
#include <Servo.h>

//
// Pin Definitions
// Using #define creates named constants for pin numbers. This makes the code
// easier to read
// and modify. If you need to change a pin, you only have to do it in one
// place.
#define TRIG1 4      // The digital pin connected to the Trigger (TRIG) pin of
                     // Ultrasonic Sensor 1.
#define ECHO1 5      // The digital pin connected to the Echo (ECHO) pin of
                     // Ultrasonic Sensor 1.
#define TRIG2 6      // The digital pin for Trigger on Sensor 2.
#define ECHO2 7      // The digital pin for Echo on Sensor 2.
#define TRIG3 2      // The digital pin for Trigger on Sensor 3.
#define ECHO3 3      // The digital pin for Echo on Sensor 3.
#define LED1_PIN 12  // The pin for the status LED. It lights up when an
                     // object is detected.

// Create two Servo objects.
// These objects will be used to send control signals to the servo motors.
Servo servo1;        // This servo will control the pan (horizontal, left-right)
                     // movement.
Servo servo2;        // This servo will control the tilt (vertical, up-down)
                     // movement.

//
// Global Variables
// These variables are accessible from any function in the sketch..
long duration1, duration2, duration3; // Variables to store the pulse duration
                                      // (in microseconds) from each sensor's echo pin.
long distance1, distance2, distance3; // Variables to store the calculated
                                      // distance (in centimeters) for each sensor.

//
// Configuration parameters
// These variables store settings for the system's behavior.
int range = 20;               // The maximum distance (in cm) to consider an
                              // object "close".
int servo1_upperlimit = 180;  // The maximum angle (in degrees) for the pan
                              // servo.
int servo2_upperlimit = 140;  // The maximum angle for the tilt servo.
                              // The minimum angle for the pan servo.
int servo1_lowerlimit = 0;
int servo2_lowerlimit = 10;   // The minimum angle for the tilt servo.

//
// Servo Position Variables
// These variables keep track of the current angle of each servo.
int pos_servo1 = servo1_lowerlimit; // Initialize pan servo position to its
                                    // minimum angle.
int pos_servo2 = servo2_lowerlimit; // Initialize tilt servo position to its
                                    // minimum angle.

//
// setup()
// This function runs once when the Arduino is powered on or reset. It's used
// for initialization.
void setup() {
  // Initialize serial communication at 9600 bits per second.
  // This allows the Arduino to send data to a computer for debugging or
  // monitoring.
  Serial.begin(9600);
  // Configure the digital pins for the ultrasonic sensors and the LED.
  // pinMode(pin, mode) sets a pin to be either an INPUT or an OUTPUT.
  pinMode(TRIG1, OUTPUT);   // TRIG pins send out a signal, so they are
                            // outputs.
  pinMode(ECHO1, INPUT);    // ECHO pins receive a signal, so they are
                            // inputs.
  pinMode(TRIG2, OUTPUT);
  pinMode(ECHO2, INPUT);
  pinMode(TRIG3, OUTPUT);
  pinMode(ECHO3, INPUT);
  pinMode(LED1_PIN, OUTPUT); // The LED pin is an output.
  // Attach the servo objects to the physical pins on the Arduino that will
  // send the PWM signals.
  // The attach() function connects a Servo object to a pin.
  servo1.attach(10); // Pan servo is controlled by digital pin 10.
  servo2.attach(11); // Tilt servo is controlled by digital pin 11.
} // end setup()

//
// distance()
// This function measures and calculates the distance from all three
// ultrasonic sensors.
void distance() {
  //
  // Measure distance for Sensor 1
  // To trigger the sensor, we send a short 10-microsecond high pulse on the
  // TRIG pin.
  digitalWrite(TRIG1, LOW);      // Ensure the trigger pin is low to start.
  delayMicroseconds(2);          // Wait for 2 microseconds.
  digitalWrite(TRIG1, HIGH);     // Set the trigger pin high to send the
                                 // ultrasonic burst.
  delayMicroseconds(10);         // Keep it high for 10 microseconds.
  digitalWrite(TRIG1, LOW);      // Set it low again.
  // Measure the time it takes for the echo pulse to come back..
  // pulseIn() waits for the pin to go HIGH, starts timing, and stops when the
  // pin goes LOW..
  // It returns the pulse length in microseconds. A timeout is used to prevent
  // getting stuck.
  duration1 = pulseIn(ECHO1, HIGH, 30000); // Timeout after 30,000 us (30 ms).
  delay(10);                               // A small delay between sensor readings to prevent interference.
  //
  // Measure distance for Sensor 2
  digitalWrite(TRIG2, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG2, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG2, LOW);
  duration2 = pulseIn(ECHO2, HIGH, 30000);
  delay(10);
  //
  // Measure distance for Sensor 3
  digitalWrite(TRIG3, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG3, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG3, LOW);
  duration3 = pulseIn(ECHO3, HIGH, 30000);
  // Convert the pulse duration into distance in centimeters.
  // The formula is: distance (duration speed of sound) / 2.
  // The speed of sound is approx. 343 m/s, which is 0.034 cm/microsecond.
  // We divide by 2 because the sound wave travels to the object and back.
  // If duration is (timeout), set distance to 999 (a large value indicating
  // no object).
  distance1 = (duration1 == 0 ? 999 : (duration1 * 0.034) / 2);
  distance2 = (duration2 == 0 ? 999 : (duration2 * 0.034) / 2);
  distance3 = (duration3 == 0 ? 999 : (duration3 * 0.034) / 2);
  // Print the calculated distances to the Serial Monitor.
  Serial.print("D1: "); Serial.print(distance1); Serial.print(" cm, ");
  Serial.print("D2: "); Serial.print(distance2); Serial.print(" cm, ");
  Serial.print("D3: "); Serial.print(distance3); Serial.println(" cm");
} // end distance()

//
// condition()
// This function is called when an object is detected. It adjusts the servos
// to "track" the object.
// It takes the servo positions by reference (&), meaning it can directly
// change the original variables.
void condition(int &pos_servo1, int &pos_servo2) {
  // If the tilt servo is angled above 125 degrees, use a modified tracking
  // logic, because if
  // the object is detected in only sensor2 then the servo1 needs to rotate in
  // one direction for one half
  // and in the other direction for the other half.
  if (pos_servo2 > 125) {
    condition_mod(pos_servo1, pos_servo2); // Call the alternative condition
                                           // function.
    return; // Exit this function because the conditions in this function are
            // not useful since the object
  }
  // is detected on the other half side
  distance(); // Get the latest distance readings.
  digitalWrite(LED1_PIN, HIGH); // Turn on the LED to indicate an object is
                                // being tracked.
  //
  // ---Pan Control (Left/Right)
  // Compare Sensor 2 and Sensor 3 to decide which way to pan.
  if (distance2 > range && distance3 < range) {
    // If Sensor 3 is closer than Sensor 2, the object is on the right. Pan
    // right.
    pos_servo1 = constrain(pos_servo1 + 5, servo1_lowerlimit,
                           servo1_upperlimit); // Increase pan angle by 5.
  }
  servo1.write(pos_servo1); // Send the new position to the pan servo.
  if (distance2 < range && distance3 > range) {
    // If Sensor 2 is closer than Sensor 3, the object is on the left. Pan
    // left.
    pos_servo1 = constrain(pos_servo1 - 5, servo1_lowerlimit,
                           servo1_upperlimit); // Decrease pan angle by 5.
    servo1.write(pos_servo1);
  }
  //--- Tilt Control (Up/Down)
  // Compare Sensor 1 and Sensor 2 to decide which way to tilt.
  if (distance1 < range && distance2 > range) {
    // If Sensor 1 is closer than Sensor 2, the object is in front. Tilt up.
    pos_servo2 = constrain(pos_servo2 + 5, servo2_lowerlimit,
                           servo2_upperlimit); // Increase tilt angle.
  }
  servo2.write(pos_servo2); // Send the new position to the tilt servo.
  if (distance1 > range && distance2 < range) {
    // If Sensor 2 is closer than Sensor 1, the object is below/behind. Tilt
    // down.
    pos_servo2 = constrain(pos_servo2 - 5, servo2_lowerlimit,
                           servo2_upperlimit); // Decrease tilt angle.
  }
  servo2.write(pos_servo2);
  // If all three sensors detect an object, hold the current position.
  if (distance1 < range && distance2 < range && distance3 < range) {
  }
  servo1.write(pos_servo1);
  servo2.write(pos_servo2);
  // After adjusting, check distances again.
  distance();
  // If an object is still in range, call this function again (recursion) to
  // continue tracking.
  if (distance1 < range || distance2 < range || distance3 < range) {
    condition(pos_servo1, pos_servo2);
  }
  // When the object is no longer in range, the recursive calls will end, and
  // this line is reached.
  digitalWrite(LED1_PIN, LOW); // Turn off the LED.
} // end condition()

//
// condition_mod()
// An alternative tracking function, used when the object is on other side.
// The pan logic is inverted compared to the main condition() function.
void condition_mod(int &pos_servo1, int &pos_servo2) {
  distance();
  digitalWrite(LED1_PIN, HIGH);
  // Pan control with inverted logic.
  if (distance2 > range && distance3 < range) {
    // Pan left instead of right.
    pos_servo1 = constrain(pos_servo1 - 5, servo1_lowerlimit,
                           servo1_upperlimit);
  }
  servo1.write(pos_servo1);
  if (distance2 < range && distance3 > range) {
    // Pan right instead of left.
    pos_servo1 = constrain(pos_servo1 + 5, servo1_lowerlimit,
                           servo1_upperlimit);
  }
  servo1.write(pos_servo1);
  // Tilt control is the same as in the main condition() function.
  if (distance1 < range && distance2 > range) {
    pos_servo2 = constrain(pos_servo2 + 5, servo2_lowerlimit,
                           servo2_upperlimit);
  }
  servo2.write(pos_servo2);
  if (distance1 > range && distance2 < range) {
    pos_servo2 = constrain(pos_servo2 - 5, servo2_lowerlimit,
                           servo2_upperlimit);
  }
  servo2.write(pos_servo2);
  // Hold position if all sensors detect an object.
  if (distance1 < range && distance2 < range && distance3 < range) {
  }
  servo1.write(pos_servo1);
  servo2.write(pos_servo2);
  // Recurse to continue tracking.
  distance();
  if (distance1 < range || distance2 < range || distance3 < range) {
    condition(pos_servo1, pos_servo2);
  }
  digitalWrite(LED1_PIN, LOW); // Turn off LED when tracking is done.
} // end condition_mod()

//
// loop()
// This function runs over and over again after setup() is finished. It
// contains the main logic.
// It performs a "raster scan" to search for objects in the environment.
void loop() {
  // === UPWARD TILT SCAN ===
  // This loop sweeps the tilt servo from its lower limit to its upper limit.
  while (pos_servo2 <= servo2_upperlimit) {
    digitalWrite(LED1_PIN, LOW); // Ensure LED is off during scanning.
    servo2.write(pos_servo2);      // Set the tilt position.
    distance();                    // Check for objects.
    if (distance1 < range || distance2 < range || distance3 < range) {
      condition(pos_servo1, pos_servo2); // If an object is found, start
                                         // tracking it.
    }
  }
  //
  // PAN RIGHT
  // At the current tilt angle, sweep the pan servo from left to right.
  while (pos_servo1 <= servo1_upperlimit) {
    distance();
    if (distance1 < range || distance2 < range || distance3 < range) {
      condition(pos_servo1, pos_servo2);
    }
    servo1.write(pos_servo1);
    delay(10);
    pos_servo1 += 5;
  }
  // Set the pan position.
  // Increment the pan angle.
  // Wait briefly for the servo to move.
  distance(); // Check one last time at the end of the sweep to check if
              // there is an
              // object in the range
  if (distance1 < range || distance2 < range || distance3 < range) {
    condition(pos_servo1, pos_servo2);
  }
  pos_servo2 += 15; // Move the tilt servo up by 15 degrees for the next pan
                    // sweep.
  servo2.write(pos_servo2);
  //
  // PAN LEFT
  // Now, sweep the pan servo from right to left.
  while (pos_servo1 >= servo1_lowerlimit) {
    distance();
    if (distance1 < range || distance2 < range || distance3 < range) {
      condition(pos_servo1, pos_servo2);
    }
    servo1.write(pos_servo1);
    delay(10);
    pos_servo1 -= 5; // Decrement the pan angle.
  }
  distance();
  if (distance1 < range || distance2 < range || distance3 < range) {
    condition(pos_servo1, pos_servo2);
  }
  pos_servo2 += 15; // Move tilt up again.
  servo2.write(pos_servo2);

  // === DOWNWARD TILT SCAN ===
  // This loop sweeps the tilt servo from its upper limit back down to its
  // lower limit.
  while (pos_servo2 >= servo2_lowerlimit) {
    distance();
    if (distance1 < range || distance2 < range || distance3 < range) {
      condition(pos_servo1, pos_servo2);
    }
    servo2.write(pos_servo2);
    //
    // PAN RIGHT
    while (pos_servo1 <= servo1_upperlimit) {
      distance();
      if (distance1 < range || distance2 < range || distance3 < range) {
        condition(pos_servo1, pos_servo2);
      }
      servo1.write(pos_servo1);
      delay(10);
      pos_servo1 += 5;
    }
    distance();
    if (distance1 < range || distance2 < range || distance3 < range) {
      condition(pos_servo1, pos_servo2);
    }
    pos_servo2 -= 15; // Move the tilt servo down.
    servo2.write(pos_servo2);
    //
    // PAN LEFT
    while (pos_servo1 >= servo1_lowerlimit) {
      distance();
      if (distance1 < range || distance2 < range || distance3 < range) {
        condition(pos_servo1, pos_servo2);
      }
      servo1.write(pos_servo1);
      delay(10);
      pos_servo1 -= 5;
    }
    distance();
    if (distance1 < range || distance2 < range || distance3 < range) {
      condition(pos_servo1, pos_servo2);
    }
    pos_servo2 -= 15; // Move tilt down again.
    servo2.write(pos_servo2);
  }
} // end loop()