#version 430 core

in vec2 TexCoords;

layout(location = 0) uniform sampler2D background;

out vec4 FragColor;

void main() {
    FragColor = vec4(texture(background, TexCoords).rgb, 1);
}