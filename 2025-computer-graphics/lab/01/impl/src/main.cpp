#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <cmath>

#ifdef _WIN32
#pragma comment(lib, "opengl32.lib")
#endif

// Global variables for rotation
float rotationX = 0.0f;
float rotationY = 0.0f;
float rotationZ = 0.0f;
float rotationSpeed = 50.0f;

// View parameters
float viewDistance = 5.0f;
float viewOffsetY = 0.0f;
bool useOrthographic = true;
bool usePerspective = false;

// Drawing mode
enum DrawMode {
    MODE_2D_LETTERS,
    MODE_3D_LETTER
};
DrawMode currentMode = MODE_2D_LETTERS;

// Letter drawing functions
void drawLetterY() {
    glBegin(GL_TRIANGLES);
    // Left diagonal arm
    glVertex2f(-0.5f, 0.8f);
    glVertex2f(-0.3f, 0.8f);
    glVertex2f(0.1f, 0.1f);
    
    glVertex2f(-0.5f, 0.8f);
    glVertex2f(0.1f, 0.1f);
    glVertex2f(-0.1f, 0.1f);
    
    // Right diagonal arm
    glVertex2f(0.3f, 0.8f);
    glVertex2f(0.5f, 0.8f);
    glVertex2f(0.1f, 0.1f);
    
    glVertex2f(0.3f, 0.8f);
    glVertex2f(0.1f, 0.1f);
    glVertex2f(-0.1f, 0.1f);
    
    // Vertical stem
    glVertex2f(-0.1f, 0.1f);
    glVertex2f(0.1f, 0.1f);
    glVertex2f(0.1f, -0.8f);
    
    glVertex2f(-0.1f, 0.1f);
    glVertex2f(0.1f, -0.8f);
    glVertex2f(-0.1f, -0.8f);
    glEnd();
}

void drawLetterL() {
    glBegin(GL_TRIANGLES);
    // Vertical part
    glVertex2f(-0.5f, 0.8f);
    glVertex2f(-0.5f, -0.8f);
    glVertex2f(-0.3f, -0.8f);
    
    glVertex2f(-0.5f, 0.8f);
    glVertex2f(-0.3f, -0.8f);
    glVertex2f(-0.3f, 0.8f);
    
    // Horizontal part
    glVertex2f(-0.5f, -0.8f);
    glVertex2f(0.3f, -0.8f);
    glVertex2f(0.3f, -0.6f);
    
    glVertex2f(-0.5f, -0.8f);
    glVertex2f(0.3f, -0.6f);
    glVertex2f(-0.5f, -0.6f);
    glEnd();
}

void drawLetterX() {
    glBegin(GL_TRIANGLES);
    // Top-left to bottom-right diagonal
    glVertex2f(-0.5f, 0.8f);
    glVertex2f(-0.3f, 0.8f);
    glVertex2f(0.5f, -0.8f);
    
    glVertex2f(-0.5f, 0.8f);
    glVertex2f(0.5f, -0.8f);
    glVertex2f(0.3f, -0.8f);
    
    // Top-right to bottom-left diagonal
    glVertex2f(0.5f, 0.8f);
    glVertex2f(0.3f, 0.8f);
    glVertex2f(-0.5f, -0.8f);
    
    glVertex2f(0.5f, 0.8f);
    glVertex2f(-0.5f, -0.8f);
    glVertex2f(-0.3f, -0.8f);
    glEnd();
}

void drawLetter3D(float depth) {
    // Front face (Y letter)
    glColor3f(0.8f, 0.2f, 0.2f);
    glBegin(GL_TRIANGLES);
    // Left arm - front (trapezoid split into triangles)
    glVertex3f(-0.5f, 0.8f, depth/2);
    glVertex3f(-0.3f, 0.8f, depth/2);
    glVertex3f(-0.1f, 0.1f, depth/2);
    
    glVertex3f(-0.5f, 0.8f, depth/2);
    glVertex3f(-0.1f, 0.1f, depth/2);
    glVertex3f(-0.3f, 0.1f, depth/2);
    
    // Right arm - front (trapezoid split into triangles)
    glVertex3f(0.5f, 0.8f, depth/2);
    glVertex3f(0.3f, 0.8f, depth/2);
    glVertex3f(0.1f, 0.1f, depth/2);
    
    glVertex3f(0.5f, 0.8f, depth/2);
    glVertex3f(0.1f, 0.1f, depth/2);
    glVertex3f(0.3f, 0.1f, depth/2);
    glEnd();
    
    glBegin(GL_QUADS);
    // Vertical stem - front
    glVertex3f(-0.1f, 0.1f, depth/2);
    glVertex3f(0.1f, 0.1f, depth/2);
    glVertex3f(0.1f, -0.8f, depth/2);
    glVertex3f(-0.1f, -0.8f, depth/2);
    glEnd();
    
    // Back face
    glColor3f(0.6f, 0.1f, 0.1f);
    glBegin(GL_TRIANGLES);
    // Left arm - back
    glVertex3f(-0.5f, 0.8f, -depth/2);
    glVertex3f(-0.1f, 0.1f, -depth/2);
    glVertex3f(-0.3f, 0.8f, -depth/2);
    
    glVertex3f(-0.5f, 0.8f, -depth/2);
    glVertex3f(-0.3f, 0.1f, -depth/2);
    glVertex3f(-0.1f, 0.1f, -depth/2);
    
    // Right arm - back
    glVertex3f(0.3f, 0.8f, -depth/2);
    glVertex3f(0.1f, 0.1f, -depth/2);
    glVertex3f(0.5f, 0.8f, -depth/2);
    
    glVertex3f(0.5f, 0.8f, -depth/2);
    glVertex3f(0.3f, 0.1f, -depth/2);
    glVertex3f(0.1f, 0.1f, -depth/2);
    glEnd();
    
    glBegin(GL_QUADS);
    // Vertical stem - back
    glVertex3f(-0.1f, 0.1f, -depth/2);
    glVertex3f(-0.1f, -0.8f, -depth/2);
    glVertex3f(0.1f, -0.8f, -depth/2);
    glVertex3f(0.1f, 0.1f, -depth/2);
    glEnd();
    
    // Side faces
    glColor3f(0.7f, 0.15f, 0.15f);
    glBegin(GL_QUADS);
    // Left arm outer edge
    glVertex3f(-0.5f, 0.8f, depth/2);
    glVertex3f(-0.3f, 0.1f, depth/2);
    glVertex3f(-0.3f, 0.1f, -depth/2);
    glVertex3f(-0.5f, 0.8f, -depth/2);
    
    // Left arm top
    glVertex3f(-0.5f, 0.8f, depth/2);
    glVertex3f(-0.5f, 0.8f, -depth/2);
    glVertex3f(-0.3f, 0.8f, -depth/2);
    glVertex3f(-0.3f, 0.8f, depth/2);
    
    // Left arm inner edge
    glVertex3f(-0.3f, 0.8f, depth/2);
    glVertex3f(-0.3f, 0.8f, -depth/2);
    glVertex3f(-0.1f, 0.1f, -depth/2);
    glVertex3f(-0.1f, 0.1f, depth/2);
    
    // Left arm bottom edge
    glVertex3f(-0.3f, 0.1f, depth/2);
    glVertex3f(-0.1f, 0.1f, depth/2);
    glVertex3f(-0.1f, 0.1f, -depth/2);
    glVertex3f(-0.3f, 0.1f, -depth/2);
    
    // Right arm outer edge
    glVertex3f(0.5f, 0.8f, depth/2);
    glVertex3f(0.5f, 0.8f, -depth/2);
    glVertex3f(0.3f, 0.1f, -depth/2);
    glVertex3f(0.3f, 0.1f, depth/2);
    
    // Right arm top
    glVertex3f(0.3f, 0.8f, depth/2);
    glVertex3f(0.3f, 0.8f, -depth/2);
    glVertex3f(0.5f, 0.8f, -depth/2);
    glVertex3f(0.5f, 0.8f, depth/2);
    
    // Right arm inner edge
    glVertex3f(0.3f, 0.8f, depth/2);
    glVertex3f(0.1f, 0.1f, depth/2);
    glVertex3f(0.1f, 0.1f, -depth/2);
    glVertex3f(0.3f, 0.8f, -depth/2);
    
    // Right arm bottom edge
    glVertex3f(0.1f, 0.1f, depth/2);
    glVertex3f(0.3f, 0.1f, depth/2);
    glVertex3f(0.3f, 0.1f, -depth/2);
    glVertex3f(0.1f, 0.1f, -depth/2);
    
    // Stem left side
    glVertex3f(-0.1f, 0.1f, depth/2);
    glVertex3f(-0.1f, -0.8f, depth/2);
    glVertex3f(-0.1f, -0.8f, -depth/2);
    glVertex3f(-0.1f, 0.1f, -depth/2);
    
    // Stem right side
    glVertex3f(0.1f, 0.1f, depth/2);
    glVertex3f(0.1f, 0.1f, -depth/2);
    glVertex3f(0.1f, -0.8f, -depth/2);
    glVertex3f(0.1f, -0.8f, depth/2);
    
    // Stem bottom
    glVertex3f(-0.1f, -0.8f, depth/2);
    glVertex3f(-0.1f, -0.8f, -depth/2);
    glVertex3f(0.1f, -0.8f, -depth/2);
    glVertex3f(0.1f, -0.8f, depth/2);
    glEnd();
}

void setupProjection(int width, int height, bool ortho) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    float aspect = (float)width / (float)height;
    
    if (ortho) {
        float size = 2.0f;
        glOrtho(-size * aspect, size * aspect, -size, size, 0.1f, 100.0f);
    } else {
        float fov = 45.0f;
        float nearPlane = 0.1f;
        float farPlane = 100.0f;
        float top = nearPlane * tanf(fov * 0.5f * 3.14159f / 180.0f);
        float right = top * aspect;
        glFrustum(-right, right, -top, top, nearPlane, farPlane);
    }
}

void setupView() {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // Camera position
    float eyeX = 0.0f;
    float eyeY = viewOffsetY * viewDistance;
    float eyeZ = viewDistance;
    
    // Look at origin
    float centerX = 0.0f;
    float centerY = 0.0f;
    float centerZ = 0.0f;
    
    // Up vector
    float upX = 0.0f;
    float upY = 1.0f;
    float upZ = 0.0f;
    
    // Compute view transformation manually
    float fx = centerX - eyeX;
    float fy = centerY - eyeY;
    float fz = centerZ - eyeZ;
    float flen = sqrtf(fx*fx + fy*fy + fz*fz);
    fx /= flen; fy /= flen; fz /= flen;
    
    float sx = fy * upZ - fz * upY;
    float sy = fz * upX - fx * upZ;
    float sz = fx * upY - fy * upX;
    float slen = sqrtf(sx*sx + sy*sy + sz*sz);
    sx /= slen; sy /= slen; sz /= slen;
    
    float ux = sy * fz - sz * fy;
    float uy = sz * fx - sx * fz;
    float uz = sx * fy - sy * fx;
    
    float m[16] = {
        sx, ux, -fx, 0,
        sy, uy, -fy, 0,
        sz, uz, -fz, 0,
        0, 0, 0, 1
    };
    
    glMultMatrixf(m);
    glTranslatef(-eyeX, -eyeY, -eyeZ);
}

int main() {
    // Initialize GLFW
    if (!glfwInit())
        return 1;
    
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    
    GLFWwindow* window = glfwCreateWindow(1280, 720, "CG Homework 1 - Name Initials", nullptr, nullptr);
    if (window == nullptr)
        return 1;
    
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    
    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = nullptr; // Disable saving .ini file
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    
    // Enable depth testing for 3D
    glEnable(GL_DEPTH_TEST);
    
    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        
        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        // Control panel
        ImGui::Begin("Control Panel");
        ImGui::Text("Computer Graphics Homework 1");
        ImGui::Separator();
        
        ImGui::Text("Mode Selection:");
        if (ImGui::RadioButton("2D Letters (YLX)", currentMode == MODE_2D_LETTERS))
            currentMode = MODE_2D_LETTERS;
        if (ImGui::RadioButton("3D Letter (Y)", currentMode == MODE_3D_LETTER))
            currentMode = MODE_3D_LETTER;
        
        ImGui::Separator();
        
        if (currentMode == MODE_3D_LETTER) {
            ImGui::Text("Rotation Controls:");
            ImGui::SliderFloat("X Axis", &rotationX, 0.0f, 360.0f);
            ImGui::SliderFloat("Y Axis", &rotationY, 0.0f, 360.0f);
            ImGui::SliderFloat("Z Axis", &rotationZ, 0.0f, 360.0f);
            
            if (ImGui::Button("Reset Rotation")) {
                rotationX = rotationY = rotationZ = 0.0f;
            }
            
            ImGui::Separator();
            ImGui::Text("Auto Rotation:");
            static bool autoRotateX = false, autoRotateY = false, autoRotateZ = false;
            ImGui::Checkbox("Auto X", &autoRotateX);
            ImGui::Checkbox("Auto Y", &autoRotateY);
            ImGui::Checkbox("Auto Z", &autoRotateZ);
            ImGui::SliderFloat("Speed", &rotationSpeed, 0.0f, 200.0f);
            
            if (autoRotateX) rotationX += rotationSpeed * io.DeltaTime;
            if (autoRotateY) rotationY += rotationSpeed * io.DeltaTime;
            if (autoRotateZ) rotationZ += rotationSpeed * io.DeltaTime;
            
            if (rotationX > 360.0f) rotationX -= 360.0f;
            if (rotationY > 360.0f) rotationY -= 360.0f;
            if (rotationZ > 360.0f) rotationZ -= 360.0f;
        }
        
        ImGui::Separator();
        ImGui::Text("View Settings:");
        ImGui::SliderFloat("Distance", &viewDistance, 2.0f, 10.0f);
        ImGui::SliderFloat("Y Offset", &viewOffsetY, 0.0f, 1.0f);
        
        ImGui::Separator();
        ImGui::Text("Projection Mode:");
        if (ImGui::RadioButton("Orthographic", useOrthographic && !usePerspective)) {
            useOrthographic = true;
            usePerspective = false;
        }
        if (ImGui::RadioButton("Perspective", usePerspective && !useOrthographic)) {
            usePerspective = true;
            useOrthographic = false;
        }
        
        ImGui::Separator();
        ImGui::Text("Application %.1f FPS", io.Framerate);
        
        ImGui::End();
        
        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Setup projection
        setupProjection(display_w, display_h, useOrthographic);
        setupView();
        
        // Draw based on mode
        if (currentMode == MODE_2D_LETTERS) {
            // Draw three 2D letters side by side: Y, L, X
            glPushMatrix();
            glTranslatef(-2.5f, 0.0f, 0.0f);
            glColor3f(1.0f, 0.3f, 0.3f);
            drawLetterY();
            glPopMatrix();
            
            glPushMatrix();
            glTranslatef(0.0f, 0.0f, 0.0f);
            glColor3f(0.3f, 1.0f, 0.3f);
            drawLetterL();
            glPopMatrix();
            
            glPushMatrix();
            glTranslatef(2.5f, 0.0f, 0.0f);
            glColor3f(0.3f, 0.3f, 1.0f);
            drawLetterX();
            glPopMatrix();
        } else {
            // Draw 3D letter Y with rotation
            glPushMatrix();
            glRotatef(rotationX, 1.0f, 0.0f, 0.0f);
            glRotatef(rotationY, 0.0f, 1.0f, 0.0f);
            glRotatef(rotationZ, 0.0f, 0.0f, 1.0f);
            drawLetter3D(0.5f);
            glPopMatrix();
        }
        
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
