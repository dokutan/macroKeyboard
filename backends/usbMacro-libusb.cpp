/*
 * usbMacro-libusb.cpp
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
#include <map>
#include <array>

#include <libusb-1.0/libusb.h>

class usbMacros_libusb{
	
	public:
	
	int openKeyboard( int VID, int PID );
	int closeKeyboard();
	int waitForKeypress( bool print_codes );    //get keypresses and execute macros
	int waitForKeypressRead();    //get keypresses and print values to stdout
	int waitForKeypress( std::array<unsigned int, 2>& event );    //overloaded function for the lua interface
	int runMacro( unsigned char keycode, unsigned char modifiers );    //execute macro
	int loadMacros( std::string configFile );    //load macros from config file
	
	private:
	
	libusb_device_handle *keyboardDevice = NULL;
	bool _detached_driver_0 = false;
	
	//store macros
	std::map< std::array<unsigned char, 2>, std::string > macros;
	
};

int usbMacros_libusb::loadMacros( std::string configFile ){
	
	std::ifstream configIn( configFile );
	
	if( !configIn.is_open() ){
		std::cout << "Error: couldn't open " << configFile << "\n";
		return 1;
	} else{    //succesfully opened file
		std::cout << "Opened " << configFile << "\n";
		
		//clear stored macros
		macros.clear();
		
		//load macros
		std::string value1 = "";
		std::string value2 = "";
		std::string value3 = "";
		std::size_t position1 = 0;
		std::size_t position2 = 0;
		std::array<unsigned char, 2> key;
		
		//load file
		for( std::string line; std::getline(configIn, line); ){
			//process individual line
			if( line.length() != 0 ){
				if( line[0] != '#' ){
					position1 = line.find("\t", position1);
					value1 = line.substr(0, position1);
					value2 = line.substr(position1+1);
					position2 = value2.find("\t", position1+1);
					value2 = value2.substr(0, position2);
					value3 = line.substr( position1+position2+2 );
					
					//store values in map
					//modifiers, keycode
					key = { (unsigned char) std::stoi(value1), (unsigned char) std::stoi(value2) };
					macros[key] = value3;
				}
			}
			
		}
		return 0;
	}
	
	return 1;
}

int usbMacros_libusb::runMacro( unsigned char keycode, unsigned char modifiers ){
	std::array<unsigned char, 2> key = {keycode, modifiers};
	std::size_t position = 0;
	std::string macroCommand;
	
	if( macros[key].length() > 0 ){
		
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
		
	}
	
	return 0;
}

int usbMacros_libusb::waitForKeypress( bool print_codes ){
	uint8_t buffer[8];
	unsigned char key_old=0, key_new=0;
	int transferred;
		
	//read from endpoint 1
	libusb_interrupt_transfer( keyboardDevice, 0x81, buffer, 8, &transferred, -1);;
	
	key_old = key_new;
	key_new = buffer[2];
	
	if( key_old == 0 && key_new != 0 ){
		if( print_codes ) std::cout << (int)buffer[0] << "\t" << (int)buffer[2] << "\n";
		runMacro( buffer[0], buffer[2] );
	}
	
	return 0;
}

int usbMacros_libusb::waitForKeypressRead(){
	uint8_t buffer[8];
	unsigned char key_old=0, key_new=0;
	int transferred;
		
	//read from endpoint 1
	libusb_interrupt_transfer( keyboardDevice, 0x81, buffer, 8, &transferred, -1);;
	
	key_old = key_new;
	key_new = buffer[2];
	
	if( key_old == 0 && key_new != 0 ){
		std::cout << (int)buffer[0] << "\t" << (int)buffer[2] << "\n";
	}
	
	
	return 0;
}

// read keypress (for lua interface)
int usbMacros_libusb::waitForKeypress( std::array<unsigned int, 2>& event ){
	
	uint8_t buffer[8];
	unsigned char key_old=0, key_new=0;
	int transferred;
	
	while( 1 ){
		
		//read from endpoint 1
		libusb_interrupt_transfer( keyboardDevice, 0x81, buffer, 8, &transferred, -1);;
		
		key_old = key_new;
		key_new = buffer[2];
		
		if( key_old == 0 && key_new != 0 ){
			//std::cout << (int)buffer[0] << "\t" << (int)buffer[2] << "\n";
			event[0] = buffer[0];
			event[1] = buffer[2];
			break;
		}
		
	}
	
	return 0;
}

//init libusb and open keyboard
int usbMacros_libusb::openKeyboard( int VID, int PID ){
	
	//vars
	int res = 0;
	
	//libusb init
	res = libusb_init( NULL );
	if( res < 0 ){
		return res;
	}
	
	//open device
	keyboardDevice = libusb_open_device_with_vid_pid( NULL, VID, PID );
	if( !keyboardDevice ){
		return res;
	}
	
	//detach kernel driver on interface 0 if active 
	if( libusb_kernel_driver_active( keyboardDevice, 0 ) ){
		res += libusb_detach_kernel_driver( keyboardDevice, 0 );
		if( res == 0 ){
			_detached_driver_0 = true;
		} else{
			return res;
		}
	}
	
	//claim interface 0
	res += libusb_claim_interface( keyboardDevice, 0 );
	if( res != 0 ){
		return res;
	}
	
	return res;
}

//close keyboard
int usbMacros_libusb::closeKeyboard(){
	
	//release interface 0 and 1
	libusb_release_interface( keyboardDevice, 0 );
	
	//attach kernel driver for interface 0
	if( _detached_driver_0 ){
		libusb_attach_kernel_driver( keyboardDevice, 0 );
	}
	
	//exit libusb
	libusb_exit( NULL );
	
	return 0;
}

