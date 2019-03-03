/*
    Lights.h

    PointLight, SpotLight, DirecitonalLight definition.
*/
#ifndef LIGHTS_H_
#define LIGHTS_H_

#include <glm/glm.hpp>

class PointLight
{
public:
    glm::vec3 position;
    glm::vec3 la;
    glm::vec3 ld;
    glm::vec3 ls;
};

#endif
