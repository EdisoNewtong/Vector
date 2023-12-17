#version 410

in vec3 position;

void main()
{
    // the position passed by the main program has already used the normalized coordinate position.
    //     so no need the do the conversion.
    gl_Position = vec4(position, 1.0);
}

