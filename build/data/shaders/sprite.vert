#version 430 core

layout(location = 0) in vec2 iPos;
layout(location = 1) in vec2 iTexCoords;

layout(location = 0) uniform vec2 position;
// Not implemented yet
layout(location = 1) uniform vec2 scale;
layout(location = 2) uniform vec2 rotation;

// Boolean indicating whether this sprite should be movable with the camera
layout(location = 4) uniform int allow_camera_drag;

layout(std140, binding = 0) uniform Camera {
    vec2 camera_position;
};

out vec2 TexCoords;

void main() {   

    TexCoords = iTexCoords;
    
    gl_Position = vec4(iPos + position - camera_position * allow_camera_drag, 0, 1);
}