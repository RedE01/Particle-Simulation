#pragma once
#include <glm/glm.hpp>

class Particle {
public:
    glm::vec3 pos, dir;
public:
    Particle();
    Particle(glm::vec3 pos);
    ~Particle();

    void applyForce(glm::vec3 pos, float force);
};