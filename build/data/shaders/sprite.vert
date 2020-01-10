#version 430 core

layout(location = 0) in vec2 iPos;
layout(location = 1) in vec2 iTexCoords;

// Boolean indicating whether this sprite should be movable with the camera
layout(location = 1) uniform int allow_camera_drag;
layout(location = 2) uniform mat4 projection;
layout(location = 3) uniform mat4 model;

layout(std140, binding = 0) uniform Camera {
    vec2 camera_position; 
};

out vec2 TexCoords;

void main() {   
    TexCoords = iTexCoords;

    gl_Position = projection * model * vec4(iPos, 0, 1) - vec4(camera_position, 0, 0) * allow_camera_drag;
}