BIN_DIR = /usr/bin

install:
	g++ macroKeyboard.cpp -o macroKeyboard -lhidapi-libusb -lusb-1.0 && \
	g++ readKeycodes.cpp -o readKeycodes -lhidapi-libusb && \
	cp ./macroKeyboard $(BIN_DIR)/macroKeyboard && \
	cp ./readKeycodes $(BIN_DIR)/readKeycodes

clean:
	rm /macroKeyboard && \
	rm ./readKeycodes
