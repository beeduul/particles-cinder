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

ParticleController::ParticleController()
{
    m_birthColor = ColorAf(.3, .5, .8);
    m_deathColor = ColorAf(.7, .3, 1);
}

bool ParticleController::updateRemove(Particle *p)
{
    bool removing = !p->update(*this);
    if (removing) {
        delete p;
    }
    return removing;
}

void ParticleController::update()
{
    list<Particle *>::iterator dead = remove_if(m_particles.begin(), m_particles.end(), bind1st(mem_fun(&ParticleController::updateRemove), this));
    m_particles.erase(dead, m_particles.end());
}

void ParticleController::draw()
{
    for( list<Particle *>::iterator p = m_particles.begin(); p != m_particles.end(); ++p ){
        (*p)->draw();
    }
}

void ParticleController::setColors(ColorAf birthColor, ColorAf deathColor)
{
    m_birthColor = birthColor;
    m_deathColor = deathColor;
}

void ParticleController::addParticleAt(const Vec2f &loc, const Vec2f &vec)
{
    ColorAf birthColor = m_birthColor + ColorAf(Rand::randFloat(-.1, .1), Rand::randFloat(-.1, .1), Rand::randFloat(-.1, .1));
    ColorAf deathColor = m_deathColor + ColorAf(Rand::randFloat(-.1, .1), Rand::randFloat(-.1, .1), Rand::randFloat(-.1, .1));
    
    m_particles.push_back(new BasicParticle(loc, vec, birthColor, deathColor));
}

void ParticleController::addParticles( int amt )
{
    for (int i = 0; i < amt; i++) {
        float x = Rand::randFloat( app::getWindowWidth() );
        float y = Rand::randFloat( app::getWindowHeight() );
        ColorAf birthColor = m_birthColor + ColorAf(Rand::randFloat(-.1, .1), Rand::randFloat(-.1, .1), Rand::randFloat(-.1, .1));
        ColorAf deathColor = m_deathColor + ColorAf(Rand::randFloat(-.1, .1), Rand::randFloat(-.1, .1), Rand::randFloat(-.1, .1));
        m_particles.push_back(new BasicParticle(Vec2f(x, y), birthColor, deathColor));
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

ColorAf ParticleController::averageColors()
{
    ColorAf average = ColorAf::gray(.5);

    if (m_particles.size() > 0) {
        for(list<Particle *>::iterator p = m_particles.begin(); p != m_particles.end(); p++)
        {
            average += (*p)->Color();
        }
        
        return average / m_particles.size();
    } else {
        return average;

    }
}

