#ifndef MODELCONTAINER_H
#define MODELCONTAINER_H

#include "obj.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <GL/gl3w.h>

class ModelContainer {
public:
    ModelContainer(const std::string& path);

    void setPosition(const glm::vec3& position);
    void setRotation(float angle, const glm::vec3& axis);
    void setScale(const glm::vec3& scale);
    void updateTransform();
    void draw(GLuint program);
    glm::mat4 modelTransform;


private:
    ObjModel model;
    GLuint modelVAO, modelVBO, texCoordBuffer, normalBuffer;
    glm::vec3 position;
    glm::vec3 scale;
    glm::mat4 rotation;
};

#endif 
