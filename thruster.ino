// -*- mode: c -*-
//
// Asteroids Lunar Lander thrust controller A/D converter
// for Braze Asteroids Multigame
//
// thruster.ino
//
// Copyright 2019 dfnum2@mail.com
//
// This program is free software: you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
// details.
//
// You should have received a copy of the GNU General Public License along with
// this program. If not, see <https://www.gnu.org/licenses/>.
//
// Source code at:
//
// https://github.com/dfnr2/asteroids-lunar-multigame-thrust-a2d


// Wiring Information:
// Chip: ATMega328P, Arduino mini or Arduino Nano
//
// PIN     NAME   FUNCTION
// 3       RST    Power connection to the 10-pin header.  RST and 5V are connected
//                through the potentiometer cable, as shown in the simulation.
//
// 4       GND    GND connection on the 10-pin header.
//
// 5-12    D2-D9  Connected to D0-D7 on the 10-pin header.
//
// 26      A7     Analog in.  Connected to the wiper on the thruster potentiometer
//
// 27      5V     Power.  Connected to RST (which is connected to +5 on the 10-pin
//                header) to power the board.  Also provides 5V to the high side of
//                The thruster potentiometer
//
// 28      RST    This is connected to ping 27 (5V) to power the device.
//
// 29      GND    Provides 0V to the potentiometer

#include <stdint.h>
#include <EEPROM.h>

uint16_t thrust_pin = A7;

// The high 2 bits of the output byte map to PORTD1:2
// So shift right 6 bits
const uint8_t himask = 0xc0;
#define HIBITS(x) (((x) & himask) >> 6)  // Port B0-B1 -> Pin D8 - Pin D9

// The low 6 bits of the output byte map to PORTB2:7
// So shift left 2 bits
const uint8_t lomask = 0x3f;
#define LOBITS(x) (((x) & lomask) << 2)  // Port D2-D7 -> Pin D2 - Pin D7

// 10-bit A/D range
const uint16_t analog_min = 0;
const uint16_t analog_max = 1023;

// From Scott Brasington, the range should be 104-167 (0x68 - 0xa7)
const uint8_t min_thrust = 0x68;
const uint8_t max_thrust = 0xa7;

// eeprom addresses to store low and high values
uint16_t lo_val_addr = 0;
uint16_t hi_val_addr = lo_val_addr+sizeof(uint16_t);

// The lowest and hightest values.  Initialize to 0 at time of programming.
uint16_t lo_val = 0;
uint16_t hi_val = 0;

void setup() {
    // Retrieve the high and low values from EEPROM.
    lo_val = EEPROM.read(lo_val_addr);
    hi_val = EEPROM.read(hi_val_addr);

    // If both are equal (from being freshly programmed), then initialize both
    // to guarantee that both will be set as the thruster is used through its
    // range.
    if (lo_val == hi_val) {
        lo_val = analog_max;
        hi_val = 0;
        EEPROM.put(lo_val_addr, lo_val);
        EEPROM.put(hi_val_addr, hi_val);
    }

    // Set up digital registers;
    DDRD = LOBITS(0xff);
    DDRB = HIBITS(0xff);
}

void out_byte(uint8_t byte)
{
    uint8_t lo_part = LOBITS(byte);
    uint8_t hi_part = HIBITS(byte);

    PORTD = lo_part;
    PORTB = hi_part;
}

void store_hi_lo_val(uint8_t val)
{
    if (val < lo_val) {
        lo_val = val;
        EEPROM.put(lo_val_addr, lo_val);
    } else if (val > hi_val) {
        hi_val = val;
        EEPROM.put(lo_val_addr, hi_val);
    }
}

void loop() {
    uint16_t val = analogRead(thrust_pin);  // read the input pin

    uint8_t thruster_out = map(val, analog_min, analog_max,
                               min_thrust, max_thrust);

    store_hi_lo_val(val);

    out_byte(thruster_out);
}
