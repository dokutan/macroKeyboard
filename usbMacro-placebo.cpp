/*
 * usbMacro-placebo.cpp
 * 
 * This backend does nothing and is intended as a template and for
 * testing purposes only.
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

class usbMacros_placebo{
	
	public:
	
	int openKeyboard( int VID, int PID );
	int closeKeyboard();
	int waitForKeypress();    //get keypresses and execute macros
	int runMacro( unsigned char keycode, unsigned char modifiers );    //execute macro
	int loadMacros( std::string configFile );    //load macros from config file
	
	private:
	
};

int usbMacros_placebo::loadMacros( std::string configFile ){
	
	return 0;
}

int usbMacros_placebo::runMacro( unsigned char keycode, unsigned char modifiers ){
	
	return 0;
}

int usbMacros_placebo::waitForKeypress(){
	
	while( 1 ){
	
	}
	
	return 0;
}

int usbMacros_placebo::openKeyboard( int VID, int PID ){
	
	return 0;
}

int usbMacros_placebo::closeKeyboard(){
	
	return 0;
}
