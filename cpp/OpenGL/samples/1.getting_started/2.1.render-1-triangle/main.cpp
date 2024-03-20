// glad.h must be placed before glfw3.h
#include <glad/glad.h>

#include <GLFW/glfw3.h>


#include <iostream>
using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

bool prepareTriangleVertexData(unsigned int& rVAO, 
                         unsigned int& rVBO, 
                         unsigned int& rVertexShaderID, 
                         unsigned int& rFragmentShaderID, 
                         unsigned int& rShaderProgramID, 
                         GLsizei LOG_BUFFER_SIZE,
                         char* errorMsg);

void drawOneTriangle(unsigned int VAO_ID, unsigned int shaderProgramID);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main()
{
    // glfw: initialize and configure , set OpenGL version as 3.3
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation , "LearnOpenGL" is the window's title 
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }    


    //
    // prepare triangle  prerequisites
    //
    unsigned int vao_id = 0;
    unsigned int vbo_id = 0;
    unsigned int vertexShader_id = 0;
    unsigned int fragmentShader_id = 0;
    unsigned int shaderProgram_id = 0;
    const GLsizei LOG_SIZE = 512;
    char errorMsg[LOG_SIZE] = { 0 };
    auto bIsPrepareOK = prepareTriangleVertexData(vao_id, vbo_id, vertexShader_id, fragmentShader_id, shaderProgram_id, LOG_SIZE, errorMsg );


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);


        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

        //
        // Actually Render Here
        //
        if ( bIsPrepareOK ) {
            drawOneTriangle(vao_id, shaderProgram_id);
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    if ( bIsPrepareOK ) {
        // optional: de-allocate all resources once they've outlived their purpose:
        glDeleteVertexArrays(1, &vao_id);
        glDeleteBuffers(1, &vbo_id);
        glDeleteProgram( shaderProgram_id );
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if( glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS ) {
        glfwSetWindowShouldClose(window, true);
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    
    // std::cout << width << " * " << height << std::endl;
    glViewport(0, 0, width, height);
}

bool prepareTriangleVertexData(unsigned int& rVAO, 
                               unsigned int& rVBO, 
                               unsigned int& rVertexShaderID, 
                               unsigned int& rFragmentShaderID, 
                               unsigned int& rShaderProgramID, 
                               GLsizei LOG_BUFFER_SIZE,
                               char* errorMsg)
{
    // 0. program side , indicate the vertex's NDC Position
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // triangle's  left  vertex
         0.5f, -0.5f, 0.0f, // triangle's  right vertex
         0.0f,  0.5f, 0.0f  // triangle's  top   vertex
    };

    //
    // 1. create a new VBO and copy the real vertex data which is given from Step 0    into VBO memory
    // 
    
    /**************************************************************************************************
     ** Error Code snippet **                         | ** Correct Code snippet **
     -------------------------------------------------|----------------------------------------|
            glGenVertexArrays(1, &rVAO);              |  glGenVertexArrays(1, &rVAO);  
                                                      |
            // bind VAO -> VAO  ( bind itself )       |  // now the current machine state belongs to VBO
            glBindVertexArray(rVAO);                  |  glGenBuffers(1, &rVBO);
                                                      |
                                                      |  // bind previous VBO  --> VAO
            glGenBuffers(1, &rVBO);                   |  glBindVertexArray(rVAO);
                                                      |
     ------------------------------------------------------------------------------------------|
                                                      
     **************************************************************************************************/

    // generate 1 Vertex Array  Object   and assign its return value to the varible VAO 
    glGenVertexArrays(1, &rVAO);

    // generate 1 Vertex Buffer Object   and assign its return value to the varible VBO 
    glGenBuffers(1, &rVBO);

    /*
    
        顶点数组对象(Vertex Array Object, VAO)可以像顶点缓冲对象那样被绑定，任何随后的顶点属性调用都会储存在这个VAO中。
        这样的好处就是，当配置顶点属性指针时，你只需要将那些调用执行一次，之后再绘制物体的时候只需要绑定相应的VAO就行了。
        这使在不同顶点数据和属性配置之间切换变得非常简单，只需要绑定不同的VAO就行了。刚刚设置的所有状态都将存储在VAO中

    */
    // bind VBO -> VAO 
    glBindVertexArray(rVAO);

    glBindBuffer(GL_ARRAY_BUFFER, rVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //
    // 2. set vertex's property id and its pointer
    //
    glVertexAttribPointer(0,    // 0 is the property id of the vertex , here 0 refers to <Position> attribute
            3,                  // 1 vertex element is made by   3 pieces of data 
            GL_FLOAT,           // each piece of data's type is GL_FLOAT
            GL_FALSE, 
            3 * sizeof(float),  // 1 vertex occupy  ( 3 * sizeof(float) )    bytes
            (void*)0);
    glEnableVertexAttribArray(0); // Enable <Position> attribute 


    //
    // 3. Init shader program 
    //
    
    // 3.1 Vertex shader
    const char* vertexShader_Code = R"(
            #version 330 core
            layout (location = 0) in vec3 aPos;

            void main()
            {
                gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
            };
    )";


    rVertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(rVertexShaderID, 1, &vertexShader_Code, NULL);
    glCompileShader(rVertexShaderID);

    // after compile , get the info of compile status 
    int success = 0;
    glGetShaderiv(rVertexShaderID, GL_COMPILE_STATUS, &success);

    // if             success == 0 , Some Error occurs
    // otherwise      suceess != 0 , OK
    if ( !success ) {
        glGetShaderInfoLog(rVertexShaderID, LOG_BUFFER_SIZE, NULL, errorMsg );
        // errorMsg[LOG_BUFFER_SIZE-1] = '\0';
        /*

            // if                               missing a ';' at the end of statement
            gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0)   

            Error info be like the following message : 0(8) : error C0000: syntax error, unexpected '}', expecting ',' or ';' at token "}"
        */
        std::cout << "[Error] When compiling <Vertex> shader program : " << errorMsg << std::endl;
        return false;
    } else {
        std::cout << "[INFO] Compiling <Vertex> shader program  successful. :) " << std::endl;
    }


    // 3.2 fragment Shader
    const char* fragmentShader_Code = R"(
            #version 330 core
            out vec4 fragColor;

            void main()
            {
                fragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
            } 
    )";
    rFragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(rFragmentShaderID, 1, &fragmentShader_Code, NULL);
    glCompileShader(rFragmentShaderID);

    glGetShaderiv(rFragmentShaderID, GL_COMPILE_STATUS, &success);

    // if             success == 0 , Some Error occurs
    // otherwise      suceess != 0 , OK
    if ( !success ) {
        glGetShaderInfoLog(rFragmentShaderID, LOG_BUFFER_SIZE, NULL, errorMsg );
        // errorMsg[LOG_BUFFER_SIZE-1] = '\0';
        std::cout << "[Error] When compiling <Fragment> shader program : " << errorMsg << std::endl;
        return false;
    } else {
        std::cout << "[INFO] Compiling <Fragment> shader program successful.  :) " << std::endl;
    }

    // 3.3 bind vertex shader and fragment shader together and pass them into final shader program
    rShaderProgramID = glCreateProgram();
    glAttachShader(rShaderProgramID, rVertexShaderID);
    glAttachShader(rShaderProgramID, rFragmentShaderID);
    glLinkProgram(rShaderProgramID);

    glGetShaderiv(rShaderProgramID, GL_LINK_STATUS, &success);

    // if             success == 0 , Some Error occurs
    // otherwise      suceess != 0 , OK
    if ( !success ) {
        glGetProgramInfoLog(rShaderProgramID, LOG_BUFFER_SIZE, NULL, errorMsg );
        // errorMsg[LOG_BUFFER_SIZE-1] = '\0';
        std::cout << "[Error] When linking the entire Shader program : " << errorMsg << std::endl;
        return false;
    } else {
        std::cout << "[INFO] Linking entire Shader program successful.  :) " << std::endl;
    }
    
    // After the shader program finished linking , delete its dependency vertex shader and fragment shader 
    glDeleteShader(rVertexShaderID);
    glDeleteShader(rFragmentShaderID);

    return true;
}


void drawOneTriangle(unsigned int VAO_ID, unsigned int shaderProgramID)
{

    glUseProgram(shaderProgramID);

    glBindVertexArray(VAO_ID);
    // We draw 1 triangle with 3 vertex 
    glDrawArrays(GL_TRIANGLES, 0, 3);

}






