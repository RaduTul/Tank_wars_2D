#version 330

// Input
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform vec3 object_color;

// Output
out vec3 color;

void main()
{
    // Deform the balloon shape
    vec3 pos = v_position;
    
    // Check if vertex is in the lower half of the sphere
    if (pos.y >= -0.5 && pos.y <= 0) {
        // Calculate deformation factor based on height
        float deformFactor = (pos.y + 0.5) * 2.0; // Maps [0, -0.5] to [1, 0]
        
        // Apply vertical stretching
        pos.y = pos.y - (0.3 * (1.0 - deformFactor));
    }

    // Pass color to fragment shader
    color = object_color;

    // Compute gl_Position with deformed position
    gl_Position = Projection * View * Model * vec4(pos, 1.0);
}