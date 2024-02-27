# 《Shader开发实战》



Use **==OpenFrameworks==** Library with GLSL ( OpenGL Shander Language ) to study shader.

Click **the link** to visit [openframework official website](http://openframeworks.cc/download)  

The repo only contains the program part of the code without the **openframworks** library.  


# Depth Test  

  glEnable(GL_DEPTH_TEST);
  glDisable(GL_DEPTH_TEST);



# Alpha Blend Disable 
  glDisable(GL_BLEND);

## OF_BLENDMODE_ALPHA 
  glEnable(GL_BLEND);
  glBlendEquation(GL_FUNC_ADD);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

## OF_BLENDMODE_ADD 
  glEnable(GL_BLEND);
  glBlendEquation(GL_FUNC_ADD);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);


