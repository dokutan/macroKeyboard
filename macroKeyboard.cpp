/*
 * macroKeyboard.cpp
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 */

// enable backends, comment out the following lines to disable backends
#define USE_BACKEND_HIDAPI
#define USE_BACKEND_LIBUSB
// enable backends end

#include <iostream>
#include <string>

// include backends
#ifdef USE_BACKEND_LIBUSB
#include "usbMacro-libusb.cpp"
#endif
#ifdef USE_BACKEND_HIDAPI
#include "usbMacro-hidapi.cpp"
#endif
#include "usbMacro-placebo.cpp"

// compile with:
// g++ macroKeyboard.cpp -o macroKeyboard -lhidapi-libusb -lusb-1.0

// run as root (don't do this)
// or add udev rule (do this)


// main part
template<class T> int run_main( T keyboard, char *VID, char *PID, char *macrofile ){
	
	// load config
	if( keyboard.loadMacros( macrofile ) != 0 ){
		return 1;
	}
	
	// open keyboard with VID and PID
	keyboard.openKeyboard( std::stoi( VID, 0, 16), std::stoi( PID, 0, 16) );
	
	// read incoming keys and execute macros		
	while(1){
		keyboard.waitForKeypress();
	}
}

int main(int argc, char* argv[])
{
	// check args
	if( argc <= 3 ){
		std::cout << "Usage: macroKeyboard VID PID macrofile backend\n";
		return 0;
	}
	
	// determine backend, select correct main class
	if( argc >= 5 ){
		std::string backend = argv[4];
		
		if( backend == "libusb" ){
			
			#ifdef USE_BACKEND_LIBUSB
			std::cout << "Using libusb backend\n";
			run_main<usbMacros_libusb>( usbMacros_libusb(), argv[1], argv[2], argv[3] );
			#else
			std::cout << "Using placebo backend\n";
			run_main<usbMacros_placebo>( usbMacros_placebo(), argv[1], argv[2], argv[3] );
			#endif
			
		} else if( backend == "hidapi" ){
			
			#ifdef USE_BACKEND_HIDAPI
			std::cout << "Using hidapi backend\n";
			run_main<usbMacros_hidapi>( usbMacros_hidapi(), argv[1], argv[2], argv[3] );
			#else
			std::cout << "Using placebo backend\n";
			run_main<usbMacros_placebo>( usbMacros_placebo(), argv[1], argv[2], argv[3] );
			#endif
			
		} else{
			
			std::cout << "Using placebo backend\n";
			run_main<usbMacros_placebo>( usbMacros_placebo(), argv[1], argv[2], argv[3] );
			
		}
	}
	
	return 0;
}
