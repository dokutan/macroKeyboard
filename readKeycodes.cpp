/*
 * readKeycodes.cpp
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

// This program has been replaced with macroKeyboard -r

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>

#include </usr/include/hidapi/hidapi.h>

//g++ readKeycodes.cpp -o readKeycodes -lhidapi-libusb

int main(int argc, char* argv[])
{
	//check args
	if( argc <= 2 ){
		std::cout << "Usage: readKeycodes VID PID\n";
		return 0;
	}
	
	int res;
	unsigned char buffer[65];
	#define MAX_STR 255
	wchar_t wstr[MAX_STR];
	hid_device *handle;
	int i;
	unsigned char key_new=0, key_old=0;

	// Open the device using the VID and PID
	handle = hid_open(std::stoi( argv[1], 0, 16), std::stoi( argv[2], 0, 16), NULL);
	
	//detect pressed keys
	while( 1 ){
		
		//request state
		buffer[1] = 0x81;
		hid_write(handle, buffer, 65);

		//read requested state
		res = hid_read(handle, buffer, 65);
		
		key_old = key_new;
		key_new = buffer[2];
		
		if( key_old == 0 && key_new != 0 ){
			std::cout << (int) buffer[0] << "\t" << (int) key_new << "\n";
		}
	}
	
	return 0;
}
