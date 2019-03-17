#include "Common.h"
#include "SkyboxRenderer.h"
#include "Camera.h"
#include "ShaderPrograms.h"
#include "Errors.h"

void SkyboxRenderer::Render(Camera& camera, GLuint cubeMap)
{
    ShaderProgram& program = ShaderPrograms::s_skybox;

    program.Use();

    glActiveTexture(GL_TEXTURE0);
    GET_AND_HANDLE_GL_ERROR();
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
    GET_AND_HANDLE_GL_ERROR();
    program.SendUniform("uSkyboxMap", 0);
    GET_AND_HANDLE_GL_ERROR();

    glm::mat4 vpMatrix = camera.ProjectionMatrix()
        * camera.EyeMatrix()
        * glm::translate(camera.GetPosition());
    program.SendUniform("vpMatrix", 1, false, vpMatrix);

    const GLsizei CUBE_VERTEX_COUNT = 12 * 3;

    glDrawArrays(GL_TRIANGLES, 0, CUBE_VERTEX_COUNT);
    GET_AND_HANDLE_GL_ERROR();

    glClear(GL_DEPTH_BUFFER_BIT);
    GET_AND_HANDLE_GL_ERROR();
}
