//-----------------------------------------------------------------------
// master-clock.cpp - Master clock code for nixie clock.

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

#include "RTClib.h"
#include "Dial.h"

// Declare some external functions we need to use.
extern void nixie_setup();
extern void nixie_multiplex();
extern void nixie_writeall();

// Realtime Clock
RTC_DS3231 rtc;

// I/O pin declarations for the RTC and its ISR
const int led_pin = 13;
const int interrupt_pin = 18;

// Flag set by the interrupt service routine. Note that this MUST be
// declared volatile to ensure that it won't be optimized away.
volatile byte isr_flag = false;

void handle_dialed_digit(int);

// Interrupt Service Routine. This function is called on the rising
// edge of the PPS (1-Pulse Per Second) signal from the RTC.

void isr()
{
    // Set the flag to indicate that the pulse has been received.
    isr_flag = true;

    static bool state = false;

    // Toggle the state tracker.
    state = !state;

    // Turn the LED on or off. This has the effect of blinking the LED
    // on every other pulse.
    if (state)
    {
	digitalWrite(led_pin, HIGH);
    }
    else
    {
	digitalWrite(led_pin, LOW);
    }
}

// Set up the ISR and associated I/O pins
void interrupt_setup()
{
    pinMode(led_pin, OUTPUT);
    pinMode(interrupt_pin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(interrupt_pin), isr, RISING);
}

// Set up the realtime clock board.
void rtc_setup()
{
    // The Begin function starts the RTC tracking, and also tells us
    // whether the RTC is present and connected.
    if (!rtc.begin())
    {
	Serial.println("Couldn't find RTC");
	while (1);
    }

    // If we detect that the RTC has lost power, print a warning that
    // the time may be wonky.
    if (rtc.lostPower())
    {
	Serial.println("Realtime clock has lost power - time may be incorrect.");
    }

    // Set up the PPS signal
    rtc.writeSqwPinMode(DS3231_SquareWave1Hz);

    // Advertise that we're ready.
    Serial.println("DS3231 Initialized.");
}

// I/O Pins for the rotary dial.
const int dial_sense_pin = 22;
const int dial_ground_pin = 24;

// The rotary dial object that we will use to read digits to alter the
// time.
RotaryDial dial(dial_sense_pin, 20);

// The main Arduino setup routine
void setup()
{
    // // Wait for the serial port to initialize.
    // while (!Serial); // for Leonardo/Micro/Zero

    // Initialize the serial port and print a startup message.
    Serial.begin(115200);
    Serial.println();
    Serial.println();
    Serial.println("Clock Initializing...");
    Serial.println();

    // Wait a sec...
    delay(1000);

    // Do the ISR setup.
    interrupt_setup();

    // Set up the realtime clock
    rtc_setup();

    // Set up the nixies
    nixie_setup();

    // Initialize the I/O pins for the rotary dial. (The Dial object
    // does not do its own setup.
    pinMode(dial_sense_pin, INPUT_PULLUP);
    pinMode(dial_ground_pin, OUTPUT);
    digitalWrite(dial_ground_pin, LOW);
}

// The variables used by the nixie code to hold the time.
extern unsigned int second;
extern unsigned int minute;
extern unsigned int hour;

// The local variables we'll use to hold the previous time, for
// purposes of comparison.
unsigned char prev_second = 0;
unsigned char prev_minute = 0;
unsigned char prev_hour = 0;

// The main Arduino event loop
void loop ()
{
#define TIME micros()

    unsigned long t = TIME;

    // How much time (in microseconds) passes between multiplexes of the
    // nixie display. By experimentation, I've found that a 1000 Hz rate
    // works well with little flicker.
    const unsigned long PERIOD = 1000;

    // This inner loop runs while waiting for the PPS interrupt to
    // occur.
    while (!isr_flag)
    {
	// If it's time to cycle the multiplexing then do so.
	if (((TIME) - t) > PERIOD)
	{
	    nixie_multiplex();
	    t = (TIME);
	}

	// Cycle the dial to check whether it's detected a digit being
	// dialed.
	unsigned int val = dial.cycle();

	// If a digit was dialed, then adjust the clock.
	if (val > 0)
	{
	    // Ten pulses corresponds to a digit of 0. Other values
	    // correspond to themselves.
	    if (val == 10) val = 0;

	    // Spit out a message, mostly for debug and diagnostic.
	    Serial.print("You dialed: ");
	    Serial.println(val);

	    // Call the function that will adjust the time.
	    handle_dialed_digit(val);
	}
    }

    // Execution reaches this point when the PPS interrupt is
    // triggered (once per second).

    // Reset the flag.
    isr_flag = false;

    // Get the current time.
    DateTime now = rtc.now();

    // Write the time out for the nixie code to see it.
    hour = now.hour();
    minute = now.minute();
    second = now.second();

    // Check to determine whether the time has actually changed. It's
    // possible for a spurious interrupt to occur when the time hasn't
    // changed.
    if ((hour != prev_hour) || (minute != prev_hour) || (second != prev_second))
    {
	// Time has changed!

	// First, record the current time
	prev_second = second;
	prev_minute = minute;
	prev_hour = hour;

	// Next, write the new time to the display
	nixie_writeall();
    }
}

// This function turns a dialed digit into an adjustment to the
// time.
void handle_dialed_digit(int digit)
{
    // The delta-t which we will determine based on which digit was
    // dialed.
    int32_t offset = 0;

    // We determine the time offset as follows: digits 1 through 5
    // will increment the time, and digits 6 through 0 decrement the
    // time.
    switch(digit)
    {
    case 1:
	// Increment the time by one second.
	offset = 1;
	break;

    case 2:
	// Increment the time by ten seconds.
	offset = 10;
	break;

    case 3:
	// Increment the time by one minute.
	offset = 60;
	break;

    case 4:
	// Increment the time by ten minutes.
	offset = 600;
	break;

    case 5:
	// Increment the time by one hour ("spring forward").
	offset = 3600;
	break;

    case 6:
	// Decrement the time by one hour ("fall back").
	offset = -3600;
	break;

    case 7:
	// Decrement the time by ten minutes.
	offset = -600;
	break;

    case 8:
	// Decrement the time by one minute.
	offset = -60;
	break;

    case 9:
	// Decrement the time by ten seconds.
	offset = -10;
	break;

    case 0:
	// Decrement the time by one second.
	offset = -1;
	break;
    }

    // Print the time adjustment for debug purposes.
    Serial.print("Time offset: ");
    Serial.println(offset);

    // If - for some reason - no offset was chosen then return now
    // without doing anything.
    if (offset == 0)
    {
	return;
    }

    // Compute an adjustment timespan offset.
    TimeSpan offset_timespan(offset);

    // Get the current time.
    DateTime now = rtc.now();

    // Offset the current time by the adjustment offset.
    DateTime new_now = now + offset_timespan;

    // Write the new time out to the realtime clock.
    RTC_DS3231::adjust(new_now);
}
