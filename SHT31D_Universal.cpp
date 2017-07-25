// Sensirion SHT32-D Temperature & Humidity Unified Sensor Library
// Unified Sensor wrapper written by Tim Jacobs in 2017
// Original code by Adafruit -- https://github.com/adafruit/Adafruit_SHT31

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "SHT31D_Universal.h"

// -----------------------------------------------------------------------------
// CONSTRUCTOR
// -----------------------------------------------------------------------------

SHT31_Unified::SHT31_Unified(int32_t tempSensorId, int32_t humiditySensorId):
  _temp(this, tempSensorId),
  _humidity(this, humiditySensorId)
{}

// -----------------------------------------------------------------------------
// PASS-THROUGH METHODS
// -----------------------------------------------------------------------------

boolean SHT31_Unified::begin(uint8_t i2caddr) {
  return _sht31.begin(i2caddr);
}

void SHT31_Unified::heater(boolean status) {
  _sht31.heater(status);
}

uint16_t SHT31_Unified::readStatus(void) {
  return _sht31.readStatus();
}

// -----------------------------------------------------------------------------
// TEMPERATURE SUBCLASS
// -----------------------------------------------------------------------------

SHT31_Unified::Temperature::Temperature(SHT31_Unified* parent, int32_t id):
  _parent(parent),
  _id(id)
{}

bool SHT31_Unified::Temperature::getEvent(sensors_event_t* event) {
  // Clear event definition.
  memset(event, 0, sizeof(sensors_event_t));
  // Populate sensor reading values.
  event->version     = sizeof(sensors_event_t);
  event->sensor_id   = _id;
  event->type        = SENSOR_TYPE_AMBIENT_TEMPERATURE;
  event->timestamp   = millis();
  event->temperature = _parent->_sht31.readTemperature();
  return true;
}

void SHT31_Unified::Temperature::getSensor(sensor_t* sensor) {
  // Clear sensor definition.
  memset(sensor, 0, sizeof(sensor_t));
  // Set sensor name.
  strncpy(sensor->name, "SHT31 Temp", sizeof(sensor->name) - 1);
  sensor->name[sizeof(sensor->name)- 1] = 0;
  // Set version and ID
  sensor->version         = SHT31_UNIFIED_SENSOR_VERSION;
  sensor->sensor_id       = _id;
  // Set type and characteristics.
  sensor->type            = SENSOR_TYPE_AMBIENT_TEMPERATURE;
  sensor->min_delay       = 1000000L;  // 1 second (in microseconds) -- not sure about this, just set it to 1s
  sensor->max_value       = 125.0F;
  sensor->min_value       = -40.0F;
  sensor->resolution      = 0.015F;
}

// -----------------------------------------------------------------------------
// HUMIDITY SUBCLASS
// -----------------------------------------------------------------------------

SHT31_Unified::Humidity::Humidity(SHT31_Unified* parent, int32_t id):
  _parent(parent),
  _id(id)
{}

bool SHT31_Unified::Humidity::getEvent(sensors_event_t* event) {
  // Clear event definition.
  memset(event, 0, sizeof(sensors_event_t));
  // Populate sensor reading values.
  event->version           = sizeof(sensors_event_t);
  event->sensor_id         = _id;
  event->type              = SENSOR_TYPE_RELATIVE_HUMIDITY;
  event->timestamp         = millis();
  event->relative_humidity = _parent->_sht31.readHumidity();
  return true;
}

void SHT31_Unified::Humidity::getSensor(sensor_t* sensor) {
  // Clear sensor definition.
  memset(sensor, 0, sizeof(sensor_t));
  // Set sensor name.
  strncpy(sensor->name, "SHT31 RHum", sizeof(sensor->name) - 1);
  sensor->name[sizeof(sensor->name)- 1] = 0;
  // Set version and ID
  sensor->version         = SHT31_UNIFIED_SENSOR_VERSION;
  sensor->sensor_id       = _id;
  // Set type and characteristics.
  sensor->type            = SENSOR_TYPE_RELATIVE_HUMIDITY;
  sensor->min_delay       = 1000000L;  // 1 second (in microseconds) -- not sure about this, just set it to 1s
  sensor->max_value       = 100.0F;
  sensor->min_value       = 0.0F;
  sensor->resolution      = 0.01F;
}
