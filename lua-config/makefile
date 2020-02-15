# makefile for macroKeyboard-lua

BIN_DIR = /usr/bin
CC = g++

# comment out the next two lines to disable the hidapi backend
CFLAGS += -D USE_BACKEND_HIDAPI
LIBS += -lhidapi-libusb

# comment out the next two lines to disable the libusb backend
CFLAGS += -D USE_BACKEND_LIBUSB
LIBS += -lusb-1.0

# comment out the next two lines to disable the libevdev backend
CFLAGS += -D USE_BACKEND_LIBEVDEV
LIBS += -levdev

LIBS += -llua

build:
	$(CC) $(CFLAGS) macroKeyboard-lua.cpp -o macroKeyboard-lua $(LIBS) -Wall

install:
	$(CC) $(CFLAGS) macroKeyboard-lua.cpp -o macroKeyboard-lua $(LIBS) -Wall && \
	cp ./macroKeyboard-lua $(BIN_DIR)/macroKeyboard-lua

clean:
	rm ./macroKeyboard-lua
