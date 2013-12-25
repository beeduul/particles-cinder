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

#include "cinder/Color.h"
#include "cinder/Perlin.h"

#include <iostream>
#include <list>

class ParticleController {
public:
    ParticleController();
    void update();
    void draw();

    void setColors(ColorAf birthColor, ColorAf deathColor);
    
    int numParticles() const { return m_particles.size(); }
    
    void addParticleAt(const Vec2f &loc, const Vec2f &vec);
    void addParticles( int amt );
    void removeParticles( int amt );

    void moveParticles(const Vec2f &offset);
    
    Perlin m_perlin;

    ColorAf averageColors();

private:
    ColorAf m_birthColor;
    ColorAf m_deathColor;
    std::list<Particle *> m_particles;

    bool updateRemove(Particle *p);
};

#endif /* defined(__ParticlesApp__ParticleController__) */
