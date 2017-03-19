#ifndef __MCP9808sensor__
#define __MCP9808sensor__

#define MCP9808_I2CADDR_DEFAULT        0x18

#define MCP9808_REG_CONFIG             0x01
#define MCP9808_REG_UPPER_TEMP         0x02
#define MCP9808_REG_LOWER_TEMP         0x03
#define MCP9808_REG_CRIT_TEMP          0x04
#define MCP9808_REG_AMBIENT_TEMP       0x05
#define MCP9808_REG_MANUF_ID           0x06
#define MCP9808_REG_DEVICE_ID          0x07
#define MCP9808_REG_RESOLUTION         0x08

#define MCP9808_REG_CONFIG_SHUTDOWN    0x0100
#define MCP9808_REG_CONFIG_CRITLOCKED  0x0080
#define MCP9808_REG_CONFIG_WINLOCKED   0x0040
#define MCP9808_REG_CONFIG_INTCLR      0x0020
#define MCP9808_REG_CONFIG_ALERTSTAT   0x0010
#define MCP9808_REG_CONFIG_ALERTCTRL   0x0008
#define MCP9808_REG_CONFIG_ALERTSEL    0x0002
#define MCP9808_REG_CONFIG_ALERTPOL    0x0002
#define MCP9808_REG_CONFIG_ALERTMODE   0x0001


template<class T_WIRE_METHOD> class MCP9808sensor
{


public:

  MCP9808sensor(T_WIRE_METHOD& wire) :
    _wire(wire)
  {
  }


  bool begin(uint8_t addr = MCP9808_I2CADDR_DEFAULT)
  {
    _wire.begin();
    _i2caddr = addr;
    if (getManufacturer() != 0x0054) return false;
    if (getID() != 0x04) return false;
    return true;
  }


  //************************************************************************/
  // Returns the temperature as degrees Celsius
  //************************************************************************/

  float getTemp( void )
  {
    union mcp9808Register
    {
      uint16_t reg;
      int temp;
      struct {uint8_t lo; uint8_t hi;} byte; 
    } ta;
    
    ta.reg = read16(MCP9808_REG_AMBIENT_TEMP);

    ta.byte.hi &= 0x1F;
    
    if (ta.temp > 4095)
    {
      ta.temp -= 8192;
    }

    return ta.temp / 16.0;

  }


  //************************************************************************/
  // Returns the temperature as degrees Fahrenheit
  //************************************************************************/

  float getTempF( void )
  {
    return getTemp() * 1.8 + 32.0;
  }


  //************************************************************************/
  // Returns the temperature as degrees Celsius
  // D E P R E C A T E D  !
  // This function is mantained for compatibility with the Adafruit library.
  // Use the getTemp() method instead!!
  //************************************************************************/

  float readTempC( void )
  {
    return getTemp();
  }


  //************************************************************************/
  // Reads the 8-bit resolution register:
  //    00 = 0.5C     (tCONV = 30ms  typical)
  //    01 = 0.25C    (tCONV = 65ms  typical)
  //    10 = 0.125C   (tCONV = 130ms typical)
  //    11 = 0.0625C  (tCONV = 250ms typical)
  //************************************************************************/

  uint8_t getResolution( void )
  {
    uint8_t res = read8(MCP9808_REG_RESOLUTION) & (uint8_t) 0x03;
    return res;
  }


  //************************************************************************/
  // Set the 8-bit resolution register:
  //    00 = 0.5C     (tCONV = 30ms  typical)
  //    01 = 0.25C    (tCONV = 65ms  typical)
  //    10 = 0.125C   (tCONV = 130ms typical)
  //    11 = 0.0625C  (tCONV = 250ms typical)
  //************************************************************************/

  void setResolution( uint8_t res)
  {
    write8(MCP9808_REG_RESOLUTION, res & (uint8_t) 0x03);
  }


  //************************************************************************/
  // Returns the typical sampling time in milliseconds for current resolution
  //    00 = 0.5C     (tCONV = 30ms  typical)
  //    01 = 0.25C    (tCONV = 65ms  typical)
  //    10 = 0.125C   (tCONV = 130ms typical)
  //    11 = 0.0625C  (tCONV = 250ms typical)
  //************************************************************************/

  uint8_t getSamplingTime( void )
  {
    switch (getResolution())
    {
      case 0:
        return 30;
      case 1:
        return 65;
      case 2:
        return 130;
      case 3:
        return 250;
    }
    return 0;  // Sugar...
  }


  //************************************************************************/
  // Reads the 16-bit "Device ID and revision register"
  // and returns the device ID (should be 0x04)
  //************************************************************************/

  uint8_t getID( void )
  {
     union mcp9808Register
    {
      uint16_t reg;
      struct {uint8_t lo; uint8_t hi;} byte; 
    } id;

    id.reg = read16(MCP9808_REG_DEVICE_ID);

    return id.byte.hi;
  }


  //************************************************************************/
  // Reads the 16-bit "Device ID and revision register"
  // and returns the device revision (0x00 = 1st revision)
  //************************************************************************/

  uint8_t getRev( void )
  {
     union mcp9808Register
    {
      uint16_t reg;
      struct {uint8_t lo; uint8_t hi;} byte; 
    } id;

    id.reg = read16(MCP9808_REG_DEVICE_ID);

    return id.byte.lo;
  }


  //************************************************************************/
  // Reads the 16-bit "Manufacturer ID register"
  // and returns the device revision (should be 0x0054)
  //************************************************************************/

  uint16_t getManufacturer( void )
  {
    return read16(MCP9808_REG_MANUF_ID);
  }


  //*************************************************************************
  // Shutdown the sensor (0.1 uA current)
  //*************************************************************************

  void shutdown( void )
  {
    write16(MCP9808_REG_CONFIG, read16(MCP9808_REG_CONFIG) | MCP9808_REG_CONFIG_SHUTDOWN);
  }


  //*************************************************************************
  // Wake up the sensor (200 uA current)
  //*************************************************************************

  void wakeup( void )
  {
    write16(MCP9808_REG_CONFIG, read16(MCP9808_REG_CONFIG) ^ MCP9808_REG_CONFIG_SHUTDOWN);
  }


  //*************************************************************************
  // Set Sensor to Shutdown-State or wake up (Conf_Register BIT8)
  // 1 = shutdown / 0 = wake up
  // D E P R E C A T E D  !
  // This function is mantained for compatibility with the Adafruit library.
  // Use the shutdown() and wakeup() methods instead!!
  //*************************************************************************

  int shutdown_wake( uint8_t sw_ID )
  {
    uint16_t conf_register = read16(MCP9808_REG_CONFIG);
    
    switch (sw_ID)
    {
      case 0:
        conf_register = conf_register ^ MCP9808_REG_CONFIG_SHUTDOWN ;
        break;
      case 1:
        conf_register = conf_register | MCP9808_REG_CONFIG_SHUTDOWN ;
        break;
    }

    write16(MCP9808_REG_CONFIG, conf_register);

    return 0;
  }



private:

  T_WIRE_METHOD& _wire;
  uint8_t _i2caddr;


  //**************************************************************************/
  // Low level I/O functions
  //**************************************************************************/

  void write8(uint8_t reg, uint8_t value)
  {
    _wire.beginTransmission(_i2caddr);
    _wire.write((uint8_t)reg);
    _wire.write(value);
    _wire.endTransmission();
  }

  uint8_t read8(uint8_t reg)
  {
    uint8_t val;

    _wire.beginTransmission(_i2caddr);
    _wire.write((uint8_t)reg);
    _wire.endTransmission();

    _wire.requestFrom((uint8_t)_i2caddr, (uint8_t) 1);
    val = _wire.read();
    return val;
  }

  void write16(uint8_t reg, uint16_t value)
  {
    _wire.beginTransmission(_i2caddr);
    _wire.write((uint8_t)reg);
    _wire.write(value >> 8);
    _wire.write(value & 0xFF);
    _wire.endTransmission();
  }

  uint16_t read16(uint8_t reg)
  {
    uint16_t val;

    _wire.beginTransmission(_i2caddr);
    _wire.write((uint8_t)reg);
    _wire.endTransmission();

    _wire.requestFrom((uint8_t)_i2caddr, (uint8_t)2);
    val = _wire.read();
    val <<= 8;
    val |= _wire.read();
    return val;
  }

};

#endif // __MCP9808sensor__
