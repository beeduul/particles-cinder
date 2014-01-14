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

#include "Params.h"

#include <cinder/gl/Vbo.h>
#include <cinder/gl/GlslProg.h>
#include <cinder/gl/Texture.h>

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
    
//    Particle();
    virtual ~Particle();
    
    virtual void kill();
    virtual bool update(const ParticleController &pc);

    virtual void update_behavior(const ParticleController &pc);
    virtual void draw();

    // accessors
    const float& radius() { return m_radius; }
    const float& radius(const float& new_radius) { return m_radius = new_radius; }
    //    const ColorAf& color() { return m_color; }
    //    const ColorAf& color(const ColorAf& new_color) { m_color.set(new_color); return m_color;}
    
    // methods
    Particle(const Vec2f &loc, ParamsPtr ptrParams);
    Particle(const Vec2f &loc, const Vec2f &vec, ParamsPtr ptrParams);
    
    ColorAf Color();
    
    static void static_initialize();
    static void bind() { if (m_circleShader) { m_circleShader.bind(); m_circleTexture.bind(0); } }
    static void unbind() { if (m_circleShader) { m_circleShader.unbind(); m_circleTexture.unbind(); } }

    
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
    
    // optimizing draw calls
    static int radiusToSegments(float radius);
    
    // precomputed gl arrays
    static std::vector<GLfloat *> m_vec_circle_verts;
    static GLfloat *getCircleVerts(float radius);
    void drawSolidCircle(const Vec2f &center, float radius);
    void drawStrokedCircle(const Vec2f &center, float radius);
    
    // VBO
    void drawSolidCircleVBO(const Vec2f &center, float radius);
    //    void drawStrokedCircleVBO(const Vec2f &center, float radius);
    
    static gl::VboMesh& getSolidCircleVboMesh(float radius);
    static std::vector<gl::VboMesh> m_vec_circle_vbo_meshes;
    
    // shaders
    static gl::GlslProg m_circleShader;
    static void initializeGlsl();
    void drawShaderCircle(const Vec2f &center, float radius);
    
    // texture
    static gl::Texture m_circleTexture;

};

#endif /* defined(__ParticlesApp__Particle__) */
