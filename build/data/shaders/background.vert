#version 430 core

layout(location = 0) in vec2 iPos;
layout(location = 1) in vec2 iTexCoords;

out vec2 TexCoords;

void main() {
    TexCoords = iTexCoords;
    gl_Position = vec4(iPos, 0, 1);
}