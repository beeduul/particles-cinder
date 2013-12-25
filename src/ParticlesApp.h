//
//  ParticlesApp.h
//  Particles
//
//  Created by Jeremy Biddle on 10/26/13.
//
//

#ifndef Particles_ParticlesApp_h
#define Particles_ParticlesApp_h

#include <iostream>
#include <cmath>

#include "cinder/app/AppNative.h"
#include "cinder/Rand.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"

#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"

#include "Params.h"
#include "ParticleController.h"

#include "ciUI/src/ciUI.h"

#ifdef USE_KINET
#include "CinderFreenect.h"
#include "cinder/Camera.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Vbo.h"
#endif

using namespace ci;
using namespace ci::app;
using namespace std;

class ParticlesApp : public AppNative {
    
public:
    
    void prepareSettings(Settings *settings);
    
	void setup();
    void update();
	void draw();
    void quit();
    
    void setupGui();
    void minifyGui();
    void expandGui();
    void guiEvent(ciUIEvent *event);
  
    virtual void mouseUp(MouseEvent event);
    virtual void mouseDown(MouseEvent event);
    //    virtual void mouseMove(MouseEvent event);
    virtual void mouseDrag(MouseEvent event);
    virtual void keyDown( KeyEvent event );
    
    void addParticleAt(Vec2f position, Vec2f vector);
    
    Params& params() {
        return m_params;
    }

protected:
    void toggleMenu();
    void setFullScreen(bool fullscreen);
    
private:
    Params m_params;
    
    bool m_capture;
    fs::path m_capturePath;
    
    ParticleController m_particleController;
  
  
    bool m_eventHandled;
  
    bool m_toggleMenu;
    ciUICanvas *m_pGUIOff;
    ciUICanvas *m_pGUIOn;
    
    ciUISlider *m_symmetrySlider;
    
    Vec2f m_lastMouseLoc;
    
    
#ifdef USE_KINECT
    bool            m_useKinect;
    KinectRef		m_kinect;
    gl::Texture		m_kinectColorTexture, m_kinectDepthTexture;
  
    // CAMERA
    CameraPersp		mCam;
    Quatf			mSceneRotation;
    Vec3f			mEye, mCenter, mUp;
    float			mCameraDistance;
    float			mKinectTilt;

    // VBO AND SHADER
    static const int VBO_X_RES  = 640;
    static const int VBO_Y_RES  = 480;
    gl::VboMesh		mVboMesh;
    gl::GlslProg	mShader;

    void setupKinect();
    void createVbo();
    bool useKinect() { return m_params.getb("kinect"); }
#endif

};



#endif
