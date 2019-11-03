# macroKeyboard
Turns a USB keyboard into a dedicated macro device. 

## Installing

- Install [hidapi](https://github.com/libusb/hidapi)
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

## Usage

### Creating the configuration

The config file consists of three tab-separated columns. The first column is a number describing the modifiers, the second number is the keycode and the third is the command that will be executed. Lines starting with a number sign '#' are being ignored.  The modifiers and keycodes can be obtained by running
```
readKeycodes VID PID
```
To reload the configuration or to create profiles "\load /path/to/configfile" can be used.

### Coniguration example

```
0	41	mpd
0	58	cantata&
0	59	mpc consume
0	42	\load example
```

### Running

WARNING: The specified keyboard can not be used for input while this program is running, so make sure you have a second keyboard attached.
Start the program with:
```
macroKeyboard VID PID macrofile
```

## Bugs and TODO

- after this program terminates the keyboard that was opened needs to unplugged and plugged back in for it to work again as a normal keyboard.
