#ifndef VIRTUALDISPLAY_H
#define VIRTUALDISPLAY_H

#include <string>

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
using namespace std;

class VirtualDisplay {
public:

	VirtualDisplay(string* displayType);

	void mouseDown (MouseEvent event);
	void mouseDragg(MouseEvent event);

	void pick(const ivec2 &mousepOS); // maybe pick projectors?

	void createGrid();
	void createShaders();
	void createDisplay();//gl::FboRef mFbo
	void createParams();
	void setUpProjectors();
	void drawProjectors();
	void setUpFboShouldBeMappedTexture(gl::FboRef mFbo);

	bool mWireframe;
	//bool mSolidframe;
	gl::VertBatchRef  mGrid;

	//gl::BatchRef mWiredDisplay;
	gl::BatchRef mDisplay;


	enum DisplayShape { SPHERE, CUBE };



	gl::GlslProgRef		mShader;
	gl::GlslProgRef		mWireShader;
	CameraPersp  vCam;
	gl::VboMeshRef  mVerticesMetch;
	gl::BatchRef mVerticesBatch;

	bool mNeedsRedraw;
	vec4 mDefaultVertexColor = vec4(0.7f, 0.3f, 0.7f, 1.0f);

	
#if ! defined ( CINDER_GL_ES )
	params::InterfaceGlRef	mParams;
#endif
	// Projector variable settings

	CameraPersp mProjector1;
	CameraPersp mProjector2;
	vec3 mLookAt1 = vec3(0, 3, 0);
	vec3 mLookAt2 = vec3(0, 3, 0);
	vec3 mEyePointProj1 = vec3(-2, 0, 0);
	vec3 mEyePointProj2 = vec3(2, 0, 0);
	float mFov = 25.0f;
	float mnearPlane = 1.0;
	float mfarPlane = 6.5;
	vec2 mLensShift = vec2(0);
private:
	// Selected Primitive and Current primitive
	DisplayShape			mShapeSelected;
	DisplayShape		mShapeCurrent;

	// divisions for the sphere
	int mDivHoriz = 15;
	int mDivVerti = 20;
	int mDivCircle = 72;
	gl::FboRef mFboShouldBeMappedTextureInFuture;


};
#endif
