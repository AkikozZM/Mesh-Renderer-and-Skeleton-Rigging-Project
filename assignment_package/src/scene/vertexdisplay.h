#ifndef VERTEXDISPLAY_H
#define VERTEXDISPLAY_H
#include "Drawable.h"
#include "Vertex.h"
#include <la.h>
#include <glm/glm.hpp>

class VertexDisplay : public Drawable
{
protected:
    Vertex* representedVertex;

public:
    VertexDisplay(OpenGLContext* context);
    ~VertexDisplay();
    void create() override;
    void updateVertex(Vertex* v);
    bool isVertexSelected() const;
    GLenum drawMode() override;
    Vertex* getRepresentedVertex();
};

#endif // VERTEXDISPLAY_H
