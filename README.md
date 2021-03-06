<img src="https://raw.githubusercontent.com/wiki/PolySync/OSCC/images/oscc_logo_title.png">


The Open Source Car Control Project is a hardware and software project detailing the conversion of a
late model vehicle into an autonomous driving research and development vehicle.

See the [Wiki](https://github.com/PolySync/OSCC/wiki) for full documentation, details, and other
information.


Repository Contents
-------------------
* **3d_models** - Technical drawings and 3D files for board enclosures and other useful parts
* **boards** - PCB schematics and board designs for control modules
* **platforms** - Arduino code and relevant files for the specific platforms
* **utils** - Utilities for controlling and interfacing with a platform

Within a specific platform (e.g., `kia_soul`), there are:
* **3d_models** - Technical drawings and 3D files related to that platform
* **firmware** - Arduino code for the control modules


Boards
-----
The sensor interface and actuator control board schematics and design files are located in the
`boards` directory. If you don't have the time or fabrication resources, the boards can be
purchased as a kit from the [OSCC website](http://oscc.io).

Thanks to [Trey German](https://www.polymorphiclabs.com) and [Macrofab](https://macrofab.com/) for
help designing the boards and getting the boards made.


Building and Installing Arduino Firmware
------------

The OSCC Project is built around a number of individual modules that interoperate to create a fully
controllable vehicle. These modules are built from Arduinos and Arduino shields designed specifically
for interfacing with various vehicle components. Once these modules have been programmed with the
accompanying firmware and installed into the vehicle, the vehicle is ready to receive control commands
sent over a CAN bus from a computer running a control program.

**Pre-requisites:** You must have Arduino Core and CMake (version 2.8 or greater) pre-installed on
your machine.

```
sudo apt install arduino-core cmake
```

OSCC uses CMake to avoid some of the limitations of the Arduino IDE. Using this method you can build
and upload the firmware from the command-line.

Check out [Arduino CMake](https://github.com/queezythegreat/arduino-cmake) for more information.

**Building the Firmware**

Navigate to the `platforms` directory and create a build directory inside of it:

```
cd platforms
mkdir build
cd build
```

To generate Makefiles, tell CMake which platform to build firmware for. If you want to build
the firmware for the Kia Soul:

```
cmake -DBUILD_KIA_SOUL=ON ..
```

By default, your firmware will have debug symbols which is good for debugging but increases
the size of the firmware significantly. To compile without debug symbols and optimizatons
enabled, use the following instead:

```
cmake -DBUILD_KIA_SOUL=ON -DCMAKE_BUILD_TYPE=Release ..
```

This will generate the necessary files for building.

Now you can build with `make`:

```
make
```

With a single `make` command, all of the firmware modules will be built. If you'd like to build only
a specific module, you can provide a target name to `make` for whichever module you'd like to build:

```
make kia-soul-brake
make kia-soul-can-gateway
make kia-soul-steering
make kia-soul-throttle
```

Once the firmware is successfully built, you can upload it to the corresponding Arduino module.
Connect to the Arduino with a USB cable and then run the following command for whichever firmware
you'd like to upload:

```
make kia-soul-brake-upload
make kia-soul-can-gateway-upload
make kia-soul-steering-upload
make kia-soul-throttle-upload
```

Sometimes it takes a little while for the Arduino to initialize once connected, so if there is an
error thrown initially, wait a little bit and then retry the command.

**Monitoring Arduino modules**

It is sometimes useful to monitor individual Arduino modules to check for proper operation and to
debug. If the modules have been built with the flag `-DCMAKE_BUILD_TYPE=Debug`, their debug
printing functionality will be enabled and they will print status information to the serial interface.

The GNU utility `screen` is one option to communicate with the Arduino via serial over USB. It can
be used to both receive the output of any `Serial.print` statements in your Arduino code, and to
push commands over serial to the Arduino. If you don't already have it installed, you can get it
with the following command:

```
sudo apt install screen
```

You can connect to the serial interface (assuming a port of `/dev/ttyACM0` and a baudrate of
`115200`) with the following command:

```
screen /dev/ttyACM0 115200
```

You can exit screen with `C-a \`.

To do more in-depth debugging you can use any of a number of serial monitoring applications.
Processing can be used quite effectively to provide output plots of data incoming across a serial
connection.

Be aware that using serial printing can affect the timing of the firmware. You may experience
strange behavior while printing that does not occur otherwise.


Unit Tests
------------

Each module has a suite of unit tests that use **Cucumber** with **Cgreen**. There are prebuilt
64-bit Linux versions in `platforms/common/testing/framework`. Boost is required for Cucumber-CPP
and has been statically linked into `libcucumber-cpp.a`. If you need to build your own versions
you can use the provided script `build_test_framework.sh` which will install the Boost dependencies
(needed for building), clone the needed repositories with specific hashes, build the Cgreen and
Cucumber-CPP libraries, and place static Boost in the Cucumber-CPP library. The built will be placed
in an `oscc_test_framework` directory in the directory that you ran the script from. You can then copy
`oscc_test_framework/cucumber-cpp` and `oscc_test_framework/cgreen` to
`platforms/common/testing/framework`.


**Running the Tests**

You must have **Cucumber** installed to run the tests:

```
sudo apt install ruby-dev
sudo gem install cucumber -v 2.0.0
```

Building and running the tests is similar to the firmware itself, but you must instead tell
CMake to build the tests instead of the firmware with the `-DTESTS=ON` flag. We also pass
the `-DCMAKE_BUILD_TYPE=Release` flag so that CMake will disable debug symbols and enable
optimizations, good things to do when running tests to ensure nothing breaks with
optimizations.

We first need a build directory in the `platforms` directory:

```
cd platforms
mkdir build
cd build
```

Then we tell CMake that we're going to be building tests:

```
cmake .. -DTESTS=ON -DCMAKE_BUILD_TYPE=Release
```

Finally we make the `run-tests` target which will build the tests for each module and
run them:

```
make run-tests
```

Each module's test can also be run individually:

```
make run-brake-tests
make run-can-gateway-tests
make run-steering-tests
make run-throttle-tests
```

If everything works correctly you should see something like this:

```
# language: en
Feature: Receiving commands

  Commands received from a controller should be processed and acted upon.

  Scenario Outline: Enable throttle command sent from controller        # platforms/kia_soul/firmware/throttle/tests/features/receiving_commands.feature:8
    Given throttle control is disabled                                  # platforms/kia_soul/firmware/throttle/tests/features/receiving_commands.feature:9
    And the accelerator position sensors have a reading of <sensor_val> # platforms/kia_soul/firmware/throttle/tests/features/receiving_commands.feature:10
    When an enable throttle command is received                         # platforms/kia_soul/firmware/throttle/tests/features/receiving_commands.feature:12
    Then control should be enabled                                      # platforms/kia_soul/firmware/throttle/tests/features/receiving_commands.feature:14
    And the last command timestamp should be set                        # platforms/kia_soul/firmware/throttle/tests/features/receiving_commands.feature:15
    And <dac_a_val> should be written to DAC A                          # platforms/kia_soul/firmware/throttle/tests/features/receiving_commands.feature:16
    And <dac_b_val> should be written to DAC B                          # platforms/kia_soul/firmware/throttle/tests/features/receiving_commands.feature:17

    Examples:
      | sensor_val | dac_a_val | dac_b_val |
      | 0          | 0         | 0         |
      | 256        | 1024      | 1024      |
      | 512        | 2048      | 2048      |
      | 1024       | 4096      | 4096      |
```


Property-Based Tests
------------

The throttle, steering, and brake modules, along with the PID controller library, also contain a series of
property based tests.
These tests use [QuickCheck for Rust](http://burntsushi.net/rustdoc/quickcheck/), so **Rust** and **Cargo** 
need to be [installed](https://www.rust-lang.org/en-US/install.html) in order to run them locally.


**Running the Tests**

To run the tests, first navigate into the folder of the module to be tested.
For example, to run the PID tests: 

```
cd platforms/common/libs/pid/tests/property
```

Cargo is then used to build the tests, including fetching any needed dependencies:

```
cargo build
```

The property based tests are also run by Cargo:

```
cargo test -- --test-threads=1
```

*(note: If you are running the property-based tests on the firmware modules, the test-threads argument is required to
prevent data corruption. If you are running the tests on the PID controller, it can be safely omitted.)*

Once the tests have completed, the output should look similar to the following:

```
running 7 tests
test bindgen_test_layout_pid_s ... ok
test check_integral_term ... ok
test check_derivative_term ... ok
test check_proportional_term ... ok
test check_reversed_inputs ... ok
test check_same_control_for_same_inputs ... ok
test check_zeroize ... ok

test result: ok. 7 passed; 0 failed; 0 ignored; 0 measured

   Doc-tests tests

running 0 tests

test result: ok. 0 passed; 0 failed; 0 ignored; 0 measured
```


Controlling Your Vehicle
------------

Now that all your Arduino modules are properly setup, it is time to start sending control commands.
There is an example application to do this that uses the Logitech F310 Gamepad. The example interfaces
to the joystick gamepad via the SDL2 joystick library and sends CAN commands over the control CAN bus
via the Kvaser CANlib SDK. These CAN control commands are interpreted by the respective Arduino
modules and used to actuate the vehicle.

**Pre-requisites:** A Logitech F310 gamepad is required, and the SDL2 library and CANlib SDK need to
be pre-installed. A CAN interface adapter, such as the [Kvaser Leaf Light](https://www.kvaser.com),
is also required.

[logitech-F310](http://a.co/3GoUlkN)

Install the SDL2 library with the command below.

```
sudo apt install libsdl2-dev
```

Install the CANlib SDK via the following procedure.

[CANlib-SDK](https://www.kvaser.com/linux-drivers-and-sdk/)

**Building Joystick Commander Code**

Navigate to the directory for the joystick commander code.

```
cd utils/joystick_commander
```

Once you are in the home directory of the joystick commander, build the code using CMake.

```
mkdir build
cd build
cmake ..
make
```

Now that the joystick commander is built it is ready to be run. However, we need to determine what
CAN interface the control CAN bus is connected to on your computer. This interface will be passed to
the joystick commander as an argument, and will be used to allow the joystick commander to communicate
with the CAN bus. To figure out what CAN interface your control CAN bus is connected to, navigate to
the examples directory in the CANlib install.

```
cd /usr/src/linuxcan/canlib/
```

Run make to ensure all the CANlib examples are built.

```
make
```

Then navigate to the examples directory of the CANlib install.

```
cd /usr/src/linuxcan/canlib/examples/
```

You can use the "listChannels" and "canmonitor" examples to determine which CAN channel your control
bus is connected to. CAN monitor will dump any data on a selected channel and list channels will tell
you what channels are available. You can use both to determine which channel you will need to use.
Once you know the correct channel number, you can run the joystick example with the command below.

```
./joystick-commander <channel-number>
```

**Controlling the Vehicle with the Joystick Gamepad**

Once the joystick commander is up and running you can use it to send commands to the Arduino modules.
The controls are listed when the programs start up. Be sure the switch on the back of the controller
is switched to the 'X' position, not 'D'. The vehicle will only respond to commands if control is
enabled with the start button. The back button disables control.


Additional Vehicles & Contributing
------------

OSCC currently has information regarding the Kia Soul PS (2014-2016), but we want to grow! The
repository is structured to facilitate including more vehicles as more is learned about them.

Please see [CONTRIBUTING.md](CONTRIBUTING.md).


License Information
-------------------

Hardware source materials (e.g. schematics, board layouts, wiring diagrams, data sheets, physical
installation documentation, 3D models, etc.) for the OSCC (Open Source Car Control) Project are
licensed under Creative Commons Attribution-ShareAlike 4.0 International (CC BY-SA 4.0).

Firmware  and software source for the OSCC (Open Source Car Control) Project is licensed under the
MIT License (MIT) unless otherwise noted (e.g. 3rd party dependencies, etc.).

Please see [LICENSE.md](LICENSE.md) for more details.


Contact Information
-------------------

Please direct questions regarding OSCC and/or licensing to help@polysync.io.

*Distributed as-is; no warranty is given.*

Copyright (c) 2017 PolySync Technologies, Inc.  All Rights Reserved.
