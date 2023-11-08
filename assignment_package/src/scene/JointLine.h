#ifndef JOINTLINE_H
#define JOINTLINE_H
#include "Drawable.h"
#include <la.h>
#include <glm/glm.hpp>

class JointLine : public Drawable
{
public:
    JointLine(OpenGLContext* context);
    ~JointLine();
    void create() override;
    GLenum drawMode() override;
    glm::vec4 start;
    glm::vec4 end;
    void setPositions(const glm::vec3 &start, const glm::vec3 &end);
};

#endif // JOINTLINE_H
