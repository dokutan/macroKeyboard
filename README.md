# macroKeyboard
Turns a Keyboard or any other input device into a dedicated macro device. Has only been tested on Linux but should work cross-platform with the correct backends.

## Installing

- Clone this repository
- Install all dependencies or disable backends, see section below
- Build with
```
sudo make install
```
- If you want to use the hidapi or libusb backend: Allow access to the USB device without root by creating a udev rule:
  - Get the device VID and PID by running lsusb
  - Place a file with the following content named macrokeyboard.rules in /etc/udev/rules.d/
```
SUBSYSTEM=="usb", ATTRS{idVendor}=="VID", ATTRS{idProduct}=="PID", MODE:="0666"
SUBSYSTEM=="usb_device", ATTRS{idVendor}=="VID", ATTRS{idProduct}=="PID", MODE:="0666"
```
  - Restart for this to take effect
- If you want to use the libevdev backend, make sure you that you have read access to the files /dev/input/event*. One possible method is as follows:
  - Run `ls -l /dev/input` to see the groups that can read from these files
  - Add your user to the relevant group with `sudo usermod -a -G group user`
- Running this program as root is strongly discouraged as all macros are executed with root privileges which is a major security risk.

## Backends

Currently four backends with slightly different features are available. To disable support for a particular backend simply comment out the appropriate lines in the makefile.

Backend | Dependencies | Supported devices | Notes
---|---|---|---
hidapi | [hidapi](https://github.com/libusb/hidapi) | Keyboards | does not support "\quit", see Bugs
libusb | [libusb](https://github.com/libusb/libusb) | Keyboards | supports "\quit", supports single keypress option
libevdev | [libevdev](https://www.freedesktop.org/software/libevdev/doc/latest/) | any supported as input device by Linux | supports "\quit", supports single keypress option, has different configuration format
placebo | none | does nothing, only intended for debugging

## Usage for the libusb and hidapi backends

### Creating the configuration

The config file consists of three tab-separated columns. The first column is a number describing the modifiers, the second number is the keycode and the third is the command that will be executed. Lines starting with a number sign '#' are being ignored. The modifiers and keycodes can be obtained by running
```
macroKeyboard -v VID -p PID -b backend -r
```
To reload the configuration or to create profiles "\load /path/to/configfile" can be used. "\quit" closes the program on supported backends.

### Configuration example

```
0	41	mpd
0	58	cantata&
0	59	mpc consume
0	42	\load example
0	72	\quit
```

### Running

Get usage information:
```
macroKeyboard -h
```

WARNING: The specified keyboard can not be used for input while this program is running, so make sure you have a second keyboard attached.
Start the program with:
```
macroKeyboard -v VID -p PID -m macrofile -b backend
```
The option "-s" can be used to quit after a single keypress.
```
macroKeyboard -v VID -p PID -m macrofile -b backend -s
```

## Usage for the libevdev backend

### Creating the configuration

The config file consists of four tab-separated columns. The first column is the event type, the second number is the event code, the third is the event value and the fourth is the command that will be executed. Lines starting with a number sign '#' are being ignored. The type, code and value can be obtained by running
```
macroKeyboard -b libevdev -e eventfile -r
```
The eventfile is any of /dev/input/event* , there are symlinks under /dev/input/by-id/ which make finding the correct device easier. One keypress generates multiple events (e.g. key up, key down), just one should be placed in the macrofile. [https://www.kernel.org/doc/html/latest/input/event-codes.html#input-event-codes] for more information. Run `evtest` on the device to get information about supported events.

To reload the configuration or to create profiles "\load /path/to/configfile" can be used. "\quit" closes the program.

### Configuration example

```
1	1	1	mpd
1	59	1	cantata&
0	60	1	mpc consume
0	14	1	\load example
0	119	1	\quit
```

### Running

See usage section above for the general options, run with
```
macroKeyboard -b libevdev -e eventfile -m macrofile
```
In contrast to the other backends, using the -s option only quits after a successful (i.e. existing) macro.

## Bugs and TODO

- Only for the hidapi backend: after this program terminates the keyboard that was opened needs to unplugged and plugged back in for it to work again as a normal keyboard. This is a limitation of the library and I don't intend to fix this, as the libusb backend does not have this limitation.

- [x] Improve commandline parsing
- [x] Add readKeycodes functionality into main program
- [x] Add option to quit after a single macro has been executed
- [ ] Show dynamic list of backends in help message
- [ ] Improve error handling
