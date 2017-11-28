#version 330 core

uniform mat4 uModelView;
uniform mat4 uProjection;
in vec4 vPosition;
out vec3 position;

void main() {
    gl_Position = uProjection*uModelView*vPosition;
    position = vPosition.xyz;
}
