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
// PIN     NAME   HEADER PIN   FUNCTION
// 3       RST    1            Power connection to the 10-pin header.  RST and 5V are connected
//                             through the potentiometer cable, as shown in the simulation.
//
// 4       GND    2            GND connection on the 10-pin header.
//
//                             Note: The header directly maps to the pokey pins, which swaps
//                             each pair of bits, so that each pair of bits in the header is
//                             swapped.  So outputs D2-D9 don't correspond directly to bits 0-7
//                             of the POKEY.  THe odd and even bits are swapped in software.
//
// 5       D2     3            POKEY D1
// 6       D3     4            POKEY D0
// 7       D4     5            POKEY D3
// 8       D5     6            POKEY D2
// 9       D6     7            POKEY D5
// 10      D7     8            POKEY D4
// 11      D8     9            POKEY D7
// 12      D9     10           POKEY D6
//
//         On the arduino nano, the A7 pin is adjacent to the +5 pin. On the
//         mini pro, the A3 pin is adjacent to the +5 pin. Using A7 on the nano
//         and A3 on the mini pro permits a simple 4-pin connector to the
//         potentiometer, with the RESET and +5 pins both soldered to the high
//         side lug of the potentiometer.

// 22      A7                  Analog in (nano) Connected to the wiper on the
//                             thruster potentiometer
//
// 26      A3                  Analog in.(mini pro) Connected to the wiper on
//                             the thruster potentiometer
//
// 27      5V                  Power.  Connected to pin 28 (RST) (which is connected to pin 1 (+5)
//                             on the 10-pin header) to power the board.  Also provides 5V to the
//                             high side of the thruster potentiometer.
//
// 28      RST                 This is connected to pin 27 (5V) to power the device.
//
// 29      GND                 Provides 0V to the potentiometer

#include <stdint.h>
#include <EEPROM.h>

// Uncomment only one of the following two lines to configure for arduino nano
// or mini pro:
uint16_t thrust_pin = A3; // uncomment this line for arduino mini pro
// uint16_t thrust_pin = A7; // uncomment this line for arduino nano

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

    // swap each pair of bits to match POKEY:
    // D7:D0 <- D6 D7, D4, D5, D2, D3, D0, D1
    uint8_t odd_bits = (byte & 0x55) << 1;
    uint8_t even_bits = (byte & 0xaa) >> 1;

    uint8_t swapped_byte = odd_bits | even_bits;

    uint8_t lo_part = LOBITS(swapped_byte);
    uint8_t hi_part = HIBITS(swapped_byte);

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
