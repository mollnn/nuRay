#version 330 core

in vec3 v_normal;
out vec4 FragColor;
uniform mat4 u_mvp;

void main()
{
    FragColor = vec4(max(0.0, 1.0 - 0.5 * normalize(v_normal).z) * vec3(1.0, 1.0, 1.0), 1.0);
}