#pragma once

#include "components/simple_scene.h"
#include "components/transform.h"
#include <unordered_map>

namespace m1
{
    class Lab7 : public gfxc::SimpleScene
    {
    public:
        Lab7();
        ~Lab7();

        void Init() override;

    private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        // Modifică semnătura metodei pentru a primi textură opțională
        void RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, Texture2D* texture = nullptr, const glm::vec3& color = glm::vec3(1));

        // Modifică semnătura metodei pentru a primi textură opțională
        void RenderBalloon(glm::vec3 position, glm::vec3 scale = glm::vec3(1), Texture2D* texture = nullptr);

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

        glm::vec3 lightPosition;
        unsigned int materialShininess;
        float materialKd;
        float materialKs;
        std::vector<std::vector<glm::vec3>> balloonPaths;  // Traiectorii pentru baloane
        std::vector<float> pathTimers;  // Timpul curent pe fiecare traiectorie
        int numPoints = 10;  // Numărul de puncte aleatorii pentru fiecare traiectorie
        // Map pentru texturi
        std::unordered_map<std::string, Texture2D*> mapTextures;
    };
}