#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

class Cube {
private:
    glm::vec3 position;      // Position relative to pedestal center
    glm::vec3 color;         // Cube color
    float localRotation;     // Local rotation angle (around its own center)
    
public:
    Cube(const glm::vec3& pos, const glm::vec3& col) 
        : position(pos), color(col), localRotation(0.0f) {}
    
    void rotate(float angle) {
        localRotation += angle;
    }
    
    void setLocalRotation(float angle) {
        localRotation = angle;
    }
    
    glm::mat4 getModelMatrix(const glm::vec3& pedestalCenter, float pedestalRotation, float globalRotation) const {
        
        glm::mat4 model = glm::mat4(1.0f);

        model = glm::rotate(model, globalRotation, glm::vec3(0.0f, 1.0f, 0.0f));

        model = glm::translate(model, pedestalCenter);

        model = glm::rotate(model, pedestalRotation, glm::vec3(0.0f, 1.0f, 0.0f));

        model = glm::translate(model, position);

        model = glm::rotate(model, localRotation, glm::vec3(0.0f, 1.0f, 0.0f));
        return model;
    }
    
    const glm::vec3& getColor() const { return color; }
};

struct GameState {
    int currentTask = 0;
    bool isRotating = false;   
    float rotationAngle = 0.0f;
    
    std::vector<Cube> pedestalCubes;
    glm::vec3 pedestalPosition;    
    float pedestalRotation;     
    float globalRotation;
    
    bool rotateLocalCubes = false;
    bool rotateGlobalCenter = false;
    bool rotatePedestalCenter = false;
};

GameState g_gameState;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch(key) {
            case GLFW_KEY_1:
                g_gameState.currentTask = 0;
                std::cout << "Task 1: Basic shapes" << std::endl;
                break;
            case GLFW_KEY_2:
                g_gameState.currentTask = 1;
                std::cout << "Task 2: Pedestal of cubes" << std::endl;
                break;
            case GLFW_KEY_3:
                g_gameState.currentTask = 2;
                std::cout << "Task 3: Rotating Shape" << std::endl;
                break;
            
            case GLFW_KEY_L:  
                if (g_gameState.currentTask == 2) {
                    g_gameState.rotateLocalCubes = !g_gameState.rotateLocalCubes;
                    std::cout << "Local cube rotation: " << (g_gameState.rotateLocalCubes ? "ON" : "OFF") << std::endl;
                }
                break;
                
            case GLFW_KEY_G:
                if (g_gameState.currentTask == 2) {
                    g_gameState.rotateGlobalCenter = !g_gameState.rotateGlobalCenter;
                    std::cout << "Global rotation: " << (g_gameState.rotateGlobalCenter ? "ON" : "OFF") << std::endl;
                }
                break;
                
                case GLFW_KEY_P:
                if (g_gameState.currentTask == 2) {
                    g_gameState.rotatePedestalCenter = !g_gameState.rotatePedestalCenter;
                    std::cout << "Pedestal rotation: " << (g_gameState.rotatePedestalCenter ? "ON" : "OFF") << std::endl;
                }
                break;
            
            case GLFW_KEY_R:
                if (g_gameState.currentTask == 1) {
                    g_gameState.isRotating = !g_gameState.isRotating;
                    std::cout << "Rotation: " << (g_gameState.isRotating ? "ON" : "OFF") << std::endl;
                }
                break;
            
            case GLFW_KEY_SPACE:
                g_gameState.rotationAngle = 0.0f;
                g_gameState.pedestalRotation = 0.0f;
                g_gameState.globalRotation = 0.0f;
                for (auto& cube : g_gameState.pedestalCubes) {
                    cube.setLocalRotation(0.0f);
                }
                std::cout << "Reset all rotations" << std::endl;
                break;
        }
    }
}

bool initGLFW() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    
    return true;
}

GLFWwindow* createWindow() {
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Modular OpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    
    return window;
}

bool initGLAD() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return false;
    }
    return true;
}

class ShaderProgram {
private:
    GLuint programID;
    void checkShaderCompile(GLuint shader) const {
        int success;
        char infoLog[512];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
        }
    }
    void checkProgramCompile(GLuint shaderProgram) const {
        int success;
        char infoLog[512];
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }
    }
public:
    ShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource) {
        unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        glCompileShader(vertexShader);

        unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
        glCompileShader(fragmentShader);
    
        checkShaderCompile(vertexShader);
        checkShaderCompile(fragmentShader);

        programID = glCreateProgram();
        glAttachShader(programID, vertexShader);
        glAttachShader(programID, fragmentShader);
        glLinkProgram(programID);
        
        checkProgramCompile(programID);
    }
    void use() {
        glUseProgram(programID);
    }
    void setMat4(const std::string &name, const glm::mat4 &mat) {
        glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    
    void setFloat(const std::string &name, float value) {
        glUniform1f(glGetUniformLocation(programID, name.c_str()), value);
    }
};

class VertexManager {
private:
    GLuint VAO, VBO;
    std::vector<float> vertices;
public:
    VertexManager() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
    }
    void setVertices(const std::vector<float>& vertices) {
        this->vertices = vertices;
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(this->vertices[0]), this->vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3* sizeof(float)));
        glEnableVertexAttribArray(0);  
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    void draw(GLenum mode) {
        glBindVertexArray(VAO);
        glDrawArrays(mode, 0, vertices.size() / 6);
        glBindVertexArray(0);
    }
    ~VertexManager() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }
};

const char* vertexShaderSource = R"(#version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aColor;
    out vec3 fColor;
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;
    void main()
    {
        gl_Position = projection * view * model * vec4(aPos, 1.0);
        fColor = aColor;
    }
)";

const char* fragmentShaderSource = R"(#version 330 core
    in vec3 fColor;
    out vec4 FragColor;
    void main()
    {
        FragColor = vec4(fColor, 1.0);
    } 
)";



void render(VertexManager& vm, std::vector<float> vertices, GLenum type = GL_TRIANGLES) {
    vm.setVertices(vertices);
    vm.draw(type);
}


void update(float deltaTime) {
    if (g_gameState.isRotating) {
        g_gameState.rotationAngle += deltaTime * 2.0f;
    }
    
    if (g_gameState.currentTask == 2) {
        float rotationSpeed = deltaTime * 1.5f;
        
        if (g_gameState.rotateLocalCubes) {
            for (auto& cube : g_gameState.pedestalCubes) {
                cube.rotate(rotationSpeed);
            }
        }
        
        if (g_gameState.rotateGlobalCenter) {
            g_gameState.globalRotation += rotationSpeed;
        }
        
        if (g_gameState.rotatePedestalCenter) {
            g_gameState.pedestalRotation += rotationSpeed;
        }
    }
}
void renderPedestal(ShaderProgram& program, VertexManager& vm, const std::vector<float>& cubeVertices) {
    for (const auto& cube : g_gameState.pedestalCubes) {
        glm::mat4 model = cube.getModelMatrix(
            g_gameState.pedestalPosition,
            g_gameState.pedestalRotation,
            g_gameState.globalRotation
        );
        
        program.setMat4("model", model);
    
        render(vm, cubeVertices);
    }
}

const std::vector<float> triangleVertices = {
    -1.0, -1.0, 0.0, 1.0, 0.0, 0.0,
     0.0,  1.0, 0.0, 0.0, 1.0, 0.0,
     1.0, -1.0, 0.0, 0.0, 0.0, 1.0
};

const std::vector<float> rectangleVertices = {
    -1.0, -1.0, 0.0, 1.0, 0.0, 0.0,
    -1.0,  1.0, 0.0, 0.0, 1.0, 0.0,
     1.0,  1.0, 0.0, 0.0, 0.0, 1.0,
     1.0, -1.0, 0.0, 0.0, 0.0, 1.0,
};

const std::vector<float> pentagonVertices = {
    0.0f,      0.85f,    0.0f, 0.0f, 1.0f, 0.0f,  
    0.809f,    0.262f,   0.0f, 0.0f, 1.0f, 0.0f,
    0.5f,     -0.688f,   0.0f, 0.0f, 1.0f, 0.0f,
   -0.5f,     -0.688f,   0.0f, 0.0f, 1.0f, 0.0f,
   -0.809f,    0.262f,   0.0f, 0.0f, 1.0f, 0.0f
};

const std::vector<float> cubeVertices = {
    -0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f,
     0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f,
     0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f,
     0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f,

    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
     0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
     0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
     0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
    -0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,

     0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f,
     0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 0.0f,
     0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f,
     0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f,
     0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 0.0f,
     0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f,

    -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
     0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
     0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
     0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 1.0f,

    -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 1.0f,
     0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 1.0f,
     0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 1.0f,
     0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f
};

 const std::vector<float> pentagon = {
        0.0, 1.0, 0.0, 1.0f, 0.0f, 1.0f,
        0.951, 0.309, 0.0, 1.0f, 0.0f, 1.0f,
        0.588, -0.809, 0.0, 1.0f, 0.0f, 1.0f,
        -0.588, -0.809, 0.0, 1.0f, 0.0f, 1.0f,
        -0.951, 0.309, 0.0, 1.0f, 0.0f, 1.0f,
 };


 const char* stripeVertexShaderSource = R"(#version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aColor;
    out vec3 fColor;
    out vec3 vPosition;
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;
    void main()
    {
        gl_Position = projection * view * model * vec4(aPos, 1.0);
        fColor = aColor;
        vPosition = aPos;
    }
)";

const char* stripeFragmentShaderSource = R"(#version 330 core
    in vec3 fColor;
    in vec3 vPosition;
    out vec4 FragColor;
    void main()
    {
        float k = 5.0;
        int sum = int(vPosition.x * k);
        if ( (sum - (sum / 2 * 2)) == 0 ) {
            FragColor = vec4(0.8, 0.8, 0, 1);
        }
        else {
            FragColor = vec4(0.5, 0.0, 0, 1);
        }
    } 
)";



int main() {
    if (!initGLFW()) return -1;
    
    GLFWwindow* window = createWindow();
    if (!window) return -1;
    
    if (!initGLAD()) return -1;
    
    ShaderProgram program(vertexShaderSource, fragmentShaderSource);
    VertexManager vertexManager;

    ShaderProgram stripedProgram(stripeVertexShaderSource, stripeFragmentShaderSource);
    
    vertexManager.setVertices(triangleVertices);
    
    glEnable(GL_DEPTH_TEST);
    
    float lastFrame = 0.0f;

    g_gameState.pedestalPosition = glm::vec3(2.0f, 0.0f, -5.0f);  // Off-center position
    
    g_gameState.pedestalCubes.push_back(Cube(glm::vec3(-1.0f, -0.5f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f))); // Bottom-left - Red
    g_gameState.pedestalCubes.push_back(Cube(glm::vec3( 1.0f, -0.5f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f))); // Bottom-right - Green
    g_gameState.pedestalCubes.push_back(Cube(glm::vec3(0.0f,  0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f))); // Top-left - Blue
    g_gameState.pedestalCubes.push_back(Cube(glm::vec3(0.0f,  -0.5f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f))); // Top-right - Yellow
    
    g_gameState.pedestalRotation = 0.0f;
    g_gameState.globalRotation = 0.0f;
    
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        update(deltaTime);
        
        processInput(window);
        
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        program.use();
        
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -5.0f));
        projection = glm::perspective(glm::radians(45.0f), (float)SCREEN_WIDTH / (float)SCREEN_WIDTH, 0.1f, 100.0f);
        
        program.setMat4("model", model);
        program.setMat4("view", view);
        program.setMat4("projection", projection);
        
        switch(g_gameState.currentTask) {
            case 0:
                model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 0.0f));
                program.setMat4("model", model);
                render(vertexManager, triangleVertices);
                model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
                program.setMat4("model", model);
                render(vertexManager, rectangleVertices, GL_TRIANGLE_FAN);
                break;
            case 1:
                g_gameState.isRotating = true;
                model = glm::translate(model, glm::vec3(-2.0f, 0.0f, -3.0f));
                model = glm::rotate(model, g_gameState.rotationAngle, glm::vec3(0.5f, 1.0f, 0.0f));
                program.setMat4("model", model);
                render(vertexManager, cubeVertices);
                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(2.0f, 0.0f, -3.0f));
                program.setMat4("model", model);
                render(vertexManager, pentagon, GL_TRIANGLE_FAN);
                
                stripedProgram.use();
                model = glm::translate(model, glm::vec3(-2.0f, 0.0f, -2.0f));
                stripedProgram.setMat4("model", model);
                stripedProgram.setMat4("view", view);
                stripedProgram.setMat4("projection", projection);
                render(vertexManager, rectangleVertices, GL_TRIANGLE_FAN);
                break;
            case 2:
                renderPedestal(program, vertexManager, cubeVertices);
                break;
        }
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}