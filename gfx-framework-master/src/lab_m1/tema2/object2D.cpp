#include "object2D.h"

#include <vector>

#include "core/engine.h"
#include "utils/gl_utils.h"


#include <vector>
#include "core/engine.h"
#include "utils/gl_utils.h"

Mesh* object2D::CreateParallelepiped(const std::string& name, glm::vec3 center, float lengthX, float lengthY, float lengthZ, glm::vec3 color)
{
    glm::vec3 corner = center - glm::vec3(lengthX / 2, lengthY / 2, lengthZ / 2);

    std::vector<VertexFormat> vertices = {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(lengthX, 0, 0), color),
        VertexFormat(corner + glm::vec3(lengthX, lengthY, 0), color),
        VertexFormat(corner + glm::vec3(0, lengthY, 0), color),
        VertexFormat(corner + glm::vec3(0, 0, lengthZ), color),
        VertexFormat(corner + glm::vec3(lengthX, 0, lengthZ), color),
        VertexFormat(corner + glm::vec3(lengthX, lengthY, lengthZ), color),
        VertexFormat(corner + glm::vec3(0, lengthY, lengthZ), color)
    };

    std::vector<unsigned int> indices = {
        0, 1, 2, 0, 2, 3,
        4, 5, 6, 4, 6, 7,
        0, 1, 5, 0, 5, 4,
        1, 2, 6, 1, 6, 5,
        2, 3, 7, 2, 7, 6,
        3, 0, 4, 3, 4, 7 
    };

    Mesh* parallelepiped = new Mesh(name);
    parallelepiped->InitFromData(vertices, indices);
    return parallelepiped;
}

Mesh* object2D::CreateCube(const std::string& name, glm::vec3 center, float length, glm::vec3 color)
{
	return object2D::CreateParallelepiped(name, center, length, length, length, color);
}
Mesh* object2D::CreateCylinder(const std::string& name, glm::vec3 center, float radius, float height, glm::vec3 color) {
    const unsigned int slices = 32;
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    // Create top and bottom circle vertices
    glm::vec3 topCenter = center + glm::vec3(0, height / 2, 0);
    glm::vec3 bottomCenter = center - glm::vec3(0, height / 2, 0);

    vertices.push_back(VertexFormat(topCenter, color));
    vertices.push_back(VertexFormat(bottomCenter, color));

    for (unsigned int i = 0; i < slices; ++i) {
        float angle = 2.0f * M_PI * i / slices;
        glm::vec3 offset(radius * cos(angle), 0, radius * sin(angle));
        vertices.push_back(VertexFormat(topCenter + offset, color));
        vertices.push_back(VertexFormat(bottomCenter + offset, color));
    }

    // Add indices for the top and bottom circles
    for (unsigned int i = 0; i < slices; ++i) {
        unsigned int next = (i + 1) % slices;
        indices.push_back(0);
        indices.push_back(2 + 2 * i);
        indices.push_back(2 + 2 * next);

        indices.push_back(1);
        indices.push_back(3 + 2 * next);
        indices.push_back(3 + 2 * i);
    }

    // Add indices for the side faces
    for (unsigned int i = 0; i < slices; ++i) {
        unsigned int next = (i + 1) % slices;
        indices.push_back(2 + 2 * i);
        indices.push_back(3 + 2 * i);
        indices.push_back(2 + 2 * next);

        indices.push_back(2 + 2 * next);
        indices.push_back(3 + 2 * i);
        indices.push_back(3 + 2 * next);
    }

    Mesh* cylinder = new Mesh(name);
    cylinder->InitFromData(vertices, indices);
    return cylinder;
}

Mesh* object2D::CreateCone(const std::string& name, glm::vec3 center, float radius, float height, glm::vec3 color) {
    const unsigned int slices = 32;
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    // Create the top vertex and bottom center
    glm::vec3 topVertex = center + glm::vec3(0, height, 0);
    glm::vec3 bottomCenter = center;

    vertices.push_back(VertexFormat(topVertex, color));
    vertices.push_back(VertexFormat(bottomCenter, color));

    for (unsigned int i = 0; i < slices; ++i) {
        float angle = 2.0f * M_PI * i / slices;
        glm::vec3 offset(radius * cos(angle), 0, radius * sin(angle));
        vertices.push_back(VertexFormat(bottomCenter + offset, color));
    }

    // Add indices for the side faces
    for (unsigned int i = 0; i < slices; ++i) {
        unsigned int next = (i + 1) % slices;
        indices.push_back(0);
        indices.push_back(2 + i);
        indices.push_back(2 + next);
    }

    // Add indices for the bottom face
    for (unsigned int i = 1; i < slices; ++i) {
        indices.push_back(1);
        indices.push_back(2 + i);
        indices.push_back(2 + i - 1);
    }
    indices.push_back(1);
    indices.push_back(2);
    indices.push_back(2 + slices - 1);

    Mesh* cone = new Mesh(name);
    cone->InitFromData(vertices, indices);
    return cone;
}

Mesh* object2D::CreatePyramidRoof(const std::string& name, glm::vec3 position, float sideLength, float height, glm::vec3 color) {
    float halfSide = sideLength / 2.0f;

    glm::vec3 bottomLeft = position + glm::vec3(-halfSide, 0, -halfSide);
    glm::vec3 bottomRight = position + glm::vec3(halfSide, 0, -halfSide);
    glm::vec3 topLeft = position + glm::vec3(-halfSide, 0, halfSide);
    glm::vec3 topRight = position + glm::vec3(halfSide, 0, halfSide);

    glm::vec3 top = position + glm::vec3(0, height, 0);

    std::vector<VertexFormat> vertices = {
        VertexFormat(bottomLeft, color),
        VertexFormat(bottomRight, color),
        VertexFormat(top, color),

        VertexFormat(bottomRight, color),
        VertexFormat(topRight, color),
        VertexFormat(top, color),

        VertexFormat(topRight, color),
        VertexFormat(topLeft, color),
        VertexFormat(top, color),

        VertexFormat(topLeft, color),
        VertexFormat(bottomLeft, color),
        VertexFormat(top, color),

        VertexFormat(bottomLeft, color),
        VertexFormat(bottomRight, color),
        VertexFormat(topRight, color),

        VertexFormat(bottomLeft, color),
        VertexFormat(topRight, color),
        VertexFormat(topLeft, color)
    };

    Mesh* pyramidRoof = new Mesh(name);
    pyramidRoof->InitFromData(vertices, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
    return pyramidRoof;
}

Mesh* object2D::CreateCircleOnGround(const std::string& name, glm::vec3 center, float radius, glm::vec3 color) {
	const unsigned int slices = 32;
	std::vector<VertexFormat> vertices;
	std::vector<unsigned int> indices;

	// Create the center vertex
	glm::vec3 centerVertex = center;

	vertices.push_back(VertexFormat(centerVertex, color));

	for (unsigned int i = 0; i < slices; ++i) {
		float angle = 2.0f * M_PI * i / slices;
		glm::vec3 offset(radius * cos(angle), 0, radius * sin(angle));
		vertices.push_back(VertexFormat(center + offset, color));
	}

	// Add indices for the side faces
	for (unsigned int i = 0; i < slices; ++i) {
		unsigned int next = (i + 1) % slices;
		indices.push_back(0);
		indices.push_back(1 + i);
		indices.push_back(1 + next);
	}

	Mesh* circle = new Mesh(name);
	circle->InitFromData(vertices, indices);
	return circle;
}

