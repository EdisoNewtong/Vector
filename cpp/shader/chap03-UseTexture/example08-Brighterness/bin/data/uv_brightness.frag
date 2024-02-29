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
    vec4 tex = texture(parrotTex, fragUV);
    tex.r *= 2.0f;    // the same is :   tex.x *= 2.0f;
    tex.g *= 2.0f;    // the same is :   tex.y *= 2.0f;
    tex.b *= 2.0f;    // the same is :   tex.z *= 2.0f;
    tex.a *= 2.0f;    // the same is :   tex.w *= 2.0f;

    outColor = tex;



    // Or 
    // outColor =  texture(parrotTex, fragUV) * 2.0f;
}

