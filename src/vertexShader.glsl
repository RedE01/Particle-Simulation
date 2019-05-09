#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aOffset;

uniform mat4 projMat;
uniform mat4 viewMat;
uniform vec2 screenSize;

void main() {
    vec2 pos = vec2(aPos.x * screenSize.y / screenSize.x, aPos.y);

    gl_Position = projMat * viewMat *  vec4(aPos * 0.025f + aOffset, 0.0f, 1.0f);
}