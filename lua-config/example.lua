-- Example for the macroKeyboard lua interface


-- The backend is selected through the backend variable
-- libevdev, libusb or hidapi
backend = "libevdev"


-- The way device is specified depends on the backend
-- libevdev: device_eventfile
-- libusb and hidapi: device_vid and device_pid
device_eventfile = "/dev/input/event*"
device_vid = "12ab"
device_pid = "12ab"


-- this callback function is executed when an input event happens
-- 
-- libevdev: three arguments will be passed
-- libusb and hidapi: two arguments will be passed
-- 
-- the function may return one value:
-- if this functions returns "quit" the whole program will exit
function input_handler( ev_type, ev_code, ev_value )
	
	-- print incoming event
	print(ev_type.." "..ev_code.." "..ev_value)
	
	-- quit if escape key is pressed
	if( ev_type == 1 and ev_code == 1 and ev_value == 1 ) then
		return "quit"
	
	-- execute mpc toggle when F1 is pressed
	elseif( ev_type == 1 and ev_code == 59 and ev_value == 1 ) then
		os.execute("mpc toggle &")
	
	-- execute mpc stop when F2 is pressed
	elseif( ev_type == 1 and ev_code == 60 and ev_value == 1 ) then
		os.execute("mpc stop &")
	end
	
end
