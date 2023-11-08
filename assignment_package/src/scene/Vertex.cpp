#include "Vertex.h"

int Vertex::lastID = 0;
Vertex::Vertex(const glm::vec3& pos)
    : position(pos), halfEdge(nullptr), id(lastID)
{
    setText(QString::number(id));
    lastID++;
}

Vertex::Vertex() : id(lastID)
{
    setText(QString::number(id));
    lastID++;
}

Vertex::~Vertex()
{
    //lastID = 0;
}

void Vertex::resetID() {
    lastID = 0;
}
void Vertex::setHe(HalfEdge* he) {
    this->halfEdge = he;
}
void Vertex::addInfluence(Joint* j, float w) {
    if (influencedJoints.size() < 2) {
        influencedJoints.push_back(j);
        jointWeights.push_back(w);
    }
    else {
        int idxToReplace = (jointWeights[0] < jointWeights[1]) ? 0 : 1;
        //check if input w is larger than current exist
        if (jointWeights[idxToReplace] < w) {
            influencedJoints[idxToReplace] = j;
            jointWeights[idxToReplace] = w;
        }
    }
}
