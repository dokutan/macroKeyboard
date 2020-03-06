/*
 * macroKeyboard-lua.cpp
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

#include <iostream>
#include <string>
#include <array>
#include <exception>
#include <getopt.h>
#include <sys/types.h>
#include <unistd.h>

// include backends
#ifdef USE_BACKEND_LIBUSB
#include "../backends/usbMacro-libusb.cpp"
#endif
#ifdef USE_BACKEND_HIDAPI
#include "../backends/usbMacro-hidapi.cpp"
#endif
#ifdef USE_BACKEND_LIBEVDEV
#include "../backends/usbMacro-libevdev.cpp"
#endif

// lua
extern "C"{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

// help message
void print_help(){
	std::cout << "macroKeyboard-lua options:\n\n";
	std::cout << "-h --help\tprint this message\n";
	std::cout << "-c --config\tlua file to be loaded (required)\n";
	std::cout << "-f --fork\tfork into the background\n";
}

// lua stack dump
void stackDump( lua_State *L ){
	std::cout << "------\n";
	for( int i = lua_gettop( L ); i > 0; i-- ){
		
		std::cout << i << "\t" << lua_typename( L, lua_type( L, i ) );
		
		if( lua_isnumber( L, i ) ){
			std::cout << "\t" << lua_tonumber( L, i ) << "\n";
		} else{
			std::cout << "\n";
		}
		
	}
	std::cout << "------\n";
}

// main function
int main(int argc, char* argv[]){
	
	
	
	// argument checking
	if( argc == 1 ){
		std::cout << "Missing arguments: run macroKeyboard-lua --help for help\n";
		return 1;
	}
	
	//command line options
	static struct option long_options[] = {
		{"help", no_argument, 0, 'h'},
		{"config", required_argument, 0, 'c'},
		{"fork", no_argument, 0, 'f'},
		{0, 0, 0, 0}
	};
	
	// parse commandline options
	int c, option_index = 0;
	bool flag_fork = false, flag_config = false;
	std::string string_config;
	while( (c = getopt_long( argc, argv, "hc:f",
	long_options, &option_index ) ) != -1 ){
		
		switch( c ){
			case 'h':
				print_help();
				return 0;
				break;
			case 'c':
				flag_config = true;
				string_config = optarg;
				break;
			case 'f':
				flag_fork = true;
				break;
			case '?':
				break;
			default:
				break;
		}
	}
	
	if( !flag_config ){
		std::cout << "Missing arguments: run macroKeyboard-lua -h for help\n";
		return 1;
	}
	
	
	
	// lua
	lua_State *L = luaL_newstate(); // open lua
	luaL_openlibs( L ); // open lua libraries
	
	// load and run lua file
	if( luaL_loadfile( L, string_config.c_str() ) || lua_pcall( L, 0, 0, 0 ) ){
		std::cout << "An error occured: " << lua_tostring( L, -1 ) << "\n";
		lua_remove( L, -1 );  // remove top value from stack
		lua_close( L );
		return 1;
	}
	
	// get backend variable from lua
	lua_getglobal( L, "backend" );
	if( !lua_isstring( L, -1 ) ){
		std::cout << "Error in lua: backend must be a string\n";
		lua_remove( L, -1 );  // remove top value from stack
		lua_close( L );
		return 1;
	}
	std::string backend = lua_tostring( L, -1 );
	lua_remove( L, -1 );  // remove top value from stack
	
	
	
	// fork into the background if requested
	if( flag_fork ){
		// create child process
		pid_t process_id = fork();
		
		// close file descriptors
		close(0); // cin
		close(1); // cout
		close(2); // cerr
		
		// quit if not child process
		if( process_id != 0 )
			return 0;
	}
	
	
	
	// check backend, get device variables
	if( backend == "libusb" ){
		
		
		
		#ifdef USE_BACKEND_LIBUSB
		std::cout << "Using libusb backend\n";
		
		// get device_vid
		lua_getglobal( L, "device_vid" );
		if( !lua_isstring( L, -1 ) ){
			std::cout << "Error in lua: device_vid must be a string\n";
			lua_close( L );
			return 1;
		}
		std::string device_vid = lua_tostring( L, -1 );
		lua_remove( L, -1 );  // remove top value from stack
		
		// get device_pid
		lua_getglobal( L, "device_pid" );
		if( !lua_isstring( L, -1 ) ){
			std::cout << "Error in lua: device_pid must be a string\n";
			lua_close( L );
			return 1;
		}
		std::string device_pid = lua_tostring( L, -1 );
		lua_remove( L, -1 );  // remove top value from stack
		
		// keyboard object
		usbMacros_libusb kbd;
		
		// convert vid and pid to int
		int vid = 0, pid = 0;
		try{
			vid = std::stoi(device_vid, 0, 16);
			pid = std::stoi(device_pid, 0, 16);
		} catch( std::exception &e ){
			std::cout << "Exception caught: " << e.what() << "\n";
			lua_close( L );
			return 1;
		}
		
		// open keyboard
		if( kbd.openKeyboard( vid, pid ) != 0 ){
			std::cout << "An error occured: couldn't open keyboard\n";
			lua_close(L);
			return 1;
		}
		
		// wait for events and run lua function input_handler
		std::array<unsigned int, 2> event;
		std::string input_handler_return;
		while( 1 ){
			
			// wait for successfull event
			if( kbd.waitForKeypress( event ) == 0 ){
				
				// push function input_handler and arguments onto the stack
				lua_getglobal( L, "input_handler" ); // load function onto stack
				lua_pushinteger( L, event[0] ); // push first argument 
				lua_pushinteger( L, event[1] ); // push second argument
				
				// call lua function input_handler
				if( lua_pcall( L, 2, 1, 0 ) != 0 ){
					std::cout << "An error occured: " << lua_tostring( L, -1 ) << "\n";
					lua_remove( L, -1 );  // remove top value from stack
					kbd.closeKeyboard();
					lua_close(L);
					return 1;
				}
				
				// get return value from input_handler
				if( lua_isstring( L, -1 ) ){
					input_handler_return = lua_tostring( L, -1 );
					
					// quit program if requested from lua
					if( input_handler_return == "quit" ){
						kbd.closeKeyboard();
						break;
					}
					
				}
				
				// remove return value from input_handler from stack
				lua_remove( L, -1 );
				
				// stackDump(L);
			}
		}
		
		#else
		std::cout << "Backend libusb is not enabled.\n";
		lua_close( L );
		return 1;
		#endif
		
		
		
	} else if( backend == "hidapi" ){
		
		
		
		#ifdef USE_BACKEND_HIDAPI
		std::cout << "Using hidapi backend\n";
		
		// get device_vid
		lua_getglobal( L, "device_vid" );
		if( !lua_isstring( L, -1 ) ){
			std::cout << "Error in lua: device_vid must be a string\n";
			lua_close( L );
			return 1;
		}
		std::string device_vid = lua_tostring( L, -1 );
		lua_remove( L, -1 );  // remove top value from stack
		
		// get device_pid
		lua_getglobal( L, "device_pid" );
		if( !lua_isstring( L, -1 ) ){
			std::cout << "Error in lua: device_pid must be a string\n";
			lua_close( L );
			return 1;
		}
		std::string device_pid = lua_tostring( L, -1 );
		lua_remove( L, -1 );  // remove top value from stack
		
		// keyboard object
		usbMacros_hidapi kbd;
		
		// convert vid and pid to int
		int vid = 0, pid = 0;
		try{
			vid = std::stoi(device_vid, 0, 16);
			pid = std::stoi(device_pid, 0, 16);
		} catch( std::exception &e ){
			std::cout << "Exception caught: " << e.what() << "\n";
			lua_close( L );
			return 1;
		}
		
		// open keyboard
		if( kbd.openKeyboard( vid, pid ) != 0 ){
			std::cout << "An error occured: couldn't open keyboard\n";
			lua_close(L);
			return 1;
		}
		
		// wait for events and run lua function input_handler
		std::array<unsigned int, 2> event;
		std::string input_handler_return;
		while( 1 ){
			
			// wait for successfull event
			if( kbd.waitForKeypress( event ) == 0 ){
				
				// push function input_handler and arguments onto the stack
				lua_getglobal( L, "input_handler" ); // load function onto stack
				lua_pushinteger( L, event[0] ); // push first argument 
				lua_pushinteger( L, event[1] ); // push second argument
				
				// call lua function input_handler
				if( lua_pcall( L, 2, 1, 0 ) != 0 ){
					std::cout << "An error occured: " << lua_tostring( L, -1 ) << "\n";
					lua_remove( L, -1 );  // remove top value from stack
					kbd.closeKeyboard();
					lua_close(L);
					return 1;
				}
				
				// get return value from input_handler
				if( lua_isstring( L, -1 ) ){
					input_handler_return = lua_tostring( L, -1 );
					
					// quit program if requested from lua
					if( input_handler_return == "quit" ){
						kbd.closeKeyboard();
						break;
					}
					
				}
				
				// remove return value from input_handler from stack
				lua_remove( L, -1 );
				
				// stackDump(L);
			}
		}
		
		#else
		std::cout << "Backend hidapi is not enabled.\n";
		lua_close( L );
		return 1;
		#endif
		
		
		
	} else if( backend == "libevdev" ){
		
		
		
		#ifdef USE_BACKEND_LIBEVDEV
		std::cout << "Using libevdev backend\n";
		
		// get device_eventfile
		lua_getglobal( L, "device_eventfile" );
		if( !lua_isstring( L, -1 ) ){
			std::cout << "Error in lua: device_eventfile must be a string\n";
			lua_close( L );
			return 1;
		}
		std::string device_eventfile = lua_tostring( L, -1 );
		lua_remove( L, -1 );  // remove top value from stack
		
		// keyboard object
		usbMacros_libevdev kbd;
		
		// open eventfile
		if( kbd.openKeyboard( device_eventfile ) != 0 ){
			std::cout << "An error occured: couldn't open eventfile\n";
			lua_close(L);
			return 1;
		}
		
		// wait for events and run lua function input_handler
		std::array<unsigned int, 3> event;
		std::string input_handler_return;
		while( 1 ){
			
			// wait for successfull event
			if( kbd.waitForKeypress( event ) == 0 ){
				
				// push function input_handler and arguments onto the stack
				lua_getglobal( L, "input_handler" ); // load function onto stack
				lua_pushinteger( L, event[0] ); // push first argument 
				lua_pushinteger( L, event[1] ); // push second argument
				lua_pushinteger( L, event[2] ); // push third argument
				
				// call lua function input_handler
				if( lua_pcall( L, 3, 1, 0 ) != 0 ){
					std::cout << "An error occured: " << lua_tostring( L, -1 ) << "\n";
					lua_remove( L, -1 );  // remove top value from stack
					kbd.closeKeyboard();
					lua_close(L);
					return 1;
				}
				
				// get return value from input_handler
				if( lua_isstring( L, -1 ) ){
					input_handler_return = lua_tostring( L, -1 );
					
					// quit program if requested from lua
					if( input_handler_return == "quit" ){
						kbd.closeKeyboard();
						break;
					}
					
				}
				
				// remove return value from input_handler from stack
				lua_remove( L, -1 );
				
				// stackDump(L);
			}
		}
		
		#else
		std::cout << "Backend libevdev is not enabled.\n";
		lua_close( L );
		return 1;
		#endif
		
		
		
	} else{
		
		std::cout << "Invalid backend\n";
		lua_close( L );
		return 1;
		
	}
	
	lua_close( L );
	return 0;
}
