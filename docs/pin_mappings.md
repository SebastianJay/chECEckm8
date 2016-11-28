## Pin Mappings

The MSP432 pinout can be seen below.

![Pinout](http://energia.nu/wordpress/wp-content/uploads/2015/03/2016-06-09-LaunchPads-MSP432-2.0-%E2%80%94-Pins-Maps.jpg)

The following pins have functions for our application.

* 6.0: SHDN for UART transceiver
* 3.2: RX for UART
* 3.3: TX for UART
* 5.5: Select A0 for muxes
* 5.4: Select A1 for muxes
* 4.7: Select A2 for muxes
* 4.5: Select A3 for muxes
* 4.4: Read D0 from mux 0
* 4.2: Read D1 from mux 1
* 4.0: Read D2 from mux 2
* 6.1: Read D3 from mux 3
* 2.5: STEP for motor 0 (x-axis)
* 3.0: DIR for motor 0 (x-axis)
* 5.7: SLEEP for motor 0 (x-axis)
* 2.7: STEP for motor 1 (y-axis)
* 2.6: DIR for motor 1 (y-axis)
* 5.6: SLEEP for motor 1 (y-axis)
* 6.6: Read signal from homing switch 0 (x-axis)
* 6.7: Read signal from homing switch 1 (y-axis)
* 2.4: PWM signal for servo

## Interface boards and connectors

We are using several integrated circuit boards that go on a master header board. One exception is the mux boards, which have a separate PCB that is located on the board layer with the piece sensors and will need cables to connect with the master board. Some other components can be connected directly to the master board without an intermediary IC.

* [DRV8825 Stepper Motor Driver Carrier](https://www.pololu.com/product/2133) and its associated [IC datasheet](https://www.pololu.com/file/download/drv8825.pdf?file_id=0J590). The driver board has 16 pins, and we use two of them. Each board will interface with a motor through 4 signals: A1, A2, B1, B2.
  * __ENABLE__: Wired to ground (or unconnected)
  * __M0__: Wired to 3.3 V
  * __M1__: Wired to 3.3 V
  * __M2__: Wired to 3.3 V (note: setting all modes high results in 1/32 microstep resolution)
  * __RESET__: Wired to 3.3 V
  * __SLEEP__: Connected to MSP as described above
  * __STEP__: Connected to MSP as described above
  * __DIR__: Connected to MSP as described above
  * __VMOT__: Wired to motor power - 12 V
  * __GND__ (top-right): Wired to motor ground (note: __VMOT__ and __GND__ should be connected through a 100 uF cap, as shown in the diagram on the webpage)
  * __B2__, __B1__, __A1__, __A2__: Signals connected to motors
  * __FAULT__: Wired to ground (or unconnected)
  * __GND__ (bottom-right): Wired to ground

* [MAX3322E RS-232 Transceiver](https://datasheets.maximintegrated.com/en/ds/MAX3322E-MAX3323E.pdf) to interface MSP432 with serial USB. See page 11 of the datasheet for the 16 pin package we are using. Our USB cable is [described here](http://www.ftdichip.com/Support/Documents/DataSheets/Cables/DS_USB_RS232_CABLES.pdf). On the PCB we can have three holes for the cable (we can solder the leads directly or use a 3 port connector). The cable's connected leads will be TXD, RXD, and GND.
  * __C1+__: Positive terminal of 0.1 uF cap that connects __C1+__ and __C1-__.
  * __V+__: Connected to positive terminal of 0.1 uF whose negative terminal connects to ground.
  * __C1-__: Negative terminal of 0.1 uF cap that connects __C1+__ and __C1-__.
  * __C2+__: Positive terminal of 0.1 uF cap that connects __C2+__ and __C2-__.
  * __C2-__: Negative terminal of 0.1 uF cap that connects __C2+__ and __C2-__.
  * __V-__: Connected to positive terminal of 0.1 uF whose negative terminal connects to ground.
  * __TOUT1__: Connected to RXD of USB cable
  * __RIN1__: Connected to TXD of USB cable
  * __VCC__: Wired to 3.3 V
  * __GND__: Wired to ground (note: __VCC__ and __GND__ should be bypassed with a 0.1 uF cap)
  * __SHDN__: Connected to SHDN on MSP (see above)
  * __VL__: Wired to 3.3 V
  * __RENABLE__: Wired to ground
  * __TXENABLE__: Wired to 3.3 V
  * __TIN1__: Connected to TX on MSP (see above)
  * __ROUT1__: Connected to RX on MSP (see above)

* [Parallax Standard Servo](https://www.parallax.com/sites/default/files/downloads/900-00005-Standard-Servo-Product-Documentation-v2.2.pdf) needs 3 signals.
  * Signal (white): Connected to PWM signal on MSP (see above)
  * Vservo (red): Wired to 5 V
  * Vss (black): Wired to ground

* [Snap Action Switch](https://www.components.omron.com/components/web/PDFLIB.nsf/0/FE0F8E8EEB5D725485257201007DD573/$file/V_1110.pdf) needs 3 signals. We will have two of these switches.
  * COM: Connected to read signal on MSP (see above)
  * NC: Wired to 3.3 V
  * NO: Wired to ground

* Mux PCB interfaces with the master board through 10 signals: A0, A1, A2, A3, D0, D1, D2, D3 (all GPIO pins from MSP), 3.3 V power, and ground.

* Unsure how power design works, but ideally our board has 4 buses: ground, 3.3 V, 5 V, 12 V. The MSP432 would be powered through the 3.3 V or 5 V.
