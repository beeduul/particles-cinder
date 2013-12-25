//
//  BasicParticle.cpp
//  ParticlesApp
//
//  Created by Jeremy Biddle on 9/22/13.
//
//

#include "Params.h"

#include "BasicParticle.h"
#include "ParticleController.h"

#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "cinder/Easing.h"

BasicParticle::BasicParticle(const Vec2f &location, ColorAf birthColor, ColorAf deathColor)
{
    initialize(
        location,
        Vec2f(Rand::randFloat(-1, 1), Rand::randFloat(-1, 1)),
        Rand::randFloat(1),
        birthColor,
        deathColor
    );
}

BasicParticle::BasicParticle(const Vec2f &location, const Vec2f &direction, ColorAf birthColor, ColorAf deathColor) :
    m_radius(Rand::randFloat(10))
{
    initialize(
        location,
        direction,
        Rand::randFloat(1),
        birthColor,
        deathColor
    );
}

void BasicParticle::initialize(const Vec2f &location, const Vec2f &direction, float velocity, ColorAf birthColor, ColorAf deathColor)
{
    m_loc = location;
    m_vec = direction;
    m_vel = velocity;

    m_radius = Params::get().getf("size");

    m_birthColor = birthColor;
    m_deathColor = deathColor;
    
    stage_duration(birth, Params::get().getf("lifespan") * .1);
    stage_duration(alive, Params::get().getf("lifespan") * .6);
    stage_duration(dying, Params::get().getf("lifespan") * .3);

    m_decay = Rand::randFloat(0.95, 0.99);
    
    a = Rand::randFloat(0.1, Params::get().getf("pulse_amplitude"));
    r = Params::get().getf("pulse_rate"); // Rand::randFloat(0, 10);
    p = app::getElapsedSeconds(); // Rand::randFloat(0, M_PI);
    
    m_drawStyle = Params::get().getb("draw_style");
    
}

void BasicParticle::update_behavior(const ParticleController &pc)
{
//    float nX = m_loc.x * 0.005f;
//    float nY = m_loc.y * 0.005f;
//    float nZ = app::getElapsedSeconds() * 0.1f;
//    Vec3f v( nX, nY, nZ );
//    float noise = pc.m_perlin.fBm( v );
//    float angle = noise * 15.0f;
//
//    m_vec += Vec2f(cos(angle), sin(angle));
//    m_vel *= m_decay;
//
//    m_loc +=  m_vec * m_vel;
    
    m_loc += m_vec * m_vel;

//    bool bounce = Params::get().getb("bounce");
//    if (bounce) { // bounce walls
//        if (m_loc.x < 0 && m_vec.x < 0) {
//            m_vec.x = -m_vec.x;
//        } else if (m_loc.x > ci::app::getWindowWidth() && m_vec.x > 0) {
//            m_vec.x = -m_vec.x;
//        }
//        
//        if (m_loc.y < 0 && m_vec.y < 0) {
//            m_vec.y = -m_vec.y;
//        } else if (m_loc.y > ci::app::getWindowHeight() && m_vec.y > 0) {
//            m_vec.y = -m_vec.y;
//        }
//    }
    
    m_vel *= m_decay;

}

ColorAf BasicParticle::Color()
{
    float t = stage_time() / stage_duration();
    return Color(stage(), t);
}

ColorAf BasicParticle::Color(Stage stage, float t)
{
    switch(stage) {
        case birth: {
            ColorAf c = m_birthColor;
            c.a = t;
            return c;
        }
        case alive: {
            return m_birthColor * (1-t) + m_deathColor * t;
        }
        case dying: {
            return m_deathColor * (1 - t);
        }
        case dead:
            return Color::black();
    }

}

void BasicParticle::draw()
{
    if (stage() == dead) return;
    
    float t = stage_time() / stage_duration();

    gl::color(Color(stage(), t));

    float phase = app::getElapsedSeconds() - p;
    float swell = (1 + sin(phase * r) * a);
    
    float radius = m_radius * swell;
    switch(stage()) {
        case birth: {
            radius = radius * easeOutBack(t); // easeInElastic(t, 5, 3);
            break;
        }
        case alive: {
//            radius = radius;
            break;
        }
        case dying: {
            radius = radius * (1 - easeOutBack(t)); // easeInElastic(t, 5, 3);
            break;
        }
        case dead: ; // no-op
    }

    if (m_drawStyle == 0) {
        gl::drawSolidCircle( loc(), radius );
    } else {
        gl::drawSolidRect(Rectf(loc().x - radius, loc().y - radius, loc().x + radius, loc().y + radius));
    }    
}