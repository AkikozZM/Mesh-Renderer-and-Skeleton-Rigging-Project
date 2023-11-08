#include "mainwindow.h"
#include <ui_mainwindow.h>
#include "cameracontrolshelp.h"
#include <QFileDialog>
#include <iostream>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->mygl->setFocus();

    connect(ui->halfEdgesListWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onHalfEdgeSelected(QListWidgetItem*)));
    connect(ui->vertsListWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onVertexSelected(QListWidgetItem*)));
    connect(ui->facesListWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onFaceSelected(QListWidgetItem*)));
    // skeleton
    connect(ui->jointsTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(onJointSelected(QTreeWidgetItem*)));
    // button
    connect(ui->ClearButton, SIGNAL(clicked()), this, SLOT(unselectAll()));
    connect(ui->AddVertexButton, SIGNAL(clicked()), this, SLOT(addVertex()));
    connect(ui->TriangulateButton, SIGNAL(clicked()), this, SLOT(triangulateFace()));
    connect(ui->SubdivisionButton, SIGNAL(clicked()), this, SLOT(subDivision()));
    // skeleton button
    connect(ui->BindSkinButton, SIGNAL(clicked()), this, SLOT(bindSkin()));
    connect(ui->jointPosXAddButton, SIGNAL(clicked()), this, SLOT(onJointPosXAdd()));
    connect(ui->jointPosXSubButton, SIGNAL(clicked()), this, SLOT(onJointPosXSub()));
    connect(ui->jointPosYAddButton, SIGNAL(clicked()), this, SLOT(onJointPosYAdd()));
    connect(ui->jointPosYSubButton, SIGNAL(clicked()), this, SLOT(onJointPosYSub()));
    connect(ui->jointPosZAddButton, SIGNAL(clicked()), this, SLOT(onJointPosZAdd()));
    connect(ui->jointPosZSubButton, SIGNAL(clicked()), this, SLOT(onJointPosZSub()));

    connect(ui->jointRotXAddButton, SIGNAL(clicked()), this, SLOT(onJointRotXAdd()));
    connect(ui->jointRotXSubButton, SIGNAL(clicked()), this, SLOT(onJointRotXSub()));
    connect(ui->jointRotYAddButton, SIGNAL(clicked()), this, SLOT(onJointRotYAdd()));
    connect(ui->jointRotYSubButton, SIGNAL(clicked()), this, SLOT(onJointRotYSub()));
    connect(ui->jointRotZAddButton, SIGNAL(clicked()), this, SLOT(onJointRotZAdd()));
    connect(ui->jointRotZSubButton, SIGNAL(clicked()), this, SLOT(onJointRotZSub()));

    // spin box
    connect(ui->faceBlueSpinBox, SIGNAL(valueChanged(double)), this, SLOT(onFaceBlueSpinBoxValueChanged(double)));
    connect(ui->faceGreenSpinBox, SIGNAL(valueChanged(double)), this, SLOT(onFaceGreenSpinBoxValueChanged(double)));
    connect(ui->faceRedSpinBox, SIGNAL(valueChanged(double)), this, SLOT(onFaceRedSpinBoxValueChanged(double)));
    connect(ui->vertPosXSpinBox, SIGNAL(valueChanged(double)), this, SLOT(onVertPosXSpinBoxValueChanged(double)));
    connect(ui->vertPosYSpinBox, SIGNAL(valueChanged(double)), this, SLOT(onVertPosYSpinBoxValueChanged(double)));
    connect(ui->vertPosZSpinBox, SIGNAL(valueChanged(double)), this, SLOT(onVertPosZSpinBoxValueChanged(double)));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionQuit_triggered()
{
    QApplication::exit();
}

void MainWindow::on_actionCamera_Controls_triggered()
{
    CameraControlsHelp* c = new CameraControlsHelp();
    c->show();
}
void MainWindow::on_actionImport_OBJ_triggered() {
    ui->mygl->drawMesh();
    populateLists();
}
void MainWindow::on_actionImport_JSON_triggered() {
    // TODO: import json
    ui->mygl->drawSkeleton();
    populateSkeleton();
}
void MainWindow::populateLists() {
    ui->vertsListWidget->clear();
    ui->halfEdgesListWidget->clear();
    ui->facesListWidget->clear();
    updateVertListWidget();
    updateFaceListWidget();
    updateHalfEdgeListWidget();
}
void MainWindow::populateSkeleton() {
    ui->jointsTreeWidget->clear();
    auto j = ui->mygl->skeletonRoot.get();
    //QTreeWidgetItem* rootItem = new QTreeWidgetItem(ui->jointsTreeWidget);
    j->setText(0, QString::fromStdString(j->getName()));
    ui->jointsTreeWidget->addTopLevelItem(j);
    updateJointListWidget(j, j);
}
void MainWindow::updateJointListWidget(Joint* joint, QTreeWidgetItem* parentItem) {
    if (!joint) {
        return;
    }
    for (const auto& child : joint->getChildren()) {
        //QTreeWidgetItem* childItem = new QTreeWidgetItem(parentItem);
        child->setText(0, QString::fromStdString(child->getName()));
        parentItem->addChild(child.get());
        updateJointListWidget(child.get(), child.get());
    }
}
void MainWindow::onHalfEdgeSelected(QListWidgetItem* item)
{
    HalfEdge* he = static_cast<HalfEdge*>(item);
    if (he) {
        ui->mygl->m_halfEdgeDisplay.updateHalfEdge(he);
        ui->mygl->m_halfEdgeDisplay.create();
        ui->mygl->selectedHalfEdge = he;
        // clear others
        ui->mygl->m_vertDisplay.updateVertex(nullptr);
        ui->mygl->m_faceDisplay.updateFace(nullptr);
        // update
        ui->mygl->update();
    }
}

void MainWindow::onVertexSelected(QListWidgetItem* item)
{
    unselectAll();
    Vertex* vert = static_cast<Vertex*>(item);
    if (vert) {
        // Get initial vertPos
        ui->vertPosXSpinBox->setValue(vert->position.x);
        ui->vertPosYSpinBox->setValue(vert->position.y);
        ui->vertPosZSpinBox->setValue(vert->position.z);

        // update
        ui->mygl->m_vertDisplay.updateVertex(vert);
        ui->mygl->m_vertDisplay.create();
        ui->mygl->selectedVertex = vert;
        // clear others
        ui->mygl->m_halfEdgeDisplay.updateHalfEdge(nullptr);
        ui->mygl->m_faceDisplay.updateFace(nullptr);
        // update
        ui->mygl->update();
    }
}

void MainWindow::onFaceSelected(QListWidgetItem* item)
{
    unselectAll();
    Face* face = static_cast<Face*>(item);
    if (face) {
        // Get initial color
        ui->faceRedSpinBox->setValue(face->color.r);
        ui->faceGreenSpinBox->setValue(face->color.g);
        ui->faceBlueSpinBox->setValue(face->color.b);
        // update fd
        ui->mygl->m_faceDisplay.updateFace(face);
        ui->mygl->m_faceDisplay.create();
        ui->mygl->selectedFace = face;
        // clear others
        ui->mygl->m_vertDisplay.updateVertex(nullptr);
        ui->mygl->m_halfEdgeDisplay.updateHalfEdge(nullptr);
        // update
        ui->mygl->update();
    }
}
// skeleton joint selects
void MainWindow::onJointSelected(QTreeWidgetItem* item) {
    Joint* j = static_cast<Joint*>(item);
    ui->mygl->selectedJoint = j;
    ui->mygl->jointSelectedTransform = j->getOverallTransformation();
    ui->mygl->m_sphereDisplay.jointSelected = true;
    ui->mygl->update();
}

void MainWindow::unselectAll() {
    ui->mygl->m_vertDisplay.updateVertex(nullptr);
    ui->mygl->m_halfEdgeDisplay.updateHalfEdge(nullptr);
    ui->mygl->m_faceDisplay.updateFace(nullptr);
    ui->mygl->update();
}
void MainWindow::addVertex() {
    ui->mygl->addVertexMidpoint();
    updateVertListWidget();
    updateHalfEdgeListWidget();

}
void MainWindow::triangulateFace() {
    ui->mygl->triangulateFace();
    ui->mygl->m_faceDisplay.destroy();
    ui->mygl->m_faceDisplay.create();
    ui->mygl->update();
    updateHalfEdgeListWidget();
    updateFaceListWidget();
}
void MainWindow::updateVertListWidget()
{
    Mesh* m = ui->mygl->getM_Mesh();
    for (auto& v : m->vertices)
    {
        ui->vertsListWidget->addItem(v.get());
    }
}

void MainWindow::updateFaceListWidget()
{
    Mesh* m = ui->mygl->getM_Mesh();
    for (auto& f : m->faces)
    {
        ui->facesListWidget->addItem(f.get());
    }
}

void MainWindow::updateHalfEdgeListWidget()
{
    Mesh* m = ui->mygl->getM_Mesh();
    for (auto& he : m->halfEdges)
    {
        ui->halfEdgesListWidget->addItem(he.get());
    }
}
void MainWindow::onFaceGreenSpinBoxValueChanged(double value) {
    FaceDisplay& fd = ui->mygl->m_faceDisplay;
    Face* f = fd.getRepresentedFace();
    if (!f) {
        return;
    }
    // update mesh
    ui->mygl->getM_Mesh()->create();
    f->color.g = value;
    fd.updateFace(f);
    fd.create();
    ui->mygl->update();
}

void MainWindow::onFaceBlueSpinBoxValueChanged(double value) {
    FaceDisplay& fd = ui->mygl->m_faceDisplay;
    Face* f = fd.getRepresentedFace();
    if (!f) {
        return;
    }
    // update mesh
    ui->mygl->getM_Mesh()->create();
    f->color.b = value;
    fd.updateFace(f);
    fd.create();
    ui->mygl->update();
}
void MainWindow::onFaceRedSpinBoxValueChanged(double value) {
    FaceDisplay& fd = ui->mygl->m_faceDisplay;
    Face* f = fd.getRepresentedFace();
    if (!f) {
        return;
    }
    // update mesh
    ui->mygl->getM_Mesh()->create();
    f->color.r = value;
    fd.updateFace(f);
    fd.create();
    ui->mygl->update();
}
void MainWindow::onVertPosXSpinBoxValueChanged(double value) {
    VertexDisplay& vd = ui->mygl->m_vertDisplay;
    Vertex* v = vd.getRepresentedVertex();
    if (!v) return;
    v->position.x = value;
    // update mesh
    ui->mygl->getM_Mesh()->create();
    vd.updateVertex(v);
    vd.create();
    ui->mygl->update();
}
void MainWindow::onVertPosYSpinBoxValueChanged(double value) {
    VertexDisplay& vd = ui->mygl->m_vertDisplay;
    Vertex* v = vd.getRepresentedVertex();
    if (!v) return;
    v->position.y = value;
    // update mesh
    ui->mygl->getM_Mesh()->create();
    vd.updateVertex(v);
    vd.create();
    ui->mygl->update();
}
void MainWindow::onVertPosZSpinBoxValueChanged(double value) {
    VertexDisplay& vd = ui->mygl->m_vertDisplay;
    Vertex* v = vd.getRepresentedVertex();
    if (!v) return;
    v->position.z = value;
    // update mesh
    ui->mygl->getM_Mesh()->create();
    vd.updateVertex(v);
    vd.create();
    ui->mygl->update();
}
void MainWindow::subDivision() {
    unselectAll();
    ui->mygl->subDivision();
    updateFaceListWidget();
    updateHalfEdgeListWidget();
    updateVertListWidget();
    ui->mygl->getM_Mesh()->create();
    ui->mygl->update();
}
void MainWindow::bindSkin() {
    // TODO: skinning function be a simple distance-based weight assignation
    ui->mygl->distanceBasedSkin();
}
void updateAllChildren(Joint* j, const glm::mat4& parentTransform) {
    if (!j) {
        return;
    }
    glm::mat4 currentTransform = parentTransform * j->getLocalTransformation();
    j->setBindMatrix(currentTransform);
    for (auto& child : j->getChildren()) {
        updateAllChildren(child.get(), currentTransform);
    }
}
void MainWindow::onJointPosXAdd() {
    if (!ui->mygl->selectedJoint) {
        return;
    }
    glm::mat4 m = ui->mygl->jointSelectedTransform;
    m = glm::translate(m, glm::vec3(1.0f, 0.0f, 0.0f));
    ui->mygl->jointSelectedTransform = m;
    // need the current joint position
    // then change the joint transform
    Joint* j = ui->mygl->selectedJoint;
    j->setPosition(glm::vec3(j->getPosition().x + 1.f, j->getPosition().y, j->getPosition().z));
    for (auto& child : j->getChildren()) {
        updateAllChildren(child.get(), m);
    }
    j->setBindMatrix(m);
    ui->mygl->getM_Mesh()->create();
    ui->mygl->setSkinMatrix();
    ui->mygl->update();
}
void MainWindow::onJointPosXSub() {
    if (!ui->mygl->selectedJoint) {
        return;
    }
    glm::mat4 m = ui->mygl->jointSelectedTransform;
    m = glm::translate(m, glm::vec3(-1.0f, 0.0f, 0.0f));
    ui->mygl->jointSelectedTransform = m;
    // need the current joint position
    // then change the joint transform
    Joint* j = ui->mygl->selectedJoint;
    j->setPosition(glm::vec3(j->getPosition().x - 1.f, j->getPosition().y, j->getPosition().z));
    for (auto& child : j->getChildren()) {
        updateAllChildren(child.get(), m);
    }
    j->setBindMatrix(m);
    ui->mygl->update();
}
void MainWindow::onJointPosYAdd(){
    if (!ui->mygl->selectedJoint) {
        return;
    }
    glm::mat4 m = ui->mygl->jointSelectedTransform;
    m = glm::translate(m, glm::vec3(.0f, 1.0f, 0.0f));
    ui->mygl->jointSelectedTransform = m;
    // need the current joint position
    // then change the joint transform
    Joint* j = ui->mygl->selectedJoint;
    j->setPosition(glm::vec3(j->getPosition().x, j->getPosition().y + 1.f, j->getPosition().z));
    for (auto& child : j->getChildren()) {
        updateAllChildren(child.get(), m);
    }
    j->setBindMatrix(m);
    ui->mygl->update();
}
void MainWindow::onJointPosYSub(){
    if (!ui->mygl->selectedJoint) {
        return;
    }
    glm::mat4 m = ui->mygl->jointSelectedTransform;
    m = glm::translate(m, glm::vec3(.0f, -1.0f, 0.0f));
    ui->mygl->jointSelectedTransform = m;
    // need the current joint position
    // then change the joint transform
    Joint* j = ui->mygl->selectedJoint;
    j->setPosition(glm::vec3(j->getPosition().x, j->getPosition().y - 1.f, j->getPosition().z));
    for (auto& child : j->getChildren()) {
        updateAllChildren(child.get(), m);
    }
    j->setBindMatrix(m);
    ui->mygl->update();
}
void MainWindow::onJointPosZAdd(){
    if (!ui->mygl->selectedJoint) {
        return;
    }
    glm::mat4 m = ui->mygl->jointSelectedTransform;
    m = glm::translate(m, glm::vec3(.0f, .0f, 1.0f));
    ui->mygl->jointSelectedTransform = m;
    // need the current joint position
    // then change the joint transform
    Joint* j = ui->mygl->selectedJoint;
    j->setPosition(glm::vec3(j->getPosition().x, j->getPosition().y, j->getPosition().z + 1.f));
    for (auto& child : j->getChildren()) {
        updateAllChildren(child.get(), m);
    }
    j->setBindMatrix(m);
    ui->mygl->update();
}
void MainWindow::onJointPosZSub(){
    if (!ui->mygl->selectedJoint) {
        return;
    }
    glm::mat4 m = ui->mygl->jointSelectedTransform;
    m = glm::translate(m, glm::vec3(.0f, .0f, -1.0f));
    ui->mygl->jointSelectedTransform = m;
    // need the current joint position
    // then change the joint transform
    Joint* j = ui->mygl->selectedJoint;
    j->setPosition(glm::vec3(j->getPosition().x, j->getPosition().y, j->getPosition().z - 1.f));
    for (auto& child : j->getChildren()) {
        updateAllChildren(child.get(), m);
    }
    j->setBindMatrix(m);
    ui->mygl->update();
}
void MainWindow::onJointRotXAdd() {
    glm::mat4 m = ui->mygl->jointSelectedTransform;
    float angle = glm::radians(5.f);
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(1.0f, 0.0f, 0.0f));
    Joint* j = ui->mygl->selectedJoint;
    glm::mat4 currentTransform = j->getBindMatrix();
    glm::mat4 newTransform = currentTransform * rotationMatrix;

    ui->mygl->jointSelectedTransform = newTransform;
    glm::quat rotationQuat = glm::angleAxis(angle, glm::vec3(1.0f, 0.0f, 0.0f));

    j->setRotation(j->getRotation() * rotationQuat);

    for (auto& child : j->getChildren()) {
        updateAllChildren(child.get(), newTransform);
    }
    j->setBindMatrix(newTransform);
    ui->mygl->update();
}
void MainWindow::onJointRotXSub() {
    glm::mat4 m = ui->mygl->jointSelectedTransform;
    float angle = glm::radians(-5.f);
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(1.0f, 0.0f, 0.0f));
    Joint* j = ui->mygl->selectedJoint;
    glm::mat4 currentTransform = j->getBindMatrix();
    glm::mat4 newTransform = currentTransform * rotationMatrix;

    ui->mygl->jointSelectedTransform = newTransform;
    glm::quat rotationQuat = glm::angleAxis(angle, glm::vec3(1.0f, 0.0f, 0.0f));

    j->setRotation(j->getRotation() * rotationQuat);


    for (auto& child : j->getChildren()) {
        updateAllChildren(child.get(), newTransform);
    }
    j->setBindMatrix(newTransform);
    ui->mygl->update();
}
void MainWindow::onJointRotYAdd() {
    glm::mat4 m = ui->mygl->jointSelectedTransform;
    float angle = glm::radians(5.f);
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(.0f, 1.0f, 0.0f));
    Joint* j = ui->mygl->selectedJoint;
    glm::mat4 currentTransform = j->getBindMatrix();
    glm::mat4 newTransform = currentTransform * rotationMatrix;

    ui->mygl->jointSelectedTransform = newTransform;
    glm::quat rotationQuat = glm::angleAxis(angle, glm::vec3(.0f, 1.0f, 0.0f));

    j->setRotation(j->getRotation() * rotationQuat);


    for (auto& child : j->getChildren()) {
        updateAllChildren(child.get(), newTransform);
    }
    j->setBindMatrix(newTransform);
    ui->mygl->update();
}
void MainWindow::onJointRotYSub() {
    glm::mat4 m = ui->mygl->jointSelectedTransform;
    float angle = glm::radians(-5.f);
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(.0f, 1.0f, 0.0f));
    Joint* j = ui->mygl->selectedJoint;
    glm::mat4 currentTransform = j->getBindMatrix();
    glm::mat4 newTransform = currentTransform * rotationMatrix;

    ui->mygl->jointSelectedTransform = newTransform;
    glm::quat rotationQuat = glm::angleAxis(angle, glm::vec3(.0f, 1.0f, 0.0f));

    j->setRotation(j->getRotation() * rotationQuat);


    for (auto& child : j->getChildren()) {
        updateAllChildren(child.get(), newTransform);
    }
    j->setBindMatrix(newTransform);
    ui->mygl->update();
}
void MainWindow::onJointRotZAdd() {
    glm::mat4 m = ui->mygl->jointSelectedTransform;
    float angle = glm::radians(5.f);
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(.0f, 0.0f, 1.0f));
    Joint* j = ui->mygl->selectedJoint;
    glm::mat4 currentTransform = j->getBindMatrix();
    glm::mat4 newTransform = currentTransform * rotationMatrix;

    ui->mygl->jointSelectedTransform = newTransform;
    glm::quat rotationQuat = glm::angleAxis(angle, glm::vec3(.0f, 0.0f, 1.0f));

    j->setRotation(j->getRotation() * rotationQuat);


    for (auto& child : j->getChildren()) {
        updateAllChildren(child.get(), newTransform);
    }
    j->setBindMatrix(newTransform);
    ui->mygl->update();
}
void MainWindow::onJointRotZSub() {
    glm::mat4 m = ui->mygl->jointSelectedTransform;
    float angle = glm::radians(-5.f);
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(.0f, .0f, 1.0f));
    Joint* j = ui->mygl->selectedJoint;
    glm::mat4 currentTransform = j->getBindMatrix();
    glm::mat4 newTransform = currentTransform * rotationMatrix;

    ui->mygl->jointSelectedTransform = newTransform;
    glm::quat rotationQuat = glm::angleAxis(angle, glm::vec3(.0f, 0.0f, 1.0f));

    j->setRotation(j->getRotation() * rotationQuat);


    for (auto& child : j->getChildren()) {
        updateAllChildren(child.get(), newTransform);
    }
    j->setBindMatrix(newTransform);
    ui->mygl->update();
}
