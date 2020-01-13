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
#include <exception>
#include <unistd.h> // for getopt

// include backends
#ifdef USE_BACKEND_LIBUSB
#include "backends/usbMacro-libusb.cpp"
#endif
#ifdef USE_BACKEND_HIDAPI
#include "backends/usbMacro-hidapi.cpp"
#endif
#ifdef USE_BACKEND_LIBEVDEV
#include "backends/usbMacro-libevdev.cpp"
#endif
#include "backends/usbMacro-placebo.cpp"

// compile with:
// g++ macroKeyboard.cpp -o macroKeyboard -lhidapi-libusb -lusb-1.0

// run as root (don't do this)
// or add udev rule (do this)

// print help message
void print_help(){
	std::cout << "macroKeyboard usage:\n\n";
	std::cout << "Required arguments:\n";
	std::cout << "\t-b=arg\tBackend (placebo";
	
	#ifdef USE_BACKEND_LIBUSB
	std::cout << ", libusb";
	#endif
	#ifdef USE_BACKEND_LIBEVDEV
	std::cout << ", libevdev";
	#endif
	#ifdef USE_BACKEND_HIDAPI
	std::cout << ", hidapi";
	#endif
	
	std::cout << ")\n";
	std::cout << "\t-m=arg\tMacrofile (not required when using -r)\n\n";
	std::cout << "Required arguments for the hidapi and libusb backends:\n";
	std::cout << "\t-p=arg\tKeyboard PID\n";
	std::cout << "\t-v=arg\tKeyboard VID\n\n";
	std::cout << "Required arguments for libevdev backend:\n";
	std::cout << "\t-e=arg\tEvent file\n\n";
	std::cout << "Optional arguments:\n";
	std::cout << "\t-r\tRead keycodes\n";
	std::cout << "\t-h\tShow this message\n";
	std::cout << "\t-s\tQuit after a single keypress on supported backends\n";
}

// main part
template<class T> int run_main( T keyboard, std::string VID, std::string PID, std::string macrofile, bool read, bool single = false ){
	
	try{
		if( !read ){ // normal mode: execute macros on keypress
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
		} else{ // read mode: print keycodes to stdout
			
			// open keyboard with VID and PID
			keyboard.openKeyboard( std::stoi( VID, 0, 16), std::stoi( PID, 0, 16) );
			
			// read incoming keys and print keycodes
			if( !single ){
				
				std::cout << "Press Ctrl+C to quit\n";
				std::cout << "Depending on the backend you might have to replug the device after this\n";
				
				while(1){
					keyboard.waitForKeypressRead();
				}
				
			} else{
				keyboard.waitForKeypressRead();
				keyboard.closeKeyboard();
			}
			
		}
	} catch( std::exception &e ){
		std::cout << "An exception occured: " << e.what() << "\n";
	}
		
	return 0;
}

// main part for libevdev
int run_main_libevdev( std::string eventfile, std::string macrofile, bool read, bool single = false ){
	
	try{
		usbMacros_libevdev keyboard;
		
		if( !read ){ // normal mode: execute macros on keypress
			// load config
			if( keyboard.loadMacros( macrofile ) != 0 ){
				return 1;
			}
			
			// open keyboard
			keyboard.openKeyboard( eventfile );
			
			// read incoming keys and execute macros
			if( !single ){
				
				// run in a loop, don't quit
				while(1){
					keyboard.waitForKeypress();
					//std::cout << "a\n";
				}
				
			} else{
				
				// wait for a single keypress, then quit
				while( keyboard.waitForKeypress() != 0 )
					;
				//keyboard.waitForKeypress();
				keyboard.closeKeyboard();
				
			}
		} else{ // read mode: print keycodes to stdout
			
			// open keyboard
			keyboard.openKeyboard( eventfile );
			
			// read incoming keys and print keycodes
			if( !single ){
				
				std::cout << "Press Ctrl+C to quit\n";
				
				while(1){
					keyboard.waitForKeypressRead();
				}
				
			} else{
				keyboard.waitForKeypressRead();
				keyboard.closeKeyboard();
			}
			
		}
	} catch( std::exception &e ){
		std::cout << "An exception occured: " << e.what() << "\n";
	}
		
	return 0;
}

int main(int argc, char* argv[])
{
	
	// parse commandline options
	int c;
	std::string vid = "", pid = "", macrofile = "", backend = "", eventfile = "";
	bool single = false;
	bool read = false;
	
	while( ( c = getopt( argc, argv, "p:v:m:b:hsre:") ) != -1 ){
		
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
			case 'r':
				read = true;
				break;
			case 'e':
				eventfile = optarg;
				break;
			default:
				break;
		}
		
	}
	
	// check for correct arguments
	if( backend == "" ){
		std::cout << "Required arguments missing: -b <backend>\n";
		std::cout << "Run macroKeyboard -h for help.\n";
		return 0;
	}
	if( (backend == "libusb" || backend == "hidapi") && (vid == "") ){
		std::cout << "Required arguments missing: -v <vid>\n";
		std::cout << "Run macroKeyboard -h for help.\n";
		return 0;
	}
	if( (backend == "libusb" || backend == "hidapi") && (pid == "") ){
		std::cout << "Required arguments missing: -p <pid>\n";
		std::cout << "Run macroKeyboard -h for help.\n";
		return 0;
	}
	if( (backend == "libevdev") && (eventfile == "") ){
		std::cout << "Required arguments missing: -e <eventfile>\n";
		std::cout << "Run macroKeyboard -h for help.\n";
		return 0;
	}
	if( macrofile == "" && !read ){
		std::cout << "Required arguments missing: -m <macrofile>\n";
		std::cout << "Run macroKeyboard -h for help.\n";
		return 0;
	}
	
	// determine backend, select correct main class
	if( backend == "libusb" ){
		
		#ifdef USE_BACKEND_LIBUSB
		std::cout << "Using libusb backend\n";
		run_main<usbMacros_libusb>( usbMacros_libusb(), vid, pid, macrofile, read, single );
		#else
		std::cout << "Backend libusb is disabled, using placebo backend\n";
		run_main<usbMacros_placebo>( usbMacros_placebo(), vid, pid, macrofile, read );
		#endif
		
	} else if( backend == "hidapi" ){
		
		#ifdef USE_BACKEND_HIDAPI
		std::cout << "Using hidapi backend\n";
		run_main<usbMacros_hidapi>( usbMacros_hidapi(), vid, pid, macrofile, read );
		#else
		std::cout << "Backend hidapi is disabled, using placebo backend\n";
		run_main<usbMacros_placebo>( usbMacros_placebo(), vid, pid, macrofile, read );
		#endif
		
	} else if( backend == "libevdev" ){
		
		#ifdef USE_BACKEND_LIBEVDEV
		std::cout << "Using libevdev backend\n";
		run_main_libevdev( eventfile, macrofile, read, single );
		#else
		std::cout << "Backend libevdev is disabled, using placebo backend\n";
		run_main<usbMacros_placebo>( usbMacros_placebo(), vid, pid, macrofile, read );
		#endif
		
	} else{
		
		std::cout << "Using placebo backend\n";
		run_main<usbMacros_placebo>( usbMacros_placebo(), vid, pid, macrofile, read );
		
	}
	
	return 0;
}
