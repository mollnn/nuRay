#version 330 core

in vec3 a_pos;
in vec3 a_normal;
uniform mat4 u_mvp;
out vec3 v_normal;

void main()
{
    vec4 pos = u_mvp * vec4(a_pos, 1.0);
    gl_Position = pos;
    vec4 normal = u_mvp * vec4(a_normal, 0.0);
    v_normal = normal.xyz;
}