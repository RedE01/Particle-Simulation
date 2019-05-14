#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aOffset;

uniform mat4 projMat;
uniform mat4 viewMat;
uniform vec2 screenSize;

void main() {
    gl_Position = projMat * (viewMat * vec4(aOffset, 1.0f) + vec4(vec3(aPos * 0.025f, 0.0f), 1.0f));
}