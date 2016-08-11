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