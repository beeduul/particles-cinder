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

//Particle::Particle() :
//    m_stage(Stage::birth),
//    m_stage_started_at(app::getElapsedSeconds())
//{
//    m_stage_durations[birth] = 0.5;
//    m_stage_durations[alive] = 1;
//    m_stage_durations[dying] = 0.5;
//    m_stage_durations[dead] = 0;
//}

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


#include <cinder/app/AppNative.h>
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "cinder/Easing.h"

std::vector<GLfloat *> Particle::m_vec_circle_verts;
std::vector<gl::VboMesh> Particle::m_vec_circle_vbo_meshes;

gl::GlslProg Particle::m_circleShader;
gl::Texture Particle::m_circleTexture;

Particle::Particle(const Vec2f &location, ParamsPtr ptrParams)
{
    initialize(
               location,
               Vec2f(Rand::randFloat(-1, 1), Rand::randFloat(-1, 1)),
               Rand::randFloat(1),
               ptrParams
               );
}

Particle::Particle(const Vec2f &location, const Vec2f &direction, ParamsPtr ptrParams)
{
    initialize(
               location,
               direction,
               Rand::randFloat(1),
               ptrParams
               );
}

void Particle::initializeGlsl()
{
    m_circleShader = gl::GlslProg(
                                  app::loadResource( "circleVert.glsl" ),
                                  app::loadResource( "circleFrag.glsl" )
                                  );
}

void Particle::static_initialize()
{
    initializeGlsl();
    
    m_circleTexture = gl::Texture(loadImage(app::loadResource("circle256.png")));
    
}

void Particle::initialize(const Vec2f &location, const Vec2f &direction, float velocity, ParamsPtr ptrParams)
{
    if (!m_circleShader) {
        Particle::static_initialize();
    }

    m_stage = Stage::birth;
    m_stage_started_at = app::getElapsedSeconds();

    m_loc = location;
    m_vec = direction;
    m_vel = velocity;
    
    m_radius = ptrParams->getf("size");
    
    ColorAf birthColor = ptrParams->getColor("birthColor") + ColorAf(Rand::randFloat(-.1, .1), Rand::randFloat(-.1, .1), Rand::randFloat(-.1, .1));
    ColorAf deathColor = ptrParams->getColor("deathColor") + ColorAf(Rand::randFloat(-.1, .1), Rand::randFloat(-.1, .1), Rand::randFloat(-.1, .1));
    
    m_birthColor = birthColor;
    m_deathColor = deathColor;
    
    stage_duration(birth, ptrParams->getf("lifespan") * .2);
    stage_duration(alive, ptrParams->getf("lifespan") * .6);
    stage_duration(dying, ptrParams->getf("lifespan") * .2);
    
    m_decay = Rand::randFloat(0.95, 0.99);
    
    m_amp = Rand::randFloat(0.9, 1.1) * ptrParams->getf("pulse_amplitude");
    m_freq = ptrParams->getf("pulse_rate"); // Rand::randFloat(0, 10);
    m_start = app::getElapsedSeconds(); // Rand::randFloat(0, M_PI);
    
    m_drawStyle = ptrParams->geti("draw_style");
    
    if (ptrParams->geti("symmetry") == 1) {
        m_gravity = Vec2f(0, 1);
    } else {
        Vec2f center(ci::app::getWindowWidth()/2, ci::app::getWindowHeight()/2);
        m_gravity = Vec2f(center - m_loc).normalized();
    }
    float gravity = ptrParams->getf("gravity");
    m_gravity *= gravity;
    
}

void Particle::update_behavior(const ParticleController &pc)
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
    
    m_vec += m_gravity;
    
}

ColorAf Particle::Color()
{
    float t = stage_time() / stage_duration();
    return Color(stage(), t);
}

ColorAf Particle::Color(Stage stage, float t)
{
    switch(stage) {
        case birth: {
            ColorAf c = m_birthColor;
            //            c.a = t;
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

void Particle::draw()
{
    if (stage() == dead) return;
    
    float t = stage_time() / stage_duration();
    
    gl::color(Color(stage(), t));
    
    float t2 = m_freq * (app::getElapsedSeconds() - m_start);
    float swell = (1 + sin(t2)) * m_amp;
    
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
    
    switch (m_drawStyle) {
        case 0:
            //drawStrokedCircle( loc(), radius );
            drawShaderCircle(loc(), radius);
            break;
        case 1:
            drawSolidCircleVBO( loc(), radius );
            break;
        case 2:
            gl::drawSolidCircle(loc(), radius);
            break;
        case 3:
            drawSolidCircle(loc(), radius);
            break;
        case 4:
            gl::drawStrokedRect(Rectf(loc().x - radius, loc().y - radius, loc().x + radius, loc().y + radius));
            break;
        case 5:
        default:
            gl::drawSolidRect(Rectf(loc().x - radius, loc().y - radius, loc().x + radius, loc().y + radius));
            break;
    }
}

int Particle::radiusToSegments(float radius)
{
    return (int)math<double>::floor( radius * M_PI * 2 );
}

GLfloat * Particle::getCircleVerts(float radius)
{
    int numSegments = radiusToSegments(radius);
    if( numSegments < 2 ) numSegments = 2;
    if( numSegments > 250 ) numSegments = 250;
    
    if (numSegments >= m_vec_circle_verts.size()) {
        m_vec_circle_verts.resize(numSegments, 0);
    }
    
    int index = numSegments-1;
    if (m_vec_circle_verts[index] == 0)
    {
        GLfloat *verts = new float[(numSegments+2)*2];
        verts[0] = 0;
        verts[1] = 0;
        for( int s = 0; s <= numSegments; s++ ) {
            float t = s / (float)numSegments * 2.0f * 3.14159f;
            verts[(s+1)*2+0] = math<float>::cos( t );
            verts[(s+1)*2+1] = math<float>::sin( t );
        }
        m_vec_circle_verts[index] = verts;
    }
    
    return m_vec_circle_verts[index];
}


void Particle::drawStrokedCircle(const Vec2f &center, float radius)
{
    int numSegments = radiusToSegments(radius);
    
    GLfloat *verts = getCircleVerts(radius);
    
    glPushMatrix();
    
    glTranslatef(center.x, center.y, 0);
    glScalef(radius, radius, 1);
    
	glEnableClientState( GL_VERTEX_ARRAY );
	glVertexPointer( 2, GL_FLOAT, 0, verts + 2);
	glDrawArrays( GL_LINE_LOOP, 0, numSegments );
	glDisableClientState( GL_VERTEX_ARRAY );
    
    glPopMatrix();
}

void Particle::drawSolidCircle(const Vec2f &center, float radius)
{
    int numSegments = radiusToSegments(radius);
    
    GLfloat *verts = getCircleVerts(radius);
    
    glPushMatrix();
    
    glTranslatef(center.x, center.y, 0);
    glScalef(radius, radius, 1);
    
	glEnableClientState( GL_VERTEX_ARRAY );
	glVertexPointer( 2, GL_FLOAT, 0, verts );
	glDrawArrays( GL_TRIANGLE_FAN, 0, numSegments + 2 );
	glDisableClientState( GL_VERTEX_ARRAY );
    
    glPopMatrix();
}

gl::VboMesh& Particle::getSolidCircleVboMesh(float radius)
{
    int numSegments = radiusToSegments(radius);
    if( numSegments < 2 ) numSegments = 2;
    if( numSegments > 250 ) numSegments = 250;
    
    if (numSegments >= m_vec_circle_vbo_meshes.size()) {
        m_vec_circle_vbo_meshes.resize(numSegments, gl::VboMesh());
    }
    
    int index = numSegments-1;
    if (m_vec_circle_vbo_meshes[index] == 0)
    {
        int numVertices = numSegments + 2;
        int numIndices = numSegments + 2;
        gl::VboMesh::Layout layout;
        layout.setStaticIndices();
        layout.setStaticPositions();
        
        gl::VboMesh mesh(numVertices, numIndices, layout, GL_TRIANGLE_FAN);
        
        std::vector<uint32_t> indices(numIndices);
        for (uint32_t i = 0; i < numIndices; i++) {
            indices[i] = i;
        }
        mesh.bufferIndices(indices);
        
        std::vector<Vec3f> vertices(numVertices);
        
        vertices[0] = Vec3f(0, 0, 0);
        for( int s = 0; s <= numSegments; s++ ) {
            float t = s / (float)numSegments * 2.0f * 3.14159f;
            vertices[s+1] = Vec3f(math<float>::cos( t ), math<float>::sin( t ), 0);
        }
        mesh.bufferPositions(vertices);
        
        m_vec_circle_vbo_meshes[index] = mesh;
    }
    
    return m_vec_circle_vbo_meshes[index];
    
}


void Particle::drawSolidCircleVBO(const Vec2f &center, float radius)
{
    gl::VboMesh mesh = getSolidCircleVboMesh(radius);
    glPushMatrix();
    glTranslatef(center.x, center.y, 0);
    glScalef(radius, radius, 1);
    
    gl::draw(mesh);
    
    glPopMatrix();
}

void Particle::drawShaderCircle(const Vec2f &center, float radius)
{
    bind();
    m_circleShader.uniform("color", Color());
    m_circleShader.uniform("texture", 0);
    
    glPushMatrix();
    glTranslatef(center.x, center.y, 0);
    Rectf r(-radius, -radius, radius, radius);
    gl::drawSolidRect(r);
    glPopMatrix();
    unbind();
}
