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
// 29      C6                  Calibrate SW. 2: (input)
// 28      C5                  Calibrate SW. 2: (output)
//                             momentarily shorting these pins commences calibration
// ***********************************************************************************
//
// **************** The following pins are used for the Mini Pro *********************
// 26      C3                  Analog in.(mini pro) Connected to the wiper on
//                             the thruster potentiometer
//
// 25      C2                  Calibrate SW. 2 (input)
// 24      C1                  Calibrate SW. 2: (output)
//                             momentarily shorting these pins commences calibration
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
//       Calibrate Sw. 1 ---| [ ]A4/SDA               D5[+]~|   D5---------|6     |
//       Calibrate Sw. 2 ---| [*]A5/SCL               D4[+] |   D4---------|5     |
//                          | [*]A6/~RESET       INT1/D3[+]~|   D3---------|4     |
//       pot-wiper ---------| [+]A7              INT0/D2[+] |   D2---------|3     |
//       pot-HI ------------| [+]5V                  GND[+] |     ---------|2     |
//       pot-HI ------------| [+]~RESET           ~RESET[+] |   C6---------|1     |
//       pot-LO ------------| [+]GND   5V MOSI GND   TX1[ ] |   D0         +------+
//                          | [ ]Vin   [ ] [ ] [ ]   RX1[ ] |   D1
//                          |          [ ] [ ] [ ]          |
//                          |          MISO SCK RST         |
//                          | NANO-V3                       |
//                          +-------------------------------+
//
//    Notes:
//      1) '+': Pin is connected
//      2) '*': Momentarily short these pins to calibrate the potentiometer, e.g.,
//         with a pushbutton switch
//      3) RST and POT-HI are both connected to the pot 5V terminal
//      4) Can't use A6 for I/O because it's also /RESET.
//
// Arduino Mini Pro pinout:
//
//                                                 D0   D1   RST
//                                  GND  GND  VCC  RX   TX   /DTR
//                               +--------------------------------+
//                               |  [ ]  [ ]  [ ]  [ ]  [ ]  [ ]  |
//                               |              FTDI              |
// Muligame 10-pin header    D1  | [ ]1/TX                 RAW[ ] |
//        +------+           D0  | [ ]0/RX                 GND[+] |-----pot-LO
//        |     1|-----------    | [+]RST        SCL/A5[ ] RST[+] |-----pot-HI
//        |     2|-----------    | [+]GND        SDA/A4[ ] VCC[+] |-----pot-HI
//        |     3|-----------D2  | [+]2/INT0    ___         A3[+] |-----pot-wiper
//        |     4|-----------D3  |~[+]3/INT1   /   \        A2[*] |-----Calibrate Sw. 1
//        |     5|-----------D4  | [+]4       /PRO  \       A1[*] |-----Calibrate Sw. 2
//        |     6|-----------D5  |~[+]5       \ MINI/       A0[ ] |
//        |     7|-----------D6  |~[+]6        \___/    SCK/13[ ] |   B5
//        |     8|-----------D7  | [+]7          A7[ ] MISO/12[ ] |   B4
//        |     9|-----------B0  | [+]8          A6[ ] MOSI/11[ ]~|   B3
//        |    10|-----------B1  |~[+]9                  SS/10[ ]~|   B2
//        +------+               |           [RST-BTN]            |
//                               +--------------------------------+
//
//    Notes:
//      1)  '+': Pin is connected
//      2)  '*': Momentarily short these pins to calibrate the potentiometer, e.g.,
//           with a pushbutton switch
//      3)  Note, RST and POT-HI are both connected to the pot 5V terminal
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
#  define CAL_BUTTON_1_DEF A4
#  define CAL_BUTTON_2_DEF A5
#elif defined(MINIPRO)
#  define THRUST_PIN_DEF A3
#  define CAL_BUTTON_1_DEF A0
#  define CAL_BUTTON_2_DEF A1
#else
#  error No board selected
#endif

const uint16_t thrust_pin = THRUST_PIN_DEF;
const uint16_t cal_button1 = CAL_BUTTON_1_DEF;
const uint16_t cal_button2 = CAL_BUTTON_2_DEF;

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
uint16_t hi_val_addr = lo_val_addr + sizeof(uint16_t);

// high and low ADC values are meaningful only after calibration.
uint16_t lo_adc_val;
uint16_t hi_adc_val;

// PROCEDURE: cal_sw_pressed
// INPUTS: none
//
// OUTPUTS: returns uint8_t, a boolean value that is true if the Calibration
// switch is pressed
//
// DESCRIPTION: See above
//
// SIDE EFFECTS: None
//
// COMPLEXITY: 1

uint8_t cal_sw_pressed(void)
{
  uint8_t sw_pressed = 0;

  digitalWrite(cal_button1, HIGH);
  sw_pressed = digitalRead(cal_button2);

  digitalWrite(cal_button1, LOW);
  return sw_pressed && !digitalRead(cal_button2);
}

// PROCEDURE: calibration_request
// INPUTS: none
// OUTPUTS: returns uint8_t, a boolean value that is true if the EEPROM reset
// condition was met (jumper shorted and removed)
//
// DESCRIPTION: see above
//
// SIDE EFFECTS: If jumper is shorted, this routine delays until the jumper
// is removed.  This is done to prevent a useless repetitive cycle of
// detecting the jumper and resetting the EEPROM.
//
// NOTES: See SIDE EFFECTS
//
// COMPLEXITY: 2
uint8_t calibration_request(void)
{
  uint8_t calibrate_request = 0;

  // If the calibration button is pressed, then wait until released. Otherwise
  // the adc value will repeatedly be written to EEPROM, serving no useful
  // function, and shortening the EEPROM life.
  while (cal_sw_pressed()) {
    calibrate_request = 1;
    // visually indicate the reset request
    digitalWrite(LED_BUILTIN, HIGH);
  }
  digitalWrite(LED_BUILTIN, LOW);
  return calibrate_request;
}


// PROCEDURE: setup
// INPUTS: none
// OUTPUTS: none
//
// DESCRIPTION: Sets up the I/O pins and initial variable values
//
// SIDE EFFECTS: See above
//
// COMPLEXITY:  1
void setup() {
  // Check if EEPROM reset is requested. Make sure the two CAL_BUTTON pins are
  // connected by testing that values written to the cal_button1 pin are read
  // back on the cal_button2 pin:

  pinMode(cal_button1, OUTPUT);
  pinMode(cal_button2, INPUT_PULLUP);

  // Set up digital registers;
  DDRD = LOBITS(0xff);
  DDRB = HIBITS(0xff);

  // Retrieve the high and low values stored in EEPROM.

  EEPROM.get(lo_val_addr, lo_adc_val);
  EEPROM.get(hi_val_addr, hi_adc_val);
}


// PROCEDURE: out_byte
// INPUTS: uint8_t byte: The byte to be output to the Asteroids Multigame.
// OUTPUTS: none
//
// DESCRIPTION: Takes a byte value to be send to the multigame and re-orders
// the bits to match the wiring of the POKEY chip.
//
// SIDE EFFECTS: Changes output port logic levels.
//
// NOTES:
//
// COMPLEXITY: 1
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


// PROCEDURE: store_hi_lo_val
// INPUTS: uint16_t val: A 16-bit DAC readout value
// OUTPUTS: none
//
// DESCRIPTION: Given a DAC reading, test whether the reading is higher or
// lower than the current high/low boundary values.  If so, set the new
// boundary (high or low) to the current reading.
//
// SIDE EFFECTS: May modify high and low boundary values for the thruster
//
// NOTES: Once the full range has been exercised, the correct high and low
// values are known. The entire range of ADC values produced by the thruster can
// then be mapped to the range of values expected by the multigame. This results
// in automatic calibration of thruster that is tolerant to different
// potentiometer values and mounting positions.
//
// COMPLEXITY: 3
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

// PROCEDURE: loop
// INPUTS: none
// OUTPUTS: none
//
// DESCRIPTION: main loop. Each pass through the loop:
// - Test if EEPROM clear condition is met, and clear the EEPROM if so.
// - Read the current ADC value.
// - Map the ADC value to the range of values expected by the Multigame.
// - Output the mapped thruster position to the Multigame port.
//
// SIDE EFFECTS: See above
//
// COMPLEXITY: 2
void loop(void) {
  // read the thruster every pass through the loop.
  uint16_t thrust_val = analogRead(thrust_pin);  // read the input pin

  // When an EEPROM clear is requested, by connecting the CAL_BUTTON1 and
  // CAL_BUTTON2 pins, reset the EEPROM by storing the current ADC reading as
  // both hi and lo values.

  if (calibration_request()) {
    lo_adc_val = thrust_val;
    hi_adc_val = thrust_val;
    EEPROM.put(hi_val_addr, hi_adc_val);
    EEPROM.put(lo_val_addr, lo_adc_val);

  } else {
    // Otherwise adjust the boundary values if the current thrust vale falls
    // outside the current bounds.
    store_hi_lo_val(thrust_val);
  }
  uint8_t thruster_out = map(thrust_val, lo_adc_val, hi_adc_val,
                             min_thrust, max_thrust);

  out_byte(thruster_out);
}
//-------|---------|---------+---------+---------+---------+---------+---------+
// Above line is 80 columns, and should display completely in editor.
//
