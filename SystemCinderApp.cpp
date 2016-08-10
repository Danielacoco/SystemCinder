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
	settings->setWindowSize(1024, 768);
}
class SystemCinderApp : public App {
  public:

	
	void setupScene();
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void drawScene();
	void keyDown(KeyEvent event) override;

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
	void setupFbo();
	void setup() override;
	void draw() override;
	void createParams();
	//void resize() override;
	void drawSecond(); // draw onto second window
	void renderSceneToFbo();
	void createNewWindow(int width, int height);
	gl::FboRef mFbo;
	
	static const int FBO_WIDTH = 256, FBO_HEIGHT = 256;


};

const int SKY_BOX_SIZE = 600;


void SystemCinderApp::setup()
{
	//create configuration object to parse program information
	Configuration* configuration = new Configuration;
	configuration->SetConfigurationVariables(XML_FILE_PATH);
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
	int i = 1;
	/* open a window for every projector, one less than the number as default window is created automatically
	 still provide the default window with an instance of window Data*/
	getWindow()->setUserData(new WindowData);
	for (i; i < configuration->GetNumberOfProjectors(); i++){
		createNewWindow(configuration->GetWidthProjectors(), configuration->GetHeightProjectors());
	}
	WindowRef mMainWindow = getWindowIndex(0);
	WindowRef mSecondWindow = getWindowIndex(1);
	mMainWindow->getSignalDraw().connect([this] { drawSecond(); });
	//mSecondWindow->connectDraw(&SystemCinderApp::drawSecond, this);
	

	//to render our scene to an FBO
	gl::Fbo::Format format;
	mFbo = gl::Fbo::create(FBO_HEIGHT, FBO_WIDTH, format.depthTexture());

	//setUp the virtual display and its projectors
	//VirtualSphere->createSphere();
	//VirtualSphere->setUpProjectors();

	// setupScene() impelmented by user
	setupScene();

	gl::enableDepthRead();
	gl::enableDepthWrite();

	// track current time
	mCurrentSeconds = getElapsedSeconds();

	// setting the cameras to view scene and simulator
	// should work parallel
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
	VirtualSphere = new VirtualDisplay(&(configuration->GetDisplayType()));
	

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
}


void SystemCinderApp::keyDown(KeyEvent event){
	if (event.getCode() == KeyEvent::KEY_w){
		VirtualSphere->mWireframe == !VirtualSphere->mWireframe;
	}

}

void SystemCinderApp::drawSecond(){
	WindowData* data = getWindow()->getUserData<WindowData>();
	gl::clear(data->mColor);
	// for testing show the rendered scene to the FBO at the top left corner
	gl::setMatricesWindow(toPixels(getWindowSize()));
	gl::draw(mFbo->getColorTexture(), Rectf(0, 0, 129, 128));
	// also get the depth texture
	gl::draw(mFbo->getDepthTexture(), Rectf(129, 0, 256, 128));
}

void SystemCinderApp::createNewWindow(int width, int height){
	WindowRef newWindow = createWindow(Window::Format().size(width, height));
	newWindow->setUserData(new WindowData);
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

	WindowData *data = getWindow()->getUserData<WindowData>();
	
	gl::color(data->mColor);
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
	VirtualSphere->createDisplay(mFbo);

	//mFbo->bindTexture();

	{
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

	

	//drawScene();
}



CINDER_APP( SystemCinderApp, RendererGl )
