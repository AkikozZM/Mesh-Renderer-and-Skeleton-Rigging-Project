#ifndef MYGL_H
#define MYGL_H

#include <openglcontext.h>
#include <utils.h>
#include <shaderprogram.h>
#include <scene/squareplane.h>
#include "camera.h"
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include "scene/Mesh.h"
#include "scene/facedisplay.h"
#include "scene/vertexdisplay.h"
#include "scene/halfedgedisplay.h"
#include "scene/SphereMesh.h"
#include "scene/Joint.h"
#include "scene/Skeleton.h"
#include "scene/JointLine.h"
#include "scene/SphereMeshDisplay.h"

class MyGL
    : public OpenGLContext
{
    Q_OBJECT
private:
    SphereMesh m_sphere;
    JointLine m_jointLine;
    SquarePlane m_geomSquare;// The instance of a unit cylinder we can use to render any cylinder
    ShaderProgram m_progLambert;// A shader program that uses lambertian reflection
    ShaderProgram m_progFlat;// A shader program that uses "flat" reflection (no shadowing at all)
    ShaderProgram prog_skeleton;
    GLuint vao; // A handle for our vertex array object. This will store the VBOs created in our geometry classes.
                // Don't worry too much about this. Just know it is necessary in order to render geometry.

    Camera m_glCamera;
    std::unique_ptr<Mesh> m_mesh;
    std::unique_ptr<Skeleton> m_skeleton;
    bool canDrawMyMesh;
    bool canDrawSkeleton;


public:
    explicit MyGL(QWidget *parent = nullptr);
    ~MyGL();

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    Mesh* drawMesh();
    void drawSkeleton();
    std::unique_ptr<Joint> parseJoint(const QJsonObject& jointObj);
    void drawJoint(const std::unique_ptr<Joint>& joint, const glm::mat4& parentTransform);

    VertexDisplay m_vertDisplay;
    HalfEdgeDisplay m_halfEdgeDisplay;
    FaceDisplay m_faceDisplay;
    SphereMeshDisplay m_sphereDisplay;

    Vertex* selectedVertex;
    Face* selectedFace;
    HalfEdge* selectedHalfEdge;
    Mesh* getM_Mesh();
    Joint* selectedJoint;

    void addVertexMidpoint();
    void triangulateFace();
    void subDivision();
    std::vector<glm::mat4> jointsTransform;
    std::unique_ptr<Joint> skeletonRoot;
    void drawSkLine(const std::unique_ptr<Joint>& joint);
    void distanceBasedSkin();
    glm::mat4 jointSelectedTransform;
    std::unordered_map<Joint*, glm::mat4> jointTransformMap;
    std::vector<Joint*> m_allJoints;
    void getAllJoints(Joint* joint, std::vector<Joint*>& jointList);
    void setSkinMatrix();

protected:
    void keyPressEvent(QKeyEvent *e);
};


#endif // MYGL_H
