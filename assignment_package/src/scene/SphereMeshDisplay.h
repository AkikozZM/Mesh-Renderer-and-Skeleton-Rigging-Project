#ifndef SPHEREMESHDISPLAY_H
#define SPHEREMESHDISPLAY_H
#include "drawable.h"
#include <la.h>

#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

class SphereMeshDisplay : public Drawable
{
public:
    SphereMeshDisplay(OpenGLContext* mp_context);
    virtual void create() override;
    GLenum drawMode() override;
    glm::mat4 getTransformationMatrix();
    void setTransformationMatrix(glm::mat4 jointTransform);
    bool jointSelected;
private:
    std::vector<glm::vec4> vertices;
    std::vector<glm::vec4> colors;
    std::vector<GLuint> indices;
    glm::mat4 transformationMatrix;

};

#endif // SPHEREMESHDISPLAY_H
