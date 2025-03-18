#include "lab_m1/lab7/lab7.h"

#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace m1;

Lab7::Lab7()
{
}

Lab7::~Lab7()
{
}

void Lab7::Init()
{

    // Load meshes
    {
        Mesh* mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("sphere");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    // Load textures
    const string sourceTextureDir = PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "lab7", "textures");
    {
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(sourceTextureDir, "crate.jpg").c_str(), GL_REPEAT);
        mapTextures["crate"] = texture;

    }

    // Create a shader program for drawing face polygon with the color of the normal
    {
        Shader* shader = new Shader("LabShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "lab7", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "lab7", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    // Light & material properties
    {
        lightPosition = glm::vec3(0, 1, 1);
        materialShininess = 30;
        materialKd = 0.5;
        materialKs = 0.5;
    }
    srand(static_cast<unsigned>(time(0)));  // Seed pentru numere aleatorii

    for (int i = 0; i < 5; i++) {
        std::vector<glm::vec3> path;
        for (int j = 0; j < numPoints; j++) {
            float x = static_cast<float>(rand() % 20 - 10);
            float y = 2.0f;  // Altitudine constantă
            float z = static_cast<float>(rand() % 20 - 10);
            path.emplace_back(glm::vec3(x, y, z));
        }
        balloonPaths.push_back(path);
        pathTimers.push_back(0.0f);  // Inițializarea timerelor pentru fiecare balon
    }
}

void Lab7::FrameStart()
{
    glClearColor(0, 0, 0.4, 1); // Dark blue background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);
}


void Lab7::Update(float deltaTimeSeconds) {
    static float time = 0;  // Timp pentru rotație
    time += deltaTimeSeconds;  // Incrementăm timpul pentru animație


    glm::vec3 centerPoint = glm::vec3(0, 2, 0);  // Punctul central de rotație
    float radii[5] = { 3.0f, 4.5f, 6.0f, 7.5f, 9.0f };  // Raze mai mari pentru distanțare
    float speeds[5] = { 1.0f, 0.8f, 1.2f, 1.1f, 0.9f };  // Viteze diferite
    for (int i = 0; i < 5; i++) {

        float angle = time * speeds[i];  // Calculăm unghiul
        float x = centerPoint.x + radii[i] * cos(angle);  // Coordonata X pe cerc
        float z = centerPoint.z + radii[i] * sin(angle);  // Coordonata Z pe cerc
        glm::vec3 position = glm::vec3(x, centerPoint.y, z);  // Poziția balonului

        glm::vec3 scale = glm::vec3(1.0f);  // Mărimea balonului
        RenderBalloon(position, scale);  // Randăm balonul
    }
    //for (int i = 0; i < 5; i++) {
    //    // Timpul curent pentru traiectorie
    //    pathTimers[i] += deltaTimeSeconds * 0.2f;
    //    int segment = static_cast<int>(pathTimers[i]) % numPoints;

    //    // Obținem punctele între care interpolăm
    //    glm::vec3 startPoint = balloonPaths[i][segment];
    //    glm::vec3 endPoint = balloonPaths[i][(segment + 1) % numPoints];
    //    float t = pathTimers[i] - static_cast<int>(pathTimers[i]);

    //    // Interpolare liniară între puncte
    //    glm::vec3 position = glm::mix(startPoint, endPoint, t);
    //    glm::vec3 scale = glm::vec3(1.0f);

    //    RenderBalloon(position, scale, mapTextures["balloon"]);
    //}
}


void Lab7::RenderBalloon(glm::vec3 position, glm::vec3 scale, Texture2D* texture)
{
    // Render balloon body (sphere)
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, position);
        modelMatrix = glm::scale(modelMatrix, scale);
        RenderSimpleMesh(meshes["sphere"], shaders["LabShader"], modelMatrix, nullptr, glm::vec3(1, 0, 0)); // Red color
    }

    // Render basket (box) - wider and taller
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, position + glm::vec3(0, -2.0f, 0) * scale.y);
        modelMatrix = glm::scale(modelMatrix, scale * glm::vec3(0.8f, 0.8f, 0.8f)); // wider on X, taller on Y
        RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, texture, glm::vec3(0.6f, 0.3f, 0.1f)); // Brown color
    }

    // Render connecting ropes (4 boxes) - straight down and properly spaced
    float ropeScale = 0.03f;     // thin ropes
    float ropeSpacing = 0.35f;   // space between ropes at the top

    for (int i = 0; i < 4; i++) {
        glm::mat4 modelMatrix = glm::mat4(1);
        float angle = glm::radians(90.0f * i);
        float x = cos(angle) * ropeSpacing;
        float z = sin(angle) * ropeSpacing;

        // Position ropes at corners of balloon and straight down
        modelMatrix = glm::translate(modelMatrix, position + glm::vec3(x, -1.0f, z) * scale.x);
        modelMatrix = glm::scale(modelMatrix, scale * glm::vec3(ropeScale, 1.0f, ropeScale));

        RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, nullptr, glm::vec3(0.4f)); // Gray color
    }
}

void Lab7::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, Texture2D* texture, const glm::vec3& color)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    glUseProgram(shader->program);

    // Setează uniformele pentru proprietățile luminii
    int light_position = glGetUniformLocation(shader->program, "light_position");
    glUniform3f(light_position, lightPosition.x, lightPosition.y, lightPosition.z);

    glm::vec3 eyePosition = GetSceneCamera()->m_transform->GetWorldPosition();
    int eye_position = glGetUniformLocation(shader->program, "eye_position");
    glUniform3f(eye_position, eyePosition.x, eyePosition.y, eyePosition.z);

    // Setează proprietățile materialului
    int material_shininess = glGetUniformLocation(shader->program, "material_shininess");
    glUniform1i(material_shininess, materialShininess);

    int material_kd = glGetUniformLocation(shader->program, "material_kd");
    glUniform1f(material_kd, materialKd);

    int material_ks = glGetUniformLocation(shader->program, "material_ks");
    glUniform1f(material_ks, materialKs);

    // Setează culoarea obiectului
    int object_color = glGetUniformLocation(shader->program, "object_color");
    glUniform3f(object_color, color.r, color.g, color.b);

    // Bind textura dacă este furnizată
    if (texture)
    {
        // Activează unitatea de textură 0
        glActiveTexture(GL_TEXTURE0);
        // Leagă textura
        glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());
        // Trimite uniforma pentru textură
        glUniform1i(glGetUniformLocation(shader->program, "texture_1"), 0);

        // Setează un flag pentru a indica prezența texturii
        glUniform1i(glGetUniformLocation(shader->program, "has_texture"), 1);
    }
    else
    {
        // Dezactivează texturarea dacă nu există textură
        glUniform1i(glGetUniformLocation(shader->program, "has_texture"), 0);
    }

    // Bind model matrix
    GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
    glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Bind view matrix
    glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
    int loc_view_matrix = glGetUniformLocation(shader->program, "View");
    glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // Bind projection matrix
    glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
    int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
    glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

void Lab7::FrameEnd()
{
    DrawCoordinateSystem();
}

void Lab7::OnInputUpdate(float deltaTime, int mods) {}
void Lab7::OnKeyPress(int key, int mods) {}
void Lab7::OnKeyRelease(int key, int mods) {}
void Lab7::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) {}
void Lab7::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) {}
void Lab7::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) {}
void Lab7::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) {}
void Lab7::OnWindowResize(int width, int height) {}