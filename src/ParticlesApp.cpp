
#include "ParticlesApp.h"

void ParticlesApp::prepareSettings(Settings *settings)
{
    settings->setTitle("Particles");
    settings->setWindowSize( 512, 512 );
    settings->setFrameRate( 60.0f );
    settings->setResizable(true);
}

void ParticlesApp::setup()
{
    setFullScreen(false);
  
    m_capture = false;
    gl::enableAlphaBlending();

#ifdef USE_KINECT
    setupKinect();
#endif
  
    setupGui();
}

void ParticlesApp::quit()
{
    delete m_pGUIOff;
    delete m_pGUIOn;
}

void ParticlesApp::update()
{
  m_eventHandled = false;
  
#ifdef USE_KINECT
    if (useKinect() && m_kinect) {
        if (m_kinect->checkNewDepthFrame())
            m_kinectDepthTexture = m_kinect->getDepthImage();
        
//        if (m_kinect->checkNewVideoFrame())
//            m_kinectColorTexture = m_kinect->getVideoImage();

        mEye = Vec3f( 0.0f, 0.0f, mCameraDistance );
        mCam.lookAt( mEye, mCenter, mUp );
    }
#endif
  
    m_pGUIOff->update();
    m_pGUIOn->update();

    m_particleController.update();
    
//    if (getElapsedFrames() % 100  == 0) {
//        cout << "num particles: # " << m_particleController.numParticles() << endl;
//    }
  
}

void ParticlesApp::draw()
{
//    ColorAf averageColor = m_particleController.averageColors();
//    ColorAf clearColor(245/255.0, 245/255.0, 220/255.0); // 1 - averageColor.r, 1 - averageColor.g, 1 - averageColor.b);
    ColorAf clearColor(Colorf::black());
    gl::clear(clearColor, true); //clearColor * .5, true );
    
    m_particleController.draw();

    if (m_capture && !m_capturePath.empty()) {
        fs::path path = m_capturePath;
        path /=  "image_" + to_string(getElapsedFrames()) + ".png";
        cout << "Writing to file " << path << endl;
        writeImage(path, copyWindowSurface() );
    }

//    if (m_params.getb("bounce")) {
//        gl::color(Color::white());
//        Rectf r = getWindowBounds();
//        gl::drawStrokedRect(r);
//    }

    Vec2f windowsize = getWindowBounds().getSize();

#ifdef USE_KINECT
    if (useKinect() && m_kinect) {
        gl::color(Color::white());
        if( m_kinectDepthTexture ) {
//            gl::draw(m_kinectDepthTexture, Rectf(0, windowsize.y - 120, 160, windowsize.y));
            gl::pushMatrices();
            gl::setMatrices( mCam );

            gl::scale( Vec3f( -1.0f, -1.0f, 1.0f ) );
            gl::rotate( mSceneRotation );
            m_kinectDepthTexture.bind( 0 );
            mShader.bind();
            mShader.uniform( "depthTex", 0 );
            mShader.uniform( "depthThresholdLo", m_params.getf("kdepthThresholdLo") );
            mShader.uniform( "depthThresholdHi", m_params.getf("kdepthThresholdHi") );
            gl::draw( mVboMesh );
            mShader.unbind();
            gl::popMatrices();
        }
//        if( m_kinectColorTexture )
//            gl::draw( m_kinectColorTexture, Rectf(165, windowsize.y - 120, 325, windowsize.y) );
    }
#endif
  
    m_pGUIOff->draw();
    m_pGUIOn->draw();

}

void ParticlesApp::setupGui()
{
    m_pGUIOff = new ciUICanvas();
    m_pGUIOff->addWidgetDown(new ciUILabelButton(false, ">>", CI_UI_FONT_LARGE));
    m_pGUIOff->registerUIEvents(this, &ParticlesApp::guiEvent);
    
    m_pGUIOn = new ciUICanvas();
    m_pGUIOn->toggleVisible();
    m_pGUIOn->addWidgetDown(new ciUILabelButton(false, "<<", CI_UI_FONT_LARGE));
//    m_pGUIOn->addWidgetDown(new ciUILabelToggle(m_params.getb("bounce"), "bounce", CI_UI_FONT_MEDIUM));
    m_pGUIOn->addWidgetDown(new ciUISlider(70, 15, 1, 40, m_params.getf("size"), "size"));
    m_pGUIOn->addWidgetDown(new ciUISlider(70, 15, 1, 20, m_params.getf("lifespan"), "lifespan"));
    m_pGUIOn->addWidgetDown(new ciUISlider(70, 15, 0, 50, m_params.getf("pulse_rate"), "pulse_rate"));
    m_pGUIOn->addWidgetDown(new ciUISlider(70, 15, 1, 8, m_params.geti("symmetry"), "symmetry", true));

    m_pGUIOn->addWidgetDown(new ciUIFPS(CI_UI_FONT_SMALL));


#ifdef USE_KINECT
    if (m_kinect) {
        m_pGUIOn->addWidgetDown(new ciUILabelToggle(m_params.getb("kinect"), "kinect", CI_UI_FONT_MEDIUM));
        m_pGUIOn->addWidgetDown(new ciUIRangeSlider(20,120,0.0,1,m_params.getf("kdepthThresholdLo"),m_params.getf("kdepthThresholdHi"), "kdepth"));
    }
#endif
  
    m_pGUIOn->registerUIEvents(this, &ParticlesApp::guiEvent);
    
}

#ifdef USE_KINECT

void ParticlesApp::setupKinect()
{
    cout << "Found " << Kinect::getNumDevices() << " Kinects" << endl;
    if (Kinect::getNumDevices() > 0) {
        //m_kinect = Kinect::create();
    } else {
        cout << "Didn't find any Kinects" << endl;
    }
    m_params.setb("kinect", (m_kinect != NULL));

    if (m_kinect) {
        // SETUP CAMERA
        mCameraDistance = 1000.0f;
        mEye			= Vec3f( 0.0f, 0.0f, mCameraDistance );
        mCenter			= Vec3f::zero();
        mUp				= Vec3f::yAxis();
        mCam.setPerspective( 75.0f, getWindowAspectRatio(), 1.0f, 8000.0f );

        // SETUP VBO AND SHADER
        createVbo();
    	mShader	= gl::GlslProg( loadResource( "mainVert.glsl" ), loadResource( "mainFrag.glsl" ) );

        // SETUP GL
        gl::enableDepthWrite();
        gl::enableDepthRead();

    }
}

void ParticlesApp::createVbo()
{
	gl::VboMesh::Layout layout;
	
	layout.setStaticPositions();
	layout.setStaticTexCoords2d();
	layout.setStaticIndices();
	
	std::vector<Vec3f> positions;
	std::vector<Vec2f> texCoords;
	std::vector<uint32_t> indices;
	
	int numVertices = VBO_X_RES * VBO_Y_RES;
	int numShapes	= ( VBO_X_RES - 1 ) * ( VBO_Y_RES - 1 );
    
	mVboMesh		= gl::VboMesh( numVertices, numShapes, layout, GL_POINTS );
	
	for( int x=0; x<VBO_X_RES; ++x ){
		for( int y=0; y<VBO_Y_RES; ++y ){
			indices.push_back( x * VBO_Y_RES + y );
            
			float xPer	= x / (float)(VBO_X_RES-1);
			float yPer	= y / (float)(VBO_Y_RES-1);
			
			positions.push_back( Vec3f( ( xPer * 2.0f - 1.0f ) * VBO_X_RES, ( yPer * 2.0f - 1.0f ) * VBO_Y_RES, 0.0f ) );
			texCoords.push_back( Vec2f( xPer, yPer ) );
		}
	}
	
	mVboMesh.bufferPositions( positions );
	mVboMesh.bufferIndices( indices );
	mVboMesh.bufferTexCoords2d( 0, texCoords );
}

#endif


void ParticlesApp::guiEvent(ciUIEvent *event)
{
    if (m_eventHandled) {
      return;
    }
  
    string name = event->widget->getName();
    bool handled = true;

    cout << "guiEvent '" << name << "'" << endl;
  
    if(name == ">>" || name == "<<")
    {
        m_toggleMenu = true;
    } else if (name == "kinect") {
        m_params.setb("kinect", !m_params.getb("kinect"));
    } else if (name == "kdepth") {
        ciUIRangeSlider *rslider = (ciUIRangeSlider *) event->widget;
        m_params.setf("kdepthThresholdLo", rslider->getScaledValueLow());
        m_params.setf("kdepthThresholdHi", rslider->getScaledValueHigh());
//    } else if (name == "bounce") {
//        m_params.setb("bounce", !m_params.getb("bounce"));
    } else if (name == "pulse_rate") {
        ciUISlider *slider = (ciUISlider *) event->widget;
        m_params.setf("pulse_rate", slider->getScaledValue());
    } else if (name == "size") {
        ciUISlider *slider = (ciUISlider *) event->widget;
        m_params.setf("size", slider->getScaledValue());
    } else if (name == "lifespan") {
        ciUISlider *slider = (ciUISlider *) event->widget;
        m_params.setf("lifespan", slider->getScaledValue());
    } else if (name == "symmetry") {
        ciUISlider *slider = (ciUISlider *) event->widget;
        m_params.seti("symmetry", slider->getScaledValue());
    } else {
        handled = false;
    }
  
    m_eventHandled = true;
    event->setHandled(handled);
    
    
}

void ParticlesApp::mouseDown( MouseEvent event )
{
    bool handled = m_eventHandled;

    cout << "mouseDown, handled: " << handled << endl;
    if (handled) {
        return;
    }
    
    if (event.isLeftDown()) {
        ColorAf birthColor(Rand::randFloat(), Rand::randFloat(), Rand::randFloat());
        ColorAf deathColor = ColorAf(1 - birthColor.r, 1 - birthColor.g, 1 - birthColor.b);
        m_particleController.setColors(birthColor, deathColor);

        m_lastMouseLoc = event.getPos();
        addParticleAt(m_lastMouseLoc, Vec2f(0, 0));
    }
}

void ParticlesApp::mouseUp(MouseEvent event)
{
    bool handled = m_eventHandled;
    cout << "mouseUp, handled: " << handled << endl;
    
    // hack for double menu click
    if (m_toggleMenu) {
        toggleMenu();
        m_toggleMenu = false;
    }
}

void ParticlesApp::mouseDrag(MouseEvent event)
{
    bool handled = m_eventHandled;
    cout << "mouseDrag, handled: " << handled << endl;

    if (handled) {
        return;
    }

    Vec2f mouse_vec = event.getPos() - m_lastMouseLoc;
    //    if (event.isLeftDown()) {
    int num_particles = Rand::randInt(10);
    for (int i = 0; i < num_particles; i++) {
        addParticleAt(event.getPos(), mouse_vec * .25);
    }
    //    }
    m_lastMouseLoc = event.getPos();
}

void ParticlesApp::addParticleAt(Vec2f position, Vec2f vector)
{
    Vec2f size = getWindowBounds().getSize();
    Vec2f center = size / 2.0;
    
    Vec2f cPosition = position - center;
    
    float pAngle = atan2(cPosition.y, cPosition.x);
    float dist = sqrt(cPosition.x * cPosition.x + cPosition.y * cPosition.y);

    float vAngle = atan2(vector.y, vector.x);
    int symmetry = m_params.geti("symmetry");
    float slice = M_PI * 2 / symmetry;

    for (int i = 0; i < symmetry; i++)
    {
        Vec2f newPos = Vec2f(cos(pAngle), sin(pAngle)) * dist + center;
        Vec2f newVec = Vec2f(cos(vAngle), sin(vAngle)) * vector.length();

        pAngle += slice;
        vAngle += slice;
        
        m_particleController.addParticleAt(newPos, newVec);
    }
    
}

void ParticlesApp::toggleMenu()
{
    m_pGUIOff->toggleVisible();
    m_pGUIOn->toggleVisible();
}

void ParticlesApp::keyDown( KeyEvent event )
{
//    cout << "keyDown '" << event.getChar() << "'";
//    if (event.isShiftDown()) {
//        cout << " SHIFT";
//    }
//    if (event.isControlDown()) {
//        cout << " CTRL";
//    }
//    if (event.isAltDown()) {
//        cout << " ALT";
//    }
//    if (event.isMetaDown()) {
//        cout << " META";
//    }
//    cout << endl;
    
    switch(event.getChar()) {
        case 'f': {
            setFullScreen(!isFullScreen());
        } break;
            
        case 'p': {
            m_particleController.addParticles(Rand::randInt(1000));
        } break;

        case 'P': {
            m_particleController.removeParticles(Rand::randInt(100));
        } break;
        
        case 's': {
            int symmetry = m_params.geti("symmetry");
            symmetry %= 8;
            symmetry++;
            m_params.seti("symmetry", symmetry);
        } break;
            
        case ' ': {
            m_capture = !m_capture;

            if (m_capture && (m_capturePath.empty() || event.isShiftDown())) {
                m_capturePath = getFolderPath();
            }
            if (m_capturePath.empty())
                m_capture = false;
        } break;
            
        case '/':
        case '?': {
            toggleMenu();
        } break;
    }
}


CINDER_APP_NATIVE( ParticlesApp, RendererGl )
