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

struct Recording {
    Recording(Vec2f p, Vec2f d, int t);
    Vec2f position;
    Vec2f direction;
    int time;
};

class ParticleController {
public:
    ParticleController();
    void update();
    void draw();

    void setParams(ParamsPtr ptrParams) { m_params = ptrParams; }
    
    int numParticles() const { return m_particles.size(); }
    
    void emitParticle(const Vec2f &position, const Vec2f &direction, ParamsPtr ptrParams);
    
    void addParticleAt(const Vec2f &position, const Vec2f &direction);
//    void addParticles( int amt );
    void removeParticles( int amt );

    void moveParticles(const Vec2f &offset);
    
    void startRecording();
    void stopRecording();
    bool isRecording();
    
    // unused
    Perlin m_perlin;

private:
    bool m_isRecording;
    float m_recordingBeganAt;
    float m_recordingLength;
    float m_lastPlaybackAt;
    std::list<Recording> m_recording;
    std::list<Recording>::iterator m_playbackHead;
    
    ParamsPtr m_params;
    std::list<Particle *> m_particles;
    
    bool updateRemove(Particle *p);
};

typedef boost::shared_ptr<ParticleController> PtrParticleController;

#endif /* defined(__ParticlesApp__ParticleController__) */
