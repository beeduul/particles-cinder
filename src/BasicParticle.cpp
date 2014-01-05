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

std::vector<GLfloat *> BasicParticle::m_vec_circle_verts;
std::vector<gl::VboMesh> BasicParticle::m_vec_circle_vbo_meshes;

BasicParticle::BasicParticle(const Vec2f &location, ParamsPtr ptrParams)
{
    initialize(
        location,
        Vec2f(Rand::randFloat(-1, 1), Rand::randFloat(-1, 1)),
        Rand::randFloat(1),
        ptrParams
    );
}

BasicParticle::BasicParticle(const Vec2f &location, const Vec2f &direction, ParamsPtr ptrParams)
{
    initialize(
        location,
        direction,
        Rand::randFloat(1),
        ptrParams
    );
}

void BasicParticle::initialize(const Vec2f &location, const Vec2f &direction, float velocity, ParamsPtr ptrParams)
{
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

    m_vec += m_gravity;

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

void BasicParticle::draw()
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
            drawStrokedCircle( loc(), radius );
            break;
        case 1:
            gl::drawSolidCircle(loc(), radius);
            break;
        case 2:
            drawSolidCircle(loc(), radius);
            break;
        case 3:
            drawSolidCircleVBO( loc(), radius );
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

int BasicParticle::radiusToSegments(float radius)
{
    return (int)math<double>::floor( radius * M_PI * 2 );
}

GLfloat * BasicParticle::getCircleVerts(float radius)
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


void BasicParticle::drawStrokedCircle(const Vec2f &center, float radius)
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

void BasicParticle::drawSolidCircle(const Vec2f &center, float radius)
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

gl::VboMesh& BasicParticle::getSolidCircleVboMesh(float radius)
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


void BasicParticle::drawSolidCircleVBO(const Vec2f &center, float radius)
{
    gl::VboMesh mesh = getSolidCircleVboMesh(radius);
    glPushMatrix();
    glTranslatef(center.x, center.y, 0);
    glScalef(radius, radius, 1);
    
    gl::draw(mesh);
    
    glPopMatrix();
}
