# makefile for macroKeyboard

BIN_DIR = /usr/bin
CC = g++

# comment out the next two lines to diable the hidapi backend
CFLAGS += -D USE_BACKEND_HIDAPI
LIBS += -lhidapi-libusb

# comment out the next two lines to diable the libusb backend
CFLAGS += -D USE_BACKEND_LIBUSB
LIBS += -lusb-1.0

build:
	$(CC) $(CFLAGS) macroKeyboard.cpp -o macroKeyboard $(LIBS) && \
	$(CC) readKeycodes.cpp -o readKeycodes -lhidapi-libusb

install:
	$(CC) $(CFLAGS) macroKeyboard.cpp -o macroKeyboard $(LIBS) && \
	$(CC) readKeycodes.cpp -o readKeycodes -lhidapi-libusb && \
	cp ./macroKeyboard $(BIN_DIR)/macroKeyboard && \
	cp ./readKeycodes $(BIN_DIR)/readKeycodes

clean:
	rm ./macroKeyboard && \
	rm ./readKeycodes
