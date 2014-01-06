//
//  Params.cpp
//  Particles
//
//  Created by Jeremy Biddle on 10/26/13.
//
//

#include "ParticlesApp.h"
#include "Params.h"

Params::Params()
{
    // kinect params
    setb("kinect", false);
    setf("kdepthThresholdLo", .3);
    setf("kdepthThresholdHi", 1);

    // TODO deprecate
    setb("bounce", false);
    
    setf("size", 10.0);
    setf("lifespan", 10.0);
    seti("density", 10);
    setf("pulse_rate", 0.0);
    setf("pulse_amplitude", 1.0);
    seti("symmetry", 1);
    seti("draw_style", 0);
    setf("gravity", 0);
}

ParamsPtr &Params::get()
{
    return ((ParticlesApp *) ci::app::App::get())->params();
}
