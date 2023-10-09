Lunar Lander thrust controller A/D adapter
==========================================

This project uses an Arduino nano or mini pro to interface a Lunar Lander thrust
controller to the Asteroids multigame Created by Scott Brasington (`Braze
Technologies <https://www.brasington.org/arcade/products/adl/>`__)

It turns out that the nano and mini have a pinout that can directly plug into
the 10-pin digital input connector on the Asteroids Multigame, so no special
adapter board or cable is needed. A cable is still needed for the potentiometer.

This adapter can work with almost any potentiometer value, and almost any
positioning of the potentiometer. The firmware will calibrate the analog values
read from the potentiometer and map those values to the recommended range of
values for the Lunar Lander game. To calibrate the thruster, simply short the
two "calibration" I/O pins momentarily, then move the thruster through its range
of motion. The highest and lowest values will automatically be stored in EEPROM,
and will be mapped to the expected high/low thruster values until another
calibration is performed, if ever. I suggest mounting a calibration switch near
the thruster, and connecting it to the calibration pins using a cable.

Because the nano and mini have the /RESET line where the multigame board
provides +5V, the /RESET line must be tied to the +5V line to ensure proper
operation. This is most easily done on the potentiometer cable, avoiding the
need for any board modifications. Note that the potentiometer cable cannot be
connected for in-circuit programming. For programming the nano over USB, the
cable can be left in place.

As a side note, since +5 and ~RESET are tied together when the potentiometer is
connected, it is a good idea to remove the RESET pushbutton, to avoid
accidentally shorting +5 to GND.

If the potentiometer cable is not installed, so that RESET is not connected to
+5V, then the device will still be powered through the protection diodes, and
the device will not be damaged.

Wiring:

.. Image:: wiring.png
   :scale: 50%
   :alt: Wiring diagram for Arduino A/D interface.


The 10-pin header connects directly from Arduino nano (or mini) pins 3-12 to the
multigame 10-pin connector:

+---------------+--------------+
|  Arduino Pin  |Multigame Pin |
+===============+==============+
|   3 (RESET)   |   1 (+5V)    |
+---------------+--------------+
|    4 (GND)    |   2 (GND)    |
+---------------+--------------+
|    5 (D2)     |    3 (D1)    |
+---------------+--------------+
|    6 (D3)     |    4 (D0)    |
+---------------+--------------+
|    7 (D4)     |    6 (D3)    |
+---------------+--------------+
|    8 (D5)     |    7 (D2)    |
+---------------+--------------+
|    9 (D6)     |    8 (D5)    |
+---------------+--------------+
|    10 (D7)    |    8 (D4)    |
+---------------+--------------+
|    11 (D8)    |    9 (D7)    |
+---------------+--------------+
|    12 (D9)    |   10 (D6)    |
+---------------+--------------+

6-pin Potentiometer+calibrate button cable (mini pro)
or
4-pin potentiometer cable + 2 pin calibrate button cable (nano):

+------------+--------------+-------------------------------+
|Nano Pin    |Mini Pro Pin  |?Connected to:                 |
+============+==============+===============================+
|A5          |A1            |Calibrate switch pin 1         |
+------------+--------------+-------------------------------+
|A6          |A2            |Calibrate switch pin 2         |
+------------+--------------+-------------------------------+
|A7          |A3            |Potentiometer wiper            |
+------------+--------------+-------------------------------+
|+5V         |+5V           |Potentiometer High side        |
+------------+--------------+-------------------------------+
|/RESET      |/RESET        |Potentiometer High side        |
+------------+--------------+-------------------------------+
|GND         |GND           |Potentiometer Low side         |
+------------+--------------+-------------------------------+


Assembly:

1) The Arduino kit should come with some breakaway headers. Break off a 10-pin
   segment to plug into the multigame, and either a 6-pin segment (for the mini)
   or a 2-pin segment and a 4-pin segment (for the nano pro), and solder them
   into place using the tables above.

2) Desolder or clip off the red reset button.  You won't need it.

3) Program the arduino using the .ino file in this repository.
   - Edit the program to Select the mini pro or the nano by uncommenting

     ``#define NANO 1`

     if you have a nano, or

     ``#define MINIPRO 1``

     if you have a mini pro.

   - compile and program the arduino using the arduino IDE.

   - You can program the nano pro with a USB cable. For the mini, you will need
     a USB-to-TTL adapter. I have found that you can slide the pins on the
     USB-TTL adapter directly into the row of 6 holes at the bottom end of the
     PCB, tilt them and apply a little pressure to make contact, and then
     program the mini without extra cables or flying wires.

4) Make the potentiometer and calibration switch as in the diagram. Correct
   wiring is important to ensure power to the nano. Note that you can use almost
   any linear potentiometer value you like. The thrusters come with 5K and 10K
   pots, but 2K or 20K will work as well, if you are making your own thruster or
   rebuilding one.

5) Plug in and enjoy. No calibration needed. After the thruster is moved through
   the full range of motion once, the thruster is fully calibrated, and the
   calibration constants are stored in EEPROM.

.. Image:: mini-pin-positions.png
   :scale: 50%
   :alt: Connector positions for the mini

.. Image:: mini-in-circuit.png
   :scale: 50%
   :alt: The mini plugged into the multi-game adapter


A simulation running the arduino software has been set up on Wokwi.com:

https://wokwi.com/arduino/projects/322006068604437075

