#pragma once

#include <string>

#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"


namespace object2D
{

    Mesh* CreateParallelepiped(const std::string& name, glm::vec3 center, float lengthX, float lengthY, float lengthZ, glm::vec3 color);
    Mesh* CreateCube(const std::string& name, glm::vec3 center, float length, glm::vec3 color);
    Mesh* CreateCylinder(const std::string& name, glm::vec3 center, float radius, float height, glm::vec3 color);
    Mesh* CreateCone(const std::string& name, glm::vec3 center, float radius, float height, glm::vec3 color);
    Mesh* CreatePyramidRoof(const std::string& name, glm::vec3 position, float sideLength, float height, glm::vec3 color);
    Mesh* CreateCircleOnGround(const std::string& name, glm::vec3 center, float radius, glm::vec3 color);

}
