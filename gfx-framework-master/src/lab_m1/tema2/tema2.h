#pragma once

#include "components/simple_scene.h"
#include "lab_m1/tema2/lab_camera.h"
#include "utils/glm_utils.h"
#include "core/gpu/vertex_format.h"

#include <vector>
#include <string>


namespace m1
{
    struct Obstacle {
        glm::vec3 position;
        bool isTree; // Dacă obiectul este un copac sau o clădire
		float scale; // Scara obiectului
    };

    class Tema2 : public gfxc::SimpleScene
    {
     public:
         struct ViewportArea
         {
             ViewportArea() : x(0), y(0), width(1), height(1) {}
             ViewportArea(int x, int y, int width, int height)
                 : x(x), y(y), width(width), height(height) {}
             int x;
             int y;
             int width;
             int height;
         };
        Tema2();
        ~Tema2();

        void Init() override;

     private:
         void GenerateTerrainMesh(int width, int depth);

        void FrameStart() override;
        void CreateMinimapMeshes();
        bool isPointInsideAABB(const glm::vec3& point, const glm::vec3& boxMin, const glm::vec3& boxMax);
        bool checkCollisionWithSphere(glm::vec3 dronePosition, glm::vec3 sphereCenter, float radius, glm::vec3 scale);
        bool checkCollisionWithCylinder(glm::vec3 dronePosition, glm::vec3 cylinderBase, float radius, float height, glm::vec3 scale);
        bool checkCollisionWithCone(glm::vec3 dronePosition, glm::vec3 coneBase, float radius, float height, glm::vec3 scale);
        bool checkCollisionWithPyramid(glm::vec3 dronePosition, glm::vec3 pyramidBase, float sideLength, float height, glm::vec3 scale);
        bool checkCollisionWithBuilding(const glm::vec3& point, const glm::vec3& buildingPosition, const float scale);
        bool checkCollisionWithBox(const glm::vec3& point, const glm::vec3& boxPosition);
        bool checkCollisionWithGround(const glm::vec3& position);
        bool checkCollisionWithObstacle(glm::vec3 point);
        void Update(float deltaTimeSeconds) override;
		void UpdateMap(float deltaTimeSeconds);
        void UpdateMinimap(float deltaTimeSeconds);
        void FrameEnd() override;

        void RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix);
        void RenderInterpolatedMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& customColor, float droneAltitude, float maxAltitude);
        void SetOrthographicProjection(float width, float height, float zNear, float zFar);

        void RenderMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix) override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

     protected:
        implemented::CameraTema *camera;
        glm::mat4 projectionMatrix;
        bool renderCameraTarget;

        // TODO(student): If you need any other class variables, define them here.
        float orthoLeft = -5.0f;
        float orthoRight = 5.0f;
        float orthoBottom = -5.0f;
        float orthoTop = 5.0f;
        float orthoNear = 0.01f;
        float orthoFar = 200.0f;


        float fov = RADIANS(60);
        float perspectiveNear = 0.01f;
        float perspectiveFar = 200.0f;

        bool isPerspective = true;

		// rotatia dronei in jurul axelor
        float rotatieX = 0;
		float rotatieY = 0;
		float rotatieZ = 0;
		float rotatieX1 = 0;
		float rotatieZ1 = 0;

		// pozitia dronei
        float xPosition = 0;
        float yPosition = 0.0f; 
        float zPosition = 0;

		// lungimea si latimea dronei
		float length = 2.0f;
		float width = 0.2f;
		float distPos = (length - width) / 2;

		// viteza dronei
		float gravity = -9.81f;
		float thrust = 0.0f;

		// obstacole
        std::vector<Obstacle> obstacles;

        // date despre pachet
		float packageX = 0;
		float packageY = 0;
		float packageZ = 0;
        bool packageAttached = false;
        float deliveryX, deliveryZ;
        bool deliveryZoneActive = false;
        int packagesDelivered = 0;
        float packageInitialX = 0;
        float packageInitialY = 0;
        float packageInitialZ = 0;

        // drona merge in sus sau in jos
        int okUp = 0;

        // minimap
        glm::mat4 minimapViewMatrix;
        glm::mat4 minimapProjectionMatrix;
        ViewportArea minimapViewportArea;

        // rotirea camerei in jurul dronei
        float cameraAngleY = 0.0f;


    };
}   // namespace m1
