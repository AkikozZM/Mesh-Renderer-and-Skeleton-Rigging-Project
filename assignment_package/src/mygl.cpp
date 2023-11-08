#include "mygl.h"
#include <la.h>
#include <iostream>
#include <QApplication>
#include <QKeyEvent>
#include <QFileDialog>
#include "scene/Mesh.h"
#include <fstream>
#include <sstream>
#include <map>
#include <glm/glm.hpp>
#include <QFileDialog>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "scene/Joint.h"


MyGL::MyGL(QWidget *parent)
    : OpenGLContext(parent),
      m_sphere(this), m_jointLine(this),
      m_geomSquare(this),
    m_progLambert(this), m_progFlat(this), prog_skeleton(this),
    m_glCamera(), canDrawMyMesh(false), canDrawSkeleton(false),
    m_vertDisplay(this), m_halfEdgeDisplay(this), m_faceDisplay(this), m_sphereDisplay(this),
    selectedVertex(nullptr), selectedFace(nullptr), selectedHalfEdge(nullptr)
{
    setFocusPolicy(Qt::StrongFocus);
}

MyGL::~MyGL()
{
    makeCurrent();
    glDeleteVertexArrays(1, &vao);
    m_sphere.destroy();
    m_geomSquare.destroy();
    m_jointLine.destroy();
    m_vertDisplay.destroy();
    m_halfEdgeDisplay.destroy();
    m_faceDisplay.destroy();
    m_sphereDisplay.destroy();
}

void MyGL::initializeGL()
{
    // Create an OpenGL context using Qt's QOpenGLFunctions_3_2_Core class
    // If you were programming in a non-Qt context you might use GLEW (GL Extension Wrangler)instead
    initializeOpenGLFunctions();
    // Print out some information about the current OpenGL context
    debugContextVersion();

    // Set a few settings/modes in OpenGL rendering
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    // Set the size with which points should be rendered
    glPointSize(5);
    // Set the color with which the screen is filled at the start of each render call.
    glClearColor(0.5, 0.5, 0.5, 1);

    printGLErrorLog();

    // Create a Vertex Attribute Object
    glGenVertexArrays(1, &vao);

    //Create the instances of Cylinder and Sphere.
    m_geomSquare.create();
    m_sphere.create();
    m_sphereDisplay.create();
    //m_jointLine.create();
    // Create and set up the diffuse shader
    m_progLambert.create(":/glsl/lambert.vert.glsl", ":/glsl/lambert.frag.glsl");
    // Create and set up the flat lighting shader
    m_progFlat.create(":/glsl/flat.vert.glsl", ":/glsl/flat.frag.glsl");
    prog_skeleton.create(":/glsl/skeleton.vert", ":/glsl/skeleton.frag");

    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.
    glBindVertexArray(vao);
}

void MyGL::resizeGL(int w, int h)
{
    //This code sets the concatenated view and perspective projection matrices used for
    //our scene's camera view.
    m_glCamera = Camera(w, h);
    glm::mat4 viewproj = m_glCamera.getViewProj();

    // Upload the view-projection matrix to our shaders (i.e. onto the graphics card)

    m_progLambert.setViewProjMatrix(viewproj);
    m_progFlat.setViewProjMatrix(viewproj);
    prog_skeleton.setViewProjMatrix(viewproj);

    printGLErrorLog();
}

//This function is called by Qt any time your GL window is supposed to update
//For example, when the function update() is called, paintGL is called implicitly.
void MyGL::paintGL()
{
    // Clear the screen so that we only see newly drawn images
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_progFlat.setViewProjMatrix(m_glCamera.getViewProj());
    m_progLambert.setViewProjMatrix(m_glCamera.getViewProj());
    m_progLambert.setCamPos(m_glCamera.eye);
    m_progFlat.setModelMatrix(glm::mat4(1.f));
    prog_skeleton.setViewProjMatrix(m_glCamera.getViewProj());
    prog_skeleton.setCamPos((m_glCamera.eye));

    if (canDrawMyMesh){
        if (!m_vertDisplay.isVertexSelected() &&
            !m_faceDisplay.isFaceSelected() &&
            !m_halfEdgeDisplay.isHalfEdgeSelected()
            ) {
            glm::mat4 model = glm::mat4(1.f);
            m_progLambert.setModelMatrix(model);
            m_progLambert.draw(*m_mesh);
        }
        else if (m_vertDisplay.isVertexSelected()) {
            glDisable(GL_DEPTH_TEST);
            glm::mat4 model = glm::mat4(1.f);
            m_progLambert.setModelMatrix(model);
            m_progLambert.draw(*m_mesh);
            m_progFlat.setModelMatrix(glm::mat4(1.f));
            m_progFlat.draw(m_vertDisplay);
            glEnable(GL_DEPTH_TEST);
        }
        else if (m_faceDisplay.isFaceSelected()) {
            glDisable(GL_DEPTH_TEST);
            glm::mat4 model = glm::mat4(1.f);
            m_progLambert.setModelMatrix(model);
            m_progLambert.draw(*m_mesh);
            m_progFlat.setModelMatrix(glm::mat4(1.f));
            m_progFlat.draw(m_faceDisplay);
            glEnable(GL_DEPTH_TEST);
        }
        else if (m_halfEdgeDisplay.isHalfEdgeSelected()) {
            glDisable(GL_DEPTH_TEST);
            glm::mat4 model = glm::mat4(1.f);
            m_progLambert.setModelMatrix(model);
            m_progLambert.draw(*m_mesh);
            m_progFlat.setModelMatrix(glm::mat4(1.f));
            m_progFlat.draw(m_halfEdgeDisplay);
            glEnable(GL_DEPTH_TEST);
        }
    }
    if (canDrawSkeleton) {
        prog_skeleton.setModelMatrix(glm::mat4(1.f));
        prog_skeleton.draw(*m_mesh);
        glDisable(GL_DEPTH_TEST);
        for (auto& j : m_allJoints) {
            m_progLambert.setModelMatrix(j->getBindMatrix());
            m_progLambert.draw(m_sphere);
        }
        drawSkLine(skeletonRoot);
        if (m_sphereDisplay.jointSelected) {
            m_progLambert.setModelMatrix(jointSelectedTransform);
            m_progLambert.draw(m_sphereDisplay);
        }
    }
    glEnable(GL_DEPTH_TEST);
    update();
}

void MyGL::drawSkLine(const std::unique_ptr<Joint>& joint) {
    if (!joint) {
        return;
    }
    // Draw the sphere for this joint
    glm::mat4 jointTransform = joint->getOverallTransformation();
    m_progLambert.setModelMatrix(jointTransform);
    m_progLambert.draw(m_sphere);

    // Draw the lines between this joint and its children
    for (const auto& child : joint->getChildren()) {
        glm::mat4 childTransform = child->getOverallTransformation();
        glm::vec3 start = glm::vec3(jointTransform[3]);
        glm::vec3 end = glm::vec3(childTransform[3]);
        m_jointLine.setPositions(start, end);
        m_jointLine.create();
        m_progFlat.setModelMatrix(glm::mat4(1.f));
        m_progFlat.draw(m_jointLine);

        // Recursively draw the child's hierarchy
        drawSkLine(child);
    }
}
void MyGL::keyPressEvent(QKeyEvent *e)
{
    float amount = 2.0f;
    if(e->modifiers() & Qt::ShiftModifier){
        amount = 10.0f;
    }
    // http://doc.qt.io/qt-5/qt.html#Key-enum
    // This could all be much more efficient if a switch
    // statement were used
    if (e->key() == Qt::Key_Escape) {
        QApplication::quit();
    } else if (e->key() == Qt::Key_Right) {
        m_glCamera.RotateAboutUp(-amount);
    } else if (e->key() == Qt::Key_Left) {
        m_glCamera.RotateAboutUp(amount);
    } else if (e->key() == Qt::Key_Up) {
        m_glCamera.RotateAboutRight(-amount);
    } else if (e->key() == Qt::Key_Down) {
        m_glCamera.RotateAboutRight(amount);
    } else if (e->key() == Qt::Key_1) {
        m_glCamera.fovy += amount;
    } else if (e->key() == Qt::Key_2) {
        m_glCamera.fovy -= amount;
    } else if (e->key() == Qt::Key_W) {
        m_glCamera.TranslateAlongLook(amount);
    } else if (e->key() == Qt::Key_S) {
        m_glCamera.TranslateAlongLook(-amount);
    } else if (e->key() == Qt::Key_D) {
        m_glCamera.TranslateAlongRight(amount);
    } else if (e->key() == Qt::Key_A) {
        m_glCamera.TranslateAlongRight(-amount);
    } else if (e->key() == Qt::Key_Q) {
        m_glCamera.TranslateAlongUp(-amount);
    } else if (e->key() == Qt::Key_E) {
        m_glCamera.TranslateAlongUp(amount);
    } else if (e->key() == Qt::Key_R) {
        m_glCamera = Camera(this->width(), this->height());
    } else if (e->key() == Qt::Key_N && selectedHalfEdge) {
    //  NEXT half-edge of the currently selected half-edge
        selectedHalfEdge = selectedHalfEdge->next;
        m_halfEdgeDisplay.updateHalfEdge(selectedHalfEdge);
        m_halfEdgeDisplay.create();
        m_faceDisplay.updateFace(nullptr);
        m_vertDisplay.updateVertex(nullptr);
    } else if (e->key() == Qt::Key_M && selectedHalfEdge) {
    // SYM half-edge of the currently selected half-edge
        selectedHalfEdge = selectedHalfEdge->sym;
        m_halfEdgeDisplay.updateHalfEdge(selectedHalfEdge);
        m_halfEdgeDisplay.create();
        m_faceDisplay.updateFace(nullptr);
        m_vertDisplay.updateVertex(nullptr);
    } else if (e->key() == Qt::Key_F && selectedHalfEdge) {
        selectedFace = selectedHalfEdge->face;
        m_faceDisplay.updateFace(selectedFace);
        m_faceDisplay.create();
        m_halfEdgeDisplay.updateHalfEdge(nullptr);
        m_vertDisplay.updateVertex(nullptr);
    } else if (e->key() == Qt::Key_V && selectedHalfEdge) {
        selectedVertex = selectedHalfEdge->vertex;
        m_vertDisplay.updateVertex(selectedVertex);
        m_vertDisplay.create();
        m_halfEdgeDisplay.updateHalfEdge(nullptr);
        m_faceDisplay.updateFace(nullptr);
    } else if (e->key() == Qt::Key_H && !e->modifiers() && selectedVertex) {
        selectedHalfEdge = selectedVertex->halfEdge;
        m_halfEdgeDisplay.updateHalfEdge(selectedHalfEdge);
        m_halfEdgeDisplay.create();
        m_faceDisplay.updateFace(nullptr);
        m_vertDisplay.updateVertex(nullptr);
    } else if (e->key() == Qt::Key_H && (e->modifiers() & Qt::ShiftModifier) && selectedFace) {
        selectedHalfEdge = selectedFace->halfEdge;
        m_halfEdgeDisplay.updateHalfEdge(selectedHalfEdge);
        m_halfEdgeDisplay.create();
        m_faceDisplay.updateFace(nullptr);
        m_vertDisplay.updateVertex(nullptr);
    }
    m_glCamera.RecomputeAttributes();
    update();  // Calls paintGL, among other things
}
Mesh* MyGL::drawMesh() {
    // reset unique ID before reload the next obj
    if (m_mesh.get() != nullptr) {
        m_mesh.get()->faces[0].get()->resetID();
        m_mesh.get()->vertices[0].get()->resetID();
        m_mesh.get()->halfEdges[0].get()->resetID();
    }
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open OBJ File"), "", tr("OBJ Files (*.obj);;All Files (*)"));
    if (!fileName.isEmpty()) {
        std::cout<<"open obj success!"<<std::endl;
        std::ifstream objFile(fileName.toStdString());
        if (!objFile.is_open()) {
            std::cerr<<"Failed to open the file."<<std::endl;
            return nullptr;
        }
        std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>(this);
        int ind = 0;
        std::vector<glm::vec4> v_pos;
        std::vector<glm::vec4> v_col;

        std::string line;
        while (std::getline(objFile, line)) {
            std::istringstream ss(line);
            std::string prefix;
            ss >> prefix;

            if (prefix == "v") {
                glm::vec3 position;
                ss >> position.x >> position.y >> position.z;
                auto vertex = std::make_unique<Vertex>(position);
                mesh->vertices.push_back(std::move(vertex));
                v_pos.push_back(glm::vec4(position.x, position.y, position.z, 1.f));
            }
            else if (prefix == "f") {
                int vertexIndex, textureIndex, normalIndex;
                std::vector<std::vector<int>> tempVerts;
                while (ss >> vertexIndex) {
                    // skip '/'
                    ss.ignore(1, '/');
                    ss >> textureIndex;
                    ss.ignore(1, '/');
                    ss >> normalIndex;
                    tempVerts.push_back({vertexIndex, textureIndex, normalIndex});
                }
                mesh->constructHalfEdge(tempVerts, ind, v_pos, v_col);
            }
        }
        mesh->createBuffer();
        objFile.close();
        m_mesh = std::move(mesh);
        canDrawMyMesh = true;
        //canDrawSkeleton = false;
        std::cout<<"finish"<<std::endl;
        return m_mesh.get();
    }
    // will not reach this line
    return nullptr;
}
Mesh* MyGL::getM_Mesh() {
    return m_mesh.get();
}
void MyGL::addVertexMidpoint() {

    if (!selectedHalfEdge) {
        return;
    }
    HalfEdge* he2 = selectedHalfEdge->sym;
    Vertex* v1 = selectedHalfEdge->vertex;
    Vertex* v2 = selectedHalfEdge->getPrevious()->vertex;
    std::cout<<v1->id<<std::endl;
    std::cout<<v2->id<<std::endl;

    glm::vec3 midpoint = (v1->position + v2->position) / 2.f;
    Vertex* v3 = new Vertex(midpoint);

    // Create two new halfedges
    HalfEdge* HE1B = new HalfEdge();
    HalfEdge* HE2B = new HalfEdge();

    HE1B->face = selectedHalfEdge->face;
    HE2B->face = he2->face;

    // Adjust pointers
    HE1B->vertex = v1;
    HE2B->vertex = v2;

    // Adjust sym next vert of he, he2, he1b, he2b
    HE1B->next = selectedHalfEdge->next;
    HE2B->next = he2->next;
    selectedHalfEdge->next = HE1B;
    he2->next = HE2B;
    selectedHalfEdge->vertex = v3;
    he2->vertex = v3;

    selectedHalfEdge->sym = HE2B, HE2B->sym = selectedHalfEdge;
    he2->sym = HE1B, HE1B->sym = he2;
    HE1B->vertex->halfEdge = HE1B;
    HE2B->vertex->halfEdge = HE2B;
    v3->halfEdge = selectedHalfEdge;

    m_mesh->vertices.push_back(std::unique_ptr<Vertex>(v3));
    m_mesh->halfEdges.push_back(std::unique_ptr<HalfEdge>(HE1B));
    m_mesh->halfEdges.push_back(std::unique_ptr<HalfEdge>(HE2B));
}
void MyGL::triangulateFace() {
    if (!selectedFace) {
        return;
    }
    HalfEdge* startEdge = selectedFace->halfEdge;
    Vertex* v = startEdge->vertex;
    HalfEdge* currEdge = startEdge->next;
    while (currEdge->next->next != startEdge) {
        auto newhe = std::make_unique<HalfEdge>();
        auto newf = std::make_unique<Face>();
        newhe->setFace(newf.get());

        currEdge->setFace(newf.get());

        currEdge->next->setFace(newf.get());

        auto newsym = std::make_unique<HalfEdge>();
        newhe->setSym(newsym.get());

        newsym->next = currEdge->next->next;
        newsym->setVertex(currEdge->next->vertex);

        newhe->next = currEdge;
        newhe->setVertex(v);

        currEdge->next->next = newhe.get();
        currEdge = newsym.get();

        // Add the new elements to the mesh
        m_mesh->halfEdges.push_back(std::move(newhe));
        m_mesh->halfEdges.push_back(std::move(newsym));
        m_mesh->faces.push_back(std::move(newf));
    }
    currEdge->setFace(selectedFace);
    startEdge->next = currEdge;
}
void MyGL::subDivision() {
    std::unordered_map<Face*, Vertex*> centers;
    std::unordered_map<HalfEdge*, Vertex*> mids;
    std::vector<std::unique_ptr<Face>> fs;
    std::vector<std::unique_ptr<HalfEdge>> hfs;
    m_mesh->meshSubDivision(centers, mids, fs, hfs);
    m_mesh->halfEdges = std::move(hfs);
    m_mesh->faces = std::move(fs);

}
void MyGL::drawSkeleton() {
    QString filename = QFileDialog::getOpenFileName(this, "Open JSON File", "", "JSON Files (*.json);;All Files (*)");
    if (filename.isEmpty()) {
        return;
    }
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly)) {
        return;
    }
    QByteArray jsonData = file.readAll();
    file.close();
    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    if(doc.isNull()) {
        std::cout<<"doc null"<<std::endl;
        return;
    }
    QJsonObject rootObj = doc.object();
    // get the root of input json file
    m_allJoints.clear();
    jointsTransform.clear();
    jointTransformMap.clear();
    auto rootJoint = parseJoint(rootObj["root"].toObject());
    drawJoint(rootJoint, glm::mat4(1.f));
    canDrawSkeleton = true;
    //canDrawMyMesh = false;
    skeletonRoot = std::move(rootJoint);
    getAllJoints(skeletonRoot.get(), m_allJoints);
    update();
}
std::unique_ptr<Joint> MyGL::parseJoint(const QJsonObject& jointObj) {
    std::string name = jointObj["name"].toString().toStdString();
    // position
    QJsonArray posArray = jointObj["pos"].toArray();
    glm::vec3 position(posArray[0].toDouble(), posArray[1].toDouble(), posArray[2].toDouble());
    // rotation [angle, axis_x, axis_y, axis_z]
    QJsonArray rotArray = jointObj["rot"].toArray();
    glm::quat rotation(rotArray[0].toDouble(), rotArray[1].toDouble(), rotArray[2].toDouble(), rotArray[3].toDouble());
    // convert to quaternion
    glm::quat q = glm::angleAxis(glm::radians(rotation[0]), glm::vec3(rotation[1], rotation[2], rotation[3]));

    // create joint
    auto joint = std::make_unique<Joint>(name, nullptr, position, q);

    // children
    QJsonArray childrenArray = jointObj["children"].toArray();
    for (int i = 0; i < childrenArray.size(); i++) {
        QJsonObject childObj = childrenArray[i].toObject();
        auto childJoint = parseJoint(childObj);
        joint->addChild(std::move(childJoint));
    }
    return joint;
}
void MyGL::drawJoint(const std::unique_ptr<Joint>& joint, const glm::mat4& parentTransform) {
    if (!joint) {
        return;
    }
    glm::mat4 jointTransform = parentTransform * joint->getLocalTransformation();
    jointsTransform.push_back(jointTransform);
    jointTransformMap[joint.get()] = jointTransform;
    joint->setBindMatrix(jointTransform);

    for (const auto& child : joint->getChildren()) {
        drawJoint(child, jointTransform);
    }
}

void MyGL::getAllJoints(Joint* joint, std::vector<Joint*>& jointList) {
    if (!joint) return;

    jointList.push_back(joint);

    for (auto& child : joint->getChildren()) {
        getAllJoints(child.get(), jointList);
    }
}

void MyGL::distanceBasedSkin() {
    if (!m_mesh || !skeletonRoot.get()) {
        std::cout<<"no skeleton or mesh imported"<<std::endl;
        return;
    }
    const int N = 2;
    for (auto& v : m_mesh->vertices) {
        std::vector<std::pair<Joint*, float>> jointDistances;
        for (Joint* j : m_allJoints) {
            float distance = glm::distance(v->position, j->getPosition());
            jointDistances.push_back({j, distance});
        }
        std::sort(jointDistances.begin(), jointDistances.end(), [](const auto& a, const auto& b) {
            return a.second < b.second;
        });
        // Get the N closest joints
        float summed_distances = 0.0f;
        for (int i = 0; i < N; i++) {
            summed_distances += jointDistances[i].second;
        }
        for (int i = 0; i < N; i++) {
            Joint* joint = jointDistances[i].first;
            float distance = jointDistances[i].second;
            float weight = 1 - (distance / summed_distances);

            v->addInfluence(joint, weight);
        }
    }
    m_mesh->create();
    setSkinMatrix();
    for (auto j : m_allJoints) {
        glm::mat4 m = glm::inverse(j->getOverallTransformation());
        j->setBindPoseMatrix(m);
    }
    std::cout<<"Bind sucess!"<<std::endl;
    canDrawMyMesh = false;
}
void MyGL::setSkinMatrix() {
    std::vector<glm::mat4>bindMatrix;
    std::vector<glm::mat4>overallTransformation;
    for (int i = 0; i < m_allJoints.size(); i++) {
        bindMatrix.push_back(m_allJoints[i]->getBindPoseMatrix());
        overallTransformation.push_back(m_allJoints[i]->getOverallTransformation());
    }
    prog_skeleton.SetBindMatrix(bindMatrix);
    prog_skeleton.SetOverallTransformation(overallTransformation);
}
