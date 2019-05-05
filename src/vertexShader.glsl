#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aOffset;

uniform vec2 screenSize;

void main() {
    vec2 pos = vec2(aPos.x * screenSize.y / screenSize.x, aPos.y);

    gl_Position = vec4(pos * 0.01f + aOffset, 1.0f, 1.0f);
}