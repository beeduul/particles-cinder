//
//  Params.h
//  Particles
//
//  Created by Jeremy Biddle on 10/26/13.
//
//

#ifndef Particles_Params_h
#define Particles_Params_h

#include <map>
#include <boost/variant.hpp>

using namespace std;

class Params {
    
    std::map<std::string, boost::variant<int, float, std::string, bool>> p;
    
public:
    
    static Params& get();
    
    float getf(std::string key) {
        return boost::get<float>(p[key]);
    }
    
    void setf(std::string key, float value) {
        p[key] = value;
    }
    
    float geti(std::string key) {
        return boost::get<int>(p[key]);
    }
    
    void seti(std::string key, int value) {
        p[key] = value;
    }
    
    bool getb(std::string key) {
        return boost::get<bool>(p[key]);;
    }
    
    void setb(std::string key, bool value) {
        p[key] = value;
        
    }

    Params();
};



#endif
