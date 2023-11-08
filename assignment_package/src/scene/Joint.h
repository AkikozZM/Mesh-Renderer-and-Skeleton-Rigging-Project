#ifndef JOINT_H
#define JOINT_H
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <QTreeWidgetItem>
#include "SphereMesh.h"
#include "scene/Vertex.h"



class Joint : public QTreeWidgetItem {
public:
    Joint();
    Joint(const std::string& name, Joint* parent, const glm::vec3& position, const glm::quat& rotation);
    ~Joint();

    const std::string& getName() const;
    Joint* getParent() const;
    const std::vector<std::unique_ptr<Joint>>& getChildren() const;
    const glm::vec3& getPosition() const;
    const glm::quat& getRotation() const;
    const glm::mat4& getBindMatrix() const;
    glm::mat4 getLocalTransformation() const;
    glm::mat4 getOverallTransformation() const;

    void setBindMatrix(const glm::mat4& bindMatrix);
    void setName(const std::string& jointName);
    void setParent(Joint* parentJoint);
    void setPosition(const glm::vec3& pos);
    void setRotation(const glm::quat& rot);

    void addChild(std::unique_ptr<Joint> child);

    void setSphereMesh(std::unique_ptr<SphereMesh>& m);
    SphereMesh* getSphereMesh();
    void setBindPoseMatrix(const glm::mat4& transform);
    glm::mat4 getBindPoseMatrix();
    std::vector<Vertex*> influencedVertices;

private:
    std::string name;
    Joint* parent;
    std::vector<std::unique_ptr<Joint>> children;
    glm::vec3 position;
    glm::quat rotation;
    glm::mat4 bindMatrix;
    glm::mat4 bindPoseMatrix;
    std::unique_ptr<SphereMesh> sphereMesh;
};

#endif // JOINT_H
