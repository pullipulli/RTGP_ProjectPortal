//
// Created by Andrea Pullia on 28/06/2026.
//

#include <Core/Graphics/Model/Mesh.h>

Mesh::Mesh(vector<Vertex>& vertices, vector<GLuint>& indices) noexcept
    : vertices(std::move(vertices)), indices(std::move(indices))
{
    this->setupMesh();
}

Mesh::Mesh(Mesh&& move) noexcept
    // Calls move for both vectors, which internally consists of a simple pointer swap between the new instance and the source one.
    : vertices(std::move(move.vertices)), indices(std::move(move.indices)), VAO(move.VAO), VBO(move.VBO), EBO(move.EBO)
{
    move.VAO = 0; // We *could* set VBO and EBO to 0 too,
    // but since we bring all the 3 values around we can use just one of them to check ownership of the 3 resources.
}

Mesh& Mesh::operator=(Mesh&& move) noexcept
{
    // calls the function which will delete (if needed) the GPU resources for this instance
    freeGPUresources();

    if (move.VAO) // source instance has GPU resources
    {
        this->vertices = std::move(move.vertices);
        this->indices = std::move(move.indices);
        this->VAO = move.VAO;
        this->VBO = move.VBO;
        this->EBO = move.EBO;

        move.VAO = 0;
    }
    else // source instance was already invalid
    {
        this->VAO = 0;
    }
    return *this;
}

Mesh::~Mesh() noexcept
{
    // calls the function which will delete (if needed) the GPU resources
    freeGPUresources();
}

void Mesh::Draw()
{
    // VAO is made "active"
    glBindVertexArray(this->VAO);
    // rendering of data in the VAO
    glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
    // VAO is "detached"
    glBindVertexArray(0);
}

void Mesh::setupMesh()
{
    // we create the buffers
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);
    glGenBuffers(1, &this->EBO);

    // VAO is made "active"
    glBindVertexArray(this->VAO);
    // we copy data in the VBO - we must set the data dimension, and the pointer to the structure cointaining the data
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);
    // we copy data in the EBO - we must set the data dimension, and the pointer to the structure cointaining the data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW);

    // we set in the VAO the pointers to the different vertex attributes (with the relative offsets inside the data structure)
    // vertex positions
    // these will be the positions to use in the layout qualifiers in the shaders ("layout (location = ...)"")
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
    // Normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));
    // Texture Coordinates
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));
    // Tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Tangent));
    // Bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Bitangent));

    // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO
    glBindVertexArray(0);
}

void Mesh::freeGPUresources()
{
    // If VAO is 0, this instance of Mesh has been through a move, and no longer owns GPU resources,
    // so there's no need for deleting.
    if (this->VAO)
    {
        glDeleteVertexArrays(1, &this->VAO);
        glDeleteBuffers(1, &this->VBO);
        glDeleteBuffers(1, &this->EBO);
    }
}