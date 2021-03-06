#include "VirtualDisplay.h"

/*constructor, parameter displayType is used to load the corresponding geometry*/

VirtualDisplay::VirtualDisplay(int displayType){
	switch (displayType){
	case (1) :
		mShapeSelected = mShapeCurrent = SPHERE;
		break;
	case (2) :
		mShapeSelected = mShapeCurrent = CUBE;
		break;
	default:
		mShapeSelected = mShapeCurrent = SPHERE;
	}

	/* if you want a wireframe geometry or a solid gemoetry, toggle mWireframe*/
	mWireframe = false;
	/* Create the UI where you can see useful information about the simulator*/
	createParams();

}

void VirtualDisplay::mouseDown(MouseEvent event){


}

void VirtualDisplay::createGrid(){
	mGrid = gl::VertBatch::create(GL_LINES);
	mGrid->begin(GL_LINES);
	for (int i = -10; i <= 10; ++i) {
		mGrid->color(Color(0.25f, 0.25f, 0.25f));
		mGrid->color(Color(0.25f, 0.25f, 0.25f));
		mGrid->color(Color(0.25f, 0.25f, 0.25f));
		mGrid->color(Color(0.25f, 0.25f, 0.25f));

		mGrid->vertex(float(i), 0.0f, -10.0f);
		mGrid->vertex(float(i), 0.0f, +10.0f);
		mGrid->vertex(-10.0f, 0.0f, float(i));
		mGrid->vertex(+10.0f, 0.0f, float(i));
	}
	mGrid->end();

	
}
void VirtualDisplay::createParams(){
	// creating parameters for the UI
#if ! defined( CINDER_GL_ES )
	vector<string> displayShape = { "Sphere", "Cube" };
	mParams = params::InterfaceGl::create(getWindow(), "Virtual Display", toPixels(ivec2(300, 500)));
	mParams->addParam("DisplayShape", displayShape, (int*)&mShapeSelected);
	
	mParams->addSeparator();
	mParams->addParam("Wire Display", &mWireframe);
	mParams->addParam("Subdivision Horizontal for Wire Display", &mDivHoriz).min(1).max(500).updateFn([this] {createDisplay(); });
	mParams->addParam("Subdivision Vertical for Wire Display", &mDivVerti).min(1).max(500).updateFn([this] {createDisplay(); });
	mParams->addParam("Subdivision Circle for Wire Display", &mDivCircle).min(1).max(500).updateFn([this] {createDisplay(); });

	mParams->addSeparator();
	mParams->addParam("Projector1 X", &mEyePointProj1.x).step(0.01f);
	mParams->addParam("Projector1 Y", &mEyePointProj1.y).step(0.01f);
	mParams->addParam("Projector1 Z", &mEyePointProj1.z).step(0.01f);
	mParams->addParam("Look At Projector1 X", &mLookAt1.x).step(0.01f);
	mParams->addParam("Look At Projector1 Y", &mLookAt1.y).step(0.01f);
	mParams->addParam("Look At Projector1 Z", &mLookAt1.z).step(0.01f);
	mParams->addSeparator();
	mParams->addParam("Projector2 X", &mEyePointProj2.x).step(0.01f);
	mParams->addParam("Projector2 Y", &mEyePointProj2.y).step(0.01f);
	mParams->addParam("Projector2 Z", &mEyePointProj2.z).step(0.01f);
	mParams->addParam("Look At Projector2 X", &mLookAt2.x).step(0.01f);
	mParams->addParam("Look At Projector2 Y", &mLookAt2.y).step(0.01f);
	mParams->addParam("Look At Projector2 Z", &mLookAt2.z).step(0.01f);
	mParams->addSeparator();
	mParams->addParam("FOV", &mFov).min(1.0f).max(179.0f);
	mParams->addParam("Near Plane", &mnearPlane).step(0.02f).min(0.1f);
	mParams->addParam("Far Plane", &mfarPlane).step(0.02f).min(0.1f);
	mParams->addParam("Lens Shift X", &mLensShift.x).step(0.01f);
	mParams->addParam("Lens Shift Y", &mLensShift.y).step(0.01f);
	mParams->addSeparator();

	mParams->setPosition(vec2(700, 0));
#endif
}

void VirtualDisplay::setUpFboShouldBeMappedTexture(gl::FboRef mFbo){

	mFboShouldBeMappedTextureInFuture = mFbo;

}
void VirtualDisplay::createDisplay(){
	/*center for world is at center of grid, displays are shifted up by the offset*/
	vec3 offset(0, 3, 0);
	switch (mShapeSelected) {
	default:
		mShapeSelected = CUBE;
	case SPHERE:
		if (mWireframe == true){
			
			mDisplay = gl::Batch::create(geom::WireSphere().radius(3).center(vec3(0, 3, 0)).subdivisionsHeight(mDivVerti).subdivisionsAxis(mDivHoriz).subdivisionsCircle(mDivCircle), mWireShader);
		}
		else {
			mFboShouldBeMappedTextureInFuture->bindTexture();
			mDisplay = gl::Batch::create(geom::Sphere().radius(3).center(vec3(0, 3, 0)), mShader); \
		}
		break;
	case CUBE:
		if (mWireframe == true){
			auto wirecube = geom::WireCube() >> geom::Scale(6) >> geom::Translate(offset);
			mDisplay = gl::Batch::create(wirecube, mWireShader);
		}
		else {
			mFboShouldBeMappedTextureInFuture->bindTexture();
			auto cube = geom::Cube() >>geom::Scale(6)>> geom::Translate(offset);
			mDisplay = gl::Batch::create(cube, mShader); 
		}
		break;

	}
}
void VirtualDisplay::createShaders(){
	try {
		mWireShader = gl::context()->getStockShader(gl::ShaderDef().color());
	}
	catch (Exception &exc) {
		exit;
	}

	auto shader = gl::ShaderDef().texture().lambert();
	mShader = gl::getStockShader(shader);
}


void VirtualDisplay::setUpProjectors(){
	mProjector1.setEyePoint(mEyePointProj1);
	mProjector2.setEyePoint(mEyePointProj2);
	mProjector1.setPerspective(mFov, (3/4.0), mnearPlane, mfarPlane );
	mProjector2.setPerspective(mFov, (3 / 4.0), mnearPlane, mfarPlane);
	mProjector1.setLensShift(mLensShift);
	mProjector2.setLensShift(mLensShift);
	mProjector1.lookAt(mLookAt1);
	mProjector2.lookAt(mLookAt2);

}
void VirtualDisplay::drawProjectors(){

	vec3 wTopLeft;
	vec3 wTopRight;
	vec3 wBottomLeft;
	vec3 wBottomRight;
	// draw the far plane Proj1
	mProjector1.getFarClipCoordinates(&wTopLeft, &wTopRight, &wBottomLeft, &wBottomRight);
	auto farPlane1 = gl::VertBatch(GL_TRIANGLE_STRIP);
	farPlane1.color(ColorA(0.0f, 0.0f, 0.0f, 0.0f));
	farPlane1.vertex(wTopLeft);
	farPlane1.vertex(wBottomLeft);
	farPlane1.vertex(wTopRight);
	farPlane1.vertex(wBottomRight);
	farPlane1.draw();

	// draw far plane pROJ2
	mProjector2.getFarClipCoordinates(&wTopLeft, &wTopRight, &wBottomLeft, &wBottomRight);
	auto farPlane2 = gl::VertBatch(GL_TRIANGLE_STRIP);
	farPlane2.color(ColorA(0.0f, 0.0f, 0.0f, 0.0f));
	farPlane2.vertex(wTopLeft);
	farPlane2.vertex(wBottomLeft);
	farPlane2.vertex(wTopRight);
	farPlane2.vertex(wBottomRight);
	farPlane2.draw();

	//draw near plane proj1
	mProjector1.getNearClipCoordinates(&wTopLeft, &wTopRight, &wBottomLeft, &wBottomRight);
	auto nearPlane1 = gl::VertBatch(GL_TRIANGLE_STRIP);
	nearPlane1.color(ColorA(0.0f, 0.0f, 0.0f, 0.0f));
	nearPlane1.vertex(wTopLeft);
	nearPlane1.vertex(wBottomLeft);
	nearPlane1.vertex(wTopRight);
	nearPlane1.vertex(wBottomRight);
	nearPlane1.draw();

	//draw near plane proj2
	mProjector2.getNearClipCoordinates(&wTopLeft, &wTopRight, &wBottomLeft, &wBottomRight);
	auto nearPlane2 = gl::VertBatch(GL_TRIANGLE_STRIP);
	nearPlane2.color(ColorA(0.0f, 0.0f, 0.0f, 0.0f));
	nearPlane2.vertex(wTopLeft);
	nearPlane2.vertex(wBottomLeft);
	nearPlane2.vertex(wTopRight);
	nearPlane2.vertex(wBottomRight);
	nearPlane2.draw();

	//ray from proj1 to lookat point
	auto ray1 = gl::VertBatch(GL_LINES);
	ray1.color(Color(0.0f, 1.0f, 0.0f));
	ray1.vertex(mProjector1.getEyePoint());
	ray1.vertex(mLookAt1);
	ray1.draw();

	//ray from proj2 to lookatpoint

	auto ray2 = gl::VertBatch(GL_LINES);
	ray2.color(Color(0.0f, 1.0f, 0.0f));
	ray2.vertex(mProjector2.getEyePoint());
	ray2.vertex(mLookAt2);
	ray2.draw();


	//draw Frustums!!
	//gl::ScopedColor(ColorA(1.0f, 0.0f, 0.0f, 1.0f));
	gl::drawFrustum(mProjector1);
	gl::drawFrustum(mProjector2);
}
	
	