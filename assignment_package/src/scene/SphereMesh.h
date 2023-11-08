#ifndef SPHEREMESH_H
#define SPHEREMESH_H
#include "drawable.h"
#include <la.h>

#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

class SphereMesh : public Drawable
{
public:
    SphereMesh(OpenGLContext* mp_context);
    virtual void create() override;
    GLenum drawMode() override;
    glm::mat4 getTransformationMatrix();
    void setTransformationMatrix(glm::mat4 jointTransform);
private:
    std::vector<glm::vec4> vertices;
    std::vector<glm::vec4> colors;
    std::vector<GLuint> indices;
    glm::mat4 transformationMatrix;
};

#endif // SPHEREMESH_H
