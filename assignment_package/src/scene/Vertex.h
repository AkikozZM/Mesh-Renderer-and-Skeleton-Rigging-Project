#ifndef VERTEX_H
#define VERTEX_H
#include <glm/glm.hpp>
#include <QListWidgetItem>

class HalfEdge;
class Joint;

class Vertex : public QListWidgetItem {
public:
    glm::vec3 position;
    HalfEdge* halfEdge;
    int id;
    Vertex(const glm::vec3& pos);
    Vertex();
    ~Vertex();
    void resetID();
    void setHe(HalfEdge* he);
// at most 2 joints
    std::vector<Joint*> influencedJoints;
    std::vector<float> jointWeights;
    void addInfluence(Joint* j, float w);

private:
    static int lastID;
};

#endif // VERTEX_H
