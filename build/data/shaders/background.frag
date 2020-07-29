#version 430 core

in vec2 TexCoords;

layout(location = 0) uniform sampler2D background;
layout(location = 4) uniform vec4 color;

out vec4 FragColor;

void main() {
    FragColor = texture(background, TexCoords).rgba * color;
}