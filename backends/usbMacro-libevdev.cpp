/*
 * usbMacro-libevdev.cpp
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
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <array>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <libevdev-1.0/libevdev/libevdev.h>
#include <poll.h>

class usbMacros_libevdev{
	
	public:
	
	int openKeyboard( std::string eventfile );
	int closeKeyboard();
	int waitForKeypress( bool print_codes);    //get keypresses and execute macros
	int waitForKeypressRead();    //get keypresses and print values to stdout
	int runMacro( int type, int code, int value );    //execute macro
	int loadMacros( std::string configFile );    //load macros from config file
	
	private:
	
	// /dev/input/event*
	int filedesc;
	// libevdev device
	struct libevdev *device = NULL;
	// store macros
	std::map< std::array<int, 3>, std::string > macros;
	// poll
	struct pollfd pollfds[1];
	
};

int usbMacros_libevdev::loadMacros( std::string configFile ){
	
	std::ifstream configIn( configFile );
	
	if( !configIn.is_open() ){
		std::cout << "Error: couldn't open " << configFile << "\n";
		return 1;
	} else{    //succesfully opened file
		std::cout << "Opened " << configFile << "\n";
		
		//clear stored macros
		macros.clear();
		
		//load macros
		std::string line;
		std::string value1 = "";
		std::string value2 = "";
		std::string value3 = "";
		std::string value4 = "";
		std::array<int, 3> event;
		
		//load file
		while( getline(configIn, line) ){
			if( line.size() != 0 ){
				if( line[0] != '#' ){
					std::stringstream linestream(line);
					std::getline(linestream, value1, '\t');
					std::getline(linestream, value2, '\t');
					std::getline(linestream, value3, '\t');
					std::getline(linestream, value4, '\t');
					event = { std::stoi(value1), std::stoi(value2), std::stoi(value3) };
					macros[event] = value4;
				}
			}
		}
		
		configIn.close();
	}
	
	return 0;
}

int usbMacros_libevdev::runMacro( int type, int code, int value ){
	std::array<int, 3> key = {type, code, value};
	std::size_t position = 0;
	std::string macroCommand;
	
	if( macros[key].length() > 0 ){ // macro exists?
		
		//get macro command
		position = macros[key].find("\t", position);
		//std::cout << macros[key].substr(position+1) << "\n";
		macroCommand =  macros[key].substr(position+1);
		
		//check if one of the builtin functions and execute command
		if( macroCommand.find("\\load") == 0 ){
			//std::cout << macroCommand;
			loadMacros( macroCommand.substr(6) );
		} else if( macroCommand.find("\\quit") == 0 ){
			closeKeyboard();
			exit(0);
		} else{
			system( macroCommand.c_str() );
		}
		
	} else{ // no macro
		return 1;
	}
	
	return 0;
}

int usbMacros_libevdev::waitForKeypress( bool print_codes ){
	
	struct input_event event;
	
	// wait for change in /dev/input/event* (no timeout)
	if( libevdev_has_event_pending( device ) == 0 ){
		poll(pollfds,1,-1);
	}
	
	if( libevdev_next_event(device, LIBEVDEV_READ_FLAG_NORMAL, &event) == 0 ){
		if( print_codes ) std::cout << event.type << "\t" << event.code << "\t" << event.value << "\n";
		return runMacro( event.type, event.code, event.value );
	}
	
	return 0;
}

int usbMacros_libevdev::waitForKeypressRead(){
	
	struct input_event event;
	
	// wait for change in /dev/input/event* (no timeout)
	if( libevdev_has_event_pending( device ) == 0 ){
		poll(pollfds,1,-1);
	}
	
	if( libevdev_next_event(device, LIBEVDEV_READ_FLAG_NORMAL, &event) == 0 ){
		std::cout << event.type << "\t"	<< event.code << "\t"	<< event.value << "\n";
	}

	return 0;
}

int usbMacros_libevdev::openKeyboard( std::string eventfile ){
	
	int res;
	
	// open /dev/input/event* and create libevdev device
	filedesc = open( eventfile.c_str(), O_RDONLY|O_NONBLOCK );
	res = libevdev_new_from_fd( filedesc, &device );
	if( res < 0 ){
		return 1;
	}
	
	// set up polling
	pollfds[0].fd = filedesc;
	pollfds[0].events = POLLIN;
	
	// grab device (no input to other programs)
	libevdev_grab(device, LIBEVDEV_GRAB );
	
	return 0;
}

int usbMacros_libevdev::closeKeyboard(){
	
	libevdev_grab(device, LIBEVDEV_UNGRAB );
	libevdev_free(device);
	return 0;
}
