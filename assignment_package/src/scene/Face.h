#ifndef FACE_H
#define FACE_H
#include <glm/glm.hpp>
#include <random>
#include <QListWidgetItem>
#include "HalfEdge.h"

class HalfEdge;

class Face : public QListWidgetItem {
public:
    glm::vec3 color;
    HalfEdge* halfEdge;
    int id;
    Face();
    ~Face();
    void resetID();
    void setHalfedge(HalfEdge* he);
    int getID();

private:
    static int lastID;
};


#endif // FACE_H
