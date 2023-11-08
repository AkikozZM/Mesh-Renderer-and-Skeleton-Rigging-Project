#include "Joint.h"
#include "glm/gtx/quaternion.hpp"
#include <glm/gtc/matrix_transform.hpp>


Joint::Joint() : name(""), parent(nullptr), position(glm::vec3(0.f)), rotation(), bindMatrix(glm::mat4(1.f)), sphereMesh(nullptr)
{
    //setText(0, QString::fromStdString(name));
}
Joint::Joint(const std::string& name,
             Joint* parent,
             const glm::vec3& position,
             const glm::quat& rotation)
    : name(name), parent(parent), position(position), rotation(rotation), sphereMesh(nullptr)
{
    //setText(0, QString::fromStdString(name));
}
Joint::~Joint()
{}
const std::string& Joint::getName() const {
    return name;
}

Joint* Joint::getParent() const {
    return parent;
}

const std::vector<std::unique_ptr<Joint>>& Joint::getChildren() const {
    return children;
}

const glm::vec3& Joint::getPosition() const {
    return position;
}

const glm::quat& Joint::getRotation() const {
    return rotation;
}

const glm::mat4& Joint::getBindMatrix() const {
    return bindMatrix;
}
glm::mat4 Joint::getLocalTransformation() const {
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.f), position);
    glm::mat4 rotationMatrix = glm::toMat4(rotation);
    return translationMatrix * rotationMatrix;
}
glm::mat4 Joint::getOverallTransformation() const {
    glm::mat4 transformation = getLocalTransformation();
    Joint* currentParent = parent;
    while (currentParent) {
        transformation = currentParent->getLocalTransformation() * transformation;
        currentParent = currentParent->getParent();
    }
    return transformation;
}
void Joint::setName(const std::string& name) {
    this->name = name;
}
void Joint::setParent(Joint* parentJoint) {
    this->parent = parentJoint;
}
void Joint::setPosition(const glm::vec3& pos) {
    this->position = pos;
}
void Joint::setRotation(const glm::quat& rot) {
    this->rotation = rot;
}
void Joint::setBindMatrix(const glm::mat4& bindMatrix) {
    this->bindMatrix = bindMatrix;
}


void Joint::addChild(std::unique_ptr<Joint> child) {
    child->setParent(this);
    children.push_back(std::move(child));
}
void Joint::setSphereMesh(std::unique_ptr<SphereMesh>& m) {
    this->sphereMesh = std::move(m);
}
SphereMesh* Joint::getSphereMesh() {
    return this->sphereMesh.get();
}
void Joint::setBindPoseMatrix(const glm::mat4& transform) {
    bindPoseMatrix = transform;
}
glm::mat4 Joint::getBindPoseMatrix() {
    return bindPoseMatrix;
}
