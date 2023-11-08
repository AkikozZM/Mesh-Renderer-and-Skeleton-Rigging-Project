#include "HalfEdge.h"

int HalfEdge::lastID = 0;

HalfEdge::HalfEdge() :
    next(nullptr), sym(nullptr), face(nullptr), vertex(nullptr), id(lastID)
{
    setText(QString::number(id));
    lastID++;

}
HalfEdge::~HalfEdge()
{
    //lastID = 0;
}
void HalfEdge::resetID() {
    lastID = 0;
}
HalfEdge* HalfEdge::getPrevious() {
    HalfEdge* current = this;
    while(current->next != this) {
        current = current->next;
    }
    return current;
}
void HalfEdge::setSym(HalfEdge* sym) {
    this->sym = sym;
    sym->sym = this;
}
void HalfEdge::setFace(Face* f) {
    this->face = f;
    f->halfEdge = this;
}
void HalfEdge::setVertex(Vertex* v) {
    this->vertex = v;
    v->halfEdge = this;
}
