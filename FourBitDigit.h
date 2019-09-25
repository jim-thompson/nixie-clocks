//-----------------------------------------------------------------------
// FourBitDigit - A simple class that represents a small integer value
// with a set of four output bits/pins.

// Copyright (c) 2017 Jim Thompson.

//-----------------------------------------------------------------------
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <Arduino.h>

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
    // Set a pin high or low depending on the corresponding bit in the
    // value.
    void maybeWritePin(unsigned int pin, unsigned int mask)
    {
	if ((_value & mask) == 0)
        {
    	    digitalWrite(pin, LOW);
	}
	else
	{
	    digitalWrite(pin, HIGH);
	}
    }

    // The value represented by this object.
    unsigned int _value;

    // The four output pins that correspond to the four bits in the
    // represented value.
    const unsigned int _bit_a_pin;
    const unsigned int _bit_b_pin;
    const unsigned int _bit_c_pin;
    const unsigned int _bit_d_pin;
};
