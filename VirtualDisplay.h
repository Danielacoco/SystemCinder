#ifndef VIRTUALDISPLAY_H
#define VIRTUALDISPLAY_H

#include "cinder\app\MouseEvent.h"
#include "glm\glm.hpp"
#include "cinder\gl\Batch.h"
#include "cinder\gl\wrapper.h"
#include "cinder/Camera.h"
#include "cinder/GeomIo.h"
#include "cinder/ImageIo.h"
#include "cinder/CameraUi.h"
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Batch.h"
#include "cinder/gl/Context.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/VboMesh.h"
#include "cinder/params/Params.h"
#include "cinder/Log.h"


using namespace cinder::app;
using namespace glm;
using namespace cinder;

class VirtualDisplay {
public:

	void mouseDown (MouseEvent event);
	void mouseDragg(MouseEvent event);

	void pick(const ivec2 &mousepOS); // maybe pick projectors?

	void createGrid();
	void createSphere();
	void setUpProjectors();
	void drawProjectors();

	bool mWireframe = true;
	gl::VertBatchRef  mGrid;

	gl::BatchRef mWiredSphere;
	gl::BatchRef mPrimitive;

	CameraPersp mProjector1;
	CameraPersp mProjector2;
	vec3 mLookAt = vec3(0, 3, 0);



	gl::GlslProgRef		mWireShader;
	CameraPersp  vCam;
	gl::VboMeshRef  mVerticesMetch;
	gl::BatchRef mVerticesBatch;

	bool mNeedsRedraw;
	vec4 mDefaultVertexColor = vec4(0.7f, 0.3f, 0.7f, 1.0f);

	int mDivHoriz = 15;
	int mDivVerti = 20;
	int mDivCircle = 72;
};

#endif

