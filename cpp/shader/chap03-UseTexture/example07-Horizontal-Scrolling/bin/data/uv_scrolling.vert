#version 410

layout (location = 0) in vec3 pos;
layout (location = 3) in vec2 uv;

uniform float time;

out vec2 fragUV;

void main()
{
    gl_Position = vec4(pos, 1.0);

    fragUV = vec2(uv.x, 1.0 - uv.y) + vec2(1.0, 0.0) * time;
}

