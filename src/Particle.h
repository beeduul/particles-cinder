//
//  Particle.h
//  ParticlesApp
//
//  Created by Jeremy Biddle on 9/21/13.
//
//

#ifndef __ParticlesApp__Particle__
#define __ParticlesApp__Particle__

#include "cinder/Cinder.h"

using namespace ci;

class ParticleController;

class Particle
{
public:
    enum Stage { birth, alive, dying, dead };

    const Stage stage() { return m_stage; }
    const Stage stage(const Stage new_stage) { return m_stage = new_stage; }
    
    float stage_time() { return app::getElapsedSeconds() - m_stage_started_at; }
    float stage_duration() { return m_stage_durations[m_stage]; }
    float stage_duration(Stage stage, float stage_duration) { return m_stage_durations[stage] = stage_duration; }
    
    const Vec2f& loc() { return m_loc; }
    const Vec2f& loc(const Vec2f& new_loc) { return m_loc = new_loc; }

    const Vec2f& vec() { return m_vec; }
    const Vec2f& vec(const Vec2f& new_vec) { return m_vec = new_vec; }

    const float& stage_started_at() { return m_stage_started_at; }
    const float& stage_started_at(const float& new_stage_started_at) { return m_stage_started_at = new_stage_started_at; }
    
    const float& vel() { return m_vel; }
    const float& vel(const float& new_vel) { return m_vel = new_vel; }
    
    Particle();
    virtual ~Particle();
    
    virtual void kill();
    virtual bool update(const ParticleController &pc);

    virtual void update_behavior(const ParticleController &pc) = 0; // subclasses should override this
    virtual void draw() = 0;

    virtual ColorAf Color() { return ColorAf::black(); }
    
protected:
    // position
    Vec2f m_loc;
    Vec2f m_vec;
    float m_vel;

private:
    
    // lifecycle
    Stage m_stage;
    float m_stage_durations[4];
    float m_stage_started_at;

    // methods
    void update_lifecycle();
};

#endif /* defined(__ParticlesApp__Particle__) */
