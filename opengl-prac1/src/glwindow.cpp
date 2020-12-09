#include <iostream>
#include <stdio.h>

#include "SDL.h"
#include <GL/glew.h>

#include "glwindow.h"
#include "geometry.h"

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp>

using namespace std;

GeometryData geometry = GeometryData();
GeometryData::Object parent;
GeometryData::Object child;

const char* glGetErrorString(GLenum error)
{
    switch(error)
    {
    case GL_NO_ERROR:
        return "GL_NO_ERROR";
    case GL_INVALID_ENUM:
        return "GL_INVALID_ENUM";
    case GL_INVALID_VALUE:
        return "GL_INVALID_VALUE";
    case GL_INVALID_OPERATION:
        return "GL_INVALID_OPERATION";
    case GL_INVALID_FRAMEBUFFER_OPERATION:
        return "GL_INVALID_FRAMEBUFFER_OPERATION";
    case GL_OUT_OF_MEMORY:
        return "GL_OUT_OF_MEMORY";
    default:
        return "UNRECOGNIZED";
    }
}

void glPrintError(const char* label="Unlabelled Error Checkpoint", bool alwaysPrint=false)
{
    GLenum error = glGetError();
    if(alwaysPrint || (error != GL_NO_ERROR))
    {
        printf("%s: OpenGL error flag is %s\n", label, glGetErrorString(error));
    }
}

GLuint loadShader(const char* shaderFilename, GLenum shaderType)
{
    FILE* shaderFile = fopen(shaderFilename, "r");
    if(!shaderFile)
    {
        return 0;
    }

    fseek(shaderFile, 0, SEEK_END);
    long shaderSize = ftell(shaderFile);
    fseek(shaderFile, 0, SEEK_SET);

    char* shaderText = new char[shaderSize+1];
    size_t readCount = fread(shaderText, 1, shaderSize, shaderFile);
    shaderText[readCount] = '\0';
    fclose(shaderFile);

    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, (const char**)&shaderText, NULL);
    glCompileShader(shader);

    delete[] shaderText;

    return shader;
}

GLuint loadShaderProgram(const char* vertShaderFilename,
                       const char* fragShaderFilename)
{
    GLuint vertShader = loadShader(vertShaderFilename, GL_VERTEX_SHADER);
    GLuint fragShader = loadShader(fragShaderFilename, GL_FRAGMENT_SHADER);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);
    glLinkProgram(program);
    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    GLint linkStatus;
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
    if(linkStatus != GL_TRUE)
    {
        GLsizei logLength = 0;
        GLchar message[1024];
        glGetProgramInfoLog(program, 1024, &logLength, message);
        cout << "Shader load error: " << message << endl;
        return 0;
    }

    return program;
}

OpenGLWindow::OpenGLWindow()
{  

    parent.position = glm::vec3(0.0f, 0.0f, 0.0f);
    parent.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    parent.scale = glm::vec3(1.3f, 1.3f, 1.3f);

    child.position = glm::vec3(0.2f, 0.2f, 0.0f);
    child.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    child.scale = glm::vec3(0.6f, 0.6f, 0.6f);

    colorIndex = 0;
}

//void OpenGLWindow::initOBJ()
//{
//    std::vector<float> temp;
//    temp = GeometryData::loadFromOBJFile("objects/bunny.obj");
//}

void OpenGLWindow::initGL()
{
    // We need to first specify what type of OpenGL context we need before we can create the window
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    sdlWin = SDL_CreateWindow("OpenGL Prac 1",
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              640, 480, SDL_WINDOW_OPENGL);
    if(!sdlWin)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Error", "Unable to create window", 0);
    }
    SDL_GLContext glc = SDL_GL_CreateContext(sdlWin);
    SDL_GL_MakeCurrent(sdlWin, glc);
    SDL_GL_SetSwapInterval(1);

    glewExperimental = true;
    GLenum glewInitResult = glewInit();
    glGetError(); // Consume the error erroneously set by glewInit()
    if(glewInitResult != GLEW_OK)
    {
        const GLubyte* errorString = glewGetErrorString(glewInitResult);
        cout << "Unable to initialize glew: " << errorString;
    }

    int glMajorVersion;
    int glMinorVersion;
    glGetIntegerv(GL_MAJOR_VERSION, &glMajorVersion);
    glGetIntegerv(GL_MINOR_VERSION, &glMinorVersion);
    cout << "Loaded OpenGL " << glMajorVersion << "." << glMinorVersion << " with:" << endl;
    cout << "\tVendor: " << glGetString(GL_VENDOR) << endl;
    cout << "\tRenderer: " << glGetString(GL_RENDERER) << endl;
    cout << "\tVersion: " << glGetString(GL_VERSION) << endl;
    cout << "\tGLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glClearColor(0,0,0,1);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Note that this path is relative to your working directory
    // when running the program (IE if you run from within build
    // then you need to place these files in build as well)
    shader = loadShaderProgram("simple.vert", "simple.frag");
    glUseProgram(shader);
//PROJECTION MATRIX
    glm::mat4 projectionMat = glm::perspective(glm::radians(90.0f), 5.0f/4.0f, 0.3f, 8.0f);
    int projectionMatrixLoc = glGetUniformLocation(shader, "projectionMatrix");
    glUniformMatrix4fv(projectionMatrixLoc, 1, false, &projectionMat[0][0]);
//SETTING UP THE CAMERA

    glm::mat4 viewingMat = glm::lookAt(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    int camLoc = glGetUniformLocation(shader, "viewingMatrix");
    glUniformMatrix4fv(camLoc, 1, false, &viewingMat[0][0]);

   // int colorLoc = glGetUniformLocation(shader, "objectColor");
   // glUniform3f(colorLoc, 1.0f, 1.0f, 1.0f);

    // Load the model that we want to use and buffer the vertex attributes
    //GeometryData geometry = loadOBJFile("tri.obj");
    
    geometry.loadFromOBJFile("sample-bunny.obj");

    int vertexLoc = glGetAttribLocation(shader, "position");

    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    cout<<geometry.vertexCount()<<endl;//
    glBufferData(GL_ARRAY_BUFFER, geometry.vertexCount()*3*sizeof(float), geometry.vertexData(), GL_STATIC_DRAW);
	
    glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, false, 0, 0);
    glEnableVertexAttribArray(vertexLoc);//glEnableVertexAttribArray(0);

    //glDrawArrays(GL_TRIANGLES, 0, geometry.vertexCount())//
    glPrintError("Setup complete", true);
}

void OpenGLWindow::render()
{

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    float colors[6] = { 1.0f, 1.0f, 1.0f,
                               0.4f, 0.3f, 0.5f,
                        };

    glm::mat4 modelling = glm::translate(glm::mat4(1.0f), parent.position); 
    modelling = glm::rotate(modelling, parent.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    modelling = glm::rotate(modelling, parent.rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    modelling = glm::rotate(modelling, parent.rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    
    modelling = glm::scale(modelling, parent.scale);
    int modelMatrixLoc = glGetUniformLocation(shader, "modelMatrix");
    glUniformMatrix4fv(modelMatrixLoc, 1, false, &modelling[0][0]);

    int colorLoc = glGetUniformLocation(shader, "objectColor");
    glUniform3fv(colorLoc, 1, &colors[3*colorIndex]);

 

    glDrawArrays(GL_TRIANGLES, 0, geometry.vertexCount());

    // Swap the front and back buffers on the window, effectively putting what we just "drew"
    // onto the screen (whereas previously it only existed in memory)
    
    glm::mat4 childMatrix = glm::translate(glm::mat4(1.0f), child.position);
    childMatrix = glm::rotate(childMatrix, child.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    childMatrix = glm::rotate(childMatrix, child.rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    childMatrix = glm::rotate(childMatrix, child.rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

    childMatrix = glm::scale(childMatrix, child.scale);

    childMatrix = modelling * childMatrix;
    glUniformMatrix4fv(modelMatrixLoc, 1, false, &childMatrix[0][0]);
    glUniform3fv(colorLoc, 1, &colors[3]);
    glDrawArrays(GL_TRIANGLES, 0, geometry.vertexCount());

    SDL_GL_SwapWindow(sdlWin);
}

// The program will exit if this function returns false
char transformation = 'a';
bool mouse = false;
char axis = 'x';

bool OpenGLWindow::handleEvent(SDL_Event e)
{
    // A list of keycode constants is available here: https://wiki.libsdl.org/SDL_Keycode
    // Note that SDL provides both Scancodes (which correspond to physical positions on the keyboard)
    // and Keycodes (which correspond to symbols on the keyboard, and might differ across layouts)
    if(mouse ==true){
   	 if(e.type == SDL_MOUSEMOTION){
		if ( transformation == 't'){
		 	if(axis =='x'){
				if(e.motion.xrel <=0){parent.position[0] -= 0.1f;}
				if(e.motion.xrel >0){parent.position[0] += 0.1f;}
		     	}
		 	if(axis =='y'){
				if(e.motion.yrel <=0){parent.position[1] += 0.1f;}
				if(e.motion.yrel >0){parent.position[1] -= 0.1f;}
		     	}
		 	if(axis =='z'){
				if(e.motion.yrel <=0){parent.position[2] -= 0.1f;}
				if(e.motion.yrel >0){parent.position[2] += 0.1f;}
		     	}
		}
		if( transformation == 'r'){
		 	if(axis =='x'){
				if(e.motion.xrel <=0){parent.rotation[0] -= glm::radians(10.0f);}
				if(e.motion.xrel >0){parent.rotation[0] += glm::radians(10.0f);}
		     	}
		 	if(axis =='y'){
				if(e.motion.yrel <=0){parent.rotation[1] += glm::radians(10.0f);}
				if(e.motion.yrel >0){parent.rotation[1] -= glm::radians(10.0f);}
		     	}
		 	if(axis =='z'){
				if(e.motion.yrel <=0){parent.rotation[2] += glm::radians(10.0f);}
				if(e.motion.yrel >0){parent.rotation[2] -= glm::radians(10.0f);}
		     	}
		}
		if( transformation == 's'){
			if(e.motion.yrel <0){
				parent.scale[0] -= 1.0f;
	        		parent.scale[1] -= 1.0f;
				parent.scale[2] -= 1.0f;
			}
			if(e.motion.yrel >0){
				parent.scale[0] += 0.8f;
	        		parent.scale[1] += 0.8f;
				parent.scale[2] += 0.8f;
			}
		}
	 }
    }	

    if(e.type == SDL_MOUSEBUTTONDOWN)//SDL_MOUSEMOTION)
    { 
	mouse = true;
    }
    if(e.type == SDL_MOUSEBUTTONUP)//SDL_MOUSEMOTION)
    { 
	mouse = false;
    }
    if(e.type == SDL_KEYDOWN)
    {
	

        if(e.key.keysym.sym == SDLK_ESCAPE)
        {
            return false;
        }

	else if(e.key.keysym.sym == SDLK_x){axis = 'x';}
	else if(e.key.keysym.sym == SDLK_y){axis = 'y';}
	else if(e.key.keysym.sym == SDLK_z){axis = 'z';}


	else if(e.key.keysym.sym == SDLK_t)
	{
	    transformation = 't';
	}
	else if(e.key.keysym.sym == SDLK_r)
	{
	    transformation = 'r';
	}
	else if(e.key.keysym.sym == SDLK_s)
	{
	    transformation = 's';
	}

    }
    return true;
}

void OpenGLWindow::cleanup()
{
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteVertexArrays(1, &vao);
    SDL_DestroyWindow(sdlWin);
}
