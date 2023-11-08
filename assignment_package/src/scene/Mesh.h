#ifndef MESH_H
#define MESH_H
#include "Vertex.h"
#include "Face.h"
#include "HalfEdge.h"
#include <vector>
#include "drawable.h"

class Mesh : public Drawable
{
public:
    Mesh(OpenGLContext* context);
    ~Mesh() override;
    std::vector<std::unique_ptr<Face>> faces;
    std::vector<std::unique_ptr<Vertex>> vertices;
    std::vector<std::unique_ptr<HalfEdge>> halfEdges;
    void create() override;

    std::vector<int> m_vertIdx;
    std::vector<glm::vec4> m_vertPos;
    std::vector<glm::vec4> m_vertCol;
    void constructHalfEdge(std::vector<std::vector<int>>& verts, int& ind,
                           std::vector<glm::vec4>& v_pos,
                           std::vector<glm::vec4>& v_col);
    void createBuffer();
    std::map<std::pair<Vertex*, Vertex*>, HalfEdge*> edgeMap;
    void meshSubDivision(std::unordered_map<Face*, Vertex*>& centers,
                         std::unordered_map<HalfEdge*, Vertex*>& mids,
                         std::vector<std::unique_ptr<Face>>& fs,
                         std::vector<std::unique_ptr<HalfEdge>>& hfs);

    HalfEdge* createHalfEdge(std::map<std::pair<Vertex*, Vertex*>, HalfEdge*>& edgeMap,
                   Vertex* begin, Vertex* end, Face* face);
};

#endif // MESH_H
