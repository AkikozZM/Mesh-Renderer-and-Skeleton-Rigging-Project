#ifndef HALFEDGE_H
#define HALFEDGE_H
#include <QListWidgetItem>
#include "Face.h"
#include "Vertex.h"

class Face;
class Vertex;
class HalfEdge : public QListWidgetItem {
public:
    HalfEdge* next;
    HalfEdge* sym;
    Face* face;
    Vertex* vertex;
    int id;
    HalfEdge();
    ~HalfEdge();
    void resetID();
    HalfEdge* getPrevious();
    void setSym(HalfEdge* sym);
    void setFace(Face* f);
    void setVertex(Vertex* v);

private:
    static int lastID;
};


#endif // HALFEDGE_H
