#version 430 core

layout(location = 0) in vec2 iPos;
layout(location = 1) in vec2 iTexCoords;

layout(location = 1) uniform float scroll;

layout(std140, binding = 0) uniform Camera {
    vec2 camera_position; // No padding
};

out vec2 TexCoords;

void main() {
    TexCoords = vec2(iTexCoords.x - scroll, iTexCoords.y - scroll);
    gl_Position = vec4(iPos - camera_position, 0, 1);
}