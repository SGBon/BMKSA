#version 330 core

uniform mat4 uModelView;
uniform mat4 uProjection;
in vec4 vPosition;
in vec3 vNormal;
out vec3 position;
out vec3 normal;

void main() {
    gl_Position = uProjection*uModelView*vPosition;
    position = vPosition.xyz;
    normal = vNormal;
}
