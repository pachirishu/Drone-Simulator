#include "lab_m1/tema2/tema2.h"
#include "lab_m1/tema2/object2D.h"

#include <vector>
#include <string>
#include <iostream>

#include "core/engine.h"
#include "utils/gl_utils.h"

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema2::Tema2()
{
}


Tema2::~Tema2()
{
}


void Tema2::Init()
{
    renderCameraTarget = true;

    // setam camera
    camera = new implemented::CameraTema();
    camera->Set(glm::vec3(0, 5, 10), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));
    camera->distanceToTarget = 10.0f; // Ajustează în funcție de preferințe
    
	//rezolutia pentru minimap
	glm::ivec2 resolution = window->GetResolution();
	minimapViewportArea = ViewportArea(50, 50, resolution.x / 5.f, resolution.y / 5.f);
    
    // baza dronei
    {
        Mesh* body1 = object2D::CreateParallelepiped("body1", glm::vec3(0, 0, 0), length, width, width, glm::vec3(0.4f, 0.4f, 0.4f));
        //meshes[body1->GetMeshID()] = body1;
        AddMeshToList(body1);
    }
    {
        Mesh* body2 = object2D::CreateParallelepiped("body2", glm::vec3(0, 0, 0), width, width, length, glm::vec3(0.4f, 0.4f, 0.4f));
        //meshes[body2->GetMeshID()] = body2;
        AddMeshToList(body2);
    }
    // elicii
    {
        Mesh* rotor1 = object2D::CreateParallelepiped("rotor1", glm::vec3(0, 0, 0), 0.1f, 0.05f, 0.8f, glm::vec3(0.1f, 0.1f, 0.1f));
        //meshes[rotor1->GetMeshID()] = rotor1;
        AddMeshToList(rotor1);
    }
    // baza eliciilor
    {
        Mesh* cube = object2D::CreateCube("cube", glm::vec3(0, 0, 0), width, glm::vec3(0.45f, 0.45f, 0.45f));
        AddMeshToList(cube);
    }

    // generarea terenului
    GenerateTerrainMesh(220, 220);

    // shader pentru teren
    {
        Shader* shader = new Shader("TemaShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;

    }

	// shader pentru obiecte
    {
        Shader* shader = new Shader("InterpolatedShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "InterpolatedVertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "InterpolatedFragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;

    }

	// obiecte 3D
    {
		Mesh* trunkMesh = object2D::CreateCylinder("trunk", glm::vec3(0, 0, 0), 0.2f, 1.0f, glm::vec3(0.55f, 0.27f, 0.07f));
		meshes[trunkMesh->GetMeshID()] = trunkMesh;
    }
    {
		Mesh* leaves1 = object2D::CreateCone("leaves1", glm::vec3(0, 0.5f, 0), 0.8f, 1.5f, glm::vec3(0.0f, 0.8f, 0.0f));
		meshes[leaves1->GetMeshID()] = leaves1;
	}
    {
		Mesh* leaves2 = object2D::CreateCone("leaves2", glm::vec3(0, 1.5f, 0), 0.6f, 1.0f, glm::vec3(0.0f, 0.8f, 0.0f));
		meshes[leaves2->GetMeshID()] = leaves2;
    }
    {
		Mesh* buildingBaseMesh = object2D::CreateParallelepiped("buildingBase", glm::vec3(0, 0, 0), 2.0f, 3.0f, 2.0f, glm::vec3(0.6f, 0.6f, 0.6f));
		meshes[buildingBaseMesh->GetMeshID()] = buildingBaseMesh;
	}
    {
        Mesh* buildingRoofMesh = object2D::CreatePyramidRoof("buildingRoof", glm::vec3(0, 1.5f, 0), 2.2f, 1.0f, glm::vec3(0.8f, 0.0f, 0.0f));
		meshes[buildingRoofMesh->GetMeshID()] = buildingRoofMesh;
    }

    // pachetul, zona de ridicare si zona de livrat
    {
		Mesh* package = object2D::CreateParallelepiped("package", glm::vec3(0, 0.25f, 0), 1.2f, 0.5f, 1.2f, glm::vec3(0.0f, 0.0f, 1.0f));
        meshes[package->GetMeshID()] = package;
    }
    {
		Mesh* packageZone = object2D::CreateCylinder("packageZone", glm::vec3(0, 1.0f, 0), 2.0f, 2.0f, glm::vec3(0.5f, 0.5f, 0.5f));
		meshes[packageZone->GetMeshID()] = packageZone;
    }
    {
		Mesh* delivery = object2D::CreateCircleOnGround("delivery", glm::vec3(0, 0, 0), 2.2f, glm::vec3(0.0f, 1.0f, 0.0f));
		meshes[delivery->GetMeshID()] = delivery;
    }

	// sageata pentru a indica directia dronei
    {

        Mesh* mesh = new Mesh("arrow");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "arrow"), "Arrow5.obj");
        meshes[mesh->GetMeshID()] = mesh;

    }

	// matricea de proiectie
    projectionMatrix = glm::perspective(fov, window->props.aspectRatio, perspectiveNear, perspectiveFar);

    // generarea obstacolelor
    int numObstacles = 100 + rand() % 100;
    for (int i = 0; i < numObstacles; ++i) {
        glm::vec3 pos;
        bool overlap;

        do {
            pos = glm::vec3(-100 + rand() % 201, 0, -100 + rand() % 201);
            overlap = std::any_of(obstacles.begin(), obstacles.end(), [&](Obstacle o) {
                return glm::distance(o.position, pos) < 10.0f;
                });
			overlap = overlap || glm::distance(pos, glm::vec3(xPosition, yPosition, zPosition)) < 10.0f;
        } while (overlap);

        // scalare aleatorie
		float scale = 1.0f + (float)(rand() % 100) / 50;

        obstacles.push_back({ pos, rand() % 2 == 0, scale });

    }

	// generare pozitia pachetului
	do {
		packageX = -100 + rand() % 201;
		packageZ = -100 + rand() % 201;
		packageY = 2.0f;
    } while (glm::distance(glm::vec3(packageX, 0, packageZ), glm::vec3(xPosition, 0, zPosition)) < 10.0f ||
        std::any_of(obstacles.begin(), obstacles.end(), [&](Obstacle o) {
            return glm::distance(o.position, glm::vec3(packageX, 0, packageZ)) < 10.0f;
            }));

    // generare mesh-uri pentru minimap
	CreateMinimapMeshes();

}

// functie pentru a face terenul
void Tema2::GenerateTerrainMesh(int width, int depth) {
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

	// generare vertecsi
    for (int z = 0; z < depth; z++) {
        for (int x = 0; x < width; x++) {
            float xPos = (float)x - (width / 2.0f);
            float zPos = (float)z - (depth / 2.0f);

            // coordonate pentru noise
            float xCoord = xPos * 0.1f;
            float zCoord = zPos * 0.1f;

            float yPos = 0.0f;

            glm::vec3 position(xPos, yPos, zPos);
            glm::vec3 color(0.039f, 0.788f, 0.231f);
            glm::vec3 normal(0, 1, 0);
            glm::vec2 texCoord((float)x / width, (float)z / depth);

            vertices.push_back(VertexFormat(position, color, normal, texCoord));
        }
    }

	// generare indecsi
    for (int z = 0; z < depth - 1; z++) {
        for (int x = 0; x < width - 1; x++) {
            unsigned int topLeft = z * width + x;
            unsigned int topRight = topLeft + 1;
            unsigned int bottomLeft = (z + 1) * width + x;
            unsigned int bottomRight = bottomLeft + 1;

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);
            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    Mesh* terrain = new Mesh("terrain");
    terrain->InitFromData(vertices, indices);
    meshes[terrain->GetMeshID()] = terrain;
}

// frame start
void Tema2::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0.122f, 0.577f, 0.788f, 0.5f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

// functie pentru a crea mesh-uri pentru minimap
void Tema2::CreateMinimapMeshes() {
    // patrat rosu pentru case
    {
        vector<VertexFormat> vertices = {
            VertexFormat(glm::vec3(-0.5f, 0, -0.5f), glm::vec3(1, 0, 0)),
            VertexFormat(glm::vec3(0.5f, 0, -0.5f), glm::vec3(1, 0, 0)),
            VertexFormat(glm::vec3(-0.5f, 0, 0.5f), glm::vec3(1, 0, 0)),
            VertexFormat(glm::vec3(0.5f, 0, 0.5f), glm::vec3(1, 0, 0))
        };

        vector<unsigned int> indices = {
            0, 1, 2,    // first triangle
            1, 3, 2     // second triangle
        };

        meshes["minimap_square"] = new Mesh("minimap_square");
        meshes["minimap_square"]->InitFromData(vertices, indices);
        meshes["minimap_square"]->SetDrawMode(GL_TRIANGLES);
    }
    // cerc verde pentru copaci
    {
        vector<VertexFormat> vertices;
        vector<unsigned int> indices;

        vertices.push_back(VertexFormat(glm::vec3(0, 0, 0)));

        const int numSegments = 12;
        for (int i = 0; i < numSegments; i++) {
            float angle = 2.0f * M_PI * i / numSegments;
            float x = cos(angle) * 0.5f;
            float z = sin(angle) * 0.5f;
            vertices.push_back(VertexFormat(glm::vec3(x, 0, z), glm::vec3(0, 1, 0)));
        }

        for (int i = 0; i < numSegments; i++) {
            indices.push_back(0);
            indices.push_back(1 + i);
            indices.push_back(1 + ((i + 1) % numSegments));
        }

        meshes["minimap_circle"] = new Mesh("minimap_circle");
        meshes["minimap_circle"]->InitFromData(vertices, indices);
        meshes["minimap_circle"]->SetDrawMode(GL_TRIANGLES);
    }
    // sageata pentru drona
    {
        vector<VertexFormat> vertices = {
            VertexFormat(glm::vec3(0, 0, 2.0f), glm::vec3(0.5f, 0.5f, 0.5f)),
            VertexFormat(glm::vec3(-1.0f, 0, -1.0f), glm::vec3(0.5f, 0.5f, 0.5f)),
            VertexFormat(glm::vec3(1.0f, 0, -1.0f), glm::vec3(0.5f, 0.5f, 0.5f)),
            VertexFormat(glm::vec3(-0.5f, 0, -1.0f), glm::vec3(0.5f, 0.5f, 0.5f)),
            VertexFormat(glm::vec3(0.5f, 0, -1.0f), glm::vec3(0.5f, 0.5f, 0.5f)),
            VertexFormat(glm::vec3(-0.5f, 0, -2.0f), glm::vec3(0.5f, 0.5f, 0.5f)),
            VertexFormat(glm::vec3(0.5f, 0, -2.0f), glm::vec3(0.5f, 0.5f, 0.5f))
        };

        vector<unsigned int> indices = {
            0, 1, 2,
            3, 4, 5,
            4, 6, 5
        };

        meshes["minimap_arrow"] = new Mesh("minimap_arrow");
        meshes["minimap_arrow"]->InitFromData(vertices, indices);
    }
    // pachetul albastru
    {
        vector<VertexFormat> vertices = {
            VertexFormat(glm::vec3(-0.5f, 0, -0.5f), glm::vec3(0, 0, 1)),
            VertexFormat(glm::vec3(0.5f, 0, -0.5f), glm::vec3(0, 0, 1)),
            VertexFormat(glm::vec3(-0.5f, 0, 0.5f), glm::vec3(0, 0, 1)),
            VertexFormat(glm::vec3(0.5f, 0, 0.5f), glm::vec3(0, 0, 1))
        };

        vector<unsigned int> indices = {
            0, 1, 2,
            1, 3, 2
        };

        meshes["minimap_package"] = new Mesh("minimap_package");
        meshes["minimap_package"]->InitFromData(vertices, indices);
    }
	// zona de livrare
	{
		vector<VertexFormat> vertices;
		vector<unsigned int> indices;

		vertices.push_back(VertexFormat(glm::vec3(0, 0, 0), glm::vec3(1, 1, 0)));

		const int numSegments = 12;
		for (int i = 0; i < numSegments; i++) {
			float angle = 2.0f * M_PI * i / numSegments;
			float x = cos(angle) * 0.5f;
			float z = sin(angle) * 0.5f;
			vertices.push_back(VertexFormat(glm::vec3(x, 0, z), glm::vec3(1, 1, 0)));
		}

		for (int i = 0; i < numSegments; i++) {
			indices.push_back(0);
			indices.push_back(1 + i);
			indices.push_back(1 + ((i + 1) % numSegments));
		}

		meshes["minimap_delivery"] = new Mesh("minimap_delivery");
		meshes["minimap_delivery"]->InitFromData(vertices, indices);
	}
}

// verificare daca un punct este in interiorul unui AABB
bool Tema2::isPointInsideAABB(const glm::vec3& point, const glm::vec3& boxMin, const glm::vec3& boxMax) {
    return (point.x >= boxMin.x && point.x <= boxMax.x &&
        point.y >= boxMin.y && point.y <= boxMax.y &&
        point.z >= boxMin.z && point.z <= boxMax.z);
}

// verificare coliziune cu sfera
bool Tema2::checkCollisionWithSphere(glm::vec3 dronePosition, glm::vec3 sphereCenter, float radius, glm::vec3 scale) {
    float effectiveRadius = radius * glm::length(scale);
    float distance = glm::distance(dronePosition, sphereCenter);
    return distance <= effectiveRadius;
}

// verificare coliziune cu cilindru
bool Tema2::checkCollisionWithCylinder(glm::vec3 dronePosition, glm::vec3 cylinderBase, float radius, float height, glm::vec3 scale) {
    float effectiveRadius = radius * glm::length(glm::vec2(scale.x, scale.z));
    float effectiveHeight = height * scale.y;
    float distanceXZ = glm::length(glm::vec2(dronePosition.x - cylinderBase.x, dronePosition.z - cylinderBase.z));
    return (distanceXZ <= effectiveRadius && dronePosition.y >= cylinderBase.y && dronePosition.y <= (cylinderBase.y + effectiveHeight));
}

// verificare coliziune cu con
bool Tema2::checkCollisionWithCone(glm::vec3 dronePosition, glm::vec3 coneBase, float radius, float height, glm::vec3 scale) {
	float effectiveRadius = radius * glm::length(glm::vec2(scale.x, scale.z));
	float effectiveHeight = height * scale.y;
	float distanceXZ = glm::length(glm::vec2(dronePosition.x - coneBase.x, dronePosition.z - coneBase.z));
	float distanceY = dronePosition.y - coneBase.y;
	return (distanceXZ <= effectiveRadius * (1 - distanceY / effectiveHeight) && dronePosition.y >= coneBase.y && dronePosition.y <= (coneBase.y + effectiveHeight));
}

// verificare coliziune cu piramida
bool Tema2::checkCollisionWithPyramid(glm::vec3 dronePosition, glm::vec3 pyramidBase, float sideLength, float height, glm::vec3 scale) {
	float effectiveSideLength = sideLength * glm::length(glm::vec2(scale.x, scale.z));
	float effectiveHeight = height * scale.y;
	float distanceXZ = glm::length(glm::vec2(dronePosition.x - pyramidBase.x, dronePosition.z - pyramidBase.z));
	float distanceY = dronePosition.y - pyramidBase.y;
	return (distanceXZ <= effectiveSideLength / 2 * (1 - distanceY / effectiveHeight) && dronePosition.y >= pyramidBase.y && dronePosition.y <= (pyramidBase.y + effectiveHeight));
}

// verificare coliziune cu cladire
bool Tema2::checkCollisionWithBuilding(const glm::vec3& point, const glm::vec3& buildingPosition, const float scale) {
    glm::vec3 boxMin = buildingPosition - glm::vec3(1.5f * scale, 0.0f, 1.5f * scale);
    glm::vec3 boxMax = buildingPosition + glm::vec3(1.5f * scale, 1.5f * scale, 1.5f * scale); 
	return isPointInsideAABB(point, boxMin, boxMax);
}

// verificare coliziune cu pachetul
bool Tema2::checkCollisionWithBox(const glm::vec3& point, const glm::vec3& boxPosition) {
	glm::vec3 boxMin = boxPosition - glm::vec3(0.7f, 0.3f, 0.7f);
	glm::vec3 boxMax = boxPosition + glm::vec3(0.7f, 0.3f, 0.7f);
	return isPointInsideAABB(point, boxMin, boxMax);
}

// verificare coliziune cu solul
bool Tema2::checkCollisionWithGround(const glm::vec3& position) {
    return position.y <= 0.0f;
}

// verificare coliziune cu un obstacol
bool Tema2::checkCollisionWithObstacle(glm::vec3 point) {
	for (const auto& obstacle : obstacles) {
		if (obstacle.isTree) {
			if (checkCollisionWithCylinder(point, obstacle.position, 0.2f, 1.0f, glm::vec3(obstacle.scale))) {
				return true;
			}
			if (checkCollisionWithCone(point, obstacle.position + glm::vec3(0, 1.0f, 0), 1.0f, 2.0f, glm::vec3(obstacle.scale))) {
				return true;
			}
		}
		else {
			if (checkCollisionWithBuilding(point, obstacle.position, obstacle.scale)) {
				return true;
			}
			if (checkCollisionWithPyramid(point, obstacle.position + glm::vec3(0, 3.0f, 0), 2.0f, 1.5f, glm::vec3(obstacle.scale))) {
				return true;
			}
		}
	}
	return false;
}

// functie de update a jocului
void Tema2::UpdateMap(float deltaTimeSeconds)
{

    glm::vec3 dronePosition = glm::vec3(xPosition, yPosition, zPosition);
    glm::vec3 correction = glm::vec3(0.0f);

    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);

    // desenarea terenului
    {
        glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0, -0.1f, 0));
        RenderSimpleMesh(meshes["terrain"], shaders["TemaShader"], modelMatrix);
    }

    // desenarea obstacolelor
    {
        for (const auto& obstacle : obstacles) {
            if (obstacle.isTree) {
                float scale = obstacle.scale;
				glm::mat4 modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, obstacle.position);
				modelMatrix = glm::scale(modelMatrix, glm::vec3(scale));
				RenderInterpolatedMesh(meshes["trunk"], shaders["InterpolatedShader"], modelMatrix, glm::vec3(0.55f, 0.27f, 0.07f), yPosition, 80.0f);
				RenderInterpolatedMesh(meshes["leaves1"], shaders["InterpolatedShader"], modelMatrix, glm::vec3(0.0f, 0.8f, 0.0f), yPosition, 80.0f);
				RenderInterpolatedMesh(meshes["leaves2"], shaders["InterpolatedShader"], modelMatrix, glm::vec3(0.0f, 0.8f, 0.0f), yPosition, 80.0f);
            }
            else {
				glm::mat4 modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, obstacle.position);
				modelMatrix = glm::scale(modelMatrix, glm::vec3(obstacle.scale));
				RenderInterpolatedMesh(meshes["buildingBase"], shaders["InterpolatedShader"], modelMatrix, glm::vec3(0.6f, 0.6f, 0.6f), yPosition, 80.0f);
				RenderInterpolatedMesh(meshes["buildingRoof"], shaders["InterpolatedShader"], modelMatrix, glm::vec3(0.8f, 0.0f, 0.0f), yPosition, 80.0f);
            }
        }
    }
    // verificarea coliziunii cu obstacolele
    {
        for (const auto& obstacle : obstacles) {
            bool collided = false;
            glm::vec3 correction(0.0f);
            if (obstacle.isTree) {
                if (checkCollisionWithCylinder(glm::vec3(xPosition, yPosition, zPosition), obstacle.position, 0.2f, 1.0f, glm::vec3(obstacle.scale))) {
                    collided = true;
                    glm::vec3 displacement = glm::vec3(xPosition, 0.0f, zPosition) - glm::vec3(obstacle.position.x, 0.0f, obstacle.position.z);
                    glm::vec3 direction = glm::vec3(xPosition - obstacle.position.x, 0.0f, zPosition - obstacle.position.z);
                    glm::vec3 normal = glm::length(direction) > 0.0f ? glm::normalize(direction) : glm::vec3(1.0f, 0.0f, 0.0f);
                    float penetrationDepth = (0.2f * obstacle.scale) - glm::length(displacement);
					if (penetrationDepth < 0.0f) {
						penetrationDepth = 0.0f;
					}
                    correction += normal * penetrationDepth;
                }
                if (checkCollisionWithCone(glm::vec3(xPosition, yPosition, zPosition), obstacle.position + glm::vec3(0, 1.0f, 0), 1.0f, 2.0f, glm::vec3(obstacle.scale))) {
                    collided = true;
                    glm::vec3 normal = glm::normalize(glm::vec3(xPosition - obstacle.position.x, yPosition - (obstacle.position.y + 1.0f), zPosition - obstacle.position.z));
                    correction += normal * 0.1f;
                }
            }
            else {
                if (checkCollisionWithBuilding(glm::vec3(xPosition, yPosition, zPosition), obstacle.position, obstacle.scale)) {
                    collided = true;
                    glm::vec3 displacement = glm::vec3(xPosition, 0.0f, zPosition) - glm::vec3(obstacle.position.x, 0.0f, obstacle.position.z);
                    glm::vec3 direction = glm::vec3(xPosition - obstacle.position.x, 0.0f, zPosition - obstacle.position.z);
                    glm::vec3 normal = glm::length(direction) > 0.0f ? glm::normalize(direction) : glm::vec3(1.0f, 0.0f, 0.0f);
                    float penetrationDepth = obstacle.scale - glm::length(displacement);
					if (penetrationDepth < 0.0f) {
						penetrationDepth = 0.0f;
					}
                    correction += glm::vec3(normal.x * penetrationDepth, 0.0f, normal.z * penetrationDepth);
                }
                if (checkCollisionWithPyramid(glm::vec3(xPosition, yPosition, zPosition), obstacle.position + glm::vec3(0, 3.0f, 0), 2.0f, 1.5f, glm::vec3(obstacle.scale))) {
                    collided = true;
                    glm::vec3 normal = glm::normalize(glm::vec3(xPosition - obstacle.position.x, yPosition - (obstacle.position.y + 3.0f), zPosition - obstacle.position.z));
                    correction += normal * 0.1f;
                }
            }
            if (collided) {
                // aplicăm corectia la pozitia dronei
                xPosition += correction.x;
                yPosition += correction.y;
                zPosition += correction.z;

                // corectare thrust
                if (thrust > 0.0f) {
                    glm::vec3 thrustDirection = glm::normalize(glm::vec3(sin(rotatieY), thrust > 3.0f ? 1.0f : 0.0f, cos(rotatieY)));
					// calculam deplasarea cauzata de thrust
                    glm::vec3 thrustDisplacement = thrustDirection * thrust * deltaTimeSeconds;
                    if (checkCollisionWithObstacle(glm::vec3(xPosition + thrustDisplacement.x, yPosition + thrustDisplacement.y, zPosition + thrustDisplacement.z))) {
						// ignoram thrust-ul daca ar duce la coliziune
                        thrustDisplacement = glm::vec3(0.0f);
                    }
					// aplicam deplasarea
                    xPosition += thrustDisplacement.x;
                    yPosition += thrustDisplacement.y;
                    zPosition += thrustDisplacement.z;
                }
            }

			// verificare coliziune cu pachetul
            if (collided && packageAttached) {
                cout << "Package dropped due to collision!" << endl;
                packageAttached = false;
                deliveryZoneActive = false;

                // revenim pachetul la pozitia initiala
                packageX = packageInitialX;
                packageY = packageInitialY;
                packageZ = packageInitialZ;
            }

        }
    }

    // pachetul
    {
		// daca pachetul este atasat de drona
        if (packageAttached) {
            packageX = xPosition;
            packageY = yPosition - 0.75f;
            packageZ = zPosition;

            // verificam coliziunea cu solul
            if (checkCollisionWithGround(glm::vec3(packageX, packageY, packageZ))) {
                cout << "Package hit the ground!" << endl;
                packageAttached = false;
                deliveryZoneActive = false;

				// readucem pachetul la pozitia initiala
                packageX = packageInitialX;
                packageY = packageInitialY;
                packageZ = packageInitialZ;
            }
        }

		// desenam pachetul
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(packageX, packageY, packageZ));
		RenderMesh(meshes["package"], shaders["VertexColor"], modelMatrix);

		// desenam zona de ridicare a pachetului
        if (!packageAttached) {
			glm::mat4 modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(packageX, 0.0f, packageZ));
			RenderMesh(meshes["packageZone"], shaders["VertexColor"], modelMatrix);

        }
    }

	// verificare coliziune cu pachetul
    {
        if (checkCollisionWithBox(glm::vec3(xPosition, yPosition, zPosition), glm::vec3(packageX, packageY, packageZ))) {
            if (!packageAttached && !deliveryZoneActive) {
                packageAttached = true;

				// salvam pozitia initiala a pachetului
                packageInitialX = packageX;
                packageInitialY = packageY;
                packageInitialZ = packageZ;

				// atasam pachetul de drona
                packageX = xPosition;
                packageY = yPosition - 0.75f;
                packageZ = zPosition;

				// generam o noua zona de livrare
                do {
                    deliveryX = -100 + rand() % 201;
                    deliveryZ = -100 + rand() % 201;
				} while (glm::distance(glm::vec3(deliveryX, 0, deliveryZ), glm::vec3(packageX, 0, packageY)) < 10.0f ||
					std::any_of(obstacles.begin(), obstacles.end(), [&](Obstacle o) {
						return glm::distance(o.position, glm::vec3(deliveryX, 0, deliveryZ)) < 10.0f;
						}));
                deliveryZoneActive = true;
            }
        }
    }

	// zona de livrare
    {
		// desenam zona de livrare
        if (deliveryZoneActive) {
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(deliveryX, 0.15f, deliveryZ));
            RenderMesh(meshes["delivery"], shaders["VertexColor"], modelMatrix);
        }

		// sageata pentru a indica directia dronei
        if (deliveryZoneActive) {
			glm::vec3 DeliveryZoneCenter = glm::vec3(deliveryX, 0, deliveryZ);
			glm::vec3 DronePosition = glm::vec3(xPosition, 0, zPosition);
            glm::vec3 directionToDelivery = DeliveryZoneCenter - DronePosition;
            float arrowRotation = atan2(directionToDelivery.x, directionToDelivery.z);
            float relativeRotation = arrowRotation;
            glm::vec3 forwardDirection = glm::vec3(cos(rotatieY), 0, -sin(rotatieY));
            glm::vec3 arrowPosition = glm::vec3(xPosition, yPosition, zPosition) + forwardDirection * 3.0f;

            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, arrowPosition);
            modelMatrix = glm::rotate(modelMatrix, glm::radians(-90.0f), glm::vec3(0, 1, 0));
            modelMatrix = glm::rotate(modelMatrix, relativeRotation, glm::vec3(0, 1, 0));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
            modelMatrix = glm::rotate(modelMatrix, glm::radians(15.0f), glm::vec3(1, 0, 0));
            RenderMesh(meshes["arrow"], shaders["VertexNormal"], modelMatrix);

        }
        else {
			// daca nu este activa zona de livrare, desenam sageata pentru a indica pachetul
			glm::vec3 PackagePosition = glm::vec3(packageX, packageY, packageZ);
			glm::vec3 DronePosition = glm::vec3(xPosition, yPosition, zPosition);
			glm::vec3 directionToPackage = PackagePosition - DronePosition;
			float arrowRotation = atan2(directionToPackage.x, directionToPackage.z);
			float relativeRotation = arrowRotation;
			glm::vec3 forwardDirection = glm::vec3(cos(rotatieY), 0, -sin(rotatieY));
			glm::vec3 arrowPosition = glm::vec3(xPosition, yPosition, zPosition) + forwardDirection * 3.0f;

			glm::mat4 modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, arrowPosition);
			modelMatrix = glm::rotate(modelMatrix, glm::radians(-90.0f), glm::vec3(0, 1, 0));
			modelMatrix = glm::rotate(modelMatrix, relativeRotation, glm::vec3(0, 1, 0));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
			modelMatrix = glm::rotate(modelMatrix, glm::radians(15.0f), glm::vec3(1, 0, 0));
			RenderMesh(meshes["arrow"], shaders["VertexNormal"], modelMatrix);

        }
    }
    
	// verificare coliziune cu zona de livrare
    {
        if (deliveryZoneActive) {
            float distanceToDelivery = glm::distance(glm::vec3(xPosition, 0, zPosition), glm::vec3(deliveryX, 0, deliveryZ));
            if (distanceToDelivery < 2.0f) {
                packagesDelivered++;
				cout << "Packages delivered: " << packagesDelivered << endl;
                deliveryZoneActive = false;
                packageAttached = false;

                // generam un nou pachet
                do {
                    packageX = -100 + rand() % 201;
                    packageZ = -100 + rand() % 201;
					packageY = 2.0f;
                } while (glm::distance(glm::vec3(packageX, 0, packageZ), glm::vec3(xPosition, 0, zPosition)) < 10.0f ||
                    std::any_of(obstacles.begin(), obstacles.end(), [&](Obstacle o) {
                        return glm::distance(o.position, glm::vec3(packageX, 0, packageZ)) < 10.0f;
                        }));
                
            }
        }

    }

    // desenare corpul dronei
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(xPosition, yPosition, zPosition));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(45.0f), glm::vec3(0, 1, 0));
		modelMatrix = glm::rotate(modelMatrix, rotatieY, glm::vec3(0, 1, 0));
		modelMatrix = glm::rotate(modelMatrix, rotatieX + rotatieX1, glm::vec3(1, 0, 0));
		modelMatrix = glm::rotate(modelMatrix, rotatieZ + rotatieZ1, glm::vec3(0, 0, 1));
        RenderMesh(meshes["body1"], shaders["VertexColor"], modelMatrix);

        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(xPosition, yPosition, zPosition));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(45.0f), glm::vec3(0, 1, 0));
		modelMatrix = glm::rotate(modelMatrix, rotatieY, glm::vec3(0, 1, 0));
		modelMatrix = glm::rotate(modelMatrix, rotatieX + rotatieX1, glm::vec3(1, 0, 0));
		modelMatrix = glm::rotate(modelMatrix, rotatieZ + rotatieZ1, glm::vec3(0, 0, 1));
        RenderMesh(meshes["body2"], shaders["VertexColor"], modelMatrix);
    }
	
    // desenare cuburi pentru elice
    {
		glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(xPosition, yPosition, zPosition));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(45.0f), glm::vec3(0, 1, 0));
		modelMatrix = glm::rotate(modelMatrix, rotatieY, glm::vec3(0, 1, 0));
		modelMatrix = glm::rotate(modelMatrix, rotatieX + rotatieX1, glm::vec3(1, 0, 0));
		modelMatrix = glm::rotate(modelMatrix, rotatieZ + rotatieZ1, glm::vec3(0, 0, 1));
		modelMatrix = glm::translate(modelMatrix, glm::vec3(distPos, width, 0));
		RenderMesh(meshes["cube"], shaders["VertexColor"], modelMatrix);

		modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(xPosition, yPosition, zPosition));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(45.0f), glm::vec3(0, 1, 0));
        modelMatrix = glm::rotate(modelMatrix, rotatieY, glm::vec3(0, 1, 0));
		modelMatrix = glm::rotate(modelMatrix, rotatieX + rotatieX1, glm::vec3(1, 0, 0));
		modelMatrix = glm::rotate(modelMatrix, rotatieZ + rotatieZ1, glm::vec3(0, 0, 1));
		modelMatrix = glm::translate(modelMatrix, glm::vec3(-distPos, width, 0));
		RenderMesh(meshes["cube"], shaders["VertexColor"], modelMatrix);

		modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(xPosition, yPosition, zPosition));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(45.0f), glm::vec3(0, 1, 0));
        modelMatrix = glm::rotate(modelMatrix, rotatieY, glm::vec3(0, 1, 0));
		modelMatrix = glm::rotate(modelMatrix, rotatieX + rotatieX1, glm::vec3(1, 0, 0));
		modelMatrix = glm::rotate(modelMatrix, rotatieZ + rotatieZ1, glm::vec3(0, 0, 1));
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0, width, distPos));
		RenderMesh(meshes["cube"], shaders["VertexColor"], modelMatrix);

		modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(xPosition, yPosition, zPosition));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(45.0f), glm::vec3(0, 1, 0));
		modelMatrix = glm::rotate(modelMatrix, rotatieY, glm::vec3(0, 1, 0));
		modelMatrix = glm::rotate(modelMatrix, rotatieX + rotatieX1, glm::vec3(1, 0, 0));
		modelMatrix = glm::rotate(modelMatrix, rotatieZ + rotatieZ1, glm::vec3(0, 0, 1));
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0, width, -distPos));
		RenderMesh(meshes["cube"], shaders["VertexColor"], modelMatrix);
    }
    
	// desenare elice
    {
       float angularSpeed = 25.0f;
       float angle = angularSpeed * Engine::GetElapsedTime();

       glm::mat4 modelMatrix = glm::mat4(1);
       modelMatrix = glm::translate(modelMatrix, glm::vec3(xPosition, yPosition, zPosition));
       modelMatrix = glm::rotate(modelMatrix, glm::radians(45.0f), glm::vec3(0, 1, 0));
	   modelMatrix = glm::rotate(modelMatrix, rotatieY, glm::vec3(0, 1, 0));
	   modelMatrix = glm::rotate(modelMatrix, rotatieX + rotatieX1, glm::vec3(1, 0, 0));
       modelMatrix = glm::rotate(modelMatrix, rotatieZ + rotatieZ1, glm::vec3(0, 0, 1));
	   modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 1.65f * width, distPos));
	   modelMatrix = glm::rotate(modelMatrix, -angle, glm::vec3(0, 1, 0));
       RenderMesh(meshes["rotor1"], shaders["VertexColor"], modelMatrix);

	   modelMatrix = glm::mat4(1);
       modelMatrix = glm::translate(modelMatrix, glm::vec3(xPosition, yPosition, zPosition));
       modelMatrix = glm::rotate(modelMatrix, glm::radians(45.0f), glm::vec3(0, 1, 0));
	   modelMatrix = glm::rotate(modelMatrix, rotatieY, glm::vec3(0, 1, 0));
	   modelMatrix = glm::rotate(modelMatrix, rotatieX + rotatieX1, glm::vec3(1, 0, 0));
	   modelMatrix = glm::rotate(modelMatrix, rotatieZ + rotatieZ1, glm::vec3(0, 0, 1));
	   modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 1.65f * width, -distPos));
	   modelMatrix = glm::rotate(modelMatrix, -angle, glm::vec3(0, 1, 0));
	   RenderMesh(meshes["rotor1"], shaders["VertexColor"], modelMatrix);

	   modelMatrix = glm::mat4(1);
       modelMatrix = glm::translate(modelMatrix, glm::vec3(xPosition, yPosition, zPosition));
       modelMatrix = glm::rotate(modelMatrix, glm::radians(45.0f), glm::vec3(0, 1, 0));
	   modelMatrix = glm::rotate(modelMatrix, rotatieY, glm::vec3(0, 1, 0));
	   modelMatrix = glm::rotate(modelMatrix, rotatieX + rotatieX1, glm::vec3(1, 0, 0));
	   modelMatrix = glm::rotate(modelMatrix, rotatieZ + rotatieZ1, glm::vec3(0, 0, 1));
	   modelMatrix = glm::translate(modelMatrix, glm::vec3(distPos, 1.65f * width, 0));
	   modelMatrix = glm::rotate(modelMatrix, angle, glm::vec3(0, 1, 0));
	   RenderMesh(meshes["rotor1"], shaders["VertexColor"], modelMatrix);


	   modelMatrix = glm::mat4(1);
       modelMatrix = glm::translate(modelMatrix, glm::vec3(xPosition, yPosition, zPosition));
       modelMatrix = glm::rotate(modelMatrix, glm::radians(45.0f), glm::vec3(0, 1, 0));
	   modelMatrix = glm::rotate(modelMatrix, rotatieY, glm::vec3(0, 1, 0));
	   modelMatrix = glm::rotate(modelMatrix, rotatieX + rotatieX1, glm::vec3(1, 0, 0));
	   modelMatrix = glm::rotate(modelMatrix, rotatieZ + rotatieZ1, glm::vec3(0, 0, 1));
	   modelMatrix = glm::translate(modelMatrix, glm::vec3(-distPos, 1.65f * width, 0));
	   modelMatrix = glm::rotate(modelMatrix, angle, glm::vec3(0, 1, 0));
	   RenderMesh(meshes["rotor1"], shaders["VertexColor"], modelMatrix);
    }

    // desenare camera
    glm::vec3 droneForward = glm::vec3(cos(rotatieY), 0, -sin(rotatieY));
    float totalRotation = rotatieY + cameraAngleY;
    glm::vec3 combinedForward = glm::vec3(cos(totalRotation), 0, -sin(totalRotation));
    camera->UpdateCamera(
        glm::vec3(xPosition, yPosition, zPosition),
        combinedForward,                  
        glm::vec3(0, 1, 0),                         
        5.0f,                                      
        2.0f                                        
    );

    // zona de miscare a dronei
	if (xPosition > 100) {
		xPosition = 100;
	}
	if (xPosition < -100) {
		xPosition = -100;
	}
	if (zPosition > 100) {
		zPosition = 100;
	}
	if (zPosition < -100) {
		zPosition = -100;
	}
	if (yPosition < 0) {
		yPosition = 0;
	}
	if (yPosition > 40) {
		yPosition = 40;
	}

}

// functie de setare a proiectiei ortografice
void Tema2::SetOrthographicProjection(float width, float height, float zNear, float zFar) {
    projectionMatrix = glm::ortho(-width / 2.0f, width / 2.0f, -height / 2.0f, height / 2.0f, zNear, zFar);
}

// functie de desenare a minimapei
void Tema2::UpdateMinimap(float deltaTimeSeconds) {
	// Clear the screen
    glClear(GL_DEPTH_BUFFER_BIT);

	// salvam parametrii camerei
    glm::vec3 savedPosition = camera->position;
    glm::vec3 savedForward = camera->forward;
    glm::vec3 savedRight = camera->right;
    glm::vec3 savedUp = camera->up;
    float savedDistance = camera->distanceToTarget;
    glm::mat4 savedProjection = projectionMatrix;

	// setam proiectia ortografica
    float orthoSize = 100.0f;
    SetOrthographicProjection(orthoSize, orthoSize, 0.01f, 200.0f);

	// pozitia dronei si a minimapei
    glm::vec3 dronePos = glm::vec3(xPosition, yPosition, zPosition);
    glm::vec3 minimapPos = dronePos + glm::vec3(0, 120.0f, 0);

    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), rotatieY + - glm::radians(90.0f), glm::vec3(0, 1, 0));
    glm::vec3 rotatedUp = glm::vec3(rotationMatrix * glm::vec4(0, 0, -1, 0));

    // seteaza camera pentru minimap
    camera->Set(minimapPos, dronePos, rotatedUp);

	// planul minimapei
    {
        glm::mat4 modelMatrix = glm::scale(glm::mat4(1), glm::vec3(300, 1, 300));
        RenderSimpleMesh(meshes["minimap_square"], shaders["TemaShader"], modelMatrix);
    }

	// randare obstacole
    for (const auto& obstacle : obstacles) {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, obstacle.position);
        float renderScale = obstacle.scale * 1.5f;
        modelMatrix = glm::scale(modelMatrix, glm::vec3(renderScale));

        if (obstacle.isTree) {
            RenderMesh(meshes["minimap_circle"], shaders["VertexColor"], modelMatrix);
        }
        else {
            RenderMesh(meshes["minimap_square"], shaders["VertexColor"], modelMatrix);
        }
    }

    // drona
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, dronePos);
        modelMatrix = glm::rotate(modelMatrix, rotatieY + RADIANS(90), glm::vec3(0, 1, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(2.0f));
        RenderMesh(meshes["minimap_arrow"], shaders["VertexColor"], modelMatrix);
    }
	// zona de livrare
	if (deliveryZoneActive) {
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(deliveryX, 0.1f, deliveryZ));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(2.5f));
		RenderMesh(meshes["minimap_delivery"], shaders["VertexColor"], modelMatrix);
	}
    // pachetul
    else {
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(packageX, packageY, packageZ));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(2.5f));
		RenderMesh(meshes["minimap_package"], shaders["VertexColor"], modelMatrix);
    }

    // resetam parametrii camerei
    camera->position = savedPosition;
    camera->forward = savedForward;
    camera->right = savedRight;
    camera->up = savedUp;
    camera->distanceToTarget = savedDistance;
    projectionMatrix = savedProjection;
}

// functia de update
void Tema2::Update(float deltaTimeSeconds) {

    glm::ivec2 resolution = window->GetResolution();
	minimapViewportArea = ViewportArea(50, 50, resolution.x / 4.f, resolution.y / 4.f);
    glViewport(0, 0, resolution.x, resolution.y);
    glEnable(GL_DEPTH_TEST);

    UpdateMap(deltaTimeSeconds);

    glViewport(minimapViewportArea.x, minimapViewportArea.y, minimapViewportArea.width, minimapViewportArea.height);
    glDisable(GL_DEPTH_TEST);
    UpdateMinimap(deltaTimeSeconds);
    glEnable(GL_DEPTH_TEST);

}

void Tema2::FrameEnd()
{

}

// functia de randare a terenului
void Tema2::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix) {
    if (!mesh || !shader || !shader->program)
        return;

    // Activate shader
    glUseProgram(shader->program);

    // Set transformation matrices
    glUniformMatrix4fv(glGetUniformLocation(shader->program, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shader->program, "view"), 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(glGetUniformLocation(shader->program, "projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // Set color interpolation uniforms
    glm::vec3 color1(0.2f, 0.5f, 0.2f); // Green
    glm::vec3 color2(0.8f, 0.8f, 0.5f); // Yellow
    glUniform3fv(glGetUniformLocation(shader->program, "color1"), 1, glm::value_ptr(color1));
    glUniform3fv(glGetUniformLocation(shader->program, "color2"), 1, glm::value_ptr(color2));

    // Bind VAO and draw the mesh
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(GL_TRIANGLES, static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);

    // Unbind VAO
    glBindVertexArray(0);
}

// functia de randare a obiectelor cu interpolare de culoare
void Tema2::RenderInterpolatedMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& customColor, float droneAltitude, float maxAltitude) {
    if (!mesh || !shader || !shader->program)
        return;

    // Activate shader
    glUseProgram(shader->program);

    // Set transformation matrices
    glUniformMatrix4fv(glGetUniformLocation(shader->program, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shader->program, "view"), 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(glGetUniformLocation(shader->program, "projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // Set altitude-based uniforms
    glUniform1f(glGetUniformLocation(shader->program, "droneAltitude"), droneAltitude);
    glUniform1f(glGetUniformLocation(shader->program, "maxAltitude"), maxAltitude);

    // Set color interpolation uniforms
    glm::vec3 grayColor(0.7f, 0.7f, 0.7f); // Default white-gray
    glUniform3fv(glGetUniformLocation(shader->program, "color1"), 1, glm::value_ptr(customColor));
    glUniform3fv(glGetUniformLocation(shader->program, "color2"), 1, glm::value_ptr(grayColor));

    // Bind VAO and draw the mesh
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(GL_TRIANGLES, static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);

    // Unbind VAO
    glBindVertexArray(0);
}

// functie de randare simpla a unui mesh
void Tema2::RenderMesh(Mesh * mesh, Shader * shader, const glm::mat4 & modelMatrix)
{
    if (!mesh || !shader || !shader->program)
        return;

    // Render an object using the specified shader and the specified position
    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    mesh->Render();
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Tema2::OnInputUpdate(float deltaTime, int mods)
{

	// rotate the helicopter around the OY axis with A and D
	if (window->KeyHold(GLFW_KEY_A))
	{
		rotatieY += deltaTime;
	}
	if (window->KeyHold(GLFW_KEY_D))
	{
		rotatieY -= deltaTime;
	}

	// rotate the helicopter around the OX axis with UP and DOWN
	if (window->KeyHold(GLFW_KEY_UP))
	{
		rotatieX += deltaTime;
		rotatieZ1 += deltaTime;
        if (rotatieX > glm::radians(15.0f)) {
            rotatieX = glm::radians(15.0f);
        }
		if (rotatieZ1 > glm::radians(15.0f)) {
			rotatieZ1 = glm::radians(15.0f);
		}
        if (thrust) {
            zPosition += ((thrust > 3.0f) ? thrust : 3.0f)  * deltaTime * cos(rotatieY);
			xPosition += ((thrust > 3.0f) ? thrust : 3.0f) * deltaTime * sin(rotatieY);
        }
	}
	if (window->KeyHold(GLFW_KEY_DOWN))
	{
		rotatieX -= deltaTime;
        rotatieZ1 -= deltaTime;
		if (rotatieX < glm::radians(-15.0f)) {
			rotatieX = glm::radians(-15.0f);
		}
		if (rotatieZ1 < glm::radians(-15.0f)) {
			rotatieZ1 = glm::radians(-15.0f);
		}
		if (thrust) {
			zPosition -= ((thrust > 3.0f) ? thrust : 3.0f) * deltaTime * cos(rotatieY);
			xPosition -= ((thrust > 3.0f) ? thrust : 3.0f) * deltaTime * sin(rotatieY);
		}
	}
	// stop the rotation if the keys are not pressed
    if ((!window->KeyHold(GLFW_KEY_UP)) && (!window->KeyHold(GLFW_KEY_DOWN))) {
        if (rotatieX > 0) {
            rotatieX -= deltaTime;
			if (rotatieX < 0) {
				rotatieX = 0;
			}
        }
		if (rotatieZ1 < 0) {
			rotatieZ1 += deltaTime;
			if (rotatieZ1 > 0) {
				rotatieZ1 = 0;
			}
		}
        if (rotatieX < 0) {
            rotatieX += deltaTime;
            if (rotatieX > 0) {
                rotatieX = 0;
            }
        }
        if (rotatieZ1 > 0) {
            rotatieZ1 -= deltaTime;
            if (rotatieZ1 < 0) {
                rotatieZ1 = 0;
            }
        }
    }

	// rotate the helicopter around the OZ axis with LEFT and RIGHT
	if (window->KeyHold(GLFW_KEY_LEFT))
	{
		rotatieZ += deltaTime;
		rotatieX1 -= deltaTime;
        if (rotatieZ > glm::radians(15.0f)) {
            rotatieZ = glm::radians(15.0f);
        }
		if (rotatieX1 < glm::radians(-15.0f)) {
			rotatieX1 = glm::radians(-15.0f);
		}
		if (thrust) {
			xPosition -= ((thrust > 3.0f) ? thrust : 3.0f) * deltaTime * cos(rotatieY);
			zPosition += ((thrust > 3.0f) ? thrust : 3.0f) * deltaTime * sin(rotatieY);
		}
        


	}
	if (window->KeyHold(GLFW_KEY_RIGHT))
	{
		rotatieZ -= deltaTime;
		rotatieX1 += deltaTime;
		if (rotatieZ < glm::radians(-15.0f)) {
			rotatieZ = glm::radians(-15.0f);
		}
		if (rotatieX1 > glm::radians(15.0f)) {
			rotatieX1 = glm::radians(15.0f);
		}
        if (thrust)
		{
			xPosition += ((thrust > 3.0f) ? thrust : 3.0f) * deltaTime * cos(rotatieY);
			zPosition -= ((thrust > 3.0f) ? thrust : 3.0f) * deltaTime * sin(rotatieY);
		}
	}
	// stop the rotation if the keys are not pressed
	if ((!window->KeyHold(GLFW_KEY_LEFT)) && (!window->KeyHold(GLFW_KEY_RIGHT))) {
		if (rotatieZ > 0) {
			rotatieZ -= deltaTime;
			if (rotatieZ < 0) {
				rotatieZ = 0;
			}
		}
		if (rotatieX1 > 0) {
			rotatieX1 -= deltaTime;
			if (rotatieX1 < 0) {
				rotatieX1 = 0;
			}
		}
		if (rotatieZ < 0) {
			rotatieZ += deltaTime;
			if (rotatieZ > 0) {
				rotatieZ = 0;
			}
		}
		if (rotatieX1 < 0) {
			rotatieX1 += deltaTime;
			if (rotatieX1 > 0) {
				rotatieX1 = 0;
			}
		}
	}

    // Thrust controls (W/S)
    if (window->KeyHold(GLFW_KEY_W))
    {
        thrust += 5.0f * deltaTime; // Increase thrust
        if (thrust > 15.0f) thrust = 15.0f; // Limit maximum thrust
        yPosition += ((window->KeyHold(GLFW_KEY_UP) || window->KeyHold(GLFW_KEY_DOWN) || window->KeyHold(GLFW_KEY_LEFT) || window->KeyHold(GLFW_KEY_RIGHT)) ? (thrust / 2) : thrust) * deltaTime;
		if (yPosition > 40.0f) yPosition = 40.0f; // Limit maximum altitude
		okUp = 1;
    }
    else {
        if (window->KeyHold(GLFW_KEY_S))
        {
            thrust -= 5.0f * deltaTime; // Decrease thrust
            if (thrust < 3.0f) thrust = 3.0f; // Limit minimum thrust
            yPosition -= 1.0f * deltaTime * ((thrust > 3.0f) ? thrust : 3.0f);
            okUp = 0;

        }
        else {
            if (thrust > 0.0f && okUp == 1)
                yPosition += thrust * 0.6f * deltaTime;
			if (thrust > 0.0f && okUp == 0)
				yPosition -= thrust * 0.6f * deltaTime;
			thrust -= 5.0f * deltaTime; // Decrease thrust
			if (thrust < 0.0f) thrust = 0.0f; // Limit minimum thrust
            if (thrust == 0.0f)
		        yPosition += gravity * 0.7f * deltaTime; // Apply gravity if no thrust is applied
	    }
    }

    if (yPosition < 0.0f) {
        yPosition = 0.0f; // Prevent falling below the ground
    }


}


void Tema2::OnKeyPress(int key, int mods)
{
    // Add key press event

    float rotationSpeed = RADIANS(10); // Viteza de rotatie

    if (key == GLFW_KEY_J) {
        // Roteste camera spre stânga
		cameraAngleY -= rotationSpeed;
    }
    if (key == GLFW_KEY_L) {
        // Roteste camera spre dreapta
		cameraAngleY += rotationSpeed;
    }

}


void Tema2::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{

}


void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema2::OnWindowResize(int width, int height)
{
}
