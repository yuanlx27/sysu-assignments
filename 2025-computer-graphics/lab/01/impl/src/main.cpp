#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <string>

// View settings
float viewD = 5.0f;
bool usePerspective = false; // false = Ortho, true = Perspective
int viewPoint = 0; // 0 = (0,0,d), 1 = (0, 0.5d, d)
bool show3D = false; // Toggle between 2D Initials and 3D Surname

// Rotation settings
float rotationX = 0.0f;
float rotationY = 0.0f;
float rotationZ = 0.0f;
float rotationSpeed = 50.0f;
bool autoRotate = false;
int rotationAxis = 1; // 0=X, 1=Y, 2=Z

// Primitive settings
enum PrimitiveMode {
    PRIM_TRIANGLES,
    PRIM_TRIANGLE_STRIP,
    PRIM_QUADS,
    PRIM_QUAD_STRIP
};
PrimitiveMode currentMode = PRIM_TRIANGLES;
int vertexCount = 0;

// Helper to draw a quad using different primitives
// Vertices order: BL, BR, TR, TL
void drawQuad(glm::vec2 v1, glm::vec2 v2, glm::vec2 v3, glm::vec2 v4) {
    switch (currentMode) {
        case PRIM_TRIANGLES:
            glBegin(GL_TRIANGLES);
            // Triangle 1: BL, BR, TL
            glVertex2f(v1.x, v1.y);
            glVertex2f(v2.x, v2.y);
            glVertex2f(v4.x, v4.y);
            // Triangle 2: BR, TR, TL
            glVertex2f(v2.x, v2.y);
            glVertex2f(v3.x, v3.y);
            glVertex2f(v4.x, v4.y);
            glEnd();
            vertexCount += 6;
            break;
        case PRIM_TRIANGLE_STRIP:
            glBegin(GL_TRIANGLE_STRIP);
            // Order: BL, BR, TL, TR
            glVertex2f(v1.x, v1.y);
            glVertex2f(v2.x, v2.y);
            glVertex2f(v4.x, v4.y);
            glVertex2f(v3.x, v3.y);
            glEnd();
            vertexCount += 4;
            break;
        case PRIM_QUADS:
            glBegin(GL_QUADS);
            // Order: BL, BR, TR, TL
            glVertex2f(v1.x, v1.y);
            glVertex2f(v2.x, v2.y);
            glVertex2f(v3.x, v3.y);
            glVertex2f(v4.x, v4.y);
            glEnd();
            vertexCount += 4;
            break;
        case PRIM_QUAD_STRIP:
            glBegin(GL_QUAD_STRIP);
            // Order: BL, BR, TL, TR (Similar to Triangle Strip but defines quads)
            glVertex2f(v1.x, v1.y);
            glVertex2f(v2.x, v2.y);
            glVertex2f(v4.x, v4.y);
            glVertex2f(v3.x, v3.y);
            glEnd();
            vertexCount += 4;
            break;
    }
}

// Helper to draw a rectangle (axis aligned)
void drawRect(float x, float y, float w, float h) {
    glm::vec2 v1(x, y);         // BL
    glm::vec2 v2(x + w, y);     // BR
    glm::vec2 v3(x + w, y + h); // TR
    glm::vec2 v4(x, y + h);     // TL
    drawQuad(v1, v2, v3, v4);
}

// Draw Letter Y
// Centered roughly at (0,0) with height ~1.0
void drawLetterY() {
    glColor3f(1.0f, 0.0f, 0.0f); // Red

    // Vertical stem (bottom half)
    // x: -0.05 to 0.05, y: -0.5 to 0.0
    drawRect(-0.05f, -0.5f, 0.1f, 0.5f);

    // Left arm
    // Left arm quad
    // BL: (-0.05, 0.0), BR: (0.05, 0.0)
    // TL: (-0.35, 0.5), TR: (-0.25, 0.5)
    // Wait, drawQuad expects BL, BR, TR, TL
    // Let's map them:
    // v1 (BL) = (-0.05, 0.0)
    // v2 (BR) = (0.05, 0.0)
    // v3 (TR) = (-0.25, 0.5)
    // v4 (TL) = (-0.35, 0.5) -- Wait, this is twisted if I use standard rect logic.
    // Let's visualize:
    // (-0.35, 0.5)   (-0.25, 0.5)
    //      TL           TR
    //
    //      BL           BR
    // (-0.05, 0.0)   (0.05, 0.0)
    // This is a valid quad.
    drawQuad(glm::vec2(-0.05f, 0.0f), glm::vec2(0.05f, 0.0f), glm::vec2(-0.25f, 0.5f), glm::vec2(-0.35f, 0.5f));

    // Right arm quad
    // BL: (-0.05, 0.0), BR: (0.05, 0.0)
    // TL: (0.25, 0.5), TR: (0.35, 0.5)
    // v1 (BL) = (-0.05, 0.0)
    // v2 (BR) = (0.05, 0.0)
    // v3 (TR) = (0.35, 0.5)
    // v4 (TL) = (0.25, 0.5)
    drawQuad(glm::vec2(-0.05f, 0.0f), glm::vec2(0.05f, 0.0f), glm::vec2(0.35f, 0.5f), glm::vec2(0.25f, 0.5f));
}

// Draw Letter L
void drawLetterL() {
    glColor3f(0.0f, 1.0f, 0.0f); // Green

    // Vertical stem
    // x: -0.25 to -0.15, y: -0.5 to 0.5
    drawRect(-0.25f, -0.5f, 0.1f, 1.0f);

    // Bottom horizontal
    // x: -0.15 to 0.25, y: -0.5 to -0.4
    drawRect(-0.15f, -0.5f, 0.4f, 0.1f);
}

// Draw Letter X
void drawLetterX() {
    glColor3f(0.0f, 0.0f, 1.0f); // Blue

    // Stroke 1: \ (Top-Left to Bottom-Right)
    // TL: (-0.35, 0.5), TR: (-0.25, 0.5)
    // BL: (0.25, -0.5), BR: (0.35, -0.5)
    // v1(BL), v2(BR), v3(TR), v4(TL)
    drawQuad(glm::vec2(0.25f, -0.5f), glm::vec2(0.35f, -0.5f), glm::vec2(-0.25f, 0.5f), glm::vec2(-0.35f, 0.5f));

    // Stroke 2: / (Bottom-Left to Top-Right)
    // BL: (-0.35, -0.5), BR: (-0.25, -0.5)
    // TL: (0.25, 0.5), TR: (0.35, 0.5)
    // v1(BL), v2(BR), v3(TR), v4(TL)
    drawQuad(glm::vec2(-0.35f, -0.5f), glm::vec2(-0.25f, -0.5f), glm::vec2(0.35f, 0.5f), glm::vec2(0.25f, 0.5f));
}

// Helper to draw an extruded quad (prism)
// v1-v4 are 2D vertices (z=0)
// thickness is the extrusion depth along Z
void drawExtrudedQuad(glm::vec2 v1, glm::vec2 v2, glm::vec2 v3, glm::vec2 v4, float thickness) {
    float zFront = thickness / 2.0f;
    float zBack = -thickness / 2.0f;

    // Front face
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(v1.x, v1.y, zFront);
    glVertex3f(v2.x, v2.y, zFront);
    glVertex3f(v3.x, v3.y, zFront);
    glVertex3f(v4.x, v4.y, zFront);
    glEnd();

    // Back face
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(v1.x, v1.y, zBack);
    glVertex3f(v4.x, v4.y, zBack);
    glVertex3f(v3.x, v3.y, zBack);
    glVertex3f(v2.x, v2.y, zBack);
    glEnd();

    // Side faces
    glBegin(GL_QUADS);
    
    // Bottom (v1-v2)
    // Normal approx (0, -1, 0) - strictly depends on orientation but good enough for simple shapes
    glVertex3f(v1.x, v1.y, zBack);
    glVertex3f(v2.x, v2.y, zBack);
    glVertex3f(v2.x, v2.y, zFront);
    glVertex3f(v1.x, v1.y, zFront);

    // Right (v2-v3)
    glVertex3f(v2.x, v2.y, zBack);
    glVertex3f(v3.x, v3.y, zBack);
    glVertex3f(v3.x, v3.y, zFront);
    glVertex3f(v2.x, v2.y, zFront);

    // Top (v3-v4)
    glVertex3f(v3.x, v3.y, zBack);
    glVertex3f(v4.x, v4.y, zBack);
    glVertex3f(v4.x, v4.y, zFront);
    glVertex3f(v3.x, v3.y, zFront);

    // Left (v4-v1)
    glVertex3f(v4.x, v4.y, zBack);
    glVertex3f(v1.x, v1.y, zBack);
    glVertex3f(v1.x, v1.y, zFront);
    glVertex3f(v4.x, v4.y, zFront);
    
    glEnd();
}

// Draw Letter Y in 3D
void drawLetterY3D() {
    glColor3f(1.0f, 0.5f, 0.0f); // Orange for 3D
    float thickness = 0.2f;

    // Vertical stem
    drawExtrudedQuad(glm::vec2(-0.05f, -0.5f), glm::vec2(0.05f, -0.5f), glm::vec2(0.05f, 0.0f), glm::vec2(-0.05f, 0.0f), thickness);

    // Left arm
    drawExtrudedQuad(glm::vec2(-0.05f, 0.0f), glm::vec2(0.05f, 0.0f), glm::vec2(-0.25f, 0.5f), glm::vec2(-0.35f, 0.5f), thickness);

    // Right arm
    drawExtrudedQuad(glm::vec2(-0.05f, 0.0f), glm::vec2(0.05f, 0.0f), glm::vec2(0.35f, 0.5f), glm::vec2(0.25f, 0.5f), thickness);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main() {
    if (!glfwInit())
        return -1;

    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Use default (compat) for simple GL calls like glBegin

    GLFWwindow* window = glfwCreateWindow(1280, 720, "CG HW1 - YLX", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = NULL; // Disable saving .ini file
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    double lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        double currentTime = glfwGetTime();
        float deltaTime = (float)(currentTime - lastTime);
        lastTime = currentTime;

        // Update rotation
        if (show3D && autoRotate) {
            float angleChange = rotationSpeed * deltaTime;
            if (rotationAxis == 0) rotationX += angleChange;
            if (rotationAxis == 1) rotationY += angleChange;
            if (rotationAxis == 2) rotationZ += angleChange;
            
            if (rotationX > 360.0f) rotationX -= 360.0f;
            if (rotationY > 360.0f) rotationY -= 360.0f;
            if (rotationZ > 360.0f) rotationZ -= 360.0f;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Control Panel");
        ImGui::Text("Application running");
        
        ImGui::Separator();
        ImGui::Text("Mode Selection");
        if (ImGui::RadioButton("2D Initials (YLX)", !show3D)) show3D = false;
        ImGui::SameLine();
        if (ImGui::RadioButton("3D Surname (Y)", show3D)) show3D = true;

        if (show3D) {
            ImGui::Separator();
            ImGui::Text("Rotation Control");
            ImGui::Checkbox("Auto Rotate", &autoRotate);
            ImGui::SliderFloat("Speed", &rotationSpeed, 0.0f, 200.0f);
            ImGui::Text("Axis:");
            ImGui::RadioButton("X", &rotationAxis, 0); ImGui::SameLine();
            ImGui::RadioButton("Y", &rotationAxis, 1); ImGui::SameLine();
            ImGui::RadioButton("Z", &rotationAxis, 2);
            
            ImGui::Text("Manual Rotation:");
            ImGui::SliderFloat("Rot X", &rotationX, 0.0f, 360.0f);
            ImGui::SliderFloat("Rot Y", &rotationY, 0.0f, 360.0f);
            ImGui::SliderFloat("Rot Z", &rotationZ, 0.0f, 360.0f);
        }

        ImGui::Separator();
        ImGui::Text("Projection & View");
        ImGui::SliderFloat("Distance (d)", &viewD, 1.0f, 20.0f);
        
        if (ImGui::RadioButton("Orthographic", !usePerspective)) usePerspective = false; 
        ImGui::SameLine();
        if (ImGui::RadioButton("Perspective", usePerspective)) usePerspective = true;
        
        ImGui::Text("View Point:");
        ImGui::RadioButton("(0, 0, d)", &viewPoint, 0);
        ImGui::RadioButton("(0, 0.5d, d)", &viewPoint, 1);

        ImGui::Separator();
        ImGui::Text("Primitives & Stats");
        if (ImGui::RadioButton("GL_TRIANGLES", currentMode == PRIM_TRIANGLES)) currentMode = PRIM_TRIANGLES;
        if (ImGui::RadioButton("GL_TRIANGLE_STRIP", currentMode == PRIM_TRIANGLE_STRIP)) currentMode = PRIM_TRIANGLE_STRIP;
        if (ImGui::RadioButton("GL_QUADS", currentMode == PRIM_QUADS)) currentMode = PRIM_QUADS;
        if (ImGui::RadioButton("GL_QUAD_STRIP", currentMode == PRIM_QUAD_STRIP)) currentMode = PRIM_QUAD_STRIP;
        
        ImGui::Text("Vertex Count: %d", vertexCount);

        ImGui::End();

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Setup Projection
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        
        float aspect = (float)display_w / (float)display_h;
        
        if (usePerspective) {
            // Perspective: fov, aspect, near, far
            glm::mat4 proj = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
            glLoadMatrixf(glm::value_ptr(proj));
        } else {
            // Orthographic: left, right, bottom, top, near, far
            // Use a fixed height to keep scale somewhat consistent
            float orthoHeight = 4.0f;
            float orthoWidth = orthoHeight * aspect;
            glOrtho(-orthoWidth/2.0f, orthoWidth/2.0f, -orthoHeight/2.0f, orthoHeight/2.0f, 0.1f, 100.0f);
        }

        // Setup View
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        
        glm::vec3 eye;
        if (viewPoint == 0) {
            eye = glm::vec3(0.0f, 0.0f, viewD);
        } else {
            eye = glm::vec3(0.0f, 0.5f * viewD, viewD);
        }
        glm::vec3 center(0.0f, 0.0f, 0.0f);
        glm::vec3 up(0.0f, 1.0f, 0.0f);
        
        glm::mat4 view = glm::lookAt(eye, center, up);
        glLoadMatrixf(glm::value_ptr(view));

        // Reset vertex count for this frame
        vertexCount = 0;

        if (!show3D) {
            // Draw Initials: Y L X
            // Draw Y (Left)
            glPushMatrix();
            glTranslatef(-1.2f, 0.0f, 0.0f);
            drawLetterY();
            glPopMatrix();

            // Draw L (Center)
            glPushMatrix();
            glTranslatef(0.0f, 0.0f, 0.0f);
            drawLetterL();
            glPopMatrix();

            // Draw X (Right)
            glPushMatrix();
            glTranslatef(1.2f, 0.0f, 0.0f);
            drawLetterX();
            glPopMatrix();
        } else {
            // Draw 3D Surname Initial (Y)
            // Enable depth test for 3D
            glEnable(GL_DEPTH_TEST);
            glClear(GL_DEPTH_BUFFER_BIT); // Clear depth buffer

            glPushMatrix();
            // Apply rotations
            glRotatef(rotationX, 1.0f, 0.0f, 0.0f);
            glRotatef(rotationY, 0.0f, 1.0f, 0.0f);
            glRotatef(rotationZ, 0.0f, 0.0f, 1.0f);

            // Center is already (0,0,0)
            drawLetterY3D();
            glPopMatrix();

            glDisable(GL_DEPTH_TEST);
        }

        // Render ImGui
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
