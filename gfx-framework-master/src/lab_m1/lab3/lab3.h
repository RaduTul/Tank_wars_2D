#pragma once

#include "components/simple_scene.h"
#include <vector>

namespace m1 {
    struct Projectile {
        glm::vec2 position;
        glm::vec2 velocity;
        bool isActive = false;
    };

    class Lab3 : public gfxc::SimpleScene {
    public:
        Lab3();
        ~Lab3();

        void Init() override;

    protected:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

        // Funcții specifice
        void GenerateTerrain(float width, float stepX);
        void CreateTerrainMesh();
        void CreateTank(glm::vec3 position, float turretAngle);
        void LaunchProjectile();
        void LaunchProjectileSecond();

        // Pentru preview traiectorie
        void DrawTrajectoryPreview(float startX, float startY, float angle, bool isSecondTank);
        const int TRAJECTORY_POINTS = 30;  // mai puține puncte
        const float TRAJECTORY_TIME_STEP = 0.05f;  // timp mai mic între puncte
        float initialVelocity = 50.0f;  // Aceeași viteză ca la proiectil

        // Variabile pentru teren și tanc
        std::vector<glm::vec2> heightmap;
        float resolutionX;
        float A1, omega1, A2, omega2;
        Mesh* terrainMesh;

        void UpdateTerrainSlide(float deltaTime);
        const float SLIDE_THRESHOLD = 0.5f;  // pragul pentru alunecare
        const float SLIDE_EPSILON = 0.1f;    // cantitatea de transfer per frame
        // Variabile pentru tanc
        float tankDirectiondreapta = 1.0;  // 1.0f pentru dreapta, -1.0f pentru stânga
        float tankDirectionstanga = -1.0;
        float initialTankY;
        float tankY;
        float tankX;
        float turretAngle = 0.0f;
        float initialTurretDirection;
        float tankAngle;
        float tankHealth = 100.0f;  // Viața maximă
        const float MAX_HEALTH = 100.0f;  // Pentru a ține minte viața maximă

        // Variabile pentru al doilea tanc
        float tankSecondX;
        float tankSecondY;
        float tankSecondHealth = 100.0f;
        float tankSecondAngle;
        float turretSecondAngle = 0.0f;
        float tankSecondDirectionRight = 1.0f;

        // Pentru proiectilul tankului 2
        Projectile projectileSecond;

        bool firstTankDestroyed = false;
        bool secondTankDestroyed = false;

        // Variabile pentru proiectil
        Projectile projectile;
        const float gravity = 98.1f;  // am mărit gravitația de 10 ori
        //nori
        void DeformTerrain(float hitX, float hitY);
        // Constante pentru limitele unghiului turelei
        const float minTurretAngle = -glm::pi<float>() / 20;  // -45°
        const float maxTurretAngle = glm::pi<float>() / 2;   // 90°

        struct Cloud {
            float x, y;
            float speed;
            float size;
        };
        std::vector<Cloud> clouds;
        void InitializeClouds();
        void UpdateClouds(float deltaTime);
        void DrawClouds();

        bool tankExploding = false;
        bool tankSecondExploding = false;
        float explosionRadius = 0.0f;
        const float MAX_EXPLOSION_RADIUS = 5.0f;
        const float EXPLOSION_SPEED = 10.0f;

        void DrawExplosion(float x, float y, float radius);
    };

}