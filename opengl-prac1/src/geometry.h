#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <vector>
#include <string>
#include <glm/vec3.hpp>

struct FaceData
{
    int vertexIndex[3];
    int texCoordIndex[3];
    int normalIndex[3];
};

class GeometryData
{
public:
    void loadFromOBJFile(std::string filename);

    int vertexCount();

    void* vertexData();
    void* textureCoordData();
    void* normalData();
    void* tangentData();
    void* bitangentData();
    struct Object
    {
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;
    };

private:
    std::vector<float> vertices;
    std::vector<float> textureCoords;
    std::vector<float> normals;
    std::vector<float> tangents;
    std::vector<float> bitangents;

    std::vector<FaceData> faces;
};

#endif
