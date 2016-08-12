/*TRACKER class for buttons, mouse (analog) and tracker, used VRPN: library and servers
More infor here: https://github.com/vrpn/vrpn/wiki and useful websites that helped me 
write the following handlers: http://www.vrgeeks.org/vrpn and describes how to set up vrpn and write
handlers for your own devices. 
Project 1 in submitted repo has and example where I print the location of the mouse. VRPN server needs to be running before you run the application.
VRPN server is located here: C:\Users\Daniela\Documents\Visual Studio 2015\Projects\build2\server_src\Debug */

#ifndef TRACKER_H
#define TRACKER_H


#include <stdio.h>
#include "vrpn\vrpn_Analog.h"
#include "vrpn\vrpn_Button.h"
//#include "vrpn\vrpn_Tracker_Fastrak.h"
#include "vrpn\vrpn_Tracker.h"
#include "vrpn\quat\quat.h"

struct positionm{
	float x;
	float y;
};

struct button {
	int b[100];
};

struct posquat {
	float x; //->position
	float y;
	float z; 
	double qx, qy, qz, qw; //-> quaternion orientation
};
class Tracker {

public:
	
	// in case we want to handle buttons
	void VRPN_CALLBACK handle_button(void* userdata, const vrpn_BUTTONCB b);

	// handle mouse, however using Mouse class in cinder might be better
	void VRPN_CALLBACK handle_analog(void* userdata, const vrpn_ANALOGCB a);

	// handle tracker position update
	void VRPN_CALLBACK handle_tracker(void* userdata, const vrpn_TRACKERCB t);


private:

};




#endif TRACKER_H