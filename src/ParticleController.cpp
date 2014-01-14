//
//  ParticleController.cpp
//  ParticlesApp
//
//  Created by Jeremy Biddle on 9/21/13.
//
//

#include "ParticleController.h"
#include "Particle.h"

#include "cinder/app/App.h"
#include "cinder/Rand.h"

#include <algorithm>    // std::max

using namespace ci;
using namespace ci::app;
using namespace std;

ParticleController::ParticleController() :
    m_isRecording(false),
    m_useGlobalParams(false)
{
    createBuffers();
}

void ParticleController::createBuffers()
{
    // The VBO containing the 4 vertices of the particles.
    // Thanks to instancing, they will be shared by all particles.
    static const GLfloat g_vertex_buffer_data[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        -0.5f, 0.5f, 0.0f,
        0.5f, 0.5f, 0.0f,
    };

    glGenVertexArraysAPPLE(1, &m_vaoID);
    glBindVertexArrayAPPLE(m_vaoID);

    glGenBuffers(1, &billboard_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
    
    // The VBO containing the positions and sizes of the particles
    glGenBuffers(1, &particles_position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    // Initialize with empty (NULL) buffer : it will be updated later, each frame.
    glBufferData(GL_ARRAY_BUFFER, kMaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
    
    // The VBO containing the colors of the particles
    glGenBuffers(1, &particles_color_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
    // Initialize with empty (NULL) buffer : it will be updated later, each frame.
    glBufferData(GL_ARRAY_BUFFER, kMaxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);
}

void ParticleController::updateBuffers()
{
    // Update the buffers that OpenGL uses for rendering.
    // There are much more sophisticated means to stream data from the CPU to the GPU,
    // but this is outside the scope of this tutorial.
    // http://www.opengl.org/wiki/Buffer_Object_Streaming
    
    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    glBufferData(GL_ARRAY_BUFFER, kMaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
    glBufferSubData(GL_ARRAY_BUFFER, 0, numParticles() * sizeof(GLfloat) * 4, &m_gpuPositionsArray[0]);
    
    glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
    glBufferData(GL_ARRAY_BUFFER, kMaxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
    glBufferSubData(GL_ARRAY_BUFFER, 0, numParticles() * sizeof(GLubyte) * 4, &m_gpuColorsArray[0]);
}

void ParticleController::fillBuffers()
{
    int i = 0;
    for(list<Particle *>::iterator p = m_particles.begin(); p != m_particles.end(); ++p ) {
        m_gpuPositionsArray[4 * i + 0] = (*p)->loc().x;
        m_gpuPositionsArray[4 * i + 1] = (*p)->loc().y;
        m_gpuPositionsArray[4 * i + 2] = 0;
        m_gpuPositionsArray[4 * i + 3] = (*p)->radius();
        
        m_gpuColorsArray[4 * i + 0] = (*p)->Color().r;
        m_gpuColorsArray[4 * i + 1] = (*p)->Color().g;
        m_gpuColorsArray[4 * i + 2] = (*p)->Color().b;
        m_gpuColorsArray[4 * i + 3] = (*p)->Color().a;
        
        i++;
    }
}

void ParticleController::drawBuffers()
{
    updateBuffers();
    
    fillBuffers();
    
    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
    glVertexAttribPointer(
                          0, // attribute. No particular reason for 0, but must match the layout in the shader.
                          3, // size
                          GL_FLOAT, // type
                          GL_FALSE, // normalized?
                          0, // stride
                          (void*)0 // array buffer offset
                          );
    
    // 2nd attribute buffer : positions of particles' centers
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    glVertexAttribPointer(
                          1, // attribute. No particular reason for 1, but must match the layout in the shader.
                          4, // size : x + y + z + size => 4
                          GL_FLOAT, // type
                          GL_FALSE, // normalized?
                          0, // stride
                          (void*)0 // array buffer offset
                          );
    
    // 3rd attribute buffer : particles' colors
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
    glVertexAttribPointer(
                          2, // attribute. No particular reason for 1, but must match the layout in the shader.
                          4, // size : r + g + b + a => 4
                          GL_UNSIGNED_BYTE, // type
                          GL_TRUE, // normalized? *** YES, this means that the unsigned char[4] will be accessible with a vec4 (floats) in the shader ***
                          0, // stride
                          (void*)0 // array buffer offset
                          );

    // These functions are specific to glDrawArrays*Instanced*.
    // The first parameter is the attribute buffer we're talking about.
    // The second parameter is the "rate at which generic vertex attributes advance when rendering multiple instances"
    // http://www.opengl.org/sdk/docs/man/xhtml/glVertexAttribDivisor.xml
    glVertexAttribDivisorARB(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
    glVertexAttribDivisorARB(1, 1); // positions : one per quad (its center) -> 1
    glVertexAttribDivisorARB(2, 1); // color : one per quad -> 1
    
    // Draw the particules !
    // This draws many times a small triangle_strip (which looks like a quad).
    // This is equivalent to :
    // for(i in ParticlesCount) : glDrawArrays(GL_TRIANGLE_STRIP, 0, 4),
    // but faster.
    glDrawArraysInstancedARB(GL_TRIANGLE_STRIP, 0, 4, numParticles());

}

bool ParticleController::isRecording()
{
    return m_isRecording;
}

void ParticleController::startRecording()
{
    if (!m_isRecording) {
        m_isRecording = true;
        m_recording.clear();
        m_recordingBeganAt = app::getElapsedSeconds();
        cout << "startRecording, m_recordingBeganAt: " << m_recordingBeganAt << endl;
    }
}

void ParticleController::stopRecording()
{
    if (m_isRecording) {
        m_isRecording = false;
        m_lastPlaybackAt = app::getElapsedSeconds();
        m_recordingLength = app::getElapsedSeconds() - m_recordingBeganAt;
        m_playbackHead = m_recording.begin();
        cout << "stopRecording, m_recordingLength: " << m_recordingLength << ", # particles: " << m_recording.size() << endl;
    }
}

void ParticleController::update()
{
    if (!m_isRecording && m_recording.size() > 0) {
        bool playingBack = true;
        while (playingBack) {
            if (m_lastPlaybackAt + m_playbackHead->time < app::getElapsedSeconds()) {
                if (m_playbackHead->type == eMouseDown) {
                    ColorAf birthColor(Rand::randFloat(), Rand::randFloat(), Rand::randFloat());
                    ColorAf deathColor = ColorAf(1 - birthColor.r, 1 - birthColor.g, 1 - birthColor.b);
                    getParams()->setColor("birthColor", birthColor);
                    getParams()->setColor("deathColor", deathColor);

                }
                emitParticle(m_playbackHead->position, m_playbackHead->direction, getParams());
                m_playbackHead++;
                
                if (m_playbackHead == m_recording.end()) {
                    m_playbackHead = m_recording.begin();
                    m_lastPlaybackAt += m_recordingLength;
                }
                
            } else {
                playingBack = false;
            }
        }
    }
    
    // remove dead particles
    list<Particle *>::iterator dead = remove_if(m_particles.begin(), m_particles.end(), bind1st(mem_fun(&ParticleController::updateRemove), this));
    m_particles.erase(dead, m_particles.end());

}

bool ParticleController::updateRemove(Particle *p)
{
    bool removing = !p->update(*this);
    if (removing) {
        delete p;
    }
    return removing;
}

void ParticleController::draw()
{
    //drawBuffers();
    
    for( list<Particle *>::iterator p = m_particles.begin(); p != m_particles.end(); ++p ){
        (*p)->draw();
    }
}

void ParticleController::emitParticle(const Vec2f &position, const Vec2f &direction, ParamsPtr ptrParams)
{
    Vec2f size = getWindowBounds().getSize();
    Vec2f center = size / 2.0;
    
    Vec2f cPosition = position - center;
    
    float pAngle = atan2(cPosition.y, cPosition.x);
    float dist = sqrt(cPosition.x * cPosition.x + cPosition.y * cPosition.y);
    
    float vAngle = atan2(direction.y, direction.x);
    int symmetry = getParams()->geti("symmetry");
    float slice = M_PI * 2 / symmetry;
    
    for (int i = 0; i < symmetry; i++)
    {
        Vec2f newPos = Vec2f(cos(pAngle), sin(pAngle)) * dist + center;
        Vec2f newDir = Vec2f(cos(vAngle), sin(vAngle)) * direction.length();
        
        pAngle += slice;
        vAngle += slice;

        int num_particles = getParams()->geti("density");
        for (int i = 0; i < num_particles; i++) {
            m_particles.push_back(new Particle(newPos, newDir, ptrParams));
        }
    }
}

void ParticleController::addParticleAt(const Vec2f &position, const Vec2f &direction, ControlType type)
{
    emitParticle(position, direction, getParams());

    if (m_isRecording) {
        float timeSinceRecordingBegan = app::getElapsedSeconds() - m_recordingBeganAt;
        m_recording.push_back(Recording(position, direction, type, timeSinceRecordingBegan));
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
