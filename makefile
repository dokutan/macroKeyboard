# makefile for macroKeyboard

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

build:
	$(CC) $(CFLAGS) macroKeyboard.cpp -o macroKeyboard $(LIBS) -Wall

install:
	$(CC) $(CFLAGS) macroKeyboard.cpp -o macroKeyboard $(LIBS) -Wall && \
	cp ./macroKeyboard $(BIN_DIR)/macroKeyboard

clean:
	rm ./macroKeyboard
