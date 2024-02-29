#version 410

// defined in c/c++ program by the following sentence :
//                         m_shader.setUniform4f( "fragColor", glm::vec4(0,1,1,1) );
uniform  vec4 fragColor;
out vec4 outColor;

void main()
{
    // set the color as the color given from vertex shader program:
    outColor = fragColor;
}
