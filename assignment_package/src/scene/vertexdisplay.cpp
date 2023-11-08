#include "vertexdisplay.h"


VertexDisplay::VertexDisplay(OpenGLContext* context)
    : Drawable(context), representedVertex(nullptr)
{}
VertexDisplay::~VertexDisplay()
{}
GLenum VertexDisplay::drawMode()
{
    return GL_POINTS;
}
void VertexDisplay::create() {
    if(representedVertex == nullptr){
        return;
    }
    glm::vec4 pos = glm::vec4(representedVertex->position, 1.0f);
    std::vector<glm::vec4> positions = {pos};
    std::vector<GLuint> idx = {0};
    std::vector<glm::vec4> colors = {glm::vec4(1.f, 1.f, 1.f, 1.f)};

    count = 1;

    generateIdx();
    bindIdx();
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size() * sizeof(GLuint), idx.data(), GL_STATIC_DRAW);

    generatePos();
    bindPos();
    mp_context->glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec4), positions.data(), GL_STATIC_DRAW);

    generateCol();
    bindCol();
    mp_context->glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec4), colors.data(), GL_STATIC_DRAW);
}
void VertexDisplay::updateVertex(Vertex* v) {
    representedVertex = v;
}

bool VertexDisplay::isVertexSelected() const {
    return representedVertex != nullptr;
}
Vertex* VertexDisplay::getRepresentedVertex() {
    return representedVertex;
}
