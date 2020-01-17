/*
 * usbMacro-hidapi.cpp
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

#include </usr/include/hidapi/hidapi.h>

class usbMacros_hidapi{
	
	public:
	
	int openKeyboard( int VID, int PID );
	int closeKeyboard();
	int waitForKeypress( bool print_codes );    //get keypresses and execute macros
	int waitForKeypressRead();    //get keypresses and print values to stdout
	int runMacro( unsigned char keycode, unsigned char modifiers );    //execute macro
	int loadMacros( std::string configFile );    //load macros from config file
	
	private:
	
	hid_device *keyboardDevice;
	
	//store macros
	std::map< std::array<unsigned char, 2>, std::string > macros;
	
};

int usbMacros_hidapi::loadMacros( std::string configFile ){
	
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

int usbMacros_hidapi::runMacro( unsigned char keycode, unsigned char modifiers ){
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
		} else{
			system( macroCommand.c_str() );
		}
		
	}
	
	return 0;
}

int usbMacros_hidapi::waitForKeypress( bool print_codes ){
	unsigned char buffer[65];
	unsigned char key_old=0, key_new=0;
	
	while( 1 ){
		
		//request keyboard state
		buffer[1] = 0x81;
		hid_write(keyboardDevice, buffer, 65);

		//read requested state
		hid_read(keyboardDevice, buffer, 65);
		
		key_old = key_new;
		key_new = buffer[2];
		
		if( key_old == 0 && key_new != 0 ){
			if( print_codes ) std::cout << (int) buffer[0] << "\t" << (int) key_new << "\n";
			runMacro( buffer[0], key_new );
		}
	}
	
	return 0;
}

int usbMacros_hidapi::waitForKeypressRead(){
	unsigned char buffer[65];
	unsigned char key_old=0, key_new=0;
	
	while( 1 ){
		
		//request keyboard state
		buffer[1] = 0x81;
		hid_write(keyboardDevice, buffer, 65);

		//read requested state
		hid_read(keyboardDevice, buffer, 65);
		
		key_old = key_new;
		key_new = buffer[2];
		
		if( key_old == 0 && key_new != 0 ){
			std::cout << (int) buffer[0] << "\t" << (int) key_new << "\n";
		}
	}
	
	return 0;
}

int usbMacros_hidapi::openKeyboard( int VID, int PID ){
	keyboardDevice = hid_open( VID, PID, NULL );
	return 0;
}

int usbMacros_hidapi::closeKeyboard(){
	
	return 0;
}
