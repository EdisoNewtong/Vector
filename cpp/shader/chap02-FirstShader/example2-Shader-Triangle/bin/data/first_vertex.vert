#version 410

in vec3 position;

void main()
{
    float x =    (position.x / 1024.0) * 2.0 - 1.0;
    float y = -( (position.y /  768.0) * 2.0 - 1.0 );
    // vertex shader need convert from hard-code screen coordinate position to shader's normalized coordinate position.
    gl_Position = vec4( x,y, 0.0,  1.0);
}

