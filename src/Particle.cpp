#include "Particle.h"
#include <iostream>

Particle::Particle() : pos(glm::vec2(0.0f, 0.0f)) {
	dir = glm::vec2(std::rand() / float(RAND_MAX) * 0.5f - 0.25f, std::rand() / float(RAND_MAX) * 0.5f - 0.25f);
}

Particle::Particle(glm::vec2 pos) : pos(pos), dir(glm::vec2(0.0f)) {
}

Particle::~Particle() {
    
}

void Particle::applyForce(glm::vec2 pos, float force) {
    glm::vec2 dir = pos - this->pos;
    float distance = dir.x*dir.x + dir.y*dir.y;
    glm::vec2 dirNorm = dir / float(sqrt(distance));
    float F = (force*force) / distance;
    this->dir += dirNorm * force;

    this->pos += this->dir * force;
}