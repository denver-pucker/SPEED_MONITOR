/* 
 * Project ULTRASONIC_SENSOR_HC-SR04
 * Author: Adrian Montoya
 * Date: 2 APRIL 2025
 * 
 * Grove - Ultrasonic Ranger 
 * Distance = echo signal high time * Sound speed (343M/S)/2
 * Distance = echo signal high time * Sound speed (1,125FT/S)/2
 * One ping of the HC0SR04 actually exists of 8 pulses at 40 kHz 
 * to do the measurement. To start a ping you need to provide a 
 * 10us pulse on the trigger input. When the distance is measured 
 * by the 8 pulses the HC0SR04 puts a pulse on the echo pin. 
 * You can calculate the distance with the length of the echo pulse 
 * and the speed of sound. The speed of sound is 340 m/s or 
 * 2.9 micro seconds per mm. We have to divide the length of the 
 * pulse by 2.9 to get the result in mm. The ping is traveling 
 * towards an object and back to the sensor again. 
 * Because of this we need to divide the result by two. 
 * Between two pings we need to keep a 60ms measurement cycle.
 */

// Include Particle Device OS APIs
#include "Particle.h"
#include <Adafruit_MQTT.h>
#include "Adafruit_MQTT/Adafruit_MQTT_SPARK.h"
#include "Adafruit_MQTT/Adafruit_MQTT.h"
#include "credentials.h"

const int TRIGGERPIN1 = D8;  // Trigger pin sensor 1
const int ECHOPIN1 = D9;     // Echo pin sensor 1
const int TRIGGERPIN2 = D7;  // Trigger pin sensor 2
const int ECHOPIN2 = D6;     // Echo pin sensor 2
const int PIR = D5;          // PIR sensor
const int DISTANCE = 1.0;    // Adjust distance as necessary in feet

float timeSensor1, timeSensor2;
float deltaTime, feetPerSecond;
float speedFPS, speedMPH;
int count = 0;
bool motion;

float duration1 = 0.0;
float duration2 = 0.0;
float distance1 = 0.0;
float distance2 = 0.0;

TCPClient TheClient; 
 
Adafruit_MQTT_SPARK mqtt(&TheClient,AIO_SERVER,AIO_SERVERPORT,AIO_USERNAME,AIO_KEY); 
Adafruit_MQTT_Publish aqPub = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/airquality");
Adafruit_MQTT_Publish dustPub = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/dustquality");

/************Declare Functions*************/
void MQTT_connect();
bool MQTT_ping();
float measureTime(int trigPin, int echoPin);
float distance(int trigPin, int echoPin);
float speed(uint32_t time, float sensor1, float sensor2);

// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(SEMI_AUTOMATIC);

// Run the application and system concurrently in separate threads
// SYSTEM_THREAD(ENABLED);

// Show system, cloud connectivity, and application logs over USB
// View logs with CLI using 'particle serial monitor --follow'
SerialLogHandler logHandler(LOG_LEVEL_INFO);

// setup() runs once, when the device is first turned on
void setup() {
  // Enable Serial Monitor
  Serial.begin (9600);
  waitFor(Serial.isConnected,10000);  //wait for Serial Monitor

  pinMode(TRIGGERPIN1,OUTPUT);
  pinMode(ECHOPIN1,INPUT); 
  pinMode(TRIGGERPIN2,OUTPUT);     
  pinMode(ECHOPIN2,INPUT); 
  pinMode(PIR,INPUT);     
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {

  // Use this to get distance
  // distance1 = distance(TRIGGERPIN1,ECHOPIN1); // Use function to calculate the distance
  // Serial.printf("Distance too far %0.2f waiting for sensor1 to get triggered......\n", distance1);
  // Serial.printf("Sensor 1 distance = %0.2f in\n",distance1); // Output to serial
  // distance2 = distance(TRIGGERPIN2,ECHOPIN2);
  // Serial.printf("Sensor 2 distance = %0.2f in\n",distance2); // Output to serial
  // if(distance1 < 25.0) {
    // Wait for sensor 1 to get triggered
  motion = digitalRead(PIR);
  Serial.printf("Motion detected %i\n",motion);
  if(motion) {
  timeSensor1 = measureTime(TRIGGERPIN1,ECHOPIN1);
  while(timeSensor1 == 0) {
    // timeSensor1 = (measureTime(TRIGGERPIN1,ECHOPIN1) * 0.0000001);
    timeSensor1 = measureTime(TRIGGERPIN1,ECHOPIN1);
  }
  // Serial.printf("timeSensor1 = %0.2f\n",timeSensor1);

  // Wait for sensor 2 to get triggered
  timeSensor2 = measureTime(TRIGGERPIN2,ECHOPIN2);
  while(timeSensor2 == 0) {
    // timeSensor2 = (measureTime(TRIGGERPIN2,ECHOPIN2) * 0.0000001);
    timeSensor2 = measureTime(TRIGGERPIN2,ECHOPIN2);
  }
  // Serial.printf("timeSensor2 = %0.2f\n",timeSensor2);
  
  deltaTime = timeSensor2 - timeSensor1;

  if(deltaTime > 0) {
    Serial.printf("Objects measured = %i\n", count);
    speedFPS = DISTANCE / deltaTime;    // speed = distance / time
    Serial.printf("speedFPS = %0.2f FPS\n",speedFPS);
    speedMPH = speedFPS * 0.681818;     // 1ft/sec = 0.681818 miles/hour
    count++;
    Serial.printf("Speed = %0.2f MPH\n\n", speedMPH);
  }
}
  delay(500);



}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;
 
  // Return if already connected.
  if (mqtt.connected()) {
    return;
  }
 
  Serial.print("Connecting to MQTT... ");
 
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.printf("Error Code %s\n",mqtt.connectErrorString(ret));
       Serial.printf("Retrying MQTT connection in 5 seconds...\n");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds and try again
  }
  Serial.printf("MQTT Connected!\n");
}

bool MQTT_ping() {
  static unsigned int last;
  bool pingStatus;

  if ((millis()-last)>120000) {
      Serial.printf("Pinging MQTT \n");
      pingStatus = mqtt.ping();
      if(!pingStatus) {
        Serial.printf("Disconnecting \n");
        mqtt.disconnect();
      }
      last = millis();
  }
  return pingStatus;
}

float measureTime(int trigPin, int echoPin) {
  digitalWrite(trigPin,LOW);
  delay(2);
  digitalWrite(trigPin,HIGH);
  delay(10);
  digitalWrite(trigPin,LOW);
  float pulse = pulseIn(echoPin,HIGH);
  float pulseTime = pulse * 0.00089;
  // return pulseIn(echoPin,HIGH)*0.0000001;   //converts to seconds
  // return pulseIn(echoPin,HIGH);   //converts to seconds
  return pulseTime;
}

float distance(int trigPin, int echoPin) {
  digitalWrite(trigPin,LOW);
  delay(2);
  digitalWrite(trigPin,HIGH);
  delay(10);
  digitalWrite(trigPin,LOW);
  float pulse = pulseIn(echoPin,HIGH);
  // Calculates the Distance in mm
  // ((time)*(Speed of sound))/ toward and backward of object) * 10

  // float distanceCalc = ((time /2.9) / 2); // Actual calculation in mm
  float distanceCalc = ((pulse / 74.0) / 2.0); // Actual calculation in inches
  return distanceCalc; // return calculated value
}

float speed(uint32_t time, float sensor1, float sensor2) {
  float mph;
    // Calculates the speed based on each sensors time
  // ((time)*(Speed of sound))/ toward and backward of object) * 10
  return mph;
}
