# turningTables
### DCC Indexing System for Atlas Turntable
*Powered by Arduino*

turningTables is an Arduino-powered indexing system for model railway turntables. It was built for the Atlas HO turntable, but because of its flexible design it could be easily adapted to work with other turntables with a similar drive system.

### How does it work?

turningTables uses a hall effect sensor as a tachometer to measure the rotations of a gear in the turntable's gearbox. In this way, after an initial calibration, the Arduino can keep track of where the turntable is positioned.

### Modifying your Turntable

This only requires a small modification to the turntable:

- Drilling one small hole for the hall effect sensor wires
- Shimming/gluing the [hall effect sensor](https://www.sparkfun.com/products/9312) to the floor of the gearbox
- Gluing a [small neodymium magnet](https://www.sparkfun.com/products/8914) to a gear inside the gearbox
- Soldering three [0.1uF ceramic capacitors](https://www.sparkfun.com/products/8375) to the turntable motor
  - Solder one from each terminal to the casing
  - Solder one between the two terminals
  - This will reduce electrical noise, which can interfere with the hall effect sensor

The magnet should be mounted to the gear that engages directly with the motor unit's worm gear, _NOT_ the special "geneva" gear that engages with the turntable.

### Parts List

- Arduino Uno
- 3 0.1uF ceramic capacitors
- Misc. Small Wire (20-22ga)
- 12v H-Bridge board or Arduino Motor Shield
  - I used one that looks like this:
  - ![](http://cdn.instructables.com/FRW/G225/I2X6RNGT/FRWG225I2X6RNGT.MEDIUM.jpg)
- [10K Ohm resistor](https://www.amazon.com/Projects-100EP51410K0-10k-Resistors-Pack/dp/B0185FGYQA) - used as pull-up resistor for hall effect sensor
- [Small perfboard](https://www.amazon.com/Honbay-Prototype-Paper-Universal-multi-sized/dp/B014F9KCHC/ref=sr_1_10?ie=UTF8&qid=1468961677&sr=8-10&keywords=perfboard)
- [Male breakaway headers](https://www.amazon.com/Gikfun-breakaway-header-Arduino-EK1017/dp/B00R17YO16/ref=sr_1_1?ie=UTF8&qid=1468961730&sr=8-1&keywords=male+breakaway+headers)
- [~12v Power Supply (at least 1A capacity)](https://www.amazon.com/Power-Adapter-Transformers-Supply-Strip/dp/B00DKSI0S8/ref=sr_1_2?ie=UTF8&qid=1468961775&sr=8-2&keywords=12v+3a)
- Small (usually grey) wire nuts
- [2 Small push-button switches](https://www.amazon.com/6x6x6mm-Momentary-Push-Button-Switch/dp/B01GN79QF8/ref=sr_1_11?ie=UTF8&qid=1468962523&sr=8-11&keywords=pushbutton+switches) - these are used to manually align the turntable. You can mount them on a control panel, but you won't need them ofte, so mounting them under the layout works fine too.

### Tools Needed

- Soldering Iron
- Wire Strippers
- Desoldering Wick (optional) _or_ Solder Sucker (optional)

### Connecting to Layout

**In order to interface the turntable with your layout, you must have a PC with JMRI. The interface to JMRI works via USB, immitating Bruce Chubb's C/MRI protocol with the [ArduinoCMRI Library.](https://github.com/madleech/ArduinoCMRI)**

The turntable is controllable as a series of turnouts in JMRI, the same way that a Walther's turntable or other indexed turntable is controlled. This means the turntable is compatible with the graphical "turntable" item on the JMRI layout editor. You can also create Logix to synchronize turnout addresses on your normal DCC system with the C/MRI turnouts, allowing you to control the turntable from a standard DCC throttle instead of only through your JMRI PC.

### Installing turningTables on your Arduino

Out of the box, turningTables is configured to work with an Atlas HO scale turntable. If you have another turntable with a similar mechanism, and you are familiar with basic Arduino programming, you could easily adapt the code to work with your turntable.

To install the sketch, click "Download ZIP" on the main GitHub repository page. Extract the files in the ZIP file. Then, open the ``turningTables/turningTables.ino`` file in the Arduino IDE, and upload it to your Arduino.

### Configuring JMRI

Once you've uploaded the code to your Arduino, you can configure the connection in JMRI. Add a new C/MRI system, and add an SMINI node at UA (address) 0. From there, add turnouts corresponding to each of your stall tracks (for example, on the Atlas turntable, there are 24 positions, so I would add turnouts 1-23 since the count begins at 0). When you throw those turnouts, the turntable should rotate to those tracks.

---

#### Need Help?

If you run into problems with the code, please make an issue on GitHub. You'll need to make an account with GitHub, but it's free and only takes a minute.
