//
// Created by Andrea Pullia on 28/06/2026.
//

#include "Core/Graphics/Model/Model.h"

Model::Model(const string& path)
{
    this->loadModel(path);
    this->modelId = path;
}

void Model::Draw()
{
    for(GLuint i = 0; i < this->meshes.size(); i++)
        this->meshes[i].Draw();
}

const std::string& Model::GetModelId() const
{
    return modelId;
}

void Model::loadModel(string path)
{
    // loading using Assimp
    // N.B.: it is possible to set, if needed, some operations to be performed by Assimp after the loading.
    // Details on the different flags to use are available at: http://assimp.sourceforge.net/lib_html/postprocess_8h.html#a64795260b95f5a4b3f3dc1be4f52e410
    // VERY IMPORTANT: calculation of Tangents and Bitangents is possible only if the model has Texture Coordinates
    // If they are not present, the calculation is skipped (but no error is provided in the following checks!)
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);

    // check for errors (see comment above)
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
        return;
    }

    // we start the recursive processing of nodes in the Assimp data structure
    this->processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
    GLuint i;

    // we process each mesh inside the current node
    for(i = 0; i < node->mNumMeshes; i++)
    {
        // the "node" object contains only the indices to objects in the scene
        // "Scene" contains all the data. Class node is used only to point to one or more mesh inside the scene and to maintain informations on relations between nodes
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        // we start processing of the Assimp mesh using processMesh method.
        // the result (an istance of the Mesh class) is added to the vector
        // we use emplace_back instead as push_back, so to have the instance created directly in the
        // vector memory, without the creation of a temp copy.
        // https://en.cppreference.com/w/cpp/container/vector/emplace_back
        this->meshes.emplace_back(processMesh(mesh));
    }
    // we then recursively process each of the children nodes
    for(i = 0; i < node->mNumChildren; i++)
    {
        this->processNode(node->mChildren[i], scene);
    }

}

Mesh Model::processMesh(aiMesh* mesh)
{
    // data structures for vertices and indices of vertices (for faces)
    vector<Vertex> vertices;
    vector<GLuint> indices;

    glm::vec3 vector;
    glm::vec2 vec;

    Vertex vertex;

    GLuint i,j;

    for(i = 0; i < mesh->mNumVertices; i++)
    {

        // the vector data type used by Assimp is different than the GLM vector needed to allocate the OpenGL buffers
        // I need to convert the data structures (from Assimp to GLM, which are fully compatible to the OpenGL)

        // vertices coordinates
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;
        // Normals
        vector.x = mesh->mNormals[i].x;
        vector.y = mesh->mNormals[i].y;
        vector.z = mesh->mNormals[i].z;
        vertex.Normal = vector;
        // Texture Coordinates
        // if the model has texture coordinates, than we assign them to a GLM data structure, otherwise we set them at 0
        // if texture coordinates are present, than Assimp can calculate tangents and bitangents, otherwise we set them at 0 too
        if(mesh->mTextureCoords[0])
        {
            // in this example we assume the model has only one set of texture coordinates. Actually, a vertex can have up to 8 different texture coordinates. For other models and formats, this code needs to be adapted and modified.
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;

            // Tangents
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.Tangent = vector;
            // Bitangents
            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.Bitangent = vector;
        }
        else{
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            cout << "WARNING::ASSIMP:: MODEL WITHOUT UV COORDINATES -> TANGENT AND BITANGENT ARE = 0" << endl;
        }
        // we add the vertex to the list
        vertices.emplace_back(vertex);
    }

    // for each face of the mesh, we retrieve the indices of its vertices , and we store them in a vector data structure
    for(i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace* face = &mesh->mFaces[i];
         for(j = 0; j < face->mNumIndices; j++)
             indices.emplace_back(face->mIndices[j]);

    }

    // we return an instance of the Mesh class created using the vertices and faces data structures we have created above.
    return Mesh(vertices, indices);
}