#include <Servo.h>

// Define pins for motor control
const int motor1A = 26;  // Replace XX with your actual pin numbers
const int motor1B = 27;
const int motor2A = 25;
const int motor2B = 33;

// Define pins for ultrasonic sensor
const int trigPin = 19;
const int echoPin = 18;

// Create a servo object
Servo servo;

// Define servo positions
const int servoMiddle = 90; // Middle position
const int servoLeft = 0;    // Left position
const int servoRight = 180; // Right position

// Define distances for obstacle detection
const int obstacleThreshold = 20; // Adjust as needed (in centimeters)

void setup() {
  // Initialize motor control pins as outputs
  pinMode(motor1A, OUTPUT);
  pinMode(motor1B, OUTPUT);
  pinMode(motor2A, OUTPUT);
  pinMode(motor2B, OUTPUT);

  // Initialize servo
  servo.attach(4); // Replace XX with your servo pin
  servo.write(servoMiddle); // Start at the middle position
  
  // Initialize ultrasonic sensor pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Initialize serial communication for debugging
  Serial.begin(115200);
}

long long int cal_dis(){
  long duration, distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration / 2) / 29.1; 
  return distance;
}

void loop() {
  // Read distance from the ultrasonic sensor
  long duration, distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration / 2) / 29.1; // Calculate distance in centimeters

  // Serial.print("distance -- ");
  // Serial.println(distance);

  // Print distance for debugging
  Serial.print("Distance: ");
  Serial.println(distance);

  // Obstacle avoidance logic
  if (distance <= obstacleThreshold) {
    // If an obstacle is detected, turn left (adjust as needed)
    // motorControl(LOW, HIGH, HIGH, LOW); // Turn left
    servo.write(servoLeft); // Turn the servo left
    // delay(250);
    delay(500);
    long long int dis1=cal_dis();
    Serial.print("dis1: ");
    Serial.println(dis1);

    servo.write(servoRight);
    delay(500);
    long long int dis2=cal_dis();
    Serial.print("dis2: ");
    Serial.println(dis2);

    servo.write(servoMiddle);

    if(dis1>dis2){
      //left
      motorControl(1, 0, 0, 1);
      delay(2000);
    }
    else{
      motorControl(0, 1, 1, 0);
      delay(2000);
    }
  } else {
    // If no obstacle is detected, move forward
    motorControl(0, 1, 0, 1); // Move forward
    servo.write(servoMiddle); // Return servo to the middle position
    delay(500);
  }
  // delay(1000);
}

// Function to control the motors
void motorControl(int motor1AState, int motor1BState, int motor2AState, int motor2BState) {
  digitalWrite(motor1A, motor1AState);
  digitalWrite(motor1B, motor1BState);
  digitalWrite(motor2A, motor2AState);
  digitalWrite(motor2B, motor2BState);
}
