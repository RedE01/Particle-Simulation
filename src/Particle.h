#pragma once
#include <glm/glm.hpp>

class Particle {
public:
    glm::vec2 pos, dir;
public:
    Particle();
    Particle(glm::vec2 pos);
    ~Particle();

    void applyForce(glm::vec2 pos, float force);
};