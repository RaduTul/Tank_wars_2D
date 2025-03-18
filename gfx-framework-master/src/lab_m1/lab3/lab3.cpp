#include "lab_m1/lab3/lab3.h"
#include "lab_m1/lab3/object2D.h"
#include <vector>
#include <iostream>

#include "lab_m1/lab3/transform2D.h"
#include "lab_m1/lab3/object2D.h"

using namespace std;
using namespace m1;

Lab3::Lab3() : terrainMesh(nullptr) {
    //resolutionX = 1280;
    //A1 = 1.0f;
   // omega1 = 1.0f;
   // A2 = 0.5f;
    //omega2 = 2.0f;
}

Lab3::~Lab3() {}

void Lab3::GenerateTerrain(float width, float stepX) {
    heightmap.clear();

    // Modificăm valorile pentru a obține dealuri mai separate
    A1 = 2.5f;       // Amplitudine mai mare
    omega1 = 0.3f;   // Frecvență mai mică pentru dealuri mai late
    A2 = 1.0f;       // A doua amplitudine mai mare
    omega2 = 0.5f;   // A doua frecvență mai mică

    for (float x = 0; x <= width; x += stepX) {
        float y = 0.5f + A1 * sin(omega1 * x) + A2 * sin(omega2 * x);
        heightmap.push_back(glm::vec2(x, y));
    }
}

void Lab3::CreateTerrainMesh() {
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    for (size_t i = 0; i < heightmap.size(); ++i) {
        vertices.emplace_back(
            glm::vec3(heightmap[i].x, heightmap[i].y, 0),
            glm::vec3(0.8f, 0.8f, 0.2f)
        );

        float lowestPoint = -10.0f;
        for (const auto& point : heightmap) {
            lowestPoint = std::min(lowestPoint, point.y);
        }
        vertices.emplace_back(
            glm::vec3(heightmap[i].x, lowestPoint, 0),
            glm::vec3(0.6f, 0.6f, 0.1f)
        );
    }

    for (unsigned int i = 0; i < (heightmap.size() * 2) - 2; ++i) {
        indices.push_back(i);
    }

    terrainMesh = new Mesh("terrain");
    terrainMesh->SetDrawMode(GL_TRIANGLE_STRIP);
    terrainMesh->InitFromData(vertices, indices);
    AddMeshToList(terrainMesh);
}

void Lab3::Init() {
    auto camera = GetSceneCamera();
    camera->SetPosition(glm::vec3(80, 0, 60));
    camera->Update();
    GetCameraInput()->SetActive(false);

    float terrainWidth = 200.0f;
    float stepX = 0.1f;

    GenerateTerrain(terrainWidth, stepX);
    CreateTerrainMesh();
    InitializeClouds();  // Inițializează norii

    // Poziția inițială a tancului
    tankX = heightmap.front().x + 140.0f;
    tankSecondX = heightmap.front().x + 20.0f;  // Aproape de marginea stângă
    // Calculăm poziția Y folosind interpolarea
    glm::vec2 A = heightmap[0];
    glm::vec2 B = heightmap[1];
    float t = (tankX - A.x) / (B.x - A.x);
    tankY = A.y * (1 - t) + B.y * t + 10.0;

    // Calculăm Y-ul inițial pentru al doilea tanc
    glm::vec2 A2 = heightmap[0];
    glm::vec2 B2 = heightmap[1];
    float t2 = (tankSecondX - A2.x) / (B2.x - A2.x);
    tankSecondY = A2.y * (1 - t) + B2.y * t2 + 10.0;
    // Inițializări
    initialTankY = tankY;
    initialTurretDirection = 1.0f;

    // Inițializare proiectil
    projectile.isActive = false;
    projectile.position = glm::vec2(tankX, tankY);

    // Inițializare proiectil pentru al doilea tanc
    projectileSecond.isActive = false;
    projectileSecond.position = glm::vec2(tankSecondX, tankSecondY);

    // Face primul tank să se uite spre stânga la început
    tankDirectiondreapta = -1.0f;
    initialTurretDirection = -1.0f;

    // Creează mesh-uri
    Mesh* trapeze1 = object2D::CreateTrapeze("trapeze1", glm::vec3(-65, 10, 0), 130, 30, glm::vec3(0.8f, 0.5f, 0.2f), true);
    AddMeshToList(trapeze1);

    Mesh* trapeze2 = object2D::CreateTrapeze("trapeze2", glm::vec3(40, 0, 0), -80, 10, glm::vec3(0.6f, 0.3f, 0.1f), true);
    AddMeshToList(trapeze2);

    Mesh* circle = object2D::CreateSemiCircle("circle", glm::vec3(0, 40, 0), 20, glm::vec3(0.8f, 0.6f, 0.4f), true);
    AddMeshToList(circle);

    Mesh* rectangle = object2D::CreateRectangle("rectangle", glm::vec3(20, 38, 0), 50, 5, glm::vec3(0.1f, 0.1f, 0.1f), true);
    AddMeshToList(rectangle);

    Mesh* trapeze1Second = object2D::CreateTrapeze("trapeze1Second",
        glm::vec3(-65, 10, 0), 130, 30, glm::vec3(0.2f, 0.5f, 0.8f), true);  // Albastru
    AddMeshToList(trapeze1Second);

    Mesh* trapeze2Second = object2D::CreateTrapeze("trapeze2Second",
        glm::vec3(40, 0, 0), -80, 10, glm::vec3(0.1f, 0.3f, 0.6f), true);
    AddMeshToList(trapeze2Second);

    Mesh* circleSecond = object2D::CreateSemiCircle("circleSecond",
        glm::vec3(0, 40, 0), 20, glm::vec3(0.4f, 0.6f, 0.8f), true);
    AddMeshToList(circleSecond);

    Mesh* rectangleSecond = object2D::CreateRectangle("rectangleSecond",
        glm::vec3(20, 38, 0), 50, 5, glm::vec3(0.1f, 0.1f, 0.3f), true);
    AddMeshToList(rectangleSecond);


    Mesh* projectileMesh = object2D::CreateCircle("projectile", glm::vec3(0, 0, 0), 5.0f, glm::vec3(1, 0, 0), true);
    AddMeshToList(projectileMesh);

    Mesh* healthBarBorder = object2D::CreateRectangle("healthBarBorder", glm::vec3(0, 0, 0), 100, 10, glm::vec3(1, 1, 1), false);  // Border alb
    AddMeshToList(healthBarBorder);

    Mesh* healthBarFill = object2D::CreateRectangle("healthBarFill", glm::vec3(0, 0, 0), 100, 10, glm::vec3(0, 0, 0), true);   // Fill negru
    AddMeshToList(healthBarFill);

    Mesh* cloudMesh = object2D::CreateCircle("cloud", glm::vec3(0, 0, 0), 1.0f, glm::vec3(1, 1, 1), true);  // Culoare (1,1,1) pentru alb pur
    AddMeshToList(cloudMesh);

}

void Lab3::FrameStart() {
    glClearColor(0.529f, 0.808f, 0.922f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);
}

void Lab3::LaunchProjectile() {
    if (!projectile.isActive) {
        float totalAngle = turretAngle + tankAngle;
        if (tankDirectiondreapta < 0) {
            totalAngle = glm::pi<float>() - turretAngle + tankAngle;
        }

        float initialVelocity = 50.0f;
        float turretLength = 2.5f;
        float heightOffset = 1.5f;  // Adăugăm un offset vertical

        // Calculăm offset-ul în funcție de direcția tankului
        float turretOffsetX = turretLength * cos(totalAngle);
        float turretOffsetY = turretLength * sin(totalAngle) + heightOffset;  // Adăugăm offset-ul vertical

        // Poziția inițială
        projectile.position = glm::vec2(
            tankX + turretOffsetX,
            tankY + turretOffsetY  // Proiectilul va porni mai sus
        );

        projectile.velocity = glm::vec2(
            initialVelocity * cos(totalAngle),
            initialVelocity * sin(totalAngle)
        );

        projectile.isActive = true;
    }
}

void Lab3::LaunchProjectileSecond() {
    if (!projectileSecond.isActive) {
        float totalAngle = turretSecondAngle + tankSecondAngle;
        if (tankSecondDirectionRight < 0) {
            totalAngle = glm::pi<float>() - turretSecondAngle + tankSecondAngle;
        }

        float initialVelocity = 50.0f;
        float turretLength = 2.5f;
        float heightOffset = 1.5f;

        float turretOffsetX = turretLength * cos(totalAngle);
        float turretOffsetY = turretLength * sin(totalAngle) + heightOffset;

        projectileSecond.position = glm::vec2(
            tankSecondX + turretOffsetX,
            tankSecondY + turretOffsetY
        );

        projectileSecond.velocity = glm::vec2(
            initialVelocity * cos(totalAngle),
            initialVelocity * sin(totalAngle)
        );

        projectileSecond.isActive = true;
    }
}

void Lab3::DrawTrajectoryPreview(float startX, float startY, float angle, bool isSecondTank) {
    // Calculate total angle based on tank direction and turret angle
    float totalAngle;
    if (isSecondTank) {
        totalAngle = turretSecondAngle + tankSecondAngle;
        if (tankSecondDirectionRight < 0) {
            totalAngle = glm::pi<float>() - turretSecondAngle + tankSecondAngle;
        }
    }
    else {
        totalAngle = turretAngle + tankAngle;
        if (tankDirectiondreapta < 0) {
            totalAngle = glm::pi<float>() - turretAngle + tankAngle;
        }
    }

    // Offset pentru poziția turelei
    float turretLength = 2.5f;
    float heightOffset = 1.5f;
    startX += turretLength * cos(totalAngle);
    startY += turretLength * sin(totalAngle) + heightOffset;

    // Calculăm viteza inițială
    float vx = initialVelocity * cos(totalAngle);
    float vy = initialVelocity * sin(totalAngle);

    std::vector<glm::vec3> points;
    points.push_back(glm::vec3(startX, startY, 0.1f));

    // Generăm punctele traiectoriei
    for (int i = 1; i < TRAJECTORY_POINTS; i++) {
        float t = i * TRAJECTORY_TIME_STEP;
        float x = startX + vx * t;
        float y = startY + vy * t - 0.5f * gravity * t * t;

        // Verificăm coliziunea cu terenul
        bool hitTerrain = false;
        for (size_t j = 0; j < heightmap.size() - 1; j++) {
            if (heightmap[j].x <= x && heightmap[j + 1].x >= x) {
                float t_terrain = (x - heightmap[j].x) / (heightmap[j + 1].x - heightmap[j].x);
                float terrainY = heightmap[j].y * (1 - t_terrain) + heightmap[j + 1].y * t_terrain - 45.0f;
                if (y <= terrainY) {
                    hitTerrain = true;
                    break;
                }
            }
        }

        if (hitTerrain || x < 0 || x > 200 || y < -100) {
            break;
        }

        points.push_back(glm::vec3(x, y, 0.1f));
    }

    // În funcția DrawTrajectoryPreview, înainte de a desena liniile:
    glLineWidth(3.0f);  // Linie mai groasă
    // Desenăm liniile traiectoriei
    for (size_t i = 0; i < points.size() - 1; i++) {
        // Creăm un nume unic pentru fiecare segment
        std::string meshName = "trajectory_" + std::to_string(i);

        // Creăm mesh-ul pentru segmentul curent
        Mesh* trajectorySegment = object2D::CreateLine(meshName,
            points[i],
            points[i + 1],
            glm::vec3(1, 1, 1));  // alb

        // Adăugăm mesh-ul la lista de mesh-uri și îl desenăm
        AddMeshToList(trajectorySegment);
            RenderMesh(meshes[meshName], shaders["VertexColor"], glm::mat4(1));
    }
}

void Lab3::UpdateTerrainSlide(float deltaTime) {
    bool terrainModified = false;
    float slideEpsilon = SLIDE_EPSILON * deltaTime;  // Ajustăm epsilon-ul în funcție de deltaTime

    // Parcurgem toate punctele din heightmap, exceptând ultimul
    for (size_t i = 0; i < heightmap.size() - 1; i++) {
        float heightDiff = abs(heightmap[i].y - heightmap[i + 1].y);

        // Dacă diferența de înălțime e mai mare decât pragul
        if (heightDiff > SLIDE_THRESHOLD) {
            // Determinăm care punct e mai înalt
            if (heightmap[i].y > heightmap[i + 1].y) {
                float transfer = min(slideEpsilon, heightDiff - SLIDE_THRESHOLD);
                heightmap[i].y -= transfer;
                heightmap[i + 1].y += transfer;
            }
            else {
                float transfer = min(slideEpsilon, heightDiff - SLIDE_THRESHOLD);
                heightmap[i + 1].y -= transfer;
                heightmap[i].y += transfer;
            }
            terrainModified = true;
        }
    }

    // Dacă terenul a fost modificat, actualizăm mesh-ul
    if (terrainModified) {
        CreateTerrainMesh();
    }
}


void Lab3::InitializeClouds() {
    // Nor deasupra primului tank
    Cloud cloud1;
    cloud1.x = 100;    // Aceeași poziție X ca primul tank (tankX)
    cloud1.y = 20;     // Puțin deasupra tankului
    cloud1.size = 20.0f;
    clouds.push_back(cloud1);

    // Nor deasupra celui de-al doilea tank
    Cloud cloud2;
    cloud2.x = 20;     // Aceeași poziție X ca al doilea tank (tankSecondX)
    cloud2.y = 20;     // Puțin deasupra tankului
    cloud2.size = 20.0f;
    clouds.push_back(cloud2);
}

void Lab3::DrawClouds() {
    for (const auto& cloud : clouds) {
        // Pentru fiecare nor, desenăm mai multe cercuri pentru a forma un nor mai mare și mai dens
        for (int i = -2; i <= 2; i++) {
            for (int j = -1; j <= 1; j++) {
                glm::mat4 cloudMatrix = glm::mat4(1);
                float offsetX = i * 100.0f;  // Spread mai mare pe X
                float offsetY = j * 10.0f;  // Spread pe Y

                cloudMatrix = glm::translate(cloudMatrix,
                    glm::vec3(cloud.x + offsetX, cloud.y + offsetY, 0.5f));  // Z și mai mare pentru a fi sigur deasupra
                cloudMatrix = glm::scale(cloudMatrix,
                    glm::vec3(cloud.size));

                // Folosim mesh-ul cloud care ar trebui să fie deja alb
                RenderMesh(meshes["cloud"], shaders["VertexColor"], cloudMatrix);
            }
        }
    }
}


void Lab3::DeformTerrain(float hitX, float hitY) {
    // Parametrii pentru deformare
    const float CRATER_RADIUS = 10.0f;  // Raza craterului
    const float MAX_DEFORMATION = 2.0f;  // Adâncimea maximă a craterului

    // Găsim indexul punctului de impact
    int centralIndex = -1;
    for (size_t i = 0; i < heightmap.size() - 1; ++i) {
        if (heightmap[i].x <= hitX && heightmap[i + 1].x >= hitX) {
            centralIndex = i;
            break;
        }
    }

    if (centralIndex >= 0) {
        // Calculăm numărul de puncte afectate de crater
        int radius = static_cast<int>(CRATER_RADIUS / (heightmap[1].x - heightmap[0].x));

        // Aplicăm deformarea într-un pattern circular
        for (int i = -radius; i <= radius; i++) {
            int currentIndex = centralIndex + i;
            if (currentIndex >= 0 && currentIndex < heightmap.size()) {
                // Calculăm distanța relativă față de centrul craterului
                float distance = abs(static_cast<float>(i)) / radius;

                // Folosim o funcție cosinus pentru a crea o formă mai naturală
                float deformation = MAX_DEFORMATION * cos(distance * M_PI / 2);
                if (deformation > 0) {
                    heightmap[currentIndex].y -= deformation;
                }
            }
        }

        // Actualizăm mesh-ul terenului
        CreateTerrainMesh();
    }
}

void Lab3::Update(float deltaTimeSeconds) {
    // 1. Desenare teren
    glm::mat4 terrainMatrix = glm::mat4(1);
    terrainMatrix = glm::scale(terrainMatrix, glm::vec3(1.0f, 1.5f, 1.0f));
    terrainMatrix = glm::translate(terrainMatrix, glm::vec3(0.0f, -30.0f, 0.0f));
    RenderMesh(meshes["terrain"], shaders["VertexColor"], terrainMatrix);
    //nori;
    // Actualizare și desenare nori
    DrawClouds();

    // 2. PRIMUL TANC
    // Calcul poziție primul tanc
    glm::vec2 A, B, A2, B2;
    for (size_t i = 0; i < heightmap.size() - 1; ++i) {
        if (heightmap[i].x <= tankX && heightmap[i + 1].x >= tankX) {
            A = heightmap[i];
            B = heightmap[i + 1];
            break;
        }
    }

    float t = (tankX - A.x) / (B.x - A.x);
    float heightOffset = -45.0f;
    tankY = (A.y * (1 - t) + B.y * t) + heightOffset;
    tankAngle = atan2(B.y - A.y, B.x - A.x);
    if (tankHealth <= 0) {
        firstTankDestroyed = true;
    }

    // Poziționare și desenare primul tanc
    glm::mat4 modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(tankX, tankY, 0.1f));
    modelMatrix = glm::rotate(modelMatrix, tankAngle, glm::vec3(0, 0, 1));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(tankDirectiondreapta * 0.05f, 0.05f, 0.025f));
    if (!firstTankDestroyed) {
        // Desenează primul tanc și toate elementele sale
        RenderMesh(meshes["trapeze1"], shaders["VertexColor"], modelMatrix);
        RenderMesh(meshes["trapeze2"], shaders["VertexColor"], modelMatrix);
        RenderMesh(meshes["circle"], shaders["VertexColor"], modelMatrix);
    }

    // Turelă primul tanc
    glm::mat4 turretMatrix = modelMatrix;
    turretMatrix = glm::translate(turretMatrix, glm::vec3(0, 40, 0));
    turretMatrix = glm::rotate(turretMatrix, turretAngle, glm::vec3(0, 0, 1));  // Elimină minus-ul
    turretMatrix = glm::translate(turretMatrix, glm::vec3(0, -40, 0));
    if (!firstTankDestroyed) {
        RenderMesh(meshes["rectangle"], shaders["VertexColor"], turretMatrix);
    }

    // Bară de viață primul tanc
    glm::mat4 healthBorderMatrix = glm::mat4(1);
    healthBorderMatrix = glm::translate(healthBorderMatrix, glm::vec3(tankX - 5.0f, tankY + 5.0f, 0.1f));
    healthBorderMatrix = glm::scale(healthBorderMatrix, glm::vec3(0.1f, 0.1f, 1.0f));
    if (!firstTankDestroyed) {
        RenderMesh(meshes["healthBarBorder"], shaders["VertexColor"], healthBorderMatrix);
    }

    glm::mat4 healthFillMatrix = glm::mat4(1);
    healthFillMatrix = glm::translate(healthFillMatrix, glm::vec3(tankX - 5.0f, tankY + 5.0f, 0.1f));
    healthFillMatrix = glm::scale(healthFillMatrix, glm::vec3(0.1f * (tankHealth / MAX_HEALTH), 0.1f, 1.0f));
    if (!firstTankDestroyed) {
        RenderMesh(meshes["healthBarFill"], shaders["VertexColor"], healthFillMatrix);
    }

    // 3. AL DOILEA TANC
    // Calcul poziție al doilea tanc
    for (size_t i = 0; i < heightmap.size() - 1; ++i) {
        if (heightmap[i].x <= tankSecondX && heightmap[i + 1].x >= tankSecondX) {
            A2 = heightmap[i];
            B2 = heightmap[i + 1];
            break;
        }
    }

    float t2 = (tankSecondX - A2.x) / (B.x - A2.x);
    tankSecondY = (A2.y * (1 - t2) + B2.y * t2) + heightOffset;
    tankSecondAngle = atan2(B2.y - A2.y, B2.x - A2.x);
    if (tankSecondHealth <= 0) {
        secondTankDestroyed = true;
    } 
    // Poziționare și desenare al doilea tanc
    glm::mat4 modelSecondMatrix = glm::mat4(1);
    modelSecondMatrix = glm::translate(modelSecondMatrix, glm::vec3(tankSecondX, tankSecondY, 0.1f));
    modelSecondMatrix = glm::rotate(modelSecondMatrix, tankSecondAngle, glm::vec3(0, 0, 1));
    modelSecondMatrix = glm::scale(modelSecondMatrix, glm::vec3(tankSecondDirectionRight * 0.05f, 0.05f, 0.025f));
    if (!secondTankDestroyed) {

        RenderMesh(meshes["trapeze1Second"], shaders["VertexColor"], modelSecondMatrix);
        RenderMesh(meshes["trapeze2Second"], shaders["VertexColor"], modelSecondMatrix);
        RenderMesh(meshes["circleSecond"], shaders["VertexColor"], modelSecondMatrix);
    }

    // Turelă al doilea tanc
    glm::mat4 turretSecondMatrix = modelSecondMatrix;
    turretSecondMatrix = glm::translate(turretSecondMatrix, glm::vec3(0, 40, 0));
    turretSecondMatrix = glm::rotate(turretSecondMatrix, turretSecondAngle, glm::vec3(0, 0, 1));
    turretSecondMatrix = glm::translate(turretSecondMatrix, glm::vec3(0, -40, 0));
    if (!secondTankDestroyed) {
        RenderMesh(meshes["rectangleSecond"], shaders["VertexColor"], turretSecondMatrix);
    }

    // Bară de viață al doilea tanc
    glm::mat4 healthBorderSecondMatrix = glm::mat4(1);
    healthBorderSecondMatrix = glm::translate(healthBorderSecondMatrix, glm::vec3(tankSecondX - 5.0f, tankSecondY + 5.0f, 0.1f));
    healthBorderSecondMatrix = glm::scale(healthBorderSecondMatrix, glm::vec3(0.1f, 0.1f, 1.0f));
    if (!secondTankDestroyed) {
        RenderMesh(meshes["healthBarBorder"], shaders["VertexColor"], healthBorderSecondMatrix);
    }

    glm::mat4 healthFillSecondMatrix = glm::mat4(1);
    healthFillSecondMatrix = glm::translate(healthFillSecondMatrix, glm::vec3(tankSecondX - 5.0f, tankSecondY + 5.0f, 0.2f));
    healthFillSecondMatrix = glm::scale(healthFillSecondMatrix, glm::vec3(0.1f * (tankSecondHealth / MAX_HEALTH), 0.1f, 1.0f));
    if (!secondTankDestroyed) {
        RenderMesh(meshes["healthBarFill"], shaders["VertexColor"], healthFillSecondMatrix);
    }

    if (!projectile.isActive) {
        if (!firstTankDestroyed) {
            DrawTrajectoryPreview(tankX, tankY, turretAngle, false);
        }
    }
    if (!projectileSecond.isActive) {
        if (!secondTankDestroyed) {
            DrawTrajectoryPreview(tankSecondX, tankSecondY, turretSecondAngle, true);
        }
    }
    // 4. PROIECTILE
    // Proiectil primul tanc
    if (projectileSecond.isActive) {
        projectileSecond.velocity.y -= gravity * deltaTimeSeconds;
        projectileSecond.position += projectileSecond.velocity * deltaTimeSeconds;

        if (projectileSecond.isActive) {
            // Verificare coliziune cu primul tanc
            float dx = projectileSecond.position.x - tankX;
            float dy = projectileSecond.position.y - tankY;
            float distanceSquared = dx * dx + dy * dy;
            if (distanceSquared < 4.0f) {
                tankHealth -= 20.0f;
                tankHealth = std::max(0.0f, tankHealth);
                projectileSecond.isActive = false;
            }

            // Verificare coliziune cu terenul
            for (size_t i = 0; i < heightmap.size() - 1; ++i) {
                //if (projectileSecond.position.y <= terrainY + 1.2) {
                //    // Creare crater
                //    int craterRadius = 20;
                //    float maxDeformation = 1.0f;
                //    for (int j = -craterRadius; j <= craterRadius; j++) {
                //        int index = i + j;
                //        if (index >= 0 && index < heightmap.size()) {
                //            float distance = abs(j);
                //            float deformation = maxDeformation * cos((distance / craterRadius) * M_PI / 2);
                //            if (distance <= craterRadius) {
                //                heightmap[index].y -= deformation;
                //            }
                //        }
                //    }
                //    CreateTerrainMesh();
                //    projectileSecond.isActive = false;
                //    break;
                //}
                if (heightmap[i].x <= projectileSecond.position.x && heightmap[i + 1].x >= projectileSecond.position.x) {
                    A = heightmap[i];
                    B = heightmap[i + 1];
                    float t = (projectileSecond.position.x - A.x) / (B.x - A.x);
                    float terrainY = A.y * (1 - t) + B.y * t + heightOffset;

                    if (projectileSecond.position.y <= terrainY + 1.2) {
                        DeformTerrain(projectileSecond.position.x, projectileSecond.position.y);
                        projectileSecond.isActive = false;
                        break;
                    }
                }
            }

            // Desenare proiectil
            if (projectileSecond.isActive) {
                glm::mat4 projectileSecondMatrix = glm::mat4(1);
                projectileSecondMatrix = glm::translate(projectileSecondMatrix,
                    glm::vec3(projectileSecond.position.x, projectileSecond.position.y, 0.1f));
                projectileSecondMatrix = glm::scale(projectileSecondMatrix, glm::vec3(0.05f));
                if (!secondTankDestroyed) {
                    RenderMesh(meshes["projectile"], shaders["VertexColor"], projectileSecondMatrix);
                }
            }
        }

        // Dezactivare proiectil dacă iese din ecran
        if (projectileSecond.position.x < 0 || projectileSecond.position.x > 200 ||
            projectileSecond.position.y < -100) {
            projectileSecond.isActive = false;
        }
    }
    // Update și desenare proiectil
     // Update and check projectile collision
    if (projectile.isActive) {
        projectile.velocity.y -= gravity * deltaTimeSeconds;
        projectile.position += projectile.velocity * deltaTimeSeconds;

        // Find terrain height at projectile's X position
        glm::vec2 A, B;
        bool found = false;
        for (size_t i = 0; i < heightmap.size() - 1; ++i) {
            if (heightmap[i].x <= projectile.position.x && heightmap[i + 1].x >= projectile.position.x) {
                A = heightmap[i];
                B = heightmap[i + 1];
                found = true;

                float t = (projectile.position.x - A.x) / (B.x - A.x);
                float terrainY = A.y * (1 - t) + B.y * t;
                float tankRadius = 2.0f;  // Raza de coliziune a tancului
                // Verificare coliziune cu primul tanc
                float dx = projectile.position.x - tankX;
                float dy = projectile.position.y - tankY;
                float distanceSquared = dx * dx + dy * dy;

                // Verificare coliziune cu al doilea tanc
                float dx2 = projectile.position.x - tankSecondX;
                float dy2 = projectile.position.y - tankSecondY;
                float distanceSquared2 = dx2 * dx2 + dy2 * dy2;

                if (distanceSquared2 < 4.0f) {  // Același radius ca și pentru primul tanc
                    tankSecondHealth -= 20.0f;
                    tankSecondHealth = std::max(0.0f, tankSecondHealth);
                    projectile.isActive = false;
                }

                if (distanceSquared < tankRadius * tankRadius) {
                    // Proiectilul a lovit tancul
                    tankHealth -= 20.0f;  // Scade 20 de puncte de viață
                    tankHealth = std::max(0.0f, tankHealth);  // Nu lăsa viața să scadă sub 0
                    projectile.isActive = false;
                    continue;
                }

                // Adăugăm același offset care îl folosim pentru tank pentru a alinia coliziunea cu vizualul
                terrainY += heightOffset;  // Folosim același heightOffset din tank

                // Verificăm doar coliziuni când proiectilul se mișcă în jos
                //if (projectile.position.y <= terrainY + 1.2) {
                //    // Create crater
                //    int craterRadius = 20;
                //    float maxDeformation = 1.0f;

                //    for (int j = -craterRadius; j <= craterRadius; j++) {
                //        int index = i + j;
                //        if (index >= 0 && index < heightmap.size()) {
                //            float distance = abs(j);
                //            float deformation = maxDeformation * cos((distance / craterRadius) * M_PI / 2);
                //            if (distance <= craterRadius) {
                //                heightmap[index].y -= deformation;
                //            }
                //        }
                //    }

                //    CreateTerrainMesh();
                //    projectile.isActive = false;
                //    break;
                //}
                if (projectile.position.y <= terrainY + 1.2) {
                    DeformTerrain(projectile.position.x, projectile.position.y);
                    projectile.isActive = false;
                    break;
                }
            }
        }

        // Deactivate projectile if it goes out of bounds
        if (projectile.position.x < 0 || projectile.position.x > 200 ||
            projectile.position.y < -100) {
            projectile.isActive = false;
        }

        // Render projectile
        if (projectile.isActive) {
            glm::mat4 projectileMatrix = glm::mat4(1);
            projectileMatrix = glm::translate(projectileMatrix,
                glm::vec3(projectile.position.x, projectile.position.y, 0.1f));
            projectileMatrix = glm::scale(projectileMatrix, glm::vec3(0.05f));

            RenderMesh(meshes["projectile"], shaders["VertexColor"], projectileMatrix);
        }
    }
}

void Lab3::OnInputUpdate(float deltaTime, int mods) {
    float speed = 10.0f * deltaTime;

    // Mișcare tanc
    if (window->KeyHold(GLFW_KEY_A)) {
        tankX -= speed;
        if (tankDirectiondreapta > 0) {
            tankDirectiondreapta = -1.0f;
        }
    }
    if (window->KeyHold(GLFW_KEY_D)) {
        tankX += speed;
        if (tankDirectiondreapta < 0) {
            tankDirectiondreapta = 1.0f;
        }
    }

    // Rotație turelă
    if (window->KeyHold(GLFW_KEY_W)) {
        turretAngle += speed / 2;
        turretAngle = std::min(turretAngle, maxTurretAngle);
    }
    if (window->KeyHold(GLFW_KEY_S)) {
        turretAngle -= speed / 2;
        turretAngle = std::max(turretAngle, minTurretAngle);
    }
    // Mișcare tanc al doilea jucător
    if (window->KeyHold(GLFW_KEY_LEFT)) {
        tankSecondX -= speed;
        if (tankSecondDirectionRight > 0) {
            tankSecondDirectionRight = -1.0f;
        }
    }
    if (window->KeyHold(GLFW_KEY_RIGHT)) {
        tankSecondX += speed;
        if (tankSecondDirectionRight < 0) {
            tankSecondDirectionRight = 1.0f;
        }
    }

    if (window->KeyHold(GLFW_KEY_UP)) {
        turretSecondAngle += speed / 2;
        turretSecondAngle = std::min(turretSecondAngle, maxTurretAngle);
    }
    if (window->KeyHold(GLFW_KEY_DOWN)) {
        turretSecondAngle -= speed / 2;
        turretSecondAngle = std::max(turretSecondAngle, minTurretAngle);
    }


}

void Lab3::OnKeyPress(int key, int mods) {
    if (!firstTankDestroyed) {
        if (key == GLFW_KEY_SPACE) {
            LaunchProjectile();
        }
    }
    if (!secondTankDestroyed) {
        if (key == GLFW_KEY_ENTER) {
            LaunchProjectileSecond();
        }
    }
}

void Lab3::FrameEnd() {}
void Lab3::OnKeyRelease(int key, int mods) {}
void Lab3::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) {}
void Lab3::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) {}
void Lab3::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) {}
void Lab3::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) {}
void Lab3::OnWindowResize(int width, int height) {
    glViewport(0, 0, width, height);
}