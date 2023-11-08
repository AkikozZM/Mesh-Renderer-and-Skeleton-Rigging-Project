#include "faceDisplay.h"
#include <vector>
#include <glm/glm.hpp>


FaceDisplay::FaceDisplay(OpenGLContext* context)
    : Drawable(context), representedFace(nullptr)
{}

FaceDisplay::~FaceDisplay()
{}

void FaceDisplay::updateFace(Face* f)
{
    representedFace = f;
}

GLenum FaceDisplay::drawMode()
{
    return GL_LINES;
}

void FaceDisplay::create()
{
    if (representedFace == nullptr) {
        return;
    }
    count = 0;

    std::vector<glm::vec4> positions;
    std::vector<glm::vec4> colors;
    std::vector<GLuint> indices;

    int index = 0;
    glm::vec3 oppColor = glm::vec3(1.0f) - representedFace->color;
    HalfEdge* startEdge = representedFace->halfEdge;
    HalfEdge* prevEdge = representedFace->halfEdge;
    HalfEdge* nextEdge = prevEdge->next;
    while (prevEdge != startEdge || index == 0) {

        indices.push_back(index++);
        indices.push_back(index++);
        Vertex* v1 = prevEdge->vertex;
        Vertex* v2 = nextEdge->vertex;

        glm::vec4 pos1 = glm::vec4(v1->position.x, v1->position.y, v1->position.z, 1.0f);
        glm::vec4 pos2 = glm::vec4(v2->position.x, v2->position.y, v2->position.z, 1.0f);

        positions.push_back(pos1);
        positions.push_back(pos2);

        colors.push_back(glm::vec4(oppColor, 1.0f));
        colors.push_back(glm::vec4(oppColor, 1.0f));

        prevEdge = prevEdge->next;
        nextEdge = nextEdge->next;
    }

    count = index;

    generateIdx();
    bindIdx();
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    generatePos();
    bindPos();
    mp_context->glBufferData(GL_ARRAY_BUFFER, count * sizeof(glm::vec4), positions.data(), GL_STATIC_DRAW);

    generateCol();
    bindCol();
    mp_context->glBufferData(GL_ARRAY_BUFFER, count * sizeof(glm::vec4), colors.data(), GL_STATIC_DRAW);
}
bool FaceDisplay::isFaceSelected() const {
    return representedFace != nullptr;
}
Face* FaceDisplay::getRepresentedFace() {
    return representedFace;
}
