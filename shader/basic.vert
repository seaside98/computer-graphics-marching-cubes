#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPosition;
out vec3 FragNormal;
flat out vec3 FragNormalFlat;

void main() {
    gl_PointSize = 4.0;
    // Fine as long as we don't scale
    FragNormal = vec3(model * vec4(normal, 1.0f));
    FragNormalFlat = FragNormal;

    FragPosition = vec3(model * vec4(position, 1.0f));
    gl_Position = projection * view * model * vec4(position, 1.0f);
}
