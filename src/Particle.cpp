#include "Particle.h"
#include <iostream>

Particle::Particle() : pos(glm::vec3(0.0f, 0.0f, 0.0f)) {
	dir = glm::vec3(std::rand() / float(RAND_MAX) * 0.5f - 0.25f, std::rand() / float(RAND_MAX) * 0.5f - 0.25f, std::rand() / float(RAND_MAX) * 0.5f - 0.25f);
}

Particle::Particle(glm::vec3 pos) : pos(pos), dir(glm::vec3(0.0f)) {
}

Particle::~Particle() {
    
}

void Particle::applyForce(glm::vec3 pos, float force) {
    glm::vec3 dir = pos - this->pos;
    float distance = dir.x*dir.x + dir.y*dir.y + dir.z*dir.z;
    glm::vec3 dirNorm = dir / float(pow(distance, 1.0f / 3.0f));
    float F = (force*force) / distance;
    this->dir += dirNorm * force;

    this->pos += this->dir * force;
}