/*SYSTEMCINDERAPP is the core of the application, this class has the methods the user can use to create its demo and it also has the private methods we will use to 
show the simulator or render the images to the projectors 
Cinders website: https://libcinder.org/ 
It is a easy to use library with a responsive community. It has core classes for cameras, meshes, geometric primitives and Objec loading. Also classes for texture
FBO, lights, VBOs, GLSL. Webcam support fro video capture and an OO wrapper for OpenCV 3.0 which I though would be useful for the BologramProject*/

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

class SystemCinderApp : public App {
public:

	/*setupScene, Update and Draw Scene are the method the user can modify to create content*/
	void setupScene();
	void update() override;
	void drawScene();
	void mouseDown(MouseEvent event) override;
	void keyDown(KeyEvent event) override;
	void mouseDrag(MouseEvent event) override;

	/*Demo specific vars*/
	gl::TextureCubeMapRef	mCubeMap;
	gl::BatchRef			mTeapotBatch, mSkyBoxBatch, mSphereBatch;
	mat4				mObjectRotation;
	CameraPersp				mCamFbo;

	//Ui cameras to control mouse movement
	CameraUi	mCamFboUi;

	VirtualDisplay		*mVirtualDisplay;

protected:
	double mCurrentSeconds;

private:

	int mDisplayType;
	void setupFbo();
	void setup() override;
	void drawMain();
	void drawFirst();
	void drawSecond(); 
	//drawethird
	void drawSimulator();
	void createParams();

	void renderSceneToFbo();
	/*FBO where we will be rendering user's scene to*/
	gl::FboRef mFbo;
	/* Array of projectors (windows for projectors) max size 5*/
	WindowRef mProjectors[5];
	/* Array of Displays/ monitors connected to the system*/
	vector<DisplayRef> mDisplays = ci::Display::getDisplays();

	/*size of FBO*/
	static const int FBO_WIDTH = 256, FBO_HEIGHT = 256;
};