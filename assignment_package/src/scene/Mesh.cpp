#include "Mesh.h"
#include <map>
#include <iostream>

Mesh::Mesh(OpenGLContext* context) : Drawable(context)
{}
Mesh::~Mesh()
{}

void Mesh::constructHalfEdge(std::vector<std::vector<int>>& verts, int& ind,
                             std::vector<glm::vec4>& v_pos,
                             std::vector<glm::vec4>& v_col) {
    int size = verts.size();
    // Reserve space for vectors
    m_vertPos.reserve(m_vertPos.size() + size);
    m_vertCol.reserve(m_vertCol.size() + size);
    halfEdges.reserve(halfEdges.size() + size);

    std::vector<std::pair<Vertex*, Vertex*>> edges;
    edges.reserve(size);

    auto face = std::make_unique<Face>();
    for (int i = 0; i < size; i++) {
        m_vertPos.push_back(v_pos[verts[i][0] - 1]);
        v_col.push_back(glm::vec4(face->color.r, face->color.g, face->color.b, 1.f));
        m_vertCol.push_back(v_col.back());

        auto he = std::make_unique<HalfEdge>();
        edges.push_back({vertices[verts[i][0] - 1].get(), vertices[(verts[(i + 1) % size][0] - 1)].get()});
        edgeMap[edges.back()] = he.get();
        halfEdges.push_back(std::move(he));

        // Set sym, vertex, face
        edgeMap[edges[i]]->setVertex(edges[i].second);
        edgeMap[edges[i]]->setFace(face.get());
        std::pair sym = {edges[i].second, edges[i].first};
        if (edgeMap.count(sym))
            edgeMap[edges[i]]->setSym(edgeMap[sym]);
    }

    // Link the edges using 'next'
    for (int i = 0; i < size - 1; i++) {
        edgeMap[edges[i]]->next = edgeMap[edges[i + 1]];
    }
    edgeMap[edges[size - 1]]->next = edgeMap[edges[0]];

    // Triangulate
    m_vertIdx.reserve(m_vertIdx.size() + 3 * (size - 2));
    for (int i = 1; i < size - 1; i++) {
        m_vertIdx.push_back(ind);
        m_vertIdx.push_back(ind + i);
        m_vertIdx.push_back(ind + i + 1);
        count += 3;
    }
    ind += size;

    faces.push_back(std::move(face));
}


void Mesh::create() {
    int index = 0;
    std::vector<glm::vec4> positions;
    std::vector<glm::vec4> colors;

    for (const auto &face : faces) {

        HalfEdge* startEdge = face.get()->halfEdge;
        HalfEdge* currEdge = startEdge;
        int count = 0;
        do {
            Vertex* v = currEdge->vertex;
            positions.push_back(glm::vec4(v->position, 1.0f));
            colors.push_back(glm::vec4(face.get()->color, 1.0f));
            currEdge = currEdge->next;
            count++;
        } while(currEdge != startEdge);

        for (int i = 0; i < count - 2; i++) {
            m_vertIdx.push_back(index);
            m_vertIdx.push_back(index + i + 1);
            m_vertIdx.push_back(index + i + 2);
        }
        index += count;
    }
    count = m_vertIdx.size();

    generateIdx();
    bindIdx();
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_vertIdx.size() * sizeof(GLuint), m_vertIdx.data(), GL_STATIC_DRAW);

    generatePos();
    bindPos();
    mp_context->glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec4), positions.data(), GL_STATIC_DRAW);

    generateCol();
    bindCol();
    mp_context->glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec4), colors.data(), GL_STATIC_DRAW);

    m_vertIdx.clear();
    m_vertPos.clear();
    m_vertCol.clear();
}
void Mesh::createBuffer() {
    count = m_vertIdx.size();

    generateIdx();
    bindIdx();
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(GLuint), m_vertIdx.data(), GL_STATIC_DRAW);

    generatePos();
    bindPos();
    mp_context->glBufferData(GL_ARRAY_BUFFER, count * sizeof(glm::vec4), m_vertPos.data(), GL_STATIC_DRAW);

    generateCol();
    bindCol();
    mp_context->glBufferData(GL_ARRAY_BUFFER, count * sizeof(glm::vec4), m_vertCol.data(), GL_STATIC_DRAW);

    m_vertIdx.clear();
    m_vertCol.clear();
    m_vertPos.clear();
}

void Mesh::meshSubDivision(std::unordered_map<Face*, Vertex*>& centers,
                           std::unordered_map<HalfEdge*, Vertex*>& mids,
                           std::vector<std::unique_ptr<Face>>& fs,
                           std::vector<std::unique_ptr<HalfEdge>>& hfs) {
    int vertexNums = 0;
    faces.back()->resetID();
    halfEdges.back()->resetID();
    // compute center point
    for (auto& f : faces) {
        auto v = std::make_unique<Vertex>();
        centers[f.get()] = v.get();
        glm::vec3 pos(f->halfEdge->vertex->position);
        HalfEdge* curr = f->halfEdge->next;
        int count = 1;
        while (curr != f->halfEdge) {
            pos += curr->vertex->position;
            curr = curr->next;
            count++;
        }
        pos /= count;
        v->position = pos;
        vertices.push_back(std::move(v));
        vertexNums++;
    }
    // compute mid point
    for (auto& f : faces) {
        HalfEdge* he = f->halfEdge;
        HalfEdge* curr = he->next;
        int size = 1;
        while (curr != he) {
            curr = curr->next;
            size++;
        }
        for (int i = 0; i < size; i++) {
            if (mids.count(curr)) {
                curr = curr->next;
                continue;
            }
            auto v = std::make_unique<Vertex>();
            Vertex* begin = curr->sym->vertex;
            Vertex* end = curr->vertex;
            glm::vec3 pos;
            if (curr->sym) {
                pos = begin->position + end->position + centers[f.get()]->position + centers[curr->sym->face]->position;
                pos /= 4;
            }
            else {
                pos = begin->position + end->position + centers[f.get()]->position;
                pos /= 3;
            }
            v->position = pos;
            mids[curr] = v.get();
            if (curr->sym) {
                mids[curr->sym] = v.get();
            }
            vertices.push_back(std::move(v));
            curr = curr->next;
            vertexNums++;
        }
    }
    // smooth
    for (unsigned int i = 0; i < vertices.size() - vertexNums; i++) {
        Vertex* v = vertices[i].get();
        HalfEdge* he = v->halfEdge;
        HalfEdge* curr = he->next->sym;
        float n = 1;
        glm::vec3 midSum(mids.at(he)->position);
        glm::vec3 centerSum(centers.at(he->face)->position);
        while (curr != he) {
            midSum += mids.at(curr)->position;
            centerSum += centers.at(curr->face)->position;
            curr = curr->next->sym;
            n++;
        }
        glm::vec3 pos;
        pos = (n - 2) * v->position / n
              + midSum / (float)pow(n, 2)
              + centerSum / (float)pow(n, 2);
        v->position = pos;
    }
    // quadrangulate
    for (const auto& f : faces) {
        HalfEdge* he = f->halfEdge;
        HalfEdge* curr = he->next;
        int n = 1;
        while (curr != he) {
            curr = curr->next;
            n++;
        }
        curr = he;
        for (int i = 0; i < n; i++) {
            auto newf = std::make_unique<Face>();

            HalfEdge* newhe1 = createHalfEdge(edgeMap, mids[curr], curr->vertex, newf.get());
            HalfEdge* newhe2 = createHalfEdge(edgeMap, curr->vertex, mids[curr->next], newf.get());
            HalfEdge* newhe3 = createHalfEdge(edgeMap, mids[curr->next], centers[f.get()], newf.get());
            HalfEdge* newhe4 = createHalfEdge(edgeMap, centers[f.get()], mids[curr], newf.get());

            newhe1->next = newhe2;
            newhe2->next = newhe3;
            newhe3->next = newhe4;
            newhe4->next = newhe1;

            hfs.emplace_back(newhe1);
            hfs.emplace_back(newhe2);
            hfs.emplace_back(newhe3);
            hfs.emplace_back(newhe4);
            fs.push_back(std::move(newf));
            curr = curr->next;
        }
    }
}
HalfEdge* Mesh::createHalfEdge(std::map<std::pair<Vertex*, Vertex*>, HalfEdge*>& edgeMap,
                               Vertex* begin, Vertex* end, Face* face) {
    auto newhe = std::make_unique<HalfEdge>();
    std::pair<Vertex*, Vertex*> key = {begin, end};
    std::pair<Vertex*, Vertex*> symKey = {end, begin};

    edgeMap[key] = newhe.get();
    if (edgeMap.count(symKey)) {
        edgeMap[symKey]->setSym(newhe.get());
    }

    newhe->setVertex(end);
    newhe->setFace(face);
    return newhe.release();
}

