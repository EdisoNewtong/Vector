#version 410

  layout ( location = 0 ) in vec3 pos;
  layout ( location = 1 ) in vec4 color; // the color is given by the graphic c/c++ program

  out vec4 fragColor;

void main()
{
    // the position passed by the main program has already used the normalized coordinate position.
    //     so no need the do the conversion.
    gl_Position = vec4(pos , 1.0);

	fragColor = color; // save the color given from the program and export it for the forthcoming fragment shader program
}
