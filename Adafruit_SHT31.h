/***************************************************
  This is a library for the SHT31 Digital Humidity & Temp Sensor

  Designed specifically to work with the SHT31 Digital sensor from Adafruit
  ----> https://www.adafruit.com/products/2857

  These sensors use I2C to communicate, 2 pins are required to interface
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution

  Interrupt/Alert additions by Tim Jacobs for CegekaLabs (2017)
 ****************************************************/

#if (ARDUINO >= 100)
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif
#include "Wire.h"

/**************************************************************************
 **************************************************************************
 **************************************************************************
 * Addresses & commands
 **************************************************************************
 **************************************************************************
 **************************************************************************/

// Nr of milliseconds to wait for measurement to complete
// -- Adafruit default: 500 (!)
// -- Datasheet: measurement should complete in 15 ms.
#define SHT31_MEASUREMENTDELAY     50

// Returns previous measurement result in case new measurement requested within
// this amount of milliseconds.
#define SHT31_MEASUREMENTCACHETTL  100

#define SHT31_DEFAULT_ADDR         0x44
#define SHT31_READSTATUS           0xF32D
#define SHT31_CLEARSTATUS          0x3041
#define SHT31_SOFTRESET            0x30A2
#define SHT31_HEATEREN             0x306D
#define SHT31_HEATERDIS            0x3066

// Single Shot Measurement commands
#define SHT31_MEAS_HIGHREP_STRETCH 0x2C06
#define SHT31_MEAS_MEDREP_STRETCH  0x2C0D
#define SHT31_MEAS_LOWREP_STRETCH  0x2C10
#define SHT31_MEAS_HIGHREP         0x2400
#define SHT31_MEAS_MEDREP          0x240B
#define SHT31_MEAS_LOWREP          0x2416

// Periodic Mode Measurement commands
#define SHT31_MEAS_HALFHZ_HIGHREP   0x2032
#define SHT31_MEAS_HALFHZ_MEDREP    0x2024
#define SHT31_MEAS_HALFHZ_LOWREP    0x202F
#define SHT31_MEAS_1HZ_HIGHREP      0x2130
#define SHT31_MEAS_1HZ_MEDREP       0x2126
#define SHT31_MEAS_1HZ_LOWREP       0x212D
#define SHT31_MEAS_2HZ_HIGHREP      0x2236
#define SHT31_MEAS_2HZ_MEDREP       0x2220
#define SHT31_MEAS_2HZ_LOWREP       0x222B
#define SHT31_MEAS_4HZ_HIGHREP      0x2334
#define SHT31_MEAS_4HZ_MEDREP       0x2322
#define SHT31_MEAS_4HZ_LOWREP       0x2329
#define SHT31_MEAS_10HZ_HIGHREP     0x2737
#define SHT31_MEAS_10HZ_MEDREP      0x2721
#define SHT31_MEAS_10HZ_LOWREP      0x272A
#define SHT31_MEAS_FETCH            0xE000
#define SHT31_MEAS_BREAK            0x3093
#define SHT31_MEAS_ART              0x2B32

// Alerting limit commands
#define SHT31_ALERT_READ_HIGHSET    0xE11F
#define SHT31_ALERT_READ_HIGHCLR    0xE114
#define SHT31_ALERT_READ_LOWSET     0xE109
#define SHT31_ALERT_READ_LOWCLR     0xE102
#define SHT31_ALERT_WRITE_HIGHSET   0x611D
#define SHT31_ALERT_WRITE_HIGHCLR   0x6116
#define SHT31_ALERT_WRITE_LOWSET    0x610B
#define SHT31_ALERT_WRITE_LOWCLR    0x6100

/**************************************************************************
 **************************************************************************
 **************************************************************************
 * Enums, structs & typedefs
 **************************************************************************
 **************************************************************************
 **************************************************************************/

typedef union {
	uint16_t rawStatus;
	struct {
		uint16_t WriteDataChecksum : 1;
		uint16_t Command : 1;
		uint16_t Reserved0 : 2;
		uint16_t SystemReset : 1;
		uint16_t Reserved1 : 5;
		uint16_t T_TrackingAlert : 1;
		uint16_t RH_TrackingAlert : 1;
		uint16_t Reserved2 : 1;
		uint16_t HeaterStatus : 1;
		uint16_t Reserved3 : 1;
		uint16_t AlertPending : 1;
	};
} Adafruit_SHT31D_Status;

struct Adafruit_SHT31D_AlertValues {
	float temperatureSet;
  float temperatureClear;
  float humiditySet;
  float humidityClear;
};

typedef enum {
	REPEATABILITY_HIGH,
	REPEATABILITY_MEDIUM,
	REPEATABILITY_LOW,
} Adafruit_SHT31D_Repeatability;

typedef enum {
	MODE_CLOCK_STRETCH,
	MODE_POLLING,
} Adafruit_SHT31D_SingleShotMode;

typedef enum {
	FREQUENCY_HALFHZ,
	FREQUENCY_1HZ,
	FREQUENCY_2HZ,
	FREQUENCY_4HZ,
	FREQUENCY_10HZ
} Adafruit_SHT31D_PeriodicFrequency;

/**************************************************************************
 **************************************************************************
 **************************************************************************
 * SHT31 Class
 **************************************************************************
 **************************************************************************
 **************************************************************************/

class Adafruit_SHT31 {
 public:
  Adafruit_SHT31();
  boolean begin(uint8_t i2caddr = SHT31_DEFAULT_ADDR);
  float readTemperature(bool force = false);
  float readHumidity(bool force = false);
  uint16_t readStatus(void);
  void reset(void);
  void heater(boolean);
  uint8_t crc8(const uint8_t *data, int len);

  // Periodic measurement mode commands
  void enablePeriodic(Adafruit_SHT31D_Repeatability rep, Adafruit_SHT31D_PeriodicFrequency freq);
  void stopPeriodic();

  // Alert & status checks
  boolean isHeaterOn();
  boolean isAlertReset();
  boolean isAlertHumidity();
  boolean isAlertTemperature();
  boolean isAlertPending();

  // Threshold Configuration
  void setAlertLimitsLow(Adafruit_SHT31D_AlertValues values);
  void setAlertLimitsHigh(Adafruit_SHT31D_AlertValues values);
  Adafruit_SHT31D_AlertValues readAlertLimitsLow();
  Adafruit_SHT31D_AlertValues readAlertLimitsHigh();

  // Clear status register
  void clearStatus();

 private:
  boolean readTempHum(bool force = false);
  uint16_t readAlertData(uint16_t command);

  void writeCommand(uint16_t cmd);
  void writeCommandData(uint16_t cmd, uint16_t data);
  float rawToTemperature(uint16_t rawvalue);
  float rawToHumidity(uint16_t rawvalue);
  uint16_t encodeAlert(float temp, float hum);
  uint16_t temperatureToRaw(double temperature);
  uint16_t humidityToRaw(double humidity);

  // Current measurement mode & parameters
  bool _periodic;
  uint32_t _lastreadtime;
  Adafruit_SHT31D_Repeatability _rep;
  Adafruit_SHT31D_SingleShotMode _ssmode;
  Adafruit_SHT31D_PeriodicFrequency _freq;
  void startPeriodic();


  Adafruit_SHT31D_Status _status;   // Last register status read
  uint8_t _i2caddr;
  //boolean readData(void);         // Not implemented, remnant of Sensirion example code :)
  float humidity, temp;
};
