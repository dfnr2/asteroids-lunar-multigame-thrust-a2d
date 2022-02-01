Lunar Lander thrust controller A/D adapter
==========================================

This project uses an Arduino nano or mini pro to interface a Lunar Lander thrust
contrller to the Asteroids multigame Created by Scott Brasington (`Braze Technologies <https://www.brasington.org/arcade/products/adl/>`__)

It turns out that the nano and mini have a pinout that can directly plug into
the 10-pin digital input connector on the Asteroids Multigame, so no special
adapter board or cable is needed. A cable is still needed for the potentiometer.

Because the nano and mini have the /RESET line where the multigame board
provides +5V, the /RESET line must be tied to the +5V line to ensure proper
operation. This is most easily done on the potentiometer cable, avoiding the
need for any board modifications. Note that the potentiometer cable cannot be
connected for in-circuit programming. For programming the nano over USB, the
cable can be left in place.

If the potentiometer cable is not installed, so that RESET is not connected to
+5V, then the device will still be powered through the protection diodes, and
the device will not be damaged.

Wiring:

.. image:: wiring.png
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

4-pin Potentiometer cable:

+------------+-------------------------------+
|Arduino Pin |Connected to:                    |
+============+===============================+
|26 (A7)     |Potentiometer wiper            |
+------------+-------------------------------+
|27 (+5V)    |Potentiometer High side and    |
|            |also pin 28 (/RESET)           |
+------------+-------------------------------+
|28 (/RESET) |Pin 27 (+5V)                   |
+------------+-------------------------------+
|29 (GND)    |Potentiometer Low side         |
+------------+-------------------------------+


A simulation running the arduino software has been set up on Wokwi.com:

https://wokwi.com/arduino/projects/322006068604437075

