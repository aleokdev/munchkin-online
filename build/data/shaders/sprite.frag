#version 430 core

in vec2 TexCoords;

layout(location = 3) uniform sampler2D sprite;

out vec4 FragColor;

void main() {
    FragColor = vec4(texture(sprite, TexCoords).rgb, 1);
}