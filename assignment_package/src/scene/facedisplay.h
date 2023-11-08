#ifndef FACEDISPLAY_H
#define FACEDISPLAY_H
#include "Drawable.h"
#include "Face.h"
#include "Vertex.h"
#include "HalfEdge.h"

class FaceDisplay : public Drawable
{
private:
    Face* representedFace;

public:
    FaceDisplay(OpenGLContext* context);
    ~FaceDisplay();
    void create() override;
    void updateFace(Face* f);
    bool isFaceSelected() const;
    GLenum drawMode() override;
    Face* getRepresentedFace();
};

#endif // FACEDISPLAY_H
