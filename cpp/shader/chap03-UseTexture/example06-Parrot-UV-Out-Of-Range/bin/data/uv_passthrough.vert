#version 410

layout (location = 0) in vec3 pos;
layout (location = 3) in vec2 uv;

out vec2 fragUV;

void main()
{
    gl_Position = vec4(pos, 1.0);
    // pass the varible "fragUV" to the next render phase ( fragment shader )
    fragUV = vec2(uv.x, 1.0 -uv.y) + vec2(0.25, 0.0);
}
