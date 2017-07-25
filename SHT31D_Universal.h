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

#ifndef SHT31D_UNIVERSAL_H
#define SHT31D_UNIVERSAL_H

#include <Adafruit_Sensor.h>
#include <Adafruit_SHT31.h>

#define SHT31_UNIFIED_SENSOR_VERSION 1

class SHT31_Unified {
public:
  SHT31_Unified(int32_t tempSensorId=-1, int32_t humiditySensorId=-1);
  boolean begin(uint8_t i2caddr = SHT31_DEFAULT_ADDR);
  void heater(boolean);
  uint16_t readStatus(void);

  class Temperature : public Adafruit_Sensor {
  public:
    Temperature(SHT31_Unified* parent, int32_t id);
    bool getEvent(sensors_event_t* event);
    void getSensor(sensor_t* sensor);

  private:
    SHT31_Unified* _parent;
    int32_t _id;

  };

  class Humidity : public Adafruit_Sensor {
  public:
    Humidity(SHT31_Unified* parent, int32_t id);
    bool getEvent(sensors_event_t* event);
    void getSensor(sensor_t* sensor);

  private:
    SHT31_Unified* _parent;
    int32_t _id;
  };

  Temperature temperature() {
    return _temp;
  }

  Humidity humidity() {
    return _humidity;
  }

private:
  Adafruit_SHT31 _sht31;
  Temperature _temp;
  Humidity _humidity;

};

#endif
