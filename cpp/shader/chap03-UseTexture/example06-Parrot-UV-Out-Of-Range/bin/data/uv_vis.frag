#version 410

// inside C++ code
//     shader.setUniformTexture("parrotTex", image, 0);
// the name must be equal to the name inside shader program  
//              "parrotTex"  
//          ==>  parrotTex 
uniform sampler2D parrotTex;

in  vec2  fragUV;
out vec4  outColor;

void main()
{
    outColor = texture(parrotTex, fragUV);
}

