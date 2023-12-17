#version 410

in  vec4 fragColor;
out vec4 outColor;

void main()
{
    // set the color as the color given from vertex shader program:
    outColor = fragColor;
}
