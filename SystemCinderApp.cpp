#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "Configuration.h"
#include "WindowData.h"
#include "VirtualDisplay.h"




using namespace ci;
using namespace ci::app;
using namespace std;
char* XML_FILE_PATH = "sampleConfigFile.xml";


void prepareSettings(App::Settings* settings)
{
	settings->setWindowSize(1024, 1024);
}
class SystemCinderApp : public App {
  public:

	
	void setupScene();
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void drawScene();
	void keyDown(KeyEvent event) override;
	void mouseDrag(MouseEvent event) override;

	gl::TextureCubeMapRef	mCubeMap;
	gl::BatchRef			mTeapotBatch, mSkyBoxBatch, mSphereBatch;
	mat4				mObjectRotation;
	CameraPersp				mCam; 
	CameraPersp				mCamFbo;
	//Ui cameras to control mouse movement

	CameraUi	mCamUi;
	CameraUi	mCamFboUi;

	VirtualDisplay		*VirtualSphere;
protected:
	double mCurrentSeconds;
	
private:

	int mDisplayType;
	void setupFbo();
	void setup() override;
	void draw() override;
	void createParams();
	//void resize() override;
	void drawFirst();
	void drawSecond(); // draw onto second window
	void renderSceneToFbo();
	void createNewWindow(int width, int height);
	gl::FboRef mFbo;

	WindowRef mProjectors[5];
	
	static const int FBO_WIDTH = 256, FBO_HEIGHT = 256;


};

const int SKY_BOX_SIZE = 600;


void SystemCinderApp::setup()
{
	//create configuration object to parse program information
	Configuration* configuration = new Configuration;
	configuration->SetConfigurationVariables(XML_FILE_PATH);
	// get what kind of Display we will be using and assign it to our attribute mDisplayType
	mDisplayType = configuration->GetDisplayID();

	// Load corresponding shaders, place in assets and use something like this:
	/*
	try{
		mMappingShader = gl::GlslProg(
			loadFile("path/shaders/mappingShader.glsl"),
			loadFile ("path/shaders/mappingShader.glsl");
	}catch(...){
		console() << "Cant load/compile shader"<<endl;

	*/
	// After parsing configuration we determine how many projectors we have
	
	/*FBO where we will be rendering our scene */
	gl::Fbo::Format format;
	mFbo = gl::Fbo::create(FBO_HEIGHT, FBO_WIDTH, format.depthTexture());

	//setUp the virtual display and its projectors
	//VirtualSphere->createSphere();
	//VirtualSphere->setUpProjectors();

	/* if we need to track time uncomment following line */
	//mCurrentSeconds = getElapsedSeconds();

	/* the cameras to view scene and simulator should work parallel, therfore if we are using simulator
	//mCamFboUi = CameraUi(&mCamFbo, getWindow());
	//set up the camera to render our sphere
	/*CameraPersp mCam(getWindowWidth(), getWindowHeight(), 60.0f);
	mCam.setPerspective(60, getWindowAspectRatio(), 1, 1000);
	mCam.lookAt(vec3(8, 8, 8), vec3(0));*/
	// TESTING HAVING ONE SIGNLE CAMERA CONTROLLING SIMUALTOR AND SCENE 
	//NEXT 3 LINES COMMENTED OUT
	/*mCam.setFov(45.0f);
	mCam.setAspectRatio(getWindowAspectRatio());
	mCamUi = CameraUi(&mCam, getWindow());*/

	/*If virtualSphere or VirtualCube where determined in configuration we call the VirtualDisplayConstructor*/
	if (mDisplayType < 3){
		VirtualSphere = new VirtualDisplay(mDisplayType);
	}
	else {

		/*Else we the hardware sphere was selected, therefore we create the corresponding windows*/
		int i = 1;
		/* open a window for every projector, if we have 2 projectors we will have 3 windows in total (1 desktop monitor, 1 for projector1 and 1 for projector2*/
		/* As this is the defult window we set its size in prepareSettings(App::Settings* settings) at top of this file */
		WindowRef mMainWindow = getWindow();
		// dektop monitor windows
		mProjectors[0] = mMainWindow;
		for (i; i <= configuration->GetNumberOfProjectors(); i++){
			mProjectors[i] = createWindow(Window::Format().size(configuration->GetWidthProjectors(), configuration->GetHeightProjectors()));
		}
		/*connect each window with it's draw method where we will generate and send the corresponding texture from the FBO.
		You will need to create a method for every projector we add to the system*/
		mProjectors[0]->getSignalDraw().connect([this] { draw(); });
		mProjectors[1]->getSignalDraw().connect([this] { drawFirst(); });
		mProjectors[2]->getSignalDraw().connect([this] { drawSecond(); });
	}

	/* setupScene() impelmented by user as if it where a normal application*/
	setupScene();

	gl::enableDepthRead();
	gl::enableDepthWrite();
}
void SystemCinderApp::setupScene()
{
	//settingFboCameraUI ---> didnt work
	//CameraPersp mCamFbo(mFbo->getWidth(), mFbo->getHeight(), 60.0f);
	//mCamFbo.setPerspective(60, mFbo->getAspectRatio(), 1, 1000);
	////cam.lookAt(vec3(8 * sin(getElapsedSeconds() / 1 + 10), 7 * sin(getElapsedSeconds() / 2), 8 * cos(getElapsedSeconds() / 4 + 11)), vec3(0));
	//mCamFbo.lookAt(vec3(10, 10, 10), vec3(0));
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

	//mSphereBatch = gl::Batch::create(geom::Sphere().subdivisions(7), envMapGlsl);
	//mSphereBatch->getGlslProg()->uniform("uCubeMapTex", 0);

	mSkyBoxBatch = gl::Batch::create(geom::Cube(), skyBoxGlsl);
	mSkyBoxBatch->getGlslProg()->uniform("uCubeMapTex", 0);

}


void SystemCinderApp::mouseDown( MouseEvent event )
{
	mCamUi.mouseDown(event);
}


void SystemCinderApp::keyDown(KeyEvent event){
	if (event.getCode() == KeyEvent::KEY_w){
		VirtualSphere->mWireframe == !(VirtualSphere->mWireframe);
	}

}
void SystemCinderApp::drawFirst(){}

void SystemCinderApp::drawSecond(){
	WindowData* data = getWindow()->getUserData<WindowData>();
	gl::clear(data->mColor);
	// for testing show the rendered scene to the FBO at the top left corner
	gl::setMatricesWindow(toPixels(getWindowSize()));
	gl::draw(mFbo->getColorTexture(), Rectf(0, 0, 256, 128));
	// also get the depth texture
	Rectf drawRect(0, 0, 256, 128);
	gl::draw(mFbo->getDepthTexture(), drawRect);//Rectf(129, 0, 256, 128)
}

void SystemCinderApp::createNewWindow(int width, int height){
	WindowRef newWindow = createWindow(Window::Format().size(width, height));
	newWindow->setUserData(new WindowData);
}
void SystemCinderApp::mouseDrag(MouseEvent event)
{
	mCamUi.mouseDrag(event);
}

void SystemCinderApp::renderSceneToFbo()
{	
	gl::ScopedFramebuffer fbScp(mFbo);
	gl::clear(Color(0, 0, 0));
	
	

	//set the viewport to match the dimensions of the FBO;
	gl::ScopedViewport scVprt(ivec2(0), mFbo->getSize());

	//set camera to rendrer the flying teapot scene
	///CameraPersp mCamFbo(mFbo->getWidth(), mFbo->getHeight(), 60.0f);
	///mCamFbo.setPerspective(60, mFbo->getAspectRatio(), 1, 1000);
	//cam.lookAt(vec3(8 * sin(getElapsedSeconds() / 1 + 10), 7 * sin(getElapsedSeconds() / 2), 8 * cos(getElapsedSeconds() / 4 + 11)), vec3(0));
	///mCamFbo.lookAt(vec3(10, 10, 10), vec3(0));
	gl::setMatrices(mCamFbo);

	// set modelview matrix to reflect current rotation
	//gl::setModelMatrix(mObjectRotation);

	drawScene();
}
void SystemCinderApp::update()
{
	// moving camera randomly around based on time
	//mCam.lookAt(vec3(8 * sin(getElapsedSeconds() / 1 + 10), 7 * sin(getElapsedSeconds() / 2), 8 * cos(getElapsedSeconds() / 4 + 11)), vec3(0));

	//elapsed time
	double timePassed = getElapsedSeconds() - mCurrentSeconds;
	mCurrentSeconds += timePassed;




	// rotate the teapot object
	mObjectRotation *= rotate(0.04f, normalize(vec3(0.1f, 1, 0.1f)));

	renderSceneToFbo();

}
void SystemCinderApp::drawScene()
{
	//gl::setMatrices(mCam);

	//draw our teapor object
	mCubeMap->bind();
	gl::pushMatrices();
	gl::multModelMatrix(mObjectRotation);
	gl::scale(vec3(4));
	mTeapotBatch->draw();
	//mSphereBatch->draw();
	gl::popMatrices();

	//draw skybox
	gl::pushMatrices();
	gl::scale(SKY_BOX_SIZE, SKY_BOX_SIZE, SKY_BOX_SIZE);
	mSkyBoxBatch->draw();
	gl::popMatrices();

}
void SystemCinderApp::draw()
{
	gl::clear(Color(1.0f, 1.0f, 1.0f));

	//WindowData *data = getWindow()->getUserData<WindowData>();
	
	//gl::color(data->mColor);
	//set up the camera to render our sphere
	/*CameraPersp mCam(getWindowWidth(), getWindowHeight(), 60.0f);
	mCam.setPerspective(60, getWindowAspectRatio(), 1, 1000);
	mCam.lookAt(vec3(8, 8, 8), vec3(0));*/


	// for testing show the rendered scene to the FBO at the top left corner
	gl::setMatricesWindow(toPixels(getWindowSize()));
	gl::draw(mFbo->getColorTexture(), Rectf(0, 0, 129, 128));
	// also get the depth texture
	gl::draw(mFbo->getDepthTexture(), Rectf(129, 0, 256, 128));
	
	//->
	//gl::ScopedMatrices scopeMatrix;
	gl::setMatrices(mCamFbo);
	
	// make texture out of the scene rendered into our FBO
	//VirtualSphere->createDisplay();

	//mFbo->bindTexture();
	VirtualSphere->setUpFboShouldBeMappedTexture(mFbo);
	VirtualSphere->createShaders();
	VirtualSphere->createDisplay();

	{
		//vec3 offset(0, 3, 0);
		//gl::translate(offset);
		// use our FBO to texture the sphere
		//gl::ScopedGlslProg shaderScp(gl::getStockShader(gl::ShaderDef().texture()));
		VirtualSphere->mDisplay->draw();
		//gl::drawSphere(vec3(0,3,0), 2.5, 1);
	}
	
	

	
	VirtualSphere->createGrid();
	VirtualSphere->mGrid->draw();
	gl::drawCoordinateFrame(4);

	VirtualSphere->setUpProjectors();
	VirtualSphere->drawProjectors();

#if ! defined( CINDER_GL_ES )
	if (VirtualSphere->mParams)
		VirtualSphere->mParams->draw();
#endif

	

	//drawScene();
}



CINDER_APP( SystemCinderApp, RendererGl, prepareSettings )
