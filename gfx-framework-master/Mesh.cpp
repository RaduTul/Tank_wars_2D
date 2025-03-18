#include "object2D.h"

namespace object2D {
    Mesh* CreateSquare(const std::string& name, glm::vec3 corner, float length, glm::vec3 color) {
        std::vector<VertexFormat> vertices = {
            VertexFormat(corner, color),
            VertexFormat(corner + glm::vec3(length, 0, 0), color),
            VertexFormat(corner + glm::vec3(length, length, 0), color),
            VertexFormat(corner + glm::vec3(0, length, 0), color)
        };

        std::vector<unsigned int> indices = { 0, 1, 2, 2, 3, 0 };

        Mesh* square = new Mesh(name);
        square->InitFromData(vertices, indices);
        return square;
    }
}
