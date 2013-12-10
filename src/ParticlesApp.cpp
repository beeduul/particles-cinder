
#include "ParticlesApp.h"

void ParticlesApp::prepareSettings(Settings *settings)
{
    settings->setWindowSize( 512, 512 );
    settings->setFrameRate( 60.0f );
    settings->setResizable(true);
}

void ParticlesApp::setup()
{
    m_capture = false;
    gl::enableAlphaBlending();
    
    setupKinect();

    setupGui();
}

void ParticlesApp::quit()
{
    delete pGuiOff;
    delete pGuiOn;
}

void ParticlesApp::update()
{
    if (useKinect() && m_kinect) {
        if (m_kinect->checkNewDepthFrame())
            m_kinectDepthTexture = m_kinect->getDepthImage();
        
//        if (m_kinect->checkNewVideoFrame())
//            m_kinectColorTexture = m_kinect->getVideoImage();

        mEye = Vec3f( 0.0f, 0.0f, mCameraDistance );
        mCam.lookAt( mEye, mCenter, mUp );
    }
    
    if (bToggleMenu) {
        pGuiOff->toggleVisible();
        pGuiOn->toggleVisible();
        bToggleMenu = false;
    }
    pGuiOff->update();
    pGuiOn->update();

    particle_controller.update();
    
    if (getElapsedFrames() % 100  == 0) {
        cout << "num particles: # " << particle_controller.numParticles() << endl;
    }
    
}

void ParticlesApp::draw()
{
//    ColorAf averageColor = particle_controller.averageColors();
//    ColorAf clearColor(245/255.0, 245/255.0, 220/255.0); // 1 - averageColor.r, 1 - averageColor.g, 1 - averageColor.b);
    ColorAf clearColor(Colorf::black());
    gl::clear(clearColor, true); //clearColor * .5, true );
    
    if (!filePath.empty()) {
        gl::draw( myImage, getWindowBounds() );
    }
    
    particle_controller.draw();

    if (m_capture && !m_capturePath.empty()) {
        fs::path path = m_capturePath;
        path /=  "image_" + to_string(getElapsedFrames()) + ".png";
        cout << "Writing to file " << path << endl;
        writeImage(path, copyWindowSurface() );
    }

    if (_params.getb("bounce")) {
        gl::color(Color::white());
        Rectf r = getWindowBounds();
        gl::drawStrokedRect(r);
    }

    Vec2f windowsize = getWindowBounds().getSize();

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
            mShader.uniform( "depthThresholdLo", _params.getf("kdepthThresholdLo") );
            mShader.uniform( "depthThresholdHi", _params.getf("kdepthThresholdHi") );
            gl::draw( mVboMesh );
            mShader.unbind();
            gl::popMatrices();
        }
//        if( m_kinectColorTexture )
//            gl::draw( m_kinectColorTexture, Rectf(165, windowsize.y - 120, 325, windowsize.y) );
    }
    
    pGuiOff->draw();
    pGuiOn->draw();

}

void ParticlesApp::setupGui()
{
    pGuiOff = new ciUICanvas();
    pGuiOff->addWidgetDown(new ciUILabelButton(false, ">>", CI_UI_FONT_LARGE));
    pGuiOff->registerUIEvents(this, &ParticlesApp::guiEvent);
    
    pGuiOn = new ciUICanvas();
    pGuiOn->toggleVisible();
    pGuiOn->addWidgetDown(new ciUILabelButton(false, "<<", CI_UI_FONT_LARGE));
    pGuiOn->addWidgetDown(new ciUILabelToggle(_params.getb("bounce"), "bounce", CI_UI_FONT_MEDIUM));
    pGuiOn->addWidgetDown(new ciUISlider(70, 15, 1, 40, _params.getf("size"), "size"));
    pGuiOn->addWidgetDown(new ciUISlider(70, 15, 1, 20, _params.getf("lifespan"), "lifespan"));
    pGuiOn->addWidgetDown(new ciUISlider(70, 15, 1, 8, _params.geti("symmetry"), "symmetry", true));

    if (m_kinect) {
        pGuiOn->addWidgetDown(new ciUILabelToggle(_params.getb("kinect"), "kinect", CI_UI_FONT_MEDIUM));
        pGuiOn->addWidgetDown(new ciUIRangeSlider(20,120,0.0,1,_params.getf("kdepthThresholdLo"),_params.getf("kdepthThresholdHi"), "kdepth"));
    }

    pGuiOn->registerUIEvents(this, &ParticlesApp::guiEvent);
    
}

void ParticlesApp::setupKinect() {
    
    cout << "Found " << Kinect::getNumDevices() << " Kinects" << endl;
    if (Kinect::getNumDevices() > 0) {
        //m_kinect = Kinect::create();
    } else {
        cout << "Didn't find any Kinects" << endl;
    }
    _params.setb("kinect", (m_kinect != NULL));

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

void ParticlesApp::guiEvent(ciUIEvent *event)
{
    string name = event->widget->getName();
    bool handled = true;
    if(name == ">>" || name == "<<")
    {
        bToggleMenu = true;
    } else if (name == "kinect") {
        _params.setb("kinect", !_params.getb("kinect"));
    } else if (name == "kdepth") {
        ciUIRangeSlider *rslider = (ciUIRangeSlider *) event->widget;
        _params.setf("kdepthThresholdLo", rslider->getScaledValueLow());
        _params.setf("kdepthThresholdHi", rslider->getScaledValueHigh());
    } else if (name == "bounce") {
        _params.setb("bounce", !_params.getb("bounce"));
    } else if (name == "size") {
        ciUISlider *slider = (ciUISlider *) event->widget;
        _params.setf("size", slider->getScaledValue());
    } else if (name == "lifespan") {
        ciUISlider *slider = (ciUISlider *) event->widget;
        _params.setf("lifespan", slider->getScaledValue());
    } else if (name == "symmetry") {
        ciUISlider *slider = (ciUISlider *) event->widget;
        _params.seti("symmetry", slider->getScaledValue());
    } else {
        handled = false;
    }
    
    event->setHandled(handled);
    
    
}

void ParticlesApp::mouseDown( MouseEvent event )
{
    if (event.isHandled()) {
        return;
    }
    
    if (event.isLeftDown()) {
        last_mouse_loc = event.getPos();
        
        ColorAf birthColor(Rand::randFloat(), Rand::randFloat(), Rand::randFloat());
        ColorAf deathColor = ColorAf(1 - birthColor.r, 1 - birthColor.g, 1 - birthColor.b);// (Rand::randFloat(), Rand::randFloat(), Rand::randFloat());
        
        particle_controller.setColors(birthColor, deathColor);

        addParticleAt(last_mouse_loc, Vec2f(0, 0));
    }
}

void ParticlesApp::mouseDrag(MouseEvent event)
{
    if (event.isHandled()) {
        return;
    }

    Vec2f mouse_vec = event.getPos() - last_mouse_loc;
    //    if (event.isLeftDown()) {
    int num_particles = Rand::randInt(10);
    for (int i = 0; i < num_particles; i++) {
        addParticleAt(event.getPos(), mouse_vec * .25);
    }
    //    }
    last_mouse_loc = event.getPos();
}

void ParticlesApp::addParticleAt(Vec2f position, Vec2f vector)
{
    Vec2f size = getWindowBounds().getSize();
    Vec2f center = size / 2.0;
    
    Vec2f cPosition = position - center;
    
    float pAngle = atan2(cPosition.y, cPosition.x);
    float dist = sqrt(cPosition.x * cPosition.x + cPosition.y * cPosition.y);

    float vAngle = atan2(vector.y, vector.x);
    int symmetry = _params.geti("symmetry");
    float slice = M_PI * 2 / symmetry;

    for (int i = 0; i < symmetry; i++)
    {
        Vec2f newPos = Vec2f(cos(pAngle), sin(pAngle)) * dist + center;
        Vec2f newVec = Vec2f(cos(vAngle), sin(vAngle)) * vector.length();

        pAngle += slice;
        vAngle += slice;
        
        particle_controller.addParticleAt(newPos, newVec);
    }
    
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
        case 'p': {
//            cout << "addParticles" << endl;
            particle_controller.addParticles(Rand::randInt(1000));
        } break;

        case 'P': {
//            cout << "removeParticles" << endl;
            particle_controller.removeParticles(Rand::randInt(100));
        } break;
        
        case 's': {
            int symmetry = _params.geti("symmetry");
            symmetry %= 8;
            symmetry++;
            _params.seti("symmetry", symmetry);
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
            bToggleMenu = true;
        } break;
    }
}


CINDER_APP_NATIVE( ParticlesApp, RendererGl )
