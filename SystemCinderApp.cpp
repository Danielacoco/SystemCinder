#include "SystemCinderApp.h"

void prepareSettings(App::Settings* settings)
{
	settings->setWindowSize(900, 700);
}

void SystemCinderApp::setup()
{
	//create configuration object to parse program information
	Configuration* configuration = new Configuration(XML_FILE_PATH);
	// get what kind of Display we will be using and assign it to our attribute mDisplayType
	mDisplayType = configuration->GetDisplayID();

	/*FBO where we will be rendering our scene */
	gl::Fbo::Format format;
	mFbo = gl::Fbo::create(FBO_HEIGHT, FBO_WIDTH, format.depthTexture());
	// Load corresponding shaders, place in assets and use something like this:
	/*
	try{
		mMappingShader = gl::GlslProg(
			loadFile("path/shaders/mappingShader.glsl"),
			loadFile ("path/shaders/mappingShader.glsl");
	}catch(...){
		console() << "Cant load/compile shader"<<endl;

	*/
	WindowRef mMainWindow = getWindow();
	// dektop monitor window goes in index 0
	mProjectors[0] = mMainWindow;
	/*If the hardware sphere was selected, we create the corresponding windows*/
	if (mDisplayType == 3){

		/*Determine how many Displays/Monitors are connected to the system*/
		/*I only have one but once we use the projectors m Display should contain a reference to all the displays connected to the system */
		mDisplays = ci::Display::getDisplays();

		int i = 1;
		/* open a window for every projector, if we have 2 projectors we will have 3 windows in total (1 desktop monitor, 1 for projector1 and 1 for projector2*/
		/* As this is the defult window we set its size in prepareSettings(App::Settings* settings) at top of this file */
		for (i; i <= configuration->GetNumberOfProjectors(); i++){
			mProjectors[i] = createWindow(Window::Format().size(configuration->GetWidthProjectors(), configuration->GetHeightProjectors()));
			/* .display sets each window to a monitor but becuase now I only have one monitor all widnows will display to the same monitor */
			//mProjectors[i] = createWindow(Window::Format().display(mDisplays[i]).size(configuration->GetWidthProjectors(), configuration->GetHeightProjectors()));
		}
		/*connect each window with it's draw method where we will generate and send the corresponding texture from the FBO.
		You will need to create a method for every projector we add to the system*/
		/* build a generate draw for each projector*/
		mProjectors[0]->getSignalDraw().connect([this] { drawMain(); });
		mProjectors[1]->getSignalDraw().connect([this] { drawFirst(); });
		mProjectors[2]->getSignalDraw().connect([this] { drawSecond(); });
		//mProjectors[3]->getSignalDraw().connect([this] { drawThird(); });



	/*Else virtualSphere or VirtualCube where determined in configuration we call the VirtualDisplayConstructor*/
	}else {
		mVirtualDisplay = new VirtualDisplay(mDisplayType);
		mProjectors[0]->getSignalDraw().connect([this] { drawSimulator(); });

	}

	setupScene();

	gl::enableDepthRead();
	gl::enableDepthWrite();
}

/* setupScene() impelmented by user as if it where a normal application*/
void SystemCinderApp::setupScene()
{
	/* Following scene was created from Cinder's sample application CubeMappingApp */
	mCamFbo.setFov(45.0f);
	mCamFbo.setAspectRatio(getWindowAspectRatio());

	mCamFboUi = CameraUi(&mCamFbo, getWindow());
	mCubeMap = gl::TextureCubeMap::create(loadImage(loadAsset("env_map.jpg")), gl::TextureCubeMap::Format().mipmap());
#if defined (CINDER_GL_ES)
	auto envMapGlsl = gl::GlslProg::create(loadAsset("env_map_es2.vert"), loadAsset("env_map_es2.frag"));
	auto skyBoxGlsl = gl::GlslProg::create(loadAsset("sky_box_es2.vert"), load Asset("sky_box_es2.frag"));
#else
	auto envMapGlsl = gl::GlslProg::create(loadAsset("env_map.vert"), loadAsset("env_map.frag"));
	auto skyBoxGlsl = gl::GlslProg::create(loadAsset("sky_box.vert"), loadAsset("sky_box.frag"));
#endif

	mTeapotBatch = gl::Batch::create(geom::Teapot().subdivisions(7), envMapGlsl);
	//send uniform to a shader
	mTeapotBatch->getGlslProg()->uniform("uCubeMapTex", 0);

	mSkyBoxBatch = gl::Batch::create(geom::Cube(), skyBoxGlsl);
	mSkyBoxBatch->getGlslProg()->uniform("uCubeMapTex", 0);

}

void SystemCinderApp::drawScene()
{
	//draw our teapot object
	mCubeMap->bind();
	gl::pushMatrices();
	gl::multModelMatrix(mObjectRotation);
	gl::scale(vec3(4));
	mTeapotBatch->draw();
	//mSphereBatch->draw();
	gl::popMatrices();

	//draw skybox
	gl::pushMatrices();
	gl::scale(600, 600, 600);
	mSkyBoxBatch->draw();
	gl::popMatrices();

}

void SystemCinderApp::renderSceneToFbo()
{
	gl::ScopedFramebuffer fbScp(mFbo);
	gl::clear(Color(0, 0, 0));

	//set the viewport to match the dimensions of the FBO;
	gl::ScopedViewport scVprt(ivec2(0), mFbo->getSize());

	gl::setMatrices(mCamFbo);

	drawScene();
}

void SystemCinderApp::update()
{
	/*update head pos and interaction goes here*/
	//elapsed time
	double timePassed = getElapsedSeconds() - mCurrentSeconds;
	mCurrentSeconds += timePassed;

	// rotate the teapot object
	mObjectRotation *= rotate(0.04f, normalize(vec3(0.1f, 1, 0.1f)));

	renderSceneToFbo();

}

void SystemCinderApp::drawMain(){ /*draw your debug window, could be FBO*/
	/* This method should include anything you would want to see while the Hardware Sphere/Display is operating,
	right now the Default window is drawing some other demo :P*/
	//----------------------//
	//CODE FROM CINDER PAGE //
	//---------------------//
	gl::clear();
	gl::enableDepthRead();
	gl::enableDepthWrite();

	CameraPersp cam;
	cam.lookAt(vec3(5, 2, 5), vec3(0, 1, 0));
	gl::setMatrices(cam);

	auto lambert = gl::ShaderDef().lambert().color();
	auto shader = gl::getStockShader(lambert);
	shader->bind();

	int numSpheres = 64;
	float maxAngle = M_PI * 7;
	float spiralRadius = 1;
	float height = 3;

	for (int s = 0; s < numSpheres; ++s) {
		float rel = s / (float)numSpheres;
		float angle = rel * maxAngle;
		float y = rel * height;
		float r = rel * spiralRadius * spiralRadius;
		vec3 offset(r * cos(angle), y, r * sin(angle));

		gl::pushModelMatrix();
		gl::translate(offset);
		gl::color(Color(CM_HSV, rel, 1, 1));
		gl::drawSphere(vec3(), 0.1f, 30);
		gl::popModelMatrix();
	}
	//---------------------------//
	//CODE FROM CINDER PAGE END //
	//-------------------------//

}

/*Draw method for first projector, Projector1 specific code goes here*/
void SystemCinderApp::drawFirst(){
	gl::viewport(getWindowSize());
	gl::setMatricesWindow(getWindowSize());

	gl::clear(Color::gray(0.5f));

	gl::drawSolidCircle(getWindowCenter(), 150.0f);

	/*loading shaders and/or projector data here*/

	//auto img = loadAsset("data.bin");
	//gl::Texture2dRef mTex = gl::Texture2d::create(img);

	//if abouve does not work we can import the points loader Qian created in texture.h/.cpp 

}

/*Draw method for first projector, Projector2 specific code goes here*/
void SystemCinderApp::drawSecond(){
	/* Following code extracted from CINDER Guide's getting started page*/
	//----------------------//
	//CODE FROM CINDER PAGE //
	//---------------------//
	gl::clear();

	// preserve the default Model matrix
	gl::pushModelMatrix();
	// move to the window center
	gl::translate(getWindowCenter());

	int numCircles = 16;
	float radius = getWindowHeight() / 2 - 30;

	for (int c = 0; c < numCircles; ++c) {
		float rel = c / (float)numCircles;
		float angle = rel * M_PI * 2;
		vec2 offset(cos(angle), sin(angle));

		// preserve the Model matrix
		gl::pushModelMatrix();
		// move to the correct position
		gl::translate(offset * radius);
		// set the color using HSV color
		gl::color(Color(CM_HSV, rel, 1, 1));
		// draw a circle relative to Model matrix
		gl::drawStrokedCircle(vec2(), 30);
		// restore the Model matrix
		gl::popModelMatrix();
	}

	// draw a white circle at window center
	gl::color(Color(1, 1, 1));
	gl::drawSolidCircle(vec2(), 15);

	// restore the default Model matrix
	gl::popModelMatrix();

	//---------------------------//
	//CODE FROM CINDER PAGE END //
	//-------------------------//

	/*loading shaders and/or projector data here*/
}

void SystemCinderApp::drawSimulator()
{
	gl::viewport(getWindowSize());
	gl::setMatricesWindow(getWindowSize());
	gl::clear(Color(0.0f, 0.0f, 0.2f));

	// for testing show the rendered scene to the FBO at the top left corner
	gl::setMatricesWindow(toPixels(getWindowSize()));
	gl::draw(mFbo->getColorTexture(), Rectf(0, 0, 129, 128));
	// also get the depth texture
	gl::draw(mFbo->getDepthTexture(), Rectf(129, 0, 256, 128));

	gl::setMatrices(mCamFbo);

	// If the Display type is not the real hardware sphere, we need to call simulator methods.
	if (mDisplayType != 3){
		mVirtualDisplay->setUpFboShouldBeMappedTexture(mFbo);
		mVirtualDisplay->createShaders();
		mVirtualDisplay->createDisplay();

		{
			mVirtualDisplay->mDisplay->draw();
		}

		mVirtualDisplay->createGrid();
		mVirtualDisplay->mGrid->draw();
		gl::drawCoordinateFrame(4);

		mVirtualDisplay->setUpProjectors();
		mVirtualDisplay->drawProjectors();

#if ! defined( CINDER_GL_ES )
		if (mVirtualDisplay->mParams)
			mVirtualDisplay->mParams->draw();
#endif

	}
}

void SystemCinderApp::mouseDrag(MouseEvent event)
{
	mCamFboUi.mouseDrag(event);
}

void SystemCinderApp::mouseDown(MouseEvent event)
{
	mCamFboUi.mouseDown(event);
}

void SystemCinderApp::keyDown(KeyEvent event){
	if (event.getCode() == KeyEvent::KEY_w){
		mVirtualDisplay->mWireframe == !(mVirtualDisplay->mWireframe);
	}

}

CINDER_APP( SystemCinderApp, RendererGl, prepareSettings )
