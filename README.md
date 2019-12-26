# macroKeyboard
Turns a USB keyboard into a dedicated macro device. Has only been tested on Linux but should work cross-platform.

## Installing

- Install all dependencies, see table below
- Clone this repository
- Build with
```
sudo make install
```
- Allow access to the USB device without root by creating a udev rule:
  - Get the device VID and PID by running lsusb
  - Place a file with the following content named macrokeyboard.rules in /etc/udev/rules.d/
```
SUBSYSTEM=="usb", ATTRS{idVendor}=="VID", ATTRS{idProduct}=="PID", MODE:="0666"
SUBSYSTEM=="usb_device", ATTRS{idVendor}=="VID", ATTRS{idProduct}=="PID", MODE:="0666"
```
  - Restart for this to take effect

## Backends

Currently three backends with slightly different features are available. To disable support for a particular backend simply comment out one of the #define statements in macroKeyboard.cpp .

Backend | Dependencies | Notes
---|---|---
hidapi | [hidapi](https://github.com/libusb/hidapi) | does not support "\quit", see Bugs
---|---|---
libusb | [libusb](https://github.com/libusb/libusb) | supports "\quit"
---|---|---
placebo | none | does nothing, only intended for debugging

## Usage

### Creating the configuration

The config file consists of three tab-separated columns. The first column is a number describing the modifiers, the second number is the keycode and the third is the command that will be executed. Lines starting with a number sign '#' are being ignored.  The modifiers and keycodes can be obtained by running
```
readKeycodes VID PID
```
To reload the configuration or to create profiles "\load /path/to/configfile" can be used. "\quit" closes the program on supported backends.

### Configuration example

```
0	41	mpd
0	58	cantata&
0	59	mpc consume
0	42	\load example
0	42	\quit
```

### Running

WARNING: The specified keyboard can not be used for input while this program is running, so make sure you have a second keyboard attached.
Start the program with:
```
macroKeyboard VID PID macrofile backend
```

## Bugs and TODO

- Only for the hidapi backend: after this program terminates the keyboard that was opened needs to unplugged and plugged back in for it to work again as a normal keyboard. This is a limitation of the library and I don't intend to fix this, as the libusb backend does not have this limitation.

- [ ] Improve commandline parsing
- [ ] Add option to quit after a single macro has been executed
