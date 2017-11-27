#version 330 core
in vec4 vPosition;

out vec3 position;

void main() {
    gl_Position = vPosition;
    position = vPosition.xyz;
}
