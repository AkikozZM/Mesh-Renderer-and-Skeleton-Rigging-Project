#include "SphereMesh.h"
#include<la.h>
#include <iostream>

SphereMesh::SphereMesh(OpenGLContext *mp_context)
    : Drawable(mp_context)
{}
GLenum SphereMesh::drawMode() {
    return GL_LINES;
}
void SphereMesh::create() {
    float radius = 0.5f;
    int segments = 12;
    float angleIncrement = 2.0f * glm::pi<float>() / segments;
    for (int circle = 0; circle < 3; circle++) {
        for (int i = 0; i < segments; i++) {
            float angle = angleIncrement * i;
            float x = cos(angle) * radius;
            float y = sin(angle) * radius;
            float z = 0.f;
            // Green
            if (circle == 0) {
                z = y;
                y = 0.f;
                colors.push_back(glm::vec4(0, 1.f, 0, 1.f));
            }
            // Blue
            else if (circle == 1) {
                colors.push_back(glm::vec4(0, 0.f, 1.f, 1.f));
            }
            // Red
            else if (circle == 2) {
                z = y;
                y = x;
                x = 0.f;
                colors.push_back(glm::vec4(1, 0.f, 0, 1.f));
            }
            vertices.push_back(glm::vec4(x, y, z, 1.f));
        }
    }
    for (int circle = 0; circle < 3; circle++)
    {
        int baseIdx = circle * segments;
        for (int i = 0; i < segments; i++)
        {
            indices.push_back(baseIdx + i);
            indices.push_back(baseIdx + (i + 1) % segments);
        }
    }
    count = segments * 2 * 3;

    generateIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    generatePos();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    mp_context->glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec4), vertices.data(), GL_STATIC_DRAW);

    generateCol();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    mp_context->glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec4), colors.data(), GL_STATIC_DRAW);
}
glm::mat4 SphereMesh::getTransformationMatrix() {
    return this->transformationMatrix;
}
void SphereMesh::setTransformationMatrix(glm::mat4 jointTransform) {
    this->transformationMatrix = jointTransform;
}
