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
// Chip: ATMega328P, 32-pin TQFP
// Note: Pin numbers indicate the Atmega328P pin number, not the arduino board
//       pin.

// PIN     NAME   HEADER PIN   FUNCTION
// 29      RST    1            Power connection to the 10-pin header.  RST and 5V are connected
//                             through the potentiometer cable, as shown in the simulation.
//
//         GND    2            GND connection on the 10-pin header.
//
//                             Note: The header directly maps to the pokey pins, which swaps
//                             each pair of bits, so that each pair of bits in the header is
//                             swapped.  So outputs D2-D9 don't correspond directly to bits 0-7
//                             of the POKEY.  THe odd and even bits are swapped in software.
//
// 32      D2     3            POKEY D1
// 1       D3     4            POKEY D0
// 2       D4     5            POKEY D3
// 9       D5     6            POKEY D2
// 10      D6     7            POKEY D5
// 11      D7     8            POKEY D4
// 12      B0     9            POKEY D7
// 13      B1     10           POKEY D6
//
//         On the arduino nano, the A7 pin is adjacent to the +5 pin. On the
//         mini pro, the A3 pin is adjacent to the +5 pin. Using A7 on the nano
//         and A3 on the mini pro permits a simple 4-pin connector to the
//         potentiometer, with the RESET and +5 pins both soldered to the high
//         side lug of the potentiometer.

// **************** The following pins are used for the Nano ************************
// 22      ADC7                Analog in.(mini pro) Connected to the wiper on
//                             the thruster potentiometer
//
// 28      C5                  EEPROM_CLEAR2: (input)
// 27      C4                  EEPROM_CLEAR1: (output)
//                             EEPROM is cleared if these lines are shorted at startup
// ***********************************************************************************
//
// **************** The following pins are used for the Mini Pro *********************
// 26      C3                  Analog in.(mini pro) Connected to the wiper on
//                             the thruster potentiometer
//
// 24      C1                  EEPROM_CLEAR2: (input)
// 23      C0                  EEPROM_CLEAR1: (output)
//                             EEPROM is cleared if these lines are shorted at startup
// ***********************************************************************************
//
//
//
// Arduino Nano pinout:
//
//                                       +-----+
//                          +------------| USB |------------+
//                          |            +-----+            |
//                     B5   | [ ]D13/SCK        MISO/D12[ ] |   B4
//                          | [ ]3.3V           MOSI/D11[+]~|   B3     Muligame 10-pin header
//                          | [ ]V.ref     ___    SS/D10[+]~|   B2         +------+
//                     C0   | [ ]A0       / N \       D9[+]~|   B1---------|10    |
//                     C1   | [ ]A1      /  A  \      D8[+] |   B0---------|9     |
//                     C2   | [ ]A2      \  N  /      D7[+] |   D7---------|8     |
//                     C3   | [ ]A3       \_0_/       D6[+]~|   D6---------|7     |
//       EEPROM_CLEAR1 -----| [*]A4/SDA               D5[+]~|   D5---------|6     |
//       EERPOM_CLEAR2 -----| [*]A5/SCL               D4[+] |   D4---------|5     |
//                          | [ ]A6              INT1/D3[+]~|   D3---------|4     |
//       pot-wiper ---------| [+]A7              INT0/D2[+] |   D2---------|3     |
//       pot-HI -----+------| [+]5V                  GND[+] |     ---------|2     |
//                   +------| [+]RST                 RST[+] |   C6---------|1     |
//       pot-LO ------------| [+]GND   5V MOSI GND   TX1[ ] |   D0         +------+
//                          | [ ]Vin   [ ] [ ] [ ]   RX1[ ] |   D1
//                          |          [ ] [ ] [ ]          |
//                          |          MISO SCK RST         |
//                          | NANO-V3                       |
//                          +-------------------------------+
//
//    Notes:
//      1)  '+': Pin is connected
//      2)  '*': Short these pins to reset the EEPROM potentiometer calibration
//      3)  Note, RST and POT-HI are shorted together in the potentiometer cable
//
//
// Arduino Mini Pro pinout:
//
//                                                 D0   D1   RST
//                                  GND  GND  VCC  RX   TX   /DTR
//                               +--------------------------------+
//                               |  [ ]  [ ]  [ ]  [ ]  [ ]  [ ]  |
//                               |              FTDI              |
// Muligame 10-pin header    D1  | [ ]1/TX                 RAW[ ] |
//        +------+           D0  | [ ]0/RX                 GND[+] |----------pot-LO
//        |     1|-----------    | [+]RST        SCL/A5[ ] RST[+] |-----+
//        |     2|-----------    | [+]GND        SDA/A4[ ] VCC[+] |-----+----pot-HI
//        |     3|-----------D2  | [+]2/INT0    ___         A3[+] |----------pot-wiper
//        |     4|-----------D3  |~[+]3/INT1   /   \        A2[ ] |   C2
//        |     5|-----------D4  | [+]4       /PRO  \       A1[*] |----------EEPROM_CLEAR2
//        |     6|-----------D5  |~[+]5       \ MINI/       A0[*] |----------EEPROM_CLEAR1
//        |     7|-----------D6  |~[+]6        \___/    SCK/13[ ] |   B5
//        |     8|-----------D7  | [+]7          A7[ ] MISO/12[ ] |   B4
//        |     9|-----------B0  | [+]8          A6[ ] MOSI/11[ ]~|   B3
//        |    10|-----------B1  |~[+]9                  SS/10[ ]~|   B2
//        +------+               |           [RST-BTN]            |
//                               +--------------------------------+
//
//    Notes:
//      1)  '+': Pin is connected
//      2)  '*': Short these pins to reset the EEPROM potentiometer calibration
//      3)  Note, RST and POT-HI are shorted together in the potentiometer cable
//
//
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
