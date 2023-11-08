#include "JointLine.h"

JointLine::JointLine(OpenGLContext* context) : Drawable(context)
{}
JointLine::~JointLine()
{  }
GLenum JointLine::drawMode() {
    return GL_LINES;
}
void JointLine::create() {
    std::vector<glm::vec4> positions;
    positions.push_back(start);
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
void JointLine::setPositions(const glm::vec3 &start, const glm::vec3 &end) {
    this->start = glm::vec4(start, 1.f);
    this->end = glm::vec4(end, 1.f);
}
