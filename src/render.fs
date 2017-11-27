in vec3 position;

void main() {
    vec4 color = vec4(0.8,0.2,0.2,1.0);
    gl_FragColor = vec4(position, 1.0);
}
