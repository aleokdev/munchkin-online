#version 430 core

layout(location = 0) uniform sampler2D sprite;
layout(location = 4) uniform vec4 color;

out vec4 FragColor;

void main() {
    FragColor = color;
}