//
//  ParticleController.h
//  ParticlesApp
//
//  Created by Jeremy Biddle on 9/21/13.
//
//

#pragma once

#ifndef __ParticlesApp__ParticleController__
#define __ParticlesApp__ParticleController__

#include "Particle.h"
#include "Params.h"

#include "cinder/Color.h"
#include "cinder/Perlin.h"

#include <iostream>
#include <list>

class ParticleController {
    
public:

    enum ControlType { eMouseDown, eMouseDrag, eMouseUp };

    ParticleController();
    void update();
    void draw();

    void setParams(ParamsPtr ptrParams) { m_params = ptrParams; }
    ParamsPtr getParams() { return m_useGlobalParams ? Params::get() : m_params; }
    
    int numParticles() const { return m_particles.size(); }
    
    void emitParticle(const Vec2f &position, const Vec2f &direction, ParamsPtr ptrParams);
    
    void addParticleAt(const Vec2f &position, const Vec2f &direction, ControlType type);
//    void addParticles( int amt );
    void removeParticles( int amt );

    void moveParticles(const Vec2f &offset);
    
    void startRecording();
    void stopRecording();
    bool isRecording();
    
    // unused
    Perlin m_perlin;

private:

    struct Recording {
        Recording(Vec2f p, Vec2f d, ControlType ty, float t) {
            position = p;
            direction = d;
            type = ty;
            time = t;
        }
        Vec2f position;
        Vec2f direction;
        ControlType type;
        float time;
    };
    
    bool m_isRecording;
    float m_recordingBeganAt;
    float m_recordingLength;
    float m_lastPlaybackAt;
    std::list<Recording> m_recording;
    std::list<Recording>::iterator m_playbackHead;
    
    bool m_useGlobalParams;
    ParamsPtr m_params;

    std::list<Particle *> m_particles;
    
    bool updateRemove(Particle *p);
};

typedef boost::shared_ptr<ParticleController> PtrParticleController;

#endif /* defined(__ParticlesApp__ParticleController__) */
