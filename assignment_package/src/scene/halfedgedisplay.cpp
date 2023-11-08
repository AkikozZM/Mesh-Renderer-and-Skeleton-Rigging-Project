#include "halfedgedisplay.h"

HalfEdgeDisplay::HalfEdgeDisplay(OpenGLContext* context)
    : Drawable(context), representedHalfEdge(nullptr)
{}
HalfEdgeDisplay::~HalfEdgeDisplay()
{}

GLenum HalfEdgeDisplay::drawMode()
{
    return GL_LINES;
}

void HalfEdgeDisplay::create() {
    if(representedHalfEdge == nullptr) {
        return;
    }
    HalfEdge* curr = representedHalfEdge->next;
    Vertex* start;
    while (curr && curr != representedHalfEdge)
    {
        start = curr->vertex;
        curr = curr->next;
    }
    // Starting vertex (red end)
    glm::vec4 start2 = glm::vec4(start->position.x, start->position.y, start->position.z, 1.0f);

    // Ending vertex (yellow end)
    glm::vec4 end = glm::vec4(representedHalfEdge->vertex->position.x, representedHalfEdge->vertex->position.y, representedHalfEdge->vertex->position.z, 1.0f);

    std::vector<glm::vec4> positions;
    positions.push_back(start2);
    positions.push_back(end);
    std::vector<GLuint> idx = {0, 1};

    count = 2;

    generateIdx();
    bindIdx();
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size() * sizeof(GLuint), idx.data(), GL_STATIC_DRAW);

    generatePos();
    bindPos();
    mp_context->glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec4), positions.data(), GL_STATIC_DRAW);

    //red and yellow
    std::vector<glm::vec4> colors = {glm::vec4(1, 0, 0, 1), glm::vec4(1, 1, 0, 1)};
    generateCol();
    bindCol();
    mp_context->glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec4), colors.data(), GL_STATIC_DRAW);
}
void HalfEdgeDisplay::updateHalfEdge(HalfEdge* h) {
    representedHalfEdge = h;
}
bool HalfEdgeDisplay::isHalfEdgeSelected() const {
    return representedHalfEdge != nullptr;
}
