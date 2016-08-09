#ifndef _WINDOWDATA_H_
#define _WINDOWDATA_H_

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder\Color.h"

using namespace ci;
class WindowData {

public:
	WindowData() : mColor( Color(0.6f, 0.6f, 0.6f))
	{}
	Color mColor;
};



#endif
