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

#include "ciUI.h"

#include "CinderFreenect.h"
#include "cinder/Camera.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Vbo.h"


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
    
	virtual void mouseDown(MouseEvent event);
    //    virtual void mouseMove(MouseEvent event);
    virtual void mouseDrag(MouseEvent event);
    virtual void keyDown( KeyEvent event );
    
    void addParticleAt(Vec2f position, Vec2f vector);
    
    Params& params() {
        return _params;
    }
    
private:
    Params _params;
    
    fs::path filePath;
    gl::Texture myImage;
    
    bool m_capture;
    fs::path m_capturePath;
    
    ParticleController particle_controller;
    
    bool bToggleMenu;
    ciUICanvas *pGuiOff;
    ciUICanvas *pGuiOn;
    
    Vec2f last_mouse_loc;

    // KINECT
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
    bool useKinect() { return _params.getb("kinect"); }

};



#endif
