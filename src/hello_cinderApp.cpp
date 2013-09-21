#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class hello_cinderApp : public AppNative {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
};

void hello_cinderApp::setup()
{
}

void hello_cinderApp::mouseDown( MouseEvent event )
{
}

void hello_cinderApp::update()
{
}

void hello_cinderApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP_NATIVE( hello_cinderApp, RendererGl )
