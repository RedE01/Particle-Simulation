#version 330 core

layout (location = 0) in vec2 aPos;

uniform vec2 offsets[100];
uniform vec2 screenSize;

void main() {
    vec2 offset = offsets[gl_InstanceID];
    gl_Position = vec4(aPos * 0.025f + offset, 1.0f, 1.0f);
}