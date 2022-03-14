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
//#define NANO 1
#define MINIPRO 1

#if defined(NANO)
#  define THRUST_PIN_DEF A7
#  define EEPROM_CLEAR_1_DEF A4
#  define EEPROM_CLEAR_2_DEF A5
#elif defined(MINIPRO)
#  define THRUST_PIN_DEF A3
#  define EEPROM_CLEAR_1_DEF A0
#  define EEPROM_CLEAR_2_DEF A1
#else
#  error No board selected
#endif

const uint16_t thrust_pin = THRUST_PIN_DEF;
const uint16_t eeprom_clear1 = EEPROM_CLEAR_1_DEF;
const uint16_t eeprom_clear2 = EEPROM_CLEAR_2_DEF;
const uint8_t reset_blink_delay = 200; // msec
const uint8_t num_blinks_on_eeprom_clear = 4;

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
const uint16_t analog_middle = ((uint16_t) (((uint32_t) analog_max + (uint32_t) analog_min) / 2));
// From Scott Brasington, the range should be 104-167 (0x68 - 0xa7)
const uint8_t min_thrust = 0x68;
const uint8_t max_thrust = 0xa7;

// eeprom addresses to store low and high values
uint16_t lo_val_addr = 0;
uint16_t hi_val_addr = lo_val_addr + sizeof(uint16_t);

// The lowest and hightest values.  Initialize to 0 at time of programming.
uint16_t lo_adc_val = analog_middle;
uint16_t hi_adc_val = analog_middle;

void blink_led(void)
{
  pinMode(LED_BUILTIN, OUTPUT);

  for (uint8_t i = 0; i < num_blinks_on_eeprom_clear; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(reset_blink_delay);
    digitalWrite(LED_BUILTIN, LOW);
    delay(reset_blink_delay);
  }
}

void setup() {
  uint8_t eeprom_reset_request = 0;

  // Check if EEPROM reset is requested. Make sure the two EEPROM_CLEAR pins are
  // connected by testing that values written to the eeprom_clear1 pin are read
  // back on the eeprom_clear2 pin:

  pinMode(eeprom_clear1, OUTPUT);
  pinMode(eeprom_clear2, INPUT_PULLUP);
  digitalWrite(eeprom_clear1, HIGH);
  eeprom_reset_request = digitalRead(eeprom_clear2);
  digitalWrite(eeprom_clear1, LOW);
  eeprom_reset_request = eeprom_reset_request && !digitalRead(eeprom_clear2);

  // Retrieve the high and low values stored in EEPROM.

  EEPROM.get(lo_val_addr, lo_adc_val);
  EEPROM.get(hi_val_addr, hi_adc_val);


  // If the maximum are equal (e.g., a new or freshly programmed device), then
  // initialize both to guarantee that both will be set as the thruster is used
  // through its range.
  //
  // Also, if an EEPROM clear is requested, by connecting the EEPROM_CLEAR1 and
  // EEPROM_CLEAR2 pins, then re-initialize the high and low adc values.
  if (eeprom_reset_request || (lo_adc_val == hi_adc_val)) {
    lo_adc_val = analog_middle;
    hi_adc_val = analog_middle;
    EEPROM.put(lo_val_addr, lo_adc_val);
    EEPROM.put(hi_val_addr, hi_adc_val);

    // indicate EEPROM reset by blinking LEDS
    blink_led();
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

void store_hi_lo_val(uint16_t val)
{
  if (val < lo_adc_val) {
    lo_adc_val = val;
    EEPROM.put(lo_val_addr, lo_adc_val);
  } else if (val > hi_adc_val) {
    hi_adc_val = val;
    EEPROM.put(hi_val_addr, hi_adc_val);
  }
}

void loop() {
  uint16_t val = analogRead(thrust_pin);  // read the input pin

  store_hi_lo_val(val);

  uint8_t thruster_out = map(val, lo_adc_val, hi_adc_val,
                             min_thrust, max_thrust);

  out_byte(thruster_out);
}
