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
    setb("kinect", false);
    setf("kdepthThresholdLo", .3);
    setf("kdepthThresholdHi", 1);
    setb("bounce", false);
    setf("size", 10.0);
    setf("lifespan", 10.0);
    setf("pulse_rate", 0.0);
    setf("pulse_amplitude", 1.0);
    seti("symmetry", 1);
    setb("draw_style", 0);
}

Params &Params::get()
{
    return ((ParticlesApp *) ci::app::App::get())->params();
}
