/****************************************************************************
SHT31D Unified Sensor Driver
*****************************************************************************
Example illustrating how to read data using the unified sensor driver (from
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

  // Initialize sensor
  if (sht31.begin(0x44)) {
    // Configure sensor
    SERIALOUT.println("Found SHT31-D Temperature/Humidity sensor!");

    // Configure periodic mode, one measurement per 2 seconds
    sht31.enablePeriodic(REPEATABILITY_HIGH, FREQUENCY_HALFHZ);

    // Read sensor data
    sensor_t sensor;
    SERIALOUT.println("------------------------------------");
    sht31.temperature().getSensor(&sensor);
    displaySensorDetails(sensor, "C");
    SERIALOUT.println("------------------------------------");
    sht31.humidity().getSensor(&sensor);
    displaySensorDetails(sensor, "%");
    SERIALOUT.println("------------------------------------");

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

  // Read sensor using periodic mode every 5 seconds
  // -> You can see all the complexity of single shot/periodic mode is handled in the driver.
  sensors_event_t event;
  sht31.temperature().getEvent(&event);
  if (!isnan(event.temperature)) {
    SERIALOUT.print("Temperature : "); SERIALOUT.print(event.temperature,2); SERIALOUT.println(" C");
  }
  sht31.humidity().getEvent(&event);
  if (!isnan(event.relative_humidity)) {
    SERIALOUT.print("Relative Humidity : "); SERIALOUT.print(event.relative_humidity,2); SERIALOUT.println(" %");
  }
  delay(5000);
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
