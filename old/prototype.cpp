// Date and time functions using a DS3231 RTC connected via I2C and Wire lib
//#include <Wire.h>
#include "RTClib.h"
#include "Dial.h"

RTC_DS3231 rtc;

const int led_pin = 13;
const int interrupt_pin = 18;
volatile byte flag = false;

const int chipSelect = 10;

void blink()
{
  flag = true;
  static bool state = false;
  state = !state;
  if (state) digitalWrite(led_pin, HIGH); else digitalWrite(led_pin, LOW);
}

void interrupt_setup()
{
    pinMode(led_pin, OUTPUT);
    pinMode(interrupt_pin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(interrupt_pin), blink, RISING);
}

void rtc_setup()
{  
  if (!rtc.begin())
  {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower())
  {
    Serial.println("Realtime clock has lost power - time may be incorrect.");
  }
  
  rtc.writeSqwPinMode(DS3231_SquareWave1Hz);
  Serial.println("DS3231 Initialized.");
}

extern void nixie_setup();
extern void nixie_multiplex();
extern void nixie_bump_time();

const int dial_sense_pin = 22;
RotaryDial dial(dial_sense_pin, 20);

void setup()
{
  while (!Serial); // for Leonardo/Micro/Zero

  Serial.begin(115200);
  Serial.println();
  Serial.println();
  Serial.println("Clock Initializing...");
  Serial.println();

  delay(1000);

  interrupt_setup();
  rtc_setup();
  nixie_setup();

  pinMode(dial_sense_pin, INPUT_PULLUP);
}

extern unsigned char SECOND;
extern unsigned char MINUTE;
extern unsigned char HOUR;

void loop ()
{
#define TIME micros()
  unsigned long t = TIME;
  // unsigned long st = 0;
  const unsigned long period = 1000;
  unsigned int val = 0;

  while(!flag)
    {
      if (((TIME) - t) > period)
      {
        nixie_multiplex();
        t = (TIME);
      }
      
      val = dial.cycle();

      if (val > 0)
      {
        if (val == 10) val = 0;
        Serial.print("You dialed: ");
        Serial.println(val);
      }
    }

    flag = false;

    DateTime now = rtc.now();

    // The RTC.now interaction takes about a millisecond, so calling
    // nixi_multiplex here helps to reduce the flicker that results
    // from interrupting the multiplexing of the display.
    nixie_multiplex();

    HOUR = now.hour();
    MINUTE = now.minute();
    SECOND = now.second();
//
//    Serial.print(now.hour(), DEC);
//    Serial.print(':');
//    Serial.print(now.minute(), DEC);
//    Serial.print(':');
//    Serial.print(now.second(), DEC);
//    Serial.println();
//
}
