#ifndef OBJ_H
#define OBJ_H

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <glm/glm.hpp>
#include "texture.h"
#include <algorithm>

struct Material {
    std::string name;
    glm::vec3 Ka;
    glm::vec3 Kd;
    glm::vec3 Ks;
    float Ns;
    GLuint map_Kd;
    float d;
    glm::vec3 Tf;
    float Ni;
    int illum;
};

class ObjModel {
public:
    std::string name;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;
    std::vector<unsigned int> indices;
    std::map<std::string, Material> materials; // store name and Material instance
    std::vector<Material> materialIndices;

    bool loadOBJ(const std::string& path) {
        name = path;
        std::ifstream file(path);
        // debug
        if (!file.is_open()) {
            std::cerr << "Failed to open OBJ file: " << path << std::endl;
            return false;
        }

        std::vector<glm::vec3> temp_vertices;
        std::vector<glm::vec3> temp_normals;
        std::vector<glm::vec2> temp_texCoords;
        std::vector<unsigned int> vertexIndices, normalIndices, texCoordIndices;

        std::string line;
        std::string currentMaterialName = "default";
        materials[currentMaterialName] = Material();
        // read file
        while (std::getline(file, line)) {
            std::istringstream ss(line); // input the content in line to ss
            std::string type;
            ss >> type;
            // read and push the vertex x/y/z
            if (type == "v") {
                glm::vec3 vertex;
                ss >> vertex.x >> vertex.y >> vertex.z;
                temp_vertices.push_back(vertex);
            }
            // read and push the normal x/y/z
            else if (type == "vn") {
                glm::vec3 normal;
                ss >> normal.x >> normal.y >> normal.z;
                temp_normals.push_back(normal);
            }
            // read and push the tex x/y/z
            else if (type == "vt") {
                glm::vec2 texCoord;
                ss >> texCoord.x >> texCoord.y;
                temp_texCoords.push_back(texCoord);
            }
            else if (type == "f") {
                std::string vertexData;
                std::vector<unsigned int> vIndices, tIndices, nIndices;
                while (ss >> vertexData) {
                    std::replace(vertexData.begin(), vertexData.end(), '/', ' ');
                    std::istringstream vss(vertexData);
                    unsigned int vIndex, tIndex, nIndex;
                    vss >> vIndex;
                    vIndices.push_back(vIndex);
                    if (vss.peek() == ' ') {
                        vss.ignore();
                        if (vss.peek() != ' ') {
                            vss >> tIndex;
                            tIndices.push_back(tIndex);
                        }
                        if (vss.peek() == ' ') {
                            vss.ignore();
                            vss >> nIndex;
                            nIndices.push_back(nIndex);
                        }
                    }
                }
                // iterate over vertex indices to form triangles from polygons
                for (size_t i = 1; i < vIndices.size() - 1; ++i) {
                    // add the first vertex of the face (vIndices[0]) and the two current vertices to form a triangle
                    vertexIndices.push_back(vIndices[0]);
                    vertexIndices.push_back(vIndices[i]);
                    vertexIndices.push_back(vIndices[i + 1]);
                    // texture
                    if (!tIndices.empty()) {
                        texCoordIndices.push_back(tIndices[0]);
                        texCoordIndices.push_back(tIndices[i]);
                        texCoordIndices.push_back(tIndices[i + 1]);
                    }
                    // normal
                    if (!nIndices.empty()) {
                        normalIndices.push_back(nIndices[0]);
                        normalIndices.push_back(nIndices[i]);
                        normalIndices.push_back(nIndices[i + 1]);
                    }
                }
                // add the current material index for this face
                materialIndices.push_back(materials[currentMaterialName]);
            }
            else if (type == "mtllib") {
                std::string mtlFilename;
                ss >> mtlFilename;
                if (!loadMTL(mtlFilename, path)) {
                    std::cerr << "Fail to load: " << path << std::endl;
                }
            }
            else if (type == "usemtl") {
                ss >> currentMaterialName;
                if (materials.find(currentMaterialName) == materials.end()) {
                    materials[currentMaterialName] = Material();
                }
            }
        }
        file.close();

        // convert vertex indices to actual vertex data and store in 'vertices' vector
        for (unsigned int i = 0; i < vertexIndices.size(); i++) {
            unsigned int vertexIndex = vertexIndices[i];
            glm::vec3 vertex = temp_vertices[vertexIndex - 1];
            vertices.push_back(vertex);
        }
        // noramal
        for (unsigned int i = 0; i < normalIndices.size(); i++) {
            unsigned int normalIndex = normalIndices[i];
            glm::vec3 normal = temp_normals[normalIndex - 1];
            normals.push_back(normal);
        }
        // texture
        for (unsigned int i = 0; i < texCoordIndices.size(); i++) {
            unsigned int texCoordIndex = texCoordIndices[i];
            glm::vec2 texCoord = temp_texCoords[texCoordIndex - 1];
            texCoords.push_back(texCoord);
        }
        for (unsigned int i = 0; i < vertices.size(); ++i) {
            indices.push_back(i);
        }
        // debug
        std::cout << "Loaded OBJ file: " << path << " with " << vertices.size() << " vertices." << std::endl;
        return true;
    }

    bool loadMTL(const std::string& mtlFilename, const std::string& objFilename) {
        // get the directory of the obj
        std::string directory = objFilename.substr(0, objFilename.find_last_of("/\\") + 1);
        // mtl file is the same directory as obj
        std::string fullPath = directory + mtlFilename;

        std::cout << "Successfully open MTL file: " << fullPath << std::endl;

        std::ifstream mtlFile(fullPath);
        if (!mtlFile.is_open()) {
            std::cerr << "Failed to open MTL file: " << fullPath << std::endl;
            return false;
        }

        std::string line;
        Material currentMaterial;
        while (std::getline(mtlFile, line)) {
            std::istringstream ss(line);
            std::string type;
            ss >> type;
            // store the new material
            if (type == "newmtl") {
                if (!currentMaterial.name.empty()) {
                    materials[currentMaterial.name] = currentMaterial;
                }
                ss >> currentMaterial.name;
                currentMaterial.map_Kd = 0;
                std::cout << "Read new material: " << currentMaterial.name << std::endl; // 输出读取的newmtl的名字
            }
            else if (type == "Ka") {
                ss >> currentMaterial.Ka.r >> currentMaterial.Ka.g >> currentMaterial.Ka.b;
            }
            else if (type == "Kd") {
                ss >> currentMaterial.Kd.r >> currentMaterial.Kd.g >> currentMaterial.Kd.b;
            }
            else if (type == "Ks") {
                ss >> currentMaterial.Ks.r >> currentMaterial.Ks.g >> currentMaterial.Ks.b;
            }
            else if (type == "Ns") {
                ss >> currentMaterial.Ns;
            }
            else if (type == "d") {
                ss >> currentMaterial.d;
            }
            else if (type == "Tf") {
                ss >> currentMaterial.Tf.r >> currentMaterial.Tf.g >> currentMaterial.Tf.b;
            }
            else if (type == "Ni") {
                ss >> currentMaterial.Ni;
            }
            else if (type == "illum") {
                ss >> currentMaterial.illum;
            }
            else if (type == "map_Kd") {
                std::string textureFilename;
                ss >> textureFilename;
                std::string texturePath = directory + textureFilename;
                std::cout << "Loading texture: " << texturePath << std::endl;
                // pass the texture id
                currentMaterial.map_Kd = setup_texture(texturePath.c_str());
                if (currentMaterial.map_Kd == 0) {
                    std::cerr << "Failed to load texture: " << texturePath << std::endl;
                }
            }
        }
        if (!currentMaterial.name.empty()) {
            materials[currentMaterial.name] = currentMaterial;
        }

        mtlFile.close();
        return true;
    }
};

#endif // OBJ_H
