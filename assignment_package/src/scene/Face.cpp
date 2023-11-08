#include "Face.h"

int Face::lastID = 0;
Face::Face() : halfEdge(nullptr), id(lastID)
{
    setText(QString::number(id));
    lastID++;

    std::random_device rd;
    std::default_random_engine rng(rd());
    std::uniform_real_distribution<float> dist(0.f, 1.f);
    //color = glm::vec3(dist(rng), dist(rng), dist(rng));
    color = glm::vec3(0, 0, 0);
}
Face::~Face()
{
    //lastID = 0;
}
void Face::resetID() {
    lastID = 0;
}
void Face::setHalfedge(HalfEdge* he) {
    this->halfEdge = he;
    he->face = this;
}
int Face::getID() {
    return lastID;
}
