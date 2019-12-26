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

// enable backends
// these defines are moved to the makefile, so use uncomment them to
// enable backends when not using the makefile
//#define USE_BACKEND_HIDAPI
//#define USE_BACKEND_LIBUSB
// enable backends end

#include <iostream>
#include <string>
#include <unistd.h>

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

// print help message
void print_help(){
	std::cout << "macroKeyboard usage:\n\n";
	std::cout << "Required arguments:\n";
	std::cout << "\t-p=arg\tKeyboard PID\n";
	std::cout << "\t-v=arg\tKeyboard VID\n";
	std::cout << "\t-m=arg\tMacrofile\n";
	std::cout << "\t-b=arg\tBackend\n\n";
	std::cout << "Optional arguments:\n";
	std::cout << "\t-h\tShow this message\n";
	std::cout << "\t-s\tQuit after a single keypress on supported backends\n";
}

// main part
template<class T> int run_main( T keyboard, std::string VID, std::string PID, std::string macrofile, bool single = false ){
	
	// load config
	if( keyboard.loadMacros( macrofile ) != 0 ){
		return 1;
	}
	
	// open keyboard with VID and PID
	keyboard.openKeyboard( std::stoi( VID, 0, 16), std::stoi( PID, 0, 16) );
	
	// read incoming keys and execute macros
	if( !single ){
		
		// run in a loop, don't quit
		while(1){
			keyboard.waitForKeypress();
		}
		
	} else{
		
		// wait for a single keypress, then quit
		keyboard.waitForKeypress();
		keyboard.closeKeyboard();
		
	}
	
	return 0;
}

int main(int argc, char* argv[])
{
	
	// parse commandline options
	int c;
	std::string vid = "", pid = "", macrofile = "", backend = "";
	bool single = false;
	
	while( ( c = getopt( argc, argv, "p:v:m:b:hs") ) != -1 ){
		
		switch(c){
			case 'p':
				pid = optarg;
				break;
			case 'v':
				vid = optarg;
				break;
			case 'm':
				macrofile = optarg;
				break;
			case 'b':
				backend = optarg;
				break;
			case 'h':
				print_help();
				return 0;
				break;
			case 's':
				single = true;
				break;
			default:
				break;
		}
		
	}
	
	if( vid == "" || pid == "" || macrofile == "" || backend == "" ){
		std::cout << "Required arguments missing\n";
		return 0;
	}
	
	// determine backend, select correct main class
	if( argc >= 5 ){
		
		if( backend == "libusb" ){
			
			#ifdef USE_BACKEND_LIBUSB
			std::cout << "Using libusb backend\n";
			run_main<usbMacros_libusb>( usbMacros_libusb(), vid, pid, macrofile, single );
			#else
			std::cout << "Using placebo backend\n";
			run_main<usbMacros_placebo>( usbMacros_placebo(), vid, pid, macrofile );
			#endif
			
		} else if( backend == "hidapi" ){
			
			#ifdef USE_BACKEND_HIDAPI
			std::cout << "Using hidapi backend\n";
			run_main<usbMacros_hidapi>( usbMacros_hidapi(), vid, pid, macrofile );
			#else
			std::cout << "Using placebo backend\n";
			run_main<usbMacros_placebo>( usbMacros_placebo(), vid, pid, macrofile );
			#endif
			
		} else{
			
			std::cout << "Using placebo backend\n";
			run_main<usbMacros_placebo>( usbMacros_placebo(), vid, pid, macrofile );
			
		}
	}
	
	return 0;
}
