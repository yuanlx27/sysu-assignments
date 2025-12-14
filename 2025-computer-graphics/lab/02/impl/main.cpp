#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cmath>

// Canvas dimensions
const int CANVAS_WIDTH = 600;
const int CANVAS_HEIGHT = 600;

// Framebuffer (RGBA)
std::vector<unsigned char> framebuffer(CANVAS_WIDTH * CANVAS_HEIGHT * 4, 255);
// Z-Buffer
std::vector<float> zbuffer(CANVAS_WIDTH * CANVAS_HEIGHT, 1.0f);

GLuint textureID;

// Vertex Structure
struct Vertex {
    glm::vec3 position; // Local Space
    glm::vec3 color;
    glm::vec3 normal;
};

// Transformed Vertex (Screen Space + Attributes)
struct PixelVertex {
    glm::vec3 position; // Screen Space (x, y, z)
    glm::vec3 color;    // Interpolated Color
    glm::vec3 normal;   // Interpolated Normal (for Phong)
    glm::vec3 worldPos; // World Position (for Phong)
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// Helper to set a pixel with Z-Buffer check
void put_pixel(int x, int y, float z, glm::vec3 color) {
    if (x < 0 || x >= CANVAS_WIDTH || y < 0 || y >= CANVAS_HEIGHT) return;
    int index = y * CANVAS_WIDTH + x;
    
    // Depth Test (assuming standard OpenGL depth range 0.0 to 1.0, where smaller is closer)
    // Note: In our manual projection, we need to ensure Z is normalized.
    if (z < zbuffer[index]) {
        zbuffer[index] = z;
        int fb_index = index * 4;
        framebuffer[fb_index] = static_cast<unsigned char>(glm::clamp(color.r, 0.0f, 1.0f) * 255);
        framebuffer[fb_index + 1] = static_cast<unsigned char>(glm::clamp(color.g, 0.0f, 1.0f) * 255);
        framebuffer[fb_index + 2] = static_cast<unsigned char>(glm::clamp(color.b, 0.0f, 1.0f) * 255);
        framebuffer[fb_index + 3] = 255; // Alpha
    }
}

// Clear buffers
void clear_buffers(glm::vec3 color) {
    unsigned char r = static_cast<unsigned char>(color.r * 255);
    unsigned char g = static_cast<unsigned char>(color.g * 255);
    unsigned char b = static_cast<unsigned char>(color.b * 255);
    for (size_t i = 0; i < framebuffer.size(); i += 4) {
        framebuffer[i] = r;
        framebuffer[i + 1] = g;
        framebuffer[i + 2] = b;
        framebuffer[i + 3] = 255;
    }
    std::fill(zbuffer.begin(), zbuffer.end(), 1.0f);
}

// DDA Line Drawing Algorithm (2D)
void draw_line_dda(glm::vec2 p1, glm::vec2 p2, glm::vec3 color) {
    float dx = p2.x - p1.x;
    float dy = p2.y - p1.y;
    float steps = std::max(std::abs(dx), std::abs(dy));

    float xInc = dx / steps;
    float yInc = dy / steps;

    float x = p1.x;
    float y = p1.y;

    for (int i = 0; i <= steps; i++) {
        // DDA doesn't use Z-buffer in this simple version, just draws on top
        // We use a dummy Z of -1.0 to force draw if we wanted, but let's just write directly
        int px = static_cast<int>(std::round(x));
        int py = static_cast<int>(std::round(y));
        if (px >= 0 && px < CANVAS_WIDTH && py >= 0 && py < CANVAS_HEIGHT) {
             int index = (py * CANVAS_WIDTH + px) * 4;
             framebuffer[index] = static_cast<unsigned char>(color.r * 255);
             framebuffer[index + 1] = static_cast<unsigned char>(color.g * 255);
             framebuffer[index + 2] = static_cast<unsigned char>(color.b * 255);
             framebuffer[index + 3] = 255;
        }
        x += xInc;
        y += yInc;
    }
}

// Bresenham's Line Algorithm (2D)
void draw_line_bresenham(glm::vec2 p1, glm::vec2 p2, glm::vec3 color) {
    int x0 = static_cast<int>(p1.x);
    int y0 = static_cast<int>(p1.y);
    int x1 = static_cast<int>(p2.x);
    int y1 = static_cast<int>(p2.y);

    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        if (x0 >= 0 && x0 < CANVAS_WIDTH && y0 >= 0 && y0 < CANVAS_HEIGHT) {
            int index = (y0 * CANVAS_WIDTH + x0) * 4;
            framebuffer[index] = static_cast<unsigned char>(color.r * 255);
            framebuffer[index + 1] = static_cast<unsigned char>(color.g * 255);
            framebuffer[index + 2] = static_cast<unsigned char>(color.b * 255);
            framebuffer[index + 3] = 255;
        }

        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

// Edge Walking Rasterization (2D)
void draw_triangle_edge_walking(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, glm::vec3 color) {
    if (p1.y > p2.y) std::swap(p1, p2);
    if (p1.y > p3.y) std::swap(p1, p3);
    if (p2.y > p3.y) std::swap(p2, p3);

    draw_line_dda(p1, p2, color);
    draw_line_dda(p2, p3, color);
    draw_line_dda(p3, p1, color);

    int y_start = static_cast<int>(p1.y);
    int y_end = static_cast<int>(p3.y);

    for (int y = y_start; y <= y_end; y++) {
        if (y < 0 || y >= CANVAS_HEIGHT) continue;

        std::vector<float> x_intersections;
        if (y >= p1.y && y <= p2.y && p1.y != p2.y) x_intersections.push_back(p1.x + (y - p1.y) / (p2.y - p1.y) * (p2.x - p1.x));
        if (y >= p1.y && y <= p3.y && p1.y != p3.y) x_intersections.push_back(p1.x + (y - p1.y) / (p3.y - p1.y) * (p3.x - p1.x));
        if (y >= p2.y && y <= p3.y && p2.y != p3.y) x_intersections.push_back(p2.x + (y - p2.y) / (p3.y - p2.y) * (p3.x - p2.x));

        if (x_intersections.size() >= 2) {
            std::sort(x_intersections.begin(), x_intersections.end());
            int x_start = static_cast<int>(x_intersections.front());
            int x_end = static_cast<int>(x_intersections.back());
            for (int x = x_start; x <= x_end; x++) {
                // Simple 2D fill, ignore Z
                if (x >= 0 && x < CANVAS_WIDTH) {
                    int index = (y * CANVAS_WIDTH + x) * 4;
                    framebuffer[index] = static_cast<unsigned char>(color.r * 255);
                    framebuffer[index + 1] = static_cast<unsigned char>(color.g * 255);
                    framebuffer[index + 2] = static_cast<unsigned char>(color.b * 255);
                    framebuffer[index + 3] = 255;
                }
            }
        }
    }
}

// --- Task 2: Gouraud Shading & Z-Buffer ---

// Interpolation helper
float interpolate(float v1, float v2, float t) {
    return v1 + (v2 - v1) * t;
}

glm::vec3 interpolate(glm::vec3 v1, glm::vec3 v2, float t) {
    return v1 + (v2 - v1) * t;
}

// Rasterize Triangle with Gouraud Shading
void rasterize_triangle_gouraud(PixelVertex v1, PixelVertex v2, PixelVertex v3) {
    // Sort by Y
    if (v1.position.y > v2.position.y) std::swap(v1, v2);
    if (v1.position.y > v3.position.y) std::swap(v1, v3);
    if (v2.position.y > v3.position.y) std::swap(v2, v3);

    int y_start = static_cast<int>(std::ceil(v1.position.y));
    int y_end = static_cast<int>(std::floor(v3.position.y));

    for (int y = y_start; y <= y_end; y++) {
        if (y < 0 || y >= CANVAS_HEIGHT) continue;

        // Find start and end X, Z, Color for this scanline
        // We have 3 edges.
        // Long edge: v1 -> v3
        // Short edges: v1 -> v2 (top half), v2 -> v3 (bottom half)
        
        float t_long = 0;
        if (v3.position.y != v1.position.y)
            t_long = (float)(y - v1.position.y) / (v3.position.y - v1.position.y);
        
        PixelVertex p_long;
        p_long.position.x = interpolate(v1.position.x, v3.position.x, t_long);
        p_long.position.z = interpolate(v1.position.z, v3.position.z, t_long);
        p_long.color = interpolate(v1.color, v3.color, t_long);

        PixelVertex p_short;
        if (y < v2.position.y) {
            // Top half: v1 -> v2
            float t_short = 0;
            if (v2.position.y != v1.position.y)
                t_short = (float)(y - v1.position.y) / (v2.position.y - v1.position.y);
            
            p_short.position.x = interpolate(v1.position.x, v2.position.x, t_short);
            p_short.position.z = interpolate(v1.position.z, v2.position.z, t_short);
            p_short.color = interpolate(v1.color, v2.color, t_short);
        } else {
            // Bottom half: v2 -> v3
            float t_short = 0;
            if (v3.position.y != v2.position.y)
                t_short = (float)(y - v2.position.y) / (v3.position.y - v2.position.y);
            
            p_short.position.x = interpolate(v2.position.x, v3.position.x, t_short);
            p_short.position.z = interpolate(v2.position.z, v3.position.z, t_short);
            p_short.color = interpolate(v2.color, v3.color, t_short);
        }

        // Ensure p_left is actually left
        PixelVertex p_left = p_long;
        PixelVertex p_right = p_short;
        if (p_left.position.x > p_right.position.x) std::swap(p_left, p_right);

        int x_start = static_cast<int>(std::ceil(p_left.position.x));
        int x_end = static_cast<int>(std::floor(p_right.position.x));

        for (int x = x_start; x <= x_end; x++) {
            if (x < 0 || x >= CANVAS_WIDTH) continue;

            float t_x = 0;
            if (p_right.position.x != p_left.position.x)
                t_x = (float)(x - p_left.position.x) / (p_right.position.x - p_left.position.x);
            
            float z = interpolate(p_left.position.z, p_right.position.z, t_x);
            glm::vec3 color = interpolate(p_left.color, p_right.color, t_x);

            put_pixel(x, y, z, color);
        }
    }
}

// Lighting Calculation (Gouraud: Per Vertex)
glm::vec3 calculate_lighting(glm::vec3 pos, glm::vec3 normal, glm::vec3 lightPos, glm::vec3 viewPos, glm::vec3 objectColor) {
    // Ambient
    float ambientStrength = 0.1f;
    glm::vec3 ambient = ambientStrength * glm::vec3(1.0f, 1.0f, 1.0f);
  
    // Diffuse
    glm::vec3 norm = glm::normalize(normal);
    glm::vec3 lightDir = glm::normalize(lightPos - pos);
    float diff = std::max(glm::dot(norm, lightDir), 0.0f);
    glm::vec3 diffuse = diff * glm::vec3(1.0f, 1.0f, 1.0f);
    
    // Specular
    float specularStrength = 0.5f;
    glm::vec3 viewDir = glm::normalize(viewPos - pos);
    glm::vec3 reflectDir = glm::reflect(-lightDir, norm);  
    float spec = std::pow(std::max(glm::dot(viewDir, reflectDir), 0.0f), 32);
    glm::vec3 specular = specularStrength * spec * glm::vec3(1.0f, 1.0f, 1.0f);  
        
    return (ambient + diffuse + specular) * objectColor;
}

// Cube Data
std::vector<Vertex> cubeVertices = {
    // Front face
    {{-1.0f, -1.0f,  1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
    {{ 1.0f, -1.0f,  1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
    {{ 1.0f,  1.0f,  1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
    {{-1.0f, -1.0f,  1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
    {{ 1.0f,  1.0f,  1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
    {{-1.0f,  1.0f,  1.0f}, {1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
    
    // Back face
    {{-1.0f, -1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
    {{-1.0f,  1.0f, -1.0f}, {1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
    {{ 1.0f,  1.0f, -1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},
    {{-1.0f, -1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
    {{ 1.0f,  1.0f, -1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},
    {{ 1.0f, -1.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},

    // Top face
    {{-1.0f,  1.0f, -1.0f}, {1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
    {{-1.0f,  1.0f,  1.0f}, {1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
    {{ 1.0f,  1.0f,  1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
    {{-1.0f,  1.0f, -1.0f}, {1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
    {{ 1.0f,  1.0f,  1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
    {{ 1.0f,  1.0f, -1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},

    // Bottom face
    {{-1.0f, -1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
    {{ 1.0f, -1.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
    {{ 1.0f, -1.0f,  1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
    {{-1.0f, -1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
    {{ 1.0f, -1.0f,  1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
    {{-1.0f, -1.0f,  1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},

    // Right face
    {{ 1.0f, -1.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
    {{ 1.0f,  1.0f, -1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
    {{ 1.0f,  1.0f,  1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
    {{ 1.0f, -1.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
    {{ 1.0f,  1.0f,  1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
    {{ 1.0f, -1.0f,  1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},

    // Left face
    {{-1.0f, -1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},
    {{-1.0f, -1.0f,  1.0f}, {1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},
    {{-1.0f,  1.0f,  1.0f}, {1.0f, 1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},
    {{-1.0f, -1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},
    {{-1.0f,  1.0f,  1.0f}, {1.0f, 1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},
    {{-1.0f,  1.0f, -1.0f}, {1.0f, 1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},
};

// Tetrahedron Data
std::vector<Vertex> tetrahedronVertices = {
    // Face 1 (0, 1, 2)
    {{ 0.0f,  1.0f,  0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.5f, 0.5f}}, // Top
    {{-1.0f, -1.0f,  1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.5f, 0.5f}}, // Front Left
    {{ 1.0f, -1.0f,  1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.5f, 0.5f}}, // Front Right

    // Face 2 (0, 2, 3)
    {{ 0.0f,  1.0f,  0.0f}, {1.0f, 0.0f, 0.0f}, {0.5f, 0.5f, -0.5f}}, // Top
    {{ 1.0f, -1.0f,  1.0f}, {0.0f, 0.0f, 1.0f}, {0.5f, 0.5f, -0.5f}}, // Front Right
    {{ 0.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 0.0f}, {0.5f, 0.5f, -0.5f}}, // Back

    // Face 3 (0, 3, 1)
    {{ 0.0f,  1.0f,  0.0f}, {1.0f, 0.0f, 0.0f}, {-0.5f, 0.5f, -0.5f}}, // Top
    {{ 0.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 0.0f}, {-0.5f, 0.5f, -0.5f}}, // Back
    {{-1.0f, -1.0f,  1.0f}, {0.0f, 1.0f, 0.0f}, {-0.5f, 0.5f, -0.5f}}, // Front Left

    // Face 4 (1, 3, 2) - Base
    {{-1.0f, -1.0f,  1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
    {{ 0.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
    {{ 1.0f, -1.0f,  1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},
};

// Rasterize Triangle with Phong Shading (Per-Pixel)
void rasterize_triangle_phong(PixelVertex v1, PixelVertex v2, PixelVertex v3, glm::vec3 lightPos, glm::vec3 cameraPos) {
    // Sort by Y
    if (v1.position.y > v2.position.y) std::swap(v1, v2);
    if (v1.position.y > v3.position.y) std::swap(v1, v3);
    if (v2.position.y > v3.position.y) std::swap(v2, v3);

    int y_start = static_cast<int>(std::ceil(v1.position.y));
    int y_end = static_cast<int>(std::floor(v3.position.y));

    for (int y = y_start; y <= y_end; y++) {
        if (y < 0 || y >= CANVAS_HEIGHT) continue;

        float t_long = 0;
        if (v3.position.y != v1.position.y)
            t_long = (float)(y - v1.position.y) / (v3.position.y - v1.position.y);
        
        PixelVertex p_long;
        p_long.position.x = interpolate(v1.position.x, v3.position.x, t_long);
        p_long.position.z = interpolate(v1.position.z, v3.position.z, t_long);
        p_long.color = interpolate(v1.color, v3.color, t_long); // Base color
        p_long.normal = interpolate(v1.normal, v3.normal, t_long);
        p_long.worldPos = interpolate(v1.worldPos, v3.worldPos, t_long);

        PixelVertex p_short;
        if (y < v2.position.y) {
            float t_short = 0;
            if (v2.position.y != v1.position.y)
                t_short = (float)(y - v1.position.y) / (v2.position.y - v1.position.y);
            
            p_short.position.x = interpolate(v1.position.x, v2.position.x, t_short);
            p_short.position.z = interpolate(v1.position.z, v2.position.z, t_short);
            p_short.color = interpolate(v1.color, v2.color, t_short);
            p_short.normal = interpolate(v1.normal, v2.normal, t_short);
            p_short.worldPos = interpolate(v1.worldPos, v2.worldPos, t_short);
        } else {
            float t_short = 0;
            if (v3.position.y != v2.position.y)
                t_short = (float)(y - v2.position.y) / (v3.position.y - v2.position.y);
            
            p_short.position.x = interpolate(v2.position.x, v3.position.x, t_short);
            p_short.position.z = interpolate(v2.position.z, v3.position.z, t_short);
            p_short.color = interpolate(v2.color, v3.color, t_short);
            p_short.normal = interpolate(v2.normal, v3.normal, t_short);
            p_short.worldPos = interpolate(v2.worldPos, v3.worldPos, t_short);
        }

        PixelVertex p_left = p_long;
        PixelVertex p_right = p_short;
        if (p_left.position.x > p_right.position.x) std::swap(p_left, p_right);

        int x_start = static_cast<int>(std::ceil(p_left.position.x));
        int x_end = static_cast<int>(std::floor(p_right.position.x));

        for (int x = x_start; x <= x_end; x++) {
            if (x < 0 || x >= CANVAS_WIDTH) continue;

            float t_x = 0;
            if (p_right.position.x != p_left.position.x)
                t_x = (float)(x - p_left.position.x) / (p_right.position.x - p_left.position.x);
            
            float z = interpolate(p_left.position.z, p_right.position.z, t_x);
            glm::vec3 baseColor = interpolate(p_left.color, p_right.color, t_x);
            glm::vec3 normal = interpolate(p_left.normal, p_right.normal, t_x);
            glm::vec3 worldPos = interpolate(p_left.worldPos, p_right.worldPos, t_x);

            // Calculate Lighting Per Pixel
            glm::vec3 finalColor = calculate_lighting(worldPos, normal, lightPos, cameraPos, baseColor);

            put_pixel(x, y, z, finalColor);
        }
    }
}

int main()
{
    // Initialize GLFW
    if (!glfwInit())
        return -1;

    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create Window
    GLFWwindow* window = glfwCreateWindow(1280, 720, "CG HW2", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Initialize ImGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.IniFilename = nullptr; // disable imgui.ini load/save
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Create Texture
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Camera & Light
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
    glm::vec3 lightPos = glm::vec3(1.2f, 1.0f, 2.0f);
    float rotationAngle = 0.0f;
    
    // State
    int currentTask = 0; // 0: Task 1 (2D), 1: Task 2 (3D)
    
    // Task 1 State
    bool showFill = true;
    bool showDDA = false;
    bool showBresenham = false;

    // Task 2 State
    int currentModel = 0; // 0: Cube, 1: Tetrahedron
    bool showWireframe = false;
    bool usePhong = false;

    // Main Loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Start ImGUI frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Logic
        clear_buffers(glm::vec3(0.1f, 0.1f, 0.1f)); // Clear to dark gray

        double t1 = glfwGetTime();

        if (currentTask == 0) {
            // Task 1: 2D Triangle
            glm::vec2 p1(100, 100);
            glm::vec2 p2(400, 300);
            glm::vec2 p3(200, 500);
            glm::vec3 color(1.0f, 0.5f, 0.2f); // Orange

            if (showFill) {
                draw_triangle_edge_walking(p1, p2, p3, color);
            }
            if (showDDA) {
                draw_line_dda(p1, p2, glm::vec3(1.0f));
                draw_line_dda(p2, p3, glm::vec3(1.0f));
                draw_line_dda(p3, p1, glm::vec3(1.0f));
            }
            if (showBresenham) {
                // Draw with a different color (e.g., Cyan) to distinguish
                glm::vec3 bresColor(0.0f, 1.0f, 1.0f);
                draw_line_bresenham(p1, p2, bresColor);
                draw_line_bresenham(p2, p3, bresColor);
                draw_line_bresenham(p3, p1, bresColor);
            }
        }
        else if (currentTask == 1) {
            // Task 2: 3D Scene
            rotationAngle += 0.002f;

            // Matrices
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::rotate(model, rotationAngle, glm::vec3(0.5f, 1.0f, 0.0f));
            
            glm::mat4 view = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)CANVAS_WIDTH / (float)CANVAS_HEIGHT, 0.1f, 100.0f);
            
            glm::mat4 mvp = projection * view * model;
            glm::mat4 normalMatrix = glm::transpose(glm::inverse(model));

            const std::vector<Vertex>& vertices = (currentModel == 0) ? cubeVertices : tetrahedronVertices;

            for (size_t i = 0; i < vertices.size(); i += 3) {
                PixelVertex pVerts[3];
                for (int j = 0; j < 3; j++) {
                    Vertex v = vertices[i + j];
                    
                    // 1. World Position
                    glm::vec4 worldPos4 = model * glm::vec4(v.position, 1.0f);
                    glm::vec3 worldPos = glm::vec3(worldPos4);
                    
                    // 2. Normal in World Space
                    glm::vec3 normal = glm::normalize(glm::vec3(normalMatrix * glm::vec4(v.normal, 0.0f)));
                    
                    // 3. Calculate Lighting (Gouraud - Per Vertex)
                    // Even if using Phong, we calculate this for Gouraud fallback or debug
                    glm::vec3 litColor = calculate_lighting(worldPos, normal, lightPos, cameraPos, v.color);
                    
                    // 4. Project to Clip Space
                    glm::vec4 clipPos = mvp * glm::vec4(v.position, 1.0f);
                    
                    // 5. Perspective Divide -> NDC
                    glm::vec3 ndc = glm::vec3(clipPos) / clipPos.w;
                    
                    // 6. Viewport Transform -> Screen Space
                    pVerts[j].position.x = (ndc.x + 1.0f) * 0.5f * CANVAS_WIDTH;
                    pVerts[j].position.y = (1.0f - ndc.y) * 0.5f * CANVAS_HEIGHT; 
                    
                    pVerts[j].position.z = ndc.z; // Depth
                    pVerts[j].color = litColor;
                    pVerts[j].normal = normal;
                    pVerts[j].worldPos = worldPos;
                }
                
                // Backface Culling
                glm::vec3 v0 = pVerts[0].position;
                glm::vec3 v1 = pVerts[1].position;
                glm::vec3 v2 = pVerts[2].position;
                float area = (v1.x - v0.x) * (v2.y - v0.y) - (v1.y - v0.y) * (v2.x - v0.x);
                
                if (area > 0) { 
                    if (showWireframe) {
                        // Draw Wireframe (using DDA on projected points)
                        // Note: This is a 2D wireframe on top of the 3D render
                        draw_line_dda(glm::vec2(v0.x, v0.y), glm::vec2(v1.x, v1.y), glm::vec3(1.0f));
                        draw_line_dda(glm::vec2(v1.x, v1.y), glm::vec2(v2.x, v2.y), glm::vec3(1.0f));
                        draw_line_dda(glm::vec2(v2.x, v2.y), glm::vec2(v0.x, v0.y), glm::vec3(1.0f));
                    } else {
                        if (usePhong) {
                            rasterize_triangle_phong(pVerts[0], pVerts[1], pVerts[2], lightPos, cameraPos);
                        } else {
                            rasterize_triangle_gouraud(pVerts[0], pVerts[1], pVerts[2]);
                        }
                    }
                }
            }
        }

        double t2 = glfwGetTime();
        double rasterTime = (t2 - t1) * 1000.0;

        // Update Texture
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CANVAS_WIDTH, CANVAS_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, framebuffer.data());

        // ImGUI Window
        ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_FirstUseEver);
        ImGui::Begin("Controls");
        
        ImGui::RadioButton("Task 1: 2D Triangle", &currentTask, 0);
        ImGui::RadioButton("Task 2: 3D Scene", &currentTask, 1);
        
        ImGui::Separator();

        if (currentTask == 0) {
            ImGui::Text("Task 1 Controls");
            ImGui::Checkbox("Fill (Edge-Walking)", &showFill);
            ImGui::Checkbox("Edges (DDA)", &showDDA);
            ImGui::Checkbox("Edges (Bresenham)", &showBresenham);
        }
        else {
            ImGui::Text("Task 2 Controls");
            const char* items[] = { "Cube", "Tetrahedron" };
            ImGui::Combo("Model", &currentModel, items, IM_ARRAYSIZE(items));
            
            ImGui::Checkbox("Wireframe Mode", &showWireframe);
            ImGui::Checkbox("Phong Shading (Per-Pixel)", &usePhong);
            
            ImGui::DragFloat3("Light Pos", &lightPos.x, 0.1f);
        }

        ImGui::Separator();
        ImGui::Text("Rasterization Time: %.3f ms", rasterTime);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        
        ImGui::End();

        ImGui::SetNextWindowPos(ImVec2(350, 20), ImGuiCond_FirstUseEver);
        ImGui::Begin("Rasterizer Output");
        ImGui::Image((void*)(intptr_t)textureID, ImVec2(CANVAS_WIDTH, CANVAS_HEIGHT));
        ImGui::End();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
