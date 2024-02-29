#version 410

  layout ( location = 0 ) in vec3 pos;

void main()
{
    // the position passed by the main program has already used the normalized coordinate position.
    //     so no need the do the conversion.
    gl_Position = vec4(pos , 1.0);
}
