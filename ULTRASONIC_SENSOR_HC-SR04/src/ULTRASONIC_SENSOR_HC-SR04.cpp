/* 
 * Project ULTRASONIC_SENSOR_HC-SR04
 * Author: Adrian Montoya
 * Date: 2 APRIL 2025
 * 
 * Grove - Ultrasonic Ranger 
 * Distance = echo signal high time * Sound speed (340M/S)/2
 */

// Include Particle Device OS APIs
#include "Particle.h"
#include <Adafruit_MQTT.h>
#include "Adafruit_MQTT/Adafruit_MQTT_SPARK.h"
#include "Adafruit_MQTT/Adafruit_MQTT.h"
#include "credentials.h"

const int ECHOPIN1 = D8; //Echo pin sensor 1
const int ECHOPIN2 = D9; //Echo pin sensor 1
float duration1 = 0.0;
float duration2 = 0.0;
float distanceIn = 0.0;

TCPClient TheClient; 
 
Adafruit_MQTT_SPARK mqtt(&TheClient,AIO_SERVER,AIO_SERVERPORT,AIO_USERNAME,AIO_KEY); 
Adafruit_MQTT_Publish aqPub = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/airquality");
Adafruit_MQTT_Publish dustPub = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/dustquality");

/************Declare Functions*************/
void MQTT_connect();
bool MQTT_ping();
float distance(uint32_t time);
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

  pinMode(ECHOPIN1,INPUT);      // sensor 1
  pinMode(ECHOPIN2,INPUT);      // sensor 2
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  // sensor 1
  digitalWrite(TRIGGERPIN1, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGERPIN1, HIGH); // Trigger pin to HIGH
  delayMicroseconds(10); // 10us high
  digitalWrite(TRIGGERPIN1, LOW); // Trigger pin to HIGH

  // sensor 2
  // digitalWrite(TRIGGERPIN2, LOW);
  // delayMicroseconds(2);
  // digitalWrite(TRIGGERPIN2, HIGH); // Trigger pin to HIGH
  // delayMicroseconds(10); // 10us high
  // digitalWrite(TRIGGERPIN2, LOW); // Trigger pin to HIGH
  
  // returns the Duration in microseconds
  duration1 = pulseIn(ECHOPIN1,HIGH); // Waits for the echo pin to get high on sensor 1
  // delayMicroseconds(15); // 10us high
  // duration2 = pulseIn(ECHOPIN2,HIGH); // Waits for the echo pin to get high on sensor 2
  distanceIn = distance(duration1); // Use function to calculate the distance
  Serial.printf("Distance = %0.2f in\n",distanceIn); // Output to serial
  delay(1000); // Wait to do next measurement
  Serial.printf("duration1 = %0.2f\n", duration1);
  // Serial.printf("duration2 = %0.2f\n", duration2);

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

float distance(uint32_t time) {
  // Calculates the Distance in mm
  // ((time)*(Speed of sound))/ toward and backward of object) * 10

  float distanceCalc; // Calculation variable
  // DistanceCalc = ((time /2.9) / 2); // Actual calculation in mm
  distanceCalc = ((time / 74.0) / 2.0); // Actual calculation in inches
  return distanceCalc; // return calculated value
}

float speed(uint32_t time, float sensor1, float sensor2) {
  float mph;
    // Calculates the speed based on each sensors time
  // ((time)*(Speed of sound))/ toward and backward of object) * 10
  return mph;
}
