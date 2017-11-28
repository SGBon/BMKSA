#version 330 core
in vec3 position;
in vec3 normal;

void main() {
    vec4 white = vec4(1.0,1.0,1.0,1.0);
    vec4 material = vec4(0.5, 0.5, 0.5, 0.1);
    vec3 light = vec3(5,5,-10.0);
    vec3 light_direction = normalize(light - position);
    vec3 reflection = normalize(reflect(-light_direction, normal));
    vec3 normal_ized = normalize(normal);
    float diffuse = clamp(dot(normal_ized,  light_direction), 0.0, 1.0);
    float specular = pow(max(0.0, dot(normal_ized, reflection)), material.w);
    vec4 color = vec4(0.8,0.2,0.2,1.0);
    vec4 ambient_color = color;//vec4(0.7, 0.0, 0.7, 1.0);
    //gl_FragColor = min(material.x*ambient_color + material.y*diffuse*color + material.z*white*specular, vec4(1.0))
    //gl_FragColor.a = color.a
    //gl_FragColor = vec4(normal, 1.0);
    gl_FragColor = vec4(1.0,1.0,1.0,1.0);
}
