//
//  Particle.cpp
//  ParticlesApp
//
//  Created by Jeremy Biddle on 9/21/13.
//
//

#include "cinder/app/App.h"
#include "Particle.h"

using namespace ci;
using namespace std;

Particle::Particle() :
    m_stage(Stage::birth),
    m_stage_started_at(app::getElapsedSeconds())
{
    m_stage_durations[birth] = 0.5;
    m_stage_durations[alive] = 1;
    m_stage_durations[dying] = 0.5;
    m_stage_durations[dead] = 0;
}

Particle::~Particle() {
}

void Particle::kill()
{
    m_stage = Stage::dying;
    m_stage_started_at = app::getElapsedSeconds();
}

void Particle::update_lifecycle()
{
    if (m_stage != dead && stage_time() > m_stage_durations[m_stage]) {
        m_stage_started_at += m_stage_durations[m_stage];
        m_stage = (Stage) (m_stage + 1);
    }
}

bool Particle::update(const ParticleController &pc)
{
    update_lifecycle();
    update_behavior(pc);
    return m_stage != dead;
}
