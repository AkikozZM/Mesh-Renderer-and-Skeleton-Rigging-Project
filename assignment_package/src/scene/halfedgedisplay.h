#ifndef HALFEDGEDISPLAY_H
#define HALFEDGEDISPLAY_H
#include "Drawable.h"
#include "HalfEdge.h"
#include "Vertex.h"
#include <la.h>
#include <glm/glm.hpp>

class HalfEdgeDisplay : public Drawable
{
protected:
    HalfEdge* representedHalfEdge;

public:
    HalfEdgeDisplay(OpenGLContext* context);
    ~HalfEdgeDisplay();
    void create() override;
    void updateHalfEdge(HalfEdge* h);
    GLenum drawMode() override;
    bool isHalfEdgeSelected() const;
};

#endif // HALFEDGEDISPLAY_H
