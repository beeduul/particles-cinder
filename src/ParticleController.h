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
    ParticleController();
    void update();
    void draw();

    void setParams(ParamsPtr ptrParams) { m_params = ptrParams; }
    
    int numParticles() const { return m_particles.size(); }
    
    void addParticleAt(const Vec2f &loc, const Vec2f &vec);
    void addParticles( int amt );
    void removeParticles( int amt );

    void moveParticles(const Vec2f &offset);
    
    // unused
    Perlin m_perlin;

private:
    ParamsPtr m_params;
    ColorAf m_birthColor;
    ColorAf m_deathColor;
    std::list<Particle *> m_particles;

    bool updateRemove(Particle *p);
};

typedef boost::shared_ptr<ParticleController> PtrParticleController;

#endif /* defined(__ParticlesApp__ParticleController__) */
