#version 410

// inside C++ code
//     shader.setUniformTexture("parrotTex", image, 0);
// the name must be equal to the name inside shader program  
//              "parrotTex"  
//          ==>  parrotTex 
uniform sampler2D parrotTex;
uniform sampler2D checkerboardTex;


in  vec2  fragUV;
out vec4  outColor;

void main()
{
    vec4 parrot  = texture(parrotTex,       fragUV);
    vec4 checker = texture(checkerboardTex, fragUV);

    // 50% parrot + 50% checker
    // outColor = mix(parrot, checker, 0.5);


    outColor = mix(checker, parrot,   checker.r);
}

