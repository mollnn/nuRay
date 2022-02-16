#version 330 core

in vec3 v_normal;
out vec4 FragColor;
uniform mat4 u_mvp;

void main()
{
    FragColor = vec4(max(-normalize(v_normal).z, 0.0) * vec3(1.0, 1.0, 1.0), 1.0);
}