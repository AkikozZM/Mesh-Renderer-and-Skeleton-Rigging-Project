#ifndef SKELETON_H
#define SKELETON_H
#include "drawable.h"
#include <la.h>

#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include "Joint.h"

class Skeleton : public Drawable
{
public:
    Skeleton(OpenGLContext* mp_context);
    std::vector<std::unique_ptr<Joint>> skeleton;
private:

};

#endif // SKELETON_H
