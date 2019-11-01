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
 * 
 */

#include <iostream>
#include <string>

#include </usr/include/hidapi/hidapi.h>
#include "usbMacro.cpp"

//g++ macroKeyboard.cpp -o macroKeyboard -lhidapi-libusb

//run as root (don't do this)
//or add udev rule (do this)

int main(int argc, char* argv[])
{
	//check args
	if( argc <= 3 ){
		std::cout << "Usage: macroKeyboard VID PID macrofile\n";
		return 0;
	}
	
	usbMacros keyboard;
	std::string configPath = argv[3];
	if( keyboard.loadMacros( configPath ) != 0 ){
		return 1;
	}
	
	//open keyboard with VID and PID
	keyboard.openKeyboard( std::stoi( argv[1], 0, 16), std::stoi( argv[2], 0, 16) );
			
	while(1){
		keyboard.waitForKeypress();
	}
	
	return 0;
}
