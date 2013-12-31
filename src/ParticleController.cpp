//
//  ParticleController.cpp
//  ParticlesApp
//
//  Created by Jeremy Biddle on 9/21/13.
//
//

#include "ParticleController.h"
#include "BasicParticle.h"

#include "cinder/app/App.h"
#include "cinder/Rand.h"

#include <algorithm>    // std::max

using namespace ci;
using namespace ci::app;
using namespace std;

Recording::Recording(Vec2f p, Vec2f d, float t)
{
    position = p;
    direction = d;
    time = t;
}

ParticleController::ParticleController() :
    m_isRecording(false)
{
}

bool ParticleController::isRecording()
{
    return m_isRecording;
}

void ParticleController::startRecording()
{
    if (!m_isRecording) {
        m_isRecording = true;
        m_recording.clear();
        m_recordingBeganAt = app::getElapsedSeconds();
        cout << "startRecording, m_recordingBeganAt: " << m_recordingBeganAt << endl;
    }
}

void ParticleController::stopRecording()
{
    if (m_isRecording) {
        m_isRecording = false;
        m_lastPlaybackAt = app::getElapsedSeconds();
        m_recordingLength = app::getElapsedSeconds() - m_recordingBeganAt;
        m_playbackHead = m_recording.begin();
        cout << "stopRecording, m_recordingLength: " << m_recordingLength << ", # particles: " << m_recording.size() << endl;
    }
}

void ParticleController::update()
{
    if (!m_isRecording && m_recording.size() > 0) {
        bool playingBack = true;
        while (playingBack) {
            if (m_lastPlaybackAt + m_playbackHead->time < app::getElapsedSeconds()) {
                emitParticle(m_playbackHead->position, m_playbackHead->direction, m_params);
                m_playbackHead++;
                
                if (m_playbackHead == m_recording.end()) {
                    m_playbackHead = m_recording.begin();
                    m_lastPlaybackAt += m_recordingLength;
                }
                
            } else {
                playingBack = false;
            }
        }
    }
    
    // remove dead particles
    list<Particle *>::iterator dead = remove_if(m_particles.begin(), m_particles.end(), bind1st(mem_fun(&ParticleController::updateRemove), this));
    m_particles.erase(dead, m_particles.end());
}

bool ParticleController::updateRemove(Particle *p)
{
    bool removing = !p->update(*this);
    if (removing) {
        delete p;
    }
    return removing;
}

void ParticleController::draw()
{
    for( list<Particle *>::iterator p = m_particles.begin(); p != m_particles.end(); ++p ){
        (*p)->draw();
    }
}

void ParticleController::emitParticle(const Vec2f &position, const Vec2f &direction, ParamsPtr ptrParams)
{
    Vec2f size = getWindowBounds().getSize();
    Vec2f center = size / 2.0;
    
    Vec2f cPosition = position - center;
    
    float pAngle = atan2(cPosition.y, cPosition.x);
    float dist = sqrt(cPosition.x * cPosition.x + cPosition.y * cPosition.y);
    
    float vAngle = atan2(direction.y, direction.x);
    int symmetry = m_params->geti("symmetry");
    float slice = M_PI * 2 / symmetry;
    
    for (int i = 0; i < symmetry; i++)
    {
        Vec2f newPos = Vec2f(cos(pAngle), sin(pAngle)) * dist + center;
        Vec2f newDir = Vec2f(cos(vAngle), sin(vAngle)) * direction.length();
        
        pAngle += slice;
        vAngle += slice;

        int num_particles = m_params->geti("density");
        for (int i = 0; i < num_particles; i++) {
            m_particles.push_back(new BasicParticle(newPos, newDir, ptrParams));
        }
    }
}

void ParticleController::addParticleAt(const Vec2f &position, const Vec2f &direction)
{
    emitParticle(position, direction, m_params);

    if (m_isRecording) {
        float timeSinceRecordingBegan = app::getElapsedSeconds() - m_recordingBeganAt;
        m_recording.push_back(Recording(position, direction, timeSinceRecordingBegan));
    }
}

void ParticleController::removeParticles( int amt )
{
    list<Particle *>::iterator p = m_particles.begin();
    while (p != m_particles.end() && amt > 0) {
        if ((*p)->stage() != Particle::Stage::dying) {
            (*p)->kill();
            amt--;
        }
        p++;
    }
}

void ParticleController::moveParticles(const Vec2f &offset)
{
    for(list<Particle *>::iterator p = m_particles.begin(); p != m_particles.end(); p++)
    {
        (*p)->loc((*p)->loc() + offset);
    }
}
