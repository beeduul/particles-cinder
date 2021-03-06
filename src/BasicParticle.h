//
//  BasicParticle.h
//  ParticlesApp
//
//  Created by Jeremy Biddle on 9/22/13.
//
//

#ifndef __ParticlesApp__BasicParticle__
#define __ParticlesApp__BasicParticle__

#include <iostream>

#include "Particle.h"
#include "Params.h"

class BasicParticle : public Particle
{
public:
    // accessors
    const float& radius() { return m_radius; }
    const float& radius(const float& new_radius) { return m_radius = new_radius; }
    //    const ColorAf& color() { return m_color; }
    //    const ColorAf& color(const ColorAf& new_color) { m_color.set(new_color); return m_color;}

    // methods
    BasicParticle(const Vec2f &loc, ParamsPtr ptrParams);
    BasicParticle(const Vec2f &loc, const Vec2f &vec, ParamsPtr ptrParams);
    virtual void update_behavior(const ParticleController &pc);
    virtual void draw();

    virtual ColorAf Color();

private:
    void initialize(const Vec2f &location, const Vec2f &direction, float velocity, ParamsPtr ptrParams);

    ColorAf Color(Stage stage, float t);
    
    // characteristics
    float m_radius;
    float m_decay;
    ColorAf m_birthColor;
    ColorAf m_deathColor;

    // move these into settings
    float m_amp;
    float m_freq;
    float m_start;
    int m_drawStyle;
    Vec2f m_gravity;
};

#endif /* defined(__ParticlesApp__BasicParticle__) */
