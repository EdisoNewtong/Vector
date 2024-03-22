// glad.h must be placed before glfw3.h
#include <glad/glad.h>

#include <GLFW/glfw3.h>


#include <iostream>
using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

bool prepareTriangleVertexData(unsigned int& rVAO, 
                               unsigned int& rVBO, 
                               unsigned int& rEBO, 
                               unsigned int& rVertexShaderID, 
                               unsigned int& rFragmentShaderID, 
                               unsigned int& rShaderProgramID, 
                               GLsizei LOG_BUFFER_SIZE,
                               char* errorMsg);

void drawOneTriangleByVertexIndices(unsigned int ebo_ID, unsigned int shaderProgramID);

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
    unsigned int ebo_id = 0;
    unsigned int vertexShader_id = 0;
    unsigned int fragmentShader_id = 0;
    unsigned int shaderProgram_id = 0;
    const GLsizei LOG_SIZE = 512;
    char errorMsg[LOG_SIZE] = { 0 };
    auto bIsPrepareOK = prepareTriangleVertexData(vao_id, vbo_id, ebo_id, vertexShader_id, fragmentShader_id, shaderProgram_id, LOG_SIZE, errorMsg );


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
            drawOneTriangleByVertexIndices(vao_id, shaderProgram_id);
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
        glDeleteBuffers(1, &ebo_id);
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
                               unsigned int& rEBO, 
                               unsigned int& rVertexShaderID, 
                               unsigned int& rFragmentShaderID, 
                               unsigned int& rShaderProgramID, 
                               GLsizei LOG_BUFFER_SIZE,
                               char* errorMsg)
{
    // 0. program side , indicate the vertex's NDC Position
    float vertices[] = {
        0.5f, 0.5f, 0.0f,   // 右上角
        0.5f, -0.5f, 0.0f,  // 右下角
        -0.5f, -0.5f, 0.0f, // 左下角
        -0.5f, 0.5f, 0.0f   // 左上角
    };

    unsigned int indices[] = {
        // 注意索引从0开始! 
        // 此例的索引(0,1,2,3)就是顶点数组vertices的下标，
        // 这样可以由下标代表顶点组合成矩形

        0, 1, 3, // 第一个三角形
        1, 2, 3  // 第二个三角形
    };

    //
    // 1. create a new VBO and copy the real vertex data which is given from Step 0    into VBO memory
    // 
    

    // generate 1 Vertex Array  Object   and assign its return value to the varible VAO 
    glGenVertexArrays(1, &rVAO);

    glGenBuffers(1, &rVBO);
    // generate 1 Vertex Buffer Object   and assign its return value to the varible VBO 
    glGenBuffers(1, &rEBO);

    /*
    
        顶点数组对象(Vertex Array Object, VAO)可以像顶点缓冲对象那样被绑定，任何随后的顶点属性调用都会储存在这个VAO中。
        这样的好处就是，当配置顶点属性指针时，你只需要将那些调用执行一次，之后再绘制物体的时候只需要绑定相应的VAO就行了。
        这使在不同顶点数据和属性配置之间切换变得非常简单，只需要绑定不同的VAO就行了。刚刚设置的所有状态都将存储在VAO中

    */
    // bind VBO -> VAO 
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(rVAO);

    // bind VBO and copy vertex positon info into array
    glBindBuffer(GL_ARRAY_BUFFER, rVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // bind EBO and copy vertex indices info into element array
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

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


void drawOneTriangleByVertexIndices(unsigned int VAO_ID, unsigned int shaderProgramID)
{
    glUseProgram(shaderProgramID);

    /*
    // We draw 1 triangle with 3 vertex 
    glDrawArrays(GL_TRIANGLES, 0, 3);
    */

    glBindVertexArray(VAO_ID);

    // set wire-frame mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // [Default] mode filled with color
    // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    //vertex index's data type is an **unsigned int**
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );
    // glBindVertexArray(0);

}






