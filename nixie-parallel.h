#include <Arduino.h>

extern unsigned char SECOND;
extern unsigned char MINUTE;
extern unsigned char HOUR;

class FourBitDigit
{
public:
  FourBitDigit(unsigned int bit_a_pin, unsigned int bit_b_pin, unsigned int bit_c_pin, unsigned int bit_d_pin)
    : _bit_a_pin(bit_a_pin),
      _bit_b_pin(bit_b_pin),
      _bit_c_pin(bit_c_pin),
      _bit_d_pin(bit_d_pin)
  {
    pinMode(_bit_a_pin, OUTPUT);
    pinMode(_bit_b_pin, OUTPUT);
    pinMode(_bit_c_pin, OUTPUT);
    pinMode(_bit_d_pin, OUTPUT);

    // digitalWrite(_bit_a_pin, LOW);
    // digitalWrite(_bit_b_pin, LOW);
    // digitalWrite(_bit_c_pin, LOW);
    // digitalWrite(_bit_d_pin, LOW);
  }

  void setValue(unsigned int value)
  {
    _value = value;
  }

  void writePins()
  {
    maybeWritePin(_bit_a_pin, 1);
    maybeWritePin(_bit_b_pin, 2);
    maybeWritePin(_bit_c_pin, 4);
    maybeWritePin(_bit_d_pin, 8);
  }
  
private:
  void maybeWritePin(unsigned int pin, unsigned int mask)
  {
    // Serial.print("Maybe writing pin");
    // Serial.println(pin);
    
    if ((_value & mask) == 0)
      {
	digitalWrite(pin, LOW);
      }
    else
      {
	digitalWrite(pin, HIGH);
      }
  }
  
  unsigned int _value;
  
  const unsigned int _bit_a_pin;
  const unsigned int _bit_b_pin;
  const unsigned int _bit_c_pin;
  const unsigned int _bit_d_pin;
};
