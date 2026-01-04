#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <iomanip>

// ---------------------------------------------------------------------------------------------------------
// OpenGL Function Loading (No GLAD/GLEW)
// ---------------------------------------------------------------------------------------------------------
#ifdef _WIN32
#define APIENTRY __stdcall
#else
#define APIENTRY
#endif

typedef ptrdiff_t GLsizeiptr;
typedef char GLchar;

// Constants
#define GL_ARRAY_BUFFER                   0x8892
#define GL_ELEMENT_ARRAY_BUFFER           0x8893
#define GL_STATIC_DRAW                    0x88E4
#define GL_FRAGMENT_SHADER                0x8B30
#define GL_VERTEX_SHADER                  0x8B31
#define GL_COMPILE_STATUS                 0x8B81
#define GL_LINK_STATUS                    0x8B82
#define GL_INFO_LOG_LENGTH                0x8B84
#define GL_DEPTH_BUFFER_BIT               0x00000100
#define GL_DEPTH_TEST                     0x0B71

// Function Pointers
typedef void (APIENTRY *PFNGLGENBUFFERSPROC) (GLsizei n, GLuint *buffers);
typedef void (APIENTRY *PFNGLBINDBUFFERPROC) (GLenum target, GLuint buffer);
typedef void (APIENTRY *PFNGLBUFFERDATAPROC) (GLenum target, GLsizeiptr size, const void *data, GLenum usage);
typedef void (APIENTRY *PFNGLGENVERTEXARRAYSPROC) (GLsizei n, GLuint *arrays);
typedef void (APIENTRY *PFNGLBINDVERTEXARRAYPROC) (GLuint array);
typedef void (APIENTRY *PFNGLENABLEVERTEXATTRIBARRAYPROC) (GLuint index);
typedef void (APIENTRY *PFNGLVERTEXATTRIBPOINTERPROC) (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);
typedef GLuint (APIENTRY *PFNGLCREATESHADERPROC) (GLenum type);
typedef void (APIENTRY *PFNGLSHADERSOURCEPROC) (GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length);
typedef void (APIENTRY *PFNGLCOMPILESHADERPROC) (GLuint shader);
typedef void (APIENTRY *PFNGLGETSHADERIVPROC) (GLuint shader, GLenum pname, GLint *params);
typedef void (APIENTRY *PFNGLGETSHADERINFOLOGPROC) (GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef GLuint (APIENTRY *PFNGLCREATEPROGRAMPROC) (void);
typedef void (APIENTRY *PFNGLATTACHSHADERPROC) (GLuint program, GLuint shader);
typedef void (APIENTRY *PFNGLLINKPROGRAMPROC) (GLuint program);
typedef void (APIENTRY *PFNGLGETPROGRAMIVPROC) (GLuint program, GLenum pname, GLint *params);
typedef void (APIENTRY *PFNGLGETPROGRAMINFOLOGPROC) (GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void (APIENTRY *PFNGLUSEPROGRAMPROC) (GLuint program);
typedef void (APIENTRY *PFNGLDELETESHADERPROC) (GLuint shader);
typedef void (APIENTRY *PFNGLDELETEPROGRAMPROC) (GLuint program);
typedef GLint (APIENTRY *PFNGLGETUNIFORMLOCATIONPROC) (GLuint program, const GLchar *name);
typedef void (APIENTRY *PFNGLUNIFORM1FPROC) (GLint location, GLfloat v0);
typedef void (APIENTRY *PFNGLUNIFORM3FPROC) (GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
typedef void (APIENTRY *PFNGLUNIFORM3FVPROC) (GLint location, GLsizei count, const GLfloat *value);
typedef void (APIENTRY *PFNGLUNIFORM4FPROC) (GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
typedef void (APIENTRY *PFNGLUNIFORMMATRIX4FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRY *PFNGLDELETEVERTEXARRAYSPROC) (GLsizei n, const GLuint *arrays);
typedef void (APIENTRY *PFNGLDELETEBUFFERSPROC) (GLsizei n, const GLuint *buffers);
typedef void (APIENTRY *PFNGLVERTEXATTRIB3FPROC) (GLuint index, GLfloat v0, GLfloat v1, GLfloat v2);

PFNGLGENBUFFERSPROC glGenBuffers = NULL;
PFNGLBINDBUFFERPROC glBindBuffer = NULL;
PFNGLBUFFERDATAPROC glBufferData = NULL;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays = NULL;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray = NULL;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray = NULL;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer = NULL;
PFNGLCREATESHADERPROC glCreateShader = NULL;
PFNGLSHADERSOURCEPROC glShaderSource = NULL;
PFNGLCOMPILESHADERPROC glCompileShader = NULL;
PFNGLGETSHADERIVPROC glGetShaderiv = NULL;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog = NULL;
PFNGLCREATEPROGRAMPROC glCreateProgram = NULL;
PFNGLATTACHSHADERPROC glAttachShader = NULL;
PFNGLLINKPROGRAMPROC glLinkProgram = NULL;
PFNGLGETPROGRAMIVPROC glGetProgramiv = NULL;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog = NULL;
PFNGLUSEPROGRAMPROC glUseProgram = NULL;
PFNGLDELETESHADERPROC glDeleteShader = NULL;
PFNGLDELETEPROGRAMPROC glDeleteProgram = NULL;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation = NULL;
PFNGLUNIFORM1FPROC glUniform1f = NULL;
PFNGLUNIFORM3FPROC glUniform3f = NULL;
PFNGLUNIFORM3FVPROC glUniform3fv = NULL;
PFNGLUNIFORM4FPROC glUniform4f = NULL;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv = NULL;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays = NULL;
PFNGLDELETEBUFFERSPROC glDeleteBuffers = NULL;
PFNGLVERTEXATTRIB3FPROC glVertexAttrib3f = NULL;

void loadOpenGLFunctions() {
    glGenBuffers = (PFNGLGENBUFFERSPROC)glfwGetProcAddress("glGenBuffers");
    glBindBuffer = (PFNGLBINDBUFFERPROC)glfwGetProcAddress("glBindBuffer");
    glBufferData = (PFNGLBUFFERDATAPROC)glfwGetProcAddress("glBufferData");
    glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)glfwGetProcAddress("glGenVertexArrays");
    glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)glfwGetProcAddress("glBindVertexArray");
    glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)glfwGetProcAddress("glEnableVertexAttribArray");
    glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)glfwGetProcAddress("glVertexAttribPointer");
    glCreateShader = (PFNGLCREATESHADERPROC)glfwGetProcAddress("glCreateShader");
    glShaderSource = (PFNGLSHADERSOURCEPROC)glfwGetProcAddress("glShaderSource");
    glCompileShader = (PFNGLCOMPILESHADERPROC)glfwGetProcAddress("glCompileShader");
    glGetShaderiv = (PFNGLGETSHADERIVPROC)glfwGetProcAddress("glGetShaderiv");
    glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)glfwGetProcAddress("glGetShaderInfoLog");
    glCreateProgram = (PFNGLCREATEPROGRAMPROC)glfwGetProcAddress("glCreateProgram");
    glAttachShader = (PFNGLATTACHSHADERPROC)glfwGetProcAddress("glAttachShader");
    glLinkProgram = (PFNGLLINKPROGRAMPROC)glfwGetProcAddress("glLinkProgram");
    glGetProgramiv = (PFNGLGETPROGRAMIVPROC)glfwGetProcAddress("glGetProgramiv");
    glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)glfwGetProcAddress("glGetProgramInfoLog");
    glUseProgram = (PFNGLUSEPROGRAMPROC)glfwGetProcAddress("glUseProgram");
    glDeleteShader = (PFNGLDELETESHADERPROC)glfwGetProcAddress("glDeleteShader");
    glDeleteProgram = (PFNGLDELETEPROGRAMPROC)glfwGetProcAddress("glDeleteProgram");
    glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)glfwGetProcAddress("glGetUniformLocation");
    glUniform1f = (PFNGLUNIFORM1FPROC)glfwGetProcAddress("glUniform1f");
    glUniform3f = (PFNGLUNIFORM3FPROC)glfwGetProcAddress("glUniform3f");
    glUniform3fv = (PFNGLUNIFORM3FVPROC)glfwGetProcAddress("glUniform3fv");
    glUniform4f = (PFNGLUNIFORM4FPROC)glfwGetProcAddress("glUniform4f");
    glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)glfwGetProcAddress("glUniformMatrix4fv");
    glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)glfwGetProcAddress("glDeleteVertexArrays");
    glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)glfwGetProcAddress("glDeleteBuffers");
    glVertexAttrib3f = (PFNGLVERTEXATTRIB3FPROC)glfwGetProcAddress("glVertexAttrib3f");

    if (!glGenBuffers || !glCreateShader || !glUniformMatrix4fv) {
        std::cerr << "ERROR: Failed to load OpenGL functions." << std::endl;
        exit(1);
    }
}

// ---------------------------------------------------------------------------------------------------------
// Minimal Math Library (No GLM)
// ---------------------------------------------------------------------------------------------------------
const float PI = 3.14159265359f;

struct Vec3 {
    float x, y, z;

    Vec3() : x(0), y(0), z(0) {}
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

    Vec3 operator+(const Vec3& other) const { return Vec3(x + other.x, y + other.y, z + other.z); }
    Vec3 operator-(const Vec3& other) const { return Vec3(x - other.x, y - other.y, z - other.z); }
    Vec3 operator*(float scalar) const { return Vec3(x * scalar, y * scalar, z * scalar); }
    Vec3 operator/(float scalar) const { return Vec3(x / scalar, y / scalar, z / scalar); }

    float dot(const Vec3& other) const { return x * other.x + y * other.y + z * other.z; }
    
    Vec3 cross(const Vec3& other) const {
        return Vec3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }

    float length() const { return std::sqrt(x * x + y * y + z * z); }

    Vec3 normalize() const {
        float len = length();
        if (len > 0) return *this / len;
        return *this;
    }
};

struct Mat4 {
    float m[4][4]; // Column-major: m[col][row]

    Mat4() {
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                m[i][j] = (i == j) ? 1.0f : 0.0f;
    }

    static Mat4 identity() { return Mat4(); }

    static Mat4 translate(const Vec3& v) {
        Mat4 res;
        res.m[3][0] = v.x;
        res.m[3][1] = v.y;
        res.m[3][2] = v.z;
        return res;
    }

    static Mat4 scale(const Vec3& v) {
        Mat4 res;
        res.m[0][0] = v.x;
        res.m[1][1] = v.y;
        res.m[2][2] = v.z;
        return res;
    }

    static Mat4 rotate(float angleRadians, const Vec3& axis) {
        Mat4 res;
        Vec3 a = axis.normalize();
        float c = std::cos(angleRadians);
        float s = std::sin(angleRadians);
        float t = 1.0f - c;

        res.m[0][0] = c + a.x * a.x * t;
        res.m[0][1] = a.x * a.y * t + a.z * s;
        res.m[0][2] = a.x * a.z * t - a.y * s;

        res.m[1][0] = a.y * a.x * t - a.z * s;
        res.m[1][1] = c + a.y * a.y * t;
        res.m[1][2] = a.y * a.z * t + a.x * s;

        res.m[2][0] = a.z * a.x * t + a.y * s;
        res.m[2][1] = a.z * a.y * t - a.x * s;
        res.m[2][2] = c + a.z * a.z * t;

        return res;
    }

    static Mat4 perspective(float fovRadians, float aspect, float nearPlane, float farPlane) {
        Mat4 res;
        // Zero out
        for(int i=0; i<4; ++i) for(int j=0; j<4; ++j) res.m[i][j] = 0.0f;

        float tanHalfFovy = std::tan(fovRadians / 2.0f);

        res.m[0][0] = 1.0f / (aspect * tanHalfFovy);
        res.m[1][1] = 1.0f / tanHalfFovy;
        res.m[2][2] = -(farPlane + nearPlane) / (farPlane - nearPlane);
        res.m[2][3] = -1.0f;
        res.m[3][2] = -(2.0f * farPlane * nearPlane) / (farPlane - nearPlane);
        
        return res;
    }

    static Mat4 lookAt(const Vec3& eye, const Vec3& center, const Vec3& up) {
        Vec3 f = (center - eye).normalize();
        Vec3 s = f.cross(up).normalize();
        Vec3 u = s.cross(f);

        Mat4 res;
        res.m[0][0] = s.x; res.m[1][0] = s.y; res.m[2][0] = s.z;
        res.m[0][1] = u.x; res.m[1][1] = u.y; res.m[2][1] = u.z;
        res.m[0][2] = -f.x; res.m[1][2] = -f.y; res.m[2][2] = -f.z;
        res.m[3][0] = -s.dot(eye);
        res.m[3][1] = -u.dot(eye);
        res.m[3][2] = f.dot(eye);
        
        return res;
    }

    Mat4 operator*(const Mat4& other) const {
        Mat4 res;
        for (int col = 0; col < 4; ++col) {
            for (int row = 0; row < 4; ++row) {
                res.m[col][row] = 0.0f;
                for (int k = 0; k < 4; ++k) {
                    res.m[col][row] += m[k][row] * other.m[col][k];
                }
            }
        }
        return res;
    }
    
    const float* value_ptr() const {
        return &m[0][0];
    }
};

// ---------------------------------------------------------------------------------------------------------
// Sphere Geometry Generation
// ---------------------------------------------------------------------------------------------------------
std::vector<float> sphereVertices;
int sphereVertexCount = 0;

void generateSphere(float radius, int stacks, int slices) {
    sphereVertices.clear();
    
    for (int i = 0; i < stacks; ++i) {
        float phi1 = (float)i / stacks * PI;
        float phi2 = (float)(i + 1) / stacks * PI;

        for (int j = 0; j < slices; ++j) {
            float theta1 = (float)j / slices * 2.0f * PI;
            float theta2 = (float)(j + 1) / slices * 2.0f * PI;

            // 4 vertices for a quad (2 triangles)
            // We'll generate 2 triangles per quad directly
            
            // Triangle 1: (phi1, theta1), (phi2, theta1), (phi2, theta2)
            // Triangle 2: (phi1, theta1), (phi2, theta2), (phi1, theta2)

            float phis[4] = {phi1, phi2, phi2, phi1};
            float thetas[4] = {theta1, theta1, theta2, theta2};

            // Vertices for 2 triangles (6 vertices total)
            int indices[6] = {0, 1, 2, 0, 2, 3};

            for (int k = 0; k < 6; ++k) {
                int idx = indices[k];
                float phi = phis[idx];
                float theta = thetas[idx];

                float x = radius * std::sin(phi) * std::cos(theta);
                float y = radius * std::cos(phi);
                float z = radius * std::sin(phi) * std::sin(theta);

                // Position
                sphereVertices.push_back(x);
                sphereVertices.push_back(y);
                sphereVertices.push_back(z);

                // Normal (normalized position for a sphere at origin)
                Vec3 n(x, y, z);
                n = n.normalize();
                sphereVertices.push_back(n.x);
                sphereVertices.push_back(n.y);
                sphereVertices.push_back(n.z);
            }
        }
    }
    sphereVertexCount = (int)(sphereVertices.size() / 6); // 6 floats per vertex (3 pos + 3 normal)
}

// ---------------------------------------------------------------------------------------------------------
// Shader Implementation
// ---------------------------------------------------------------------------------------------------------
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;  
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 lightPos; 
uniform vec3 viewPos; 
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform float shininess;

void main()
{
    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;  
        
    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
} 
)";

void checkCompileErrors(unsigned int shader, std::string type)
{
    int success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}

unsigned int createShaderProgram(const char* vShaderCode, const char* fShaderCode) {
    unsigned int vertex, fragment;
    
    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");
    
    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");
    
    // shader Program
    unsigned int ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");
    
    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return ID;
}

// Forward declarations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// Settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    if (!glfwInit())
        return -1;

    // Use Compatibility Profile for legacy OpenGL support (glVertex)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CG HW3: Phong Shading & VBO", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    // ------------------------
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr; // Disable saving .ini file
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Load OpenGL functions
    loadOpenGLFunctions();
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    // Math Library Verification
    // -------------------------
    std::cout << "Verifying Math Library..." << std::endl;
    Vec3 v1(1.0f, 0.0f, 0.0f);
    Vec3 v2(0.0f, 1.0f, 0.0f);
    Vec3 v3 = v1.cross(v2);
    std::cout << "Cross Product (1,0,0)x(0,1,0) = (" << v3.x << ", " << v3.y << ", " << v3.z << ")" << std::endl;
    
    Mat4 trans = Mat4::translate(Vec3(1.0f, 2.0f, 3.0f));
    std::cout << "Translation Matrix (1,2,3) [3][0]: " << trans.m[3][0] << std::endl;

    // Generate Sphere
    // ---------------
    generateSphere(1.0f, 20, 20);
    std::cout << "Generated Sphere with " << sphereVertexCount << " vertices." << std::endl;

    // Shader Compilation Verification
    // -------------------------------
    unsigned int shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);
    std::cout << "Shader Program Created with ID: " << shaderProgram << std::endl;

    // VBO/VAO Setup
    // -------------
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(float), sphereVertices.data(), GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Main loop
    // ---------
    bool useVBO = true;
    while (!glfwWindowShouldClose(window))
    {
        // Input
        processInput(window);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        static float lightPos[3] = {1.2f, 1.0f, 2.0f};
        static float lightColor[3] = {1.0f, 1.0f, 1.0f};
        static float objectColor[3] = {1.0f, 0.5f, 0.31f};
        static float shininess = 32.0f;

        // 1. Show a simple window
        {
            ImGui::Begin("Control Panel");
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::Checkbox("Use VBO", &useVBO);
            
            ImGui::Separator();
            ImGui::Text("Light Settings");
            ImGui::DragFloat3("Light Position", lightPos, 0.1f);
            ImGui::ColorEdit3("Light Color", lightColor);
            
            ImGui::Separator();
            ImGui::Text("Material Settings");
            ImGui::ColorEdit3("Object Color", objectColor);
            ImGui::DragFloat("Shininess", &shininess, 1.0f, 1.0f, 256.0f);
            
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear depth buffer too
        glEnable(GL_DEPTH_TEST); // Enable depth testing

        glUseProgram(shaderProgram);

        // Set Uniforms
        // Light Position
        glUniform3f(glGetUniformLocation(shaderProgram, "lightPos"), lightPos[0], lightPos[1], lightPos[2]);
        // View Position (Camera)
        glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), 0.0f, 0.0f, 3.0f);
        // Light Color
        glUniform3f(glGetUniformLocation(shaderProgram, "lightColor"), lightColor[0], lightColor[1], lightColor[2]);
        // Object Color
        glUniform3f(glGetUniformLocation(shaderProgram, "objectColor"), objectColor[0], objectColor[1], objectColor[2]);
        // Shininess
        glUniform1f(glGetUniformLocation(shaderProgram, "shininess"), shininess);

        // Transformations
        Mat4 projection = Mat4::perspective(45.0f * PI / 180.0f, (float)display_w / (float)display_h, 0.1f, 100.0f);
        Mat4 view = Mat4::translate(Vec3(0.0f, 0.0f, -3.0f)); // Move camera back
        Mat4 model = Mat4::identity();
        
        // Rotate the sphere over time
        float time = (float)glfwGetTime();
        model = Mat4::rotate(time, Vec3(0.5f, 1.0f, 0.0f));

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, projection.value_ptr());
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, view.value_ptr());
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, model.value_ptr());

        if (useVBO) {
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, sphereVertexCount);
        } else {
            // Immediate Mode Rendering
            glBindVertexArray(0); // Unbind VAO
            
            glBegin(GL_TRIANGLES);
            for (int i = 0; i < sphereVertexCount; ++i) {
                int baseIndex = i * 6;
                // Normal (Location 1)
                glVertexAttrib3f(1, sphereVertices[baseIndex + 3], sphereVertices[baseIndex + 4], sphereVertices[baseIndex + 5]);
                // Position (Location 0) - Using glVertex3f to ensure vertex submission
                glVertex3f(sphereVertices[baseIndex], sphereVertices[baseIndex + 1], sphereVertices[baseIndex + 2]);
            }
            glEnd();
        }

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
