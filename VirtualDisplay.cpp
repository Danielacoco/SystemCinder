#include "VirtualDisplay.h"

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

void VirtualDisplay::createSphere(){

	try {
		mWireShader = gl::context()->getStockShader(gl::ShaderDef().color());
	}
	catch (Exception &exc) {
		exit;
	}


	if (mWireframe == true){
		mWiredSphere = gl::Batch::create(geom::WireSphere().radius(3).center(vec3(0,3,0)).subdivisionsHeight(mDivVerti).subdivisionsAxis(mDivHoriz).subdivisionsCircle(mDivCircle), mWireShader);
	}
	else {

		mWiredSphere = gl::Batch::create(geom::Sphere().radius(3).center(vec3(0, 3, 0)), mWireShader);
	}



}

void VirtualDisplay::setUpProjectors(){

	vec3 EyePointProj1 = vec3(-2, 0, 0);
	vec3 EyePointProj2 = vec3(2, 0, 0); 
	float Fov = 25.0f;
	float nearPlane = 1.0;
	float farPlane = 6.5;
	vec2 LensShift = vec2(0);
	

	mProjector1.setEyePoint(EyePointProj1);
	mProjector2.setEyePoint(EyePointProj2);
	mProjector1.setPerspective(Fov, (3/4.0), nearPlane, farPlane );
	mProjector2.setPerspective(Fov, (3 / 4.0), nearPlane, farPlane);
	mProjector1.setLensShift(LensShift);
	mProjector2.setLensShift(LensShift);
	mProjector1.lookAt(mLookAt);
	mProjector2.lookAt(mLookAt);

	


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
	ray1.vertex(mLookAt);
	ray1.draw();

	//ray from proj2 to lookatpoint

	auto ray2 = gl::VertBatch(GL_LINES);
	ray2.color(Color(0.0f, 1.0f, 0.0f));
	ray2.vertex(mProjector2.getEyePoint());
	ray2.vertex(mLookAt);
	ray2.draw();


	//draw Fustums!!
	//gl::color(ColorA(1.0f, 0.0f, 0.0f, 0.0f));
	gl::drawFrustum(mProjector1);
	gl::drawFrustum(mProjector2);
}
	
	