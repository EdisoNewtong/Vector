#version 410

layout (location = 0) in vec3 pos;
layout (location = 3) in vec2 uv;

uniform mat4 transform;

out vec2 fragUV;

void main()
{
  //gl_Position =  vec4(pos, 1.0) * transform;  // Error
	gl_Position =  transform * vec4(pos, 1.0);   // Correct

	fragUV = vec2(uv.x, 1.0-uv.y);
}


