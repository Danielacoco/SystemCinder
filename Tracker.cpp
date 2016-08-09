#include "Tracker.h"

void Tracker::handle_button(void* userdata, const vrpn_BUTTONCB b){
	printf("Button %d is in state %d           \n", b.button, b.state);
	fflush(stdout);
	//Add something to do when button is pressed


}

void Tracker::handle_analog(void* userdata, const vrpn_ANALOGCB a){
	positionm *p = (positionm *)userdata;

	p->x = a.channel[0];
	p->y = a.channel[1];


}

void Tracker::handle_tracker(void* userdata, const vrpn_TRACKERCB t){
	posquat *pq = (posquat *)userdata;

	pq->x = t.pos[0];
	pq->y = t.pos[1];
	pq->z = t.pos[2];
	pq->qx = t.quat[Q_X];
	pq->qy = t.quat[Q_Y];
	pq->qz = t.quat[Q_Z];
	pq->qw = t.quat[Q_W];
}