#include "SystemComponent.h"
#include "GraphicsDemo.h"
#include "Errors.h"
#include <iostream>
#include <glad.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "System.h"
#include "Object.h"
#include "Camera.h"
#include "stb_image.h"

GLuint PrepareShaderProgram();
void LoadFbxSdkObject(const char* const filename, std::vector<std::shared_ptr<Object>>& objectStack);

struct SubMesh
{
    SubMesh() : IndexOffset(0), TriangleCount(0) {}

    int IndexOffset;
    int TriangleCount;
};

namespace
{
    
    GLint s_mvLocation;
    GLint s_projLocation;
    GLint s_eyeLocation;
    std::vector<std::shared_ptr<Object>> s_objectStack;

    GLuint s_program;

    Object object;
    Camera camera;
}

void GraphicsDemo::OnStart()
{
    s_program = PrepareShaderProgram();

    s_mvLocation = glGetUniformLocation(s_program, "mvMatrix");
    GET_AND_HANDLE_GL_ERROR();
    s_projLocation = glGetUniformLocation(s_program, "projMatrix");
    GET_AND_HANDLE_GL_ERROR();
    s_eyeLocation = glGetUniformLocation(s_program, "eyeMatrix");
    GET_AND_HANDLE_GL_ERROR();

    camera.LookAt(glm::vec3(50, 50, 50), glm::vec3(), glm::vec3(0, 1, 0));

    LoadFbxSdkObject("./Resources/56-fbx/fbx/Dragon 2.5_fbx.fbx", s_objectStack);
}

void GraphicsDemo::Update(float dt)
{
    const float M_PI = 3.14159f;
    const float currentTime = System::Instance()->CurrentTime();
    const float f = (float)currentTime * (float)M_PI * 0.1f;

    glm::mat4 mvMatrix = glm::identity<glm::mat4>();

    mvMatrix = glm::translate(mvMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
    mvMatrix = glm::translate(mvMatrix, glm::vec3(sinf(2.1f * f) * 0.5f, cosf(1.7f * f) *0.5f, sinf(1.3f*f)));
    mvMatrix = glm::rotate(mvMatrix, (float)currentTime * 45.0f / M_PI / 2, glm::vec3(0.0f, 1.0f, 0.0f));
    mvMatrix = glm::rotate(mvMatrix, (float)currentTime * 81.0f / M_PI / 2, glm::vec3(1.0f, 0.0f, 0.0f));

    object.SetTransformMatrix(mvMatrix);
}

void GraphicsDemo::Render()
{
    GLfloat color[] = { 0, 0.8f, 0.7f, 1.0f };

    glClearBufferfv(GL_COLOR, 0, color);

    // Render(object);
    for (auto& pObject : s_objectStack)
    {
        Render(*pObject);
    }
}

void GraphicsDemo::Free()
{
    object.Free();
}

void GraphicsDemo::OnWndProc(HWND hwnd, UINT uMsg, WPARAM wParama, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_SIZE:
        short width = LOWORD(lParam);
        short height = HIWORD(lParam);
        camera.SetFrustum(glm::pi<float>() * 0.25f, width / 0.5f, height / 0.5f,
            0.1f, 1000.0f);
    }
}

void GraphicsDemo::Render(Object & object)
{
    glUseProgram(s_program);
    
    glUniformMatrix4fv(s_mvLocation, 1, GL_FALSE, (float*)&object.GetTransformMatrix()[0][0]);
    glUniformMatrix4fv(s_projLocation, 1, GL_FALSE, (float*)&camera.ProjectionMatrix()[0][0]);
    glUniformMatrix4fv(s_eyeLocation, 1, GL_FALSE, (float*)&camera.EyeMatrix()[0][0]);

    object.Render();
}
