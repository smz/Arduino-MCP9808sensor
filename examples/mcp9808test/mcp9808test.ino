#include <Wire.h>
#include <MCP9808sensor.h>

#define SERIAL_SPEED 57600

#define TEMP_RESOLUTION 0x03
// 0x00 = 0.5C     (tCONV = 30ms )
// 0x01 = 0.25C    (tCONV = 65ms )
// 0x02 = 0.125C   (tCONV = 130ms)
// 0x03 = 0.0625C  (tCONV = 250ms)

// Create the MCP9808 temperature sensor object
MCP9808sensor<TwoWire> tempsensor(Wire);

uint8_t samplingTime;

void setup() {
  Serial.begin(SERIAL_SPEED);
  Serial.println(F("MCP9808 demo"));
  
  // Make sure the sensor is found, you can also pass in a different i2c
  // address with tempsensor.begin(0x19) for example
  if (!tempsensor.begin())
  {
    Serial.println(F("Couldn't find MCP9808!"));
    while (true);
  }

  tempsensor.setResolution(TEMP_RESOLUTION);
  samplingTime = tempsensor.getSamplingTime();

  Serial.print(F("Manufacturer: "));
  Serial.println(tempsensor.getManufacturer());
  Serial.print(F("Device ID: "));
  Serial.println(tempsensor.getID());
  Serial.print(F("Hardware version: "));
  Serial.println(tempsensor.getRev());
  Serial.print(F("Sampling time: "));
  Serial.print(samplingTime);
  Serial.println(F("ms"));
  Serial.println(F("-----------------------"));
  
}

void loop() {
  // Read and print out the temperature
  float c = tempsensor.getTemp();
  Serial.print(F("Temp: "));
  Serial.println(c); 
  delay(samplingTime);
}
