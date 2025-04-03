/* 
 * Project Grove-Ultrasonic-Ranger
 * Author: Adrian Montoya
 * Date: 3 APRIL 2025
 */

// Include Particle Device OS APIs
#include "Particle.h"
#include "Grove-Ultrasonic-Ranger.h"
#include <Adafruit_MQTT.h>
#include "Adafruit_MQTT/Adafruit_MQTT_SPARK.h"
#include "Adafruit_MQTT/Adafruit_MQTT.h"
#include "credentials.h"

const int SIGNALPIN1 = D2; //Echo pin sensor 1
const int SIGNALPIN2 = D3; //Echo pin sensor 1
float duration1 = 0.0;
float duration2 = 0.0;
float distanceIn = 0.0;
float rangeInInches1, rangeInInches2;
float rangeInCentimeters1, rangeInCentimeters2;
bool sensorStatus1, sensorStatus2;

Ultrasonic ultrasonic1(SIGNALPIN1);
Ultrasonic ultrasonic2(SIGNALPIN2);

TCPClient TheClient; 
 
Adafruit_MQTT_SPARK mqtt(&TheClient,AIO_SERVER,AIO_SERVERPORT,AIO_USERNAME,AIO_KEY); 
Adafruit_MQTT_Publish speedPub = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/speed");
Adafruit_MQTT_Publish counterPub = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/counter");

/************Declare Functions*************/
void MQTT_connect();
bool MQTT_ping();
float distance(uint32_t time);
float speed(uint32_t time, float sensor1, float sensor2);

// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(SEMI_AUTOMATIC);

// Run the application and system concurrently in separate threads
SYSTEM_THREAD(ENABLED);

// Show system, cloud connectivity, and application logs over USB
// View logs with CLI using 'particle serial monitor --follow'
SerialLogHandler logHandler(LOG_LEVEL_INFO);

// setup() runs once, when the device is first turned on
void setup() {
  // Enable Serial Monitor
  Serial.begin (9600);
  waitFor(Serial.isConnected,10000);  //wait for Serial Monitor

}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  MQTT_connect;
  MQTT_ping;
 
  rangeInInches1 = ultrasonic1.MeasureInInches();
  Serial.printf("The distance to obstacles in front of ultrasonic1 is: %0.2f inches\n", rangeInInches1);
  rangeInInches2 = ultrasonic2.MeasureInInches();
  Serial.printf("The distance to obstacles in front of ultrasonic2 is: %0.2f inches\n", rangeInInches2);
  delay(1000);
 
  rangeInCentimeters1 = ultrasonic1.MeasureInCentimeters();
  Serial.printf("The distance to obstacles in front of ultrasonic1 is: %0.2f cm\n", rangeInCentimeters1);
  rangeInCentimeters2 = ultrasonic2.MeasureInCentimeters();
  Serial.printf("The distance to obstacles in front of ultrasonic2 is: %0.2f cm\n", rangeInCentimeters2);

  delay(1000);
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care of connecting.
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