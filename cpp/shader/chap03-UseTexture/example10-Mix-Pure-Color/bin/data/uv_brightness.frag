#version 410

// inside C++ code
//     shader.setUniformTexture("parrotTex", image, 0);
// the name must be equal to the name inside shader program  
//              "parrotTex"  
//          ==>  parrotTex 
uniform sampler2D parrotTex;

// uniform float brightness;


in  vec2  fragUV;
out vec4  outColor;

void main()
{
    // - Color : Gray
    outColor = texture(parrotTex, fragUV) - vec4(0.5, 0.5, 0.5, 0.0);

    // + Color : Blue
    // outColor = texture(parrotTex, fragUV) + vec4(0.25, 0.25, 1.0, 0.0);

    // + Color : Red
    // outColor = texture(parrotTex, fragUV) + vec4(1.0, 0.0, 0.0, 1.0);

    // + Color : Gray
    // outColor = texture(parrotTex, fragUV) + vec4(0.5, 0.5, 0.5, 0.0);
}

