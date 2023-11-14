#include <Servo.h>
#include <WiFi.h>
#include <ThingSpeak.h>

// Define pins for motor control
const int motor1A = 26;  // Replace XX with your actual pin numbers
const int motor1B = 27;
const int motor2A = 25;
const int motor2B = 33;

const int motorsigA=12;
const int motorsigB=13;

const int freq = 30000;
const int pwmChannel = 14;
const int resolution = 8;
int dutyCycle = 160;

// Define pins for ultrasonic sensor
const int trigPin = 19;
const int echoPin = 18;



// Replace these values with your ThingSpeak channel details
const char *ssid = "YOUR_WIFI_SSID";
const char *password = "YOUR_WIFI_PASSWORD";
const char *server = "api.thingspeak.com";
String apiKey = "YOUR_API_KEY";
int channelID = YOUR_CHANNEL_ID;



// Create a servo object
Servo servo;

// Define servo positions
const int servoMiddle = 80; // Middle position
const int servoLeft = 180;    // Left position
const int servoRight = -40; // Right position

// Define distances for obstacle detection
const int obstacleThreshold = 20; // Adjust as needed (in centimeters)

void setup() {
  // Initialize motor control pins as outputs
  pinMode(motor1A, OUTPUT);
  pinMode(motor1B, OUTPUT);
  pinMode(motor2A, OUTPUT);
  pinMode(motor2B, OUTPUT);

  pinMode(motorsigA, OUTPUT);
  pinMode(motorsigB, OUTPUT); // 

  // Initialize servo
  servo.attach(4); // Replace XX with your servo pin
  servo.write(servoMiddle); // Start at the middle position
  
  ledcSetup(pwmChannel, freq, resolution);
  ledcAttachPin(motorsigA, pwmChannel);
  ledcAttachPin(motorsigB, pwmChannel);
  // Initialize ultrasonic sensor pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Initialize serial communication for debugging
  Serial.begin(115200);
  connectToWiFi();
  
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
  ledcWrite(pwmChannel, dutyCycle);
  // Read distance from the ultrasonic sensor
  long duration, distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration / 2) / 29.1; // Calculate distance in centimeters

  updateThingSpeak(distance);

  // Serial.print("distance -- ");
  // Serial.println(distance);

  // Print distance for debugging
  Serial.print("Distance: ");
  Serial.println(distance);

  // Obstacle avoidance logic
  if (distance <= obstacleThreshold) {
    // If an obstacle is detected, turn left (adjust as needed)
    // motorControl(LOW, HIGH, HIGH, LOW); // Turn left
    motorControl(1, 0, 1, 0);
    delay(600);
    motorControl(0, 0, 0, 0);
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
  // dutyCycle = 0;
  // while (dutyCycle <= 255){
  //   ledcWrite(pwmChannel, dutyCycle);   
  //   Serial.print("Forward with duty cycle: ");
  //   Serial.println(dutyCycle);
  //   dutyCycle = dutyCycle + 5;
  //   delay(500);
  // }
  ledcWrite(pwmChannel, dutyCycle);  
  delay(100);
}

// Function to control the motors
void motorControl(int motor1AState, int motor1BState, int motor2AState, int motor2BState) {
  digitalWrite(motor1A, motor1AState);
  digitalWrite(motor1B, motor1BState);
  digitalWrite(motor2A, motor2AState);
  digitalWrite(motor2B, motor2BState);
}

//Connect to wifi

void connectToWiFi() {
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}


void updateThingSpeak(int value) {
  // Establish a connection to the ThingSpeak server
  WiFiClient client;
  if (client.connect(server, 80)) {
    // Build the HTTP POST request
    String postStr = "field1=" + String(value);
    postStr += "&api_key=" + apiKey;

    // Make a HTTP request
    client.println("POST /update HTTP/1.1");
    client.println("Host: " + String(server));
    client.println("Connection: close");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("Content-Length: " + String(postStr.length()));
    client.println();
    client.println(postStr);

    // Print the server response to the serial monitor
    while (client.available()) {
      char c = client.read();
      Serial.print(c);
    }

    // Close the connection
    client.stop();
  } else {
    // Unable to establish a connection
    Serial.println("Unable to connect to ThingSpeak");
  }
}



