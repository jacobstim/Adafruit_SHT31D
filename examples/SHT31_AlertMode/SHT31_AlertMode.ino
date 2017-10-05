/****************************************************************************
SHT31D Unified Sensor Driver
*****************************************************************************
Example illustrating how to use alerts in the unified sensor driver (from
Adafruit's Sensor framework).

Written in 2017 by Tim Jacobs
*****************************************************************************/

#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <SHT31D_Universal.h>

#define SERIALOUT Serial

SHT31_Unified sht31;

// =============================================================================
// =============================================================================
// =============================================================================

void setup() {
  SERIALOUT.begin(57600);
  delay(1000);
  
  // Initialize sensor
  if (sht31.begin(0x44)) {
    // Configure sensor
    SERIALOUT.println("Found SHT31-D Temperature/Humidity sensor!");

    // Read sensor data
    sensor_t sensor;
    SERIALOUT.println("------------------------------------");
    sht31.temperature().getSensor(&sensor);
    displaySensorDetails(sensor, "C");
    SERIALOUT.println("------------------------------------");
    sht31.humidity().getSensor(&sensor);
    displaySensorDetails(sensor, "%");
    SERIALOUT.println("------------------------------------");

    // Alerting requires periodic mode to be turned on
    sht31.enablePeriodic(REPEATABILITY_HIGH, FREQUENCY_HALFHZ);

    // Configure thresholds
    Adafruit_SHT31D_AlertValues thresholds;
    thresholds.temperatureSet = 18.0; thresholds.temperatureClear = 19.0;
    thresholds.humiditySet = 30; thresholds.humidityClear = 32;
    SERIALOUT.println("-> Alert Limits Set:");
    SERIALOUT.print(  "      LOW SET    : temp = "); SERIALOUT.print(thresholds.temperatureSet,2); SERIALOUT.print(" / hum = "); SERIALOUT.println(thresholds.humiditySet,2);
    SERIALOUT.print(  "      LOW CLEAR  : temp = "); SERIALOUT.print(thresholds.temperatureClear,2); SERIALOUT.print(" / hum = "); SERIALOUT.println(thresholds.humidityClear,2);
    sht31.setAlertLimitsLow(thresholds);
    thresholds.temperatureSet = 28.0; thresholds.temperatureClear = 27.0;
    thresholds.humiditySet = 60; thresholds.humidityClear = 58;
    SERIALOUT.print(  "      HIGH SET   : temp = "); SERIALOUT.print(thresholds.temperatureSet,2); SERIALOUT.print(" / hum = "); SERIALOUT.println(thresholds.humiditySet,2);
    SERIALOUT.print(  "      HIGH CLEAR : temp = "); SERIALOUT.print(thresholds.temperatureClear,2); SERIALOUT.print(" / hum = "); SERIALOUT.println(thresholds.humidityClear,2);
    sht31.setAlertLimitsHigh(thresholds);

    // Read back thresholds; some rounding occurs in the sensor so check actual values set
    thresholds = sht31.readAlertLimitsLow();
    SERIALOUT.println("-> Alert Limits Retrieved:");
    SERIALOUT.print(  "      LOW SET    : temp = "); SERIALOUT.print(thresholds.temperatureSet,2); SERIALOUT.print(" / hum = "); SERIALOUT.println(thresholds.humiditySet,2);
    SERIALOUT.print(  "      LOW CLEAR  : temp = "); SERIALOUT.print(thresholds.temperatureClear,2); SERIALOUT.print(" / hum = "); SERIALOUT.println(thresholds.humidityClear,2);
    thresholds = sht31.readAlertLimitsHigh();
    SERIALOUT.print(  "      HIGH SET   : temp = "); SERIALOUT.print(thresholds.temperatureSet,2); SERIALOUT.print(" / hum = "); SERIALOUT.println(thresholds.humiditySet,2);
    SERIALOUT.print(  "      HIGH CLEAR : temp = "); SERIALOUT.print(thresholds.temperatureClear,2); SERIALOUT.print(" / hum = "); SERIALOUT.println(thresholds.humidityClear,2);

  } else {
    SERIALOUT.println("Couldn't find SHT31-D Temperature/Humidity sensor!");
    while(1);
  }
}

// =============================================================================
// =============================================================================
// =============================================================================

 // Track number of loops
uint32_t loopcounter = 1;

void loop() {
  SERIALOUT.print("=== LOOP "); SERIALOUT.print(loopcounter); SERIALOUT.println(" ======================================");

  uint32_t startTime = millis();

  // Wait for an incident for at most 5 seconds
  while(millis() < startTime + 5000) {
    // Does sensor report an alert?
    if(sht31.isAlertPending()) {
      // Is this a temperature or a humidity interrupt?
      if(sht31.isAlertTemperature()) {
        SERIALOUT.println("ALERT: Temperature threshold exceeded!");
      } else if (sht31.isAlertHumidity()) {
        SERIALOUT.println("ALERT: Relative Humidity threshold exceeded!");
      } else {
        // Unknown alert type, presumably a reset indication of sensor
        if (sht31.isAlertReset()) {
          SERIALOUT.println("SHT31D indicating powerup/reset has occured!");
        } else {
          SERIALOUT.println("ERROR: Unknown SHT31-D alert!");
        }
      }
      // Clear alert
      sht31.clearStatus();
      break;
    }
    // Wait a tiny bit to prevent sensor hammering in while loop
    delayMicroseconds(10000);
  }

  // Time's up! Just read temperature & humidity
  sensors_event_t event;
  sht31.temperature().getEvent(&event);
  if (!isnan(event.temperature)) {
    SERIALOUT.print("Temperature : "); SERIALOUT.print(event.temperature,2); SERIALOUT.println(" C");
  }
  sht31.humidity().getEvent(&event);
  if (!isnan(event.relative_humidity)) {
    SERIALOUT.print("Relative Humidity : "); SERIALOUT.print(event.relative_humidity,2); SERIALOUT.println(" %");
  }
  loopcounter++;
}

// =============================================================================
// =============================================================================
// =============================================================================

/**************************************************************************/
/*
    Displays some basic information on this sensor from the unified
    sensor API sensor_t type (see Adafruit_Sensor for more information)
*/
/**************************************************************************/
void displaySensorDetails(sensor_t sensor, String siunit) {
  SERIALOUT.print  ("Sensor:       "); SERIALOUT.println(sensor.name);
  SERIALOUT.print  ("Driver Ver:   "); SERIALOUT.println(sensor.version);
  SERIALOUT.print  ("Unique ID:    "); SERIALOUT.println(sensor.sensor_id);
  SERIALOUT.print  ("Max Value:    "); SERIALOUT.print(sensor.max_value); SERIALOUT.print(" "); SERIALOUT.println(siunit);
  SERIALOUT.print  ("Min Value:    "); SERIALOUT.print(sensor.min_value); SERIALOUT.print(" "); SERIALOUT.println(siunit);
  SERIALOUT.print  ("Resolution:   "); SERIALOUT.print(sensor.resolution); SERIALOUT.print(" "); SERIALOUT.println(siunit);
}
