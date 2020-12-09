#ifndef GL_WINDOW_H
#define GL_WINDOW_H

#include <GL/glew.h>

#include "geometry.h"
//#include "entity.h"

#include <glm/vec3.hpp>

class OpenGLWindow
{
public:
    OpenGLWindow();

    void initGL();
    void initOBJ();
    void render();
    bool handleEvent(SDL_Event e);
    void cleanup();

private:
    SDL_Window* sdlWin;

    GLuint vao;
    GLuint shader;
    GLuint vertexBuffer;

    int colorIndex;

};

#endif
