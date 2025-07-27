// texxtures cames from Nasa website : https://planetpixelemporium.com/mercury.html
// sphre came from blender default sphre with a triangulate modifier aplied before export

// rocket from : https://sketchfab.com/3d-models/space-craft-f7026b90bf9b44c99c15f7afc87bcdd3

// textures from :https://ambientcg.com/

//
//   COMP 371 Labs Framework
//
//   Created by Nicolas Bergeron on 20/06/2019.
//

//  sudo apt install libstb-dev

// #include <stb/stb_image.h> not found
// Created include/stb/
// Added -I../include

// Problem 2: “undefined reference to __glew..., glfw..., gl...”
// -lGLEW -lGL -lglfw -lm

// Problem 3: IntelliSense includePath warning : bear -- g++ ...
// bear -- g++ ../Tut04/Tut04.cpp -o Tut04 -I../include -lGLEW -lGL -lglfw -lm

#include <iostream>
#include <list>
#include <algorithm>
#include "OBJloader.h"
#include "OBJloaderV2.h"

#define GLEW_STATIC 1 // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>  // Include GLEW - OpenGL Extension Wrangler

#include <GLFW/glfw3.h> // GLFW provides a cross-platform interface for creating a graphical context,
                        // initializing OpenGL and binding inputs

#include <glm/glm.hpp>                  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices
#include <glm/common.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

using namespace glm;
using namespace std;

GLuint loadTexture(const char *filename);

const char *getVertexShaderSource();

const char *getFragmentShaderSource();

int compileAndLinkShaders(const char *vertexShaderSource, const char *fragmentShaderSource);

void setUVScale(int shaderProgram, glm::vec2 scale)
{
    glUseProgram(shaderProgram);
    GLuint uvScaleLocation = glGetUniformLocation(shaderProgram, "uvScale");
    glUniform2fv(uvScaleLocation, 1, &scale[0]);
}

struct TexturedColoredVertex
{
    TexturedColoredVertex(vec3 _position, vec3 _color, vec2 _uv)
        : position(_position), color(_color), uv(_uv) {}

    vec3 position;
    vec3 color;
    vec2 uv;
};

GLuint setupModelVBO(string path, int &vertexCount)
{
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> UVs;

    // read the vertex data from the model's OBJ file
    loadOBJ(path.c_str(), vertices, normals, UVs);

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO); // Becomes active VAO
    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).

    // Vertex VBO setup
    GLuint vertices_VBO;
    glGenBuffers(1, &vertices_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, vertices_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);
    glEnableVertexAttribArray(0);

    // Normals VBO setup
    // UVs → location 2
    GLuint uvs_VBO;
    glGenBuffers(1, &uvs_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, uvs_VBO);
    glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec2), &UVs.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    // Normals → location 3
    GLuint normals_VBO;
    glGenBuffers(1, &normals_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, normals_VBO);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(3);

    glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs, as we are using multiple VAOs)
    vertexCount = vertices.size();
    return VAO;
}
// Sets up a model using an Element Buffer Object to refer to vertex data
/// changed the order in which the info align to match the shaders
GLuint setupModelEBO(string path, int &vertexCount)
{
    vector<int> vertexIndices; // The contiguous sets of three indices of vertices, normals and UVs, used to make a triangle
    vector<glm::vec3> vertices;
    vector<glm::vec3> normals;
    vector<glm::vec2> UVs;

    // read the vertices from the cube.obj file
    // We won't be needing the normals or UVs for this program
    loadOBJ2(path.c_str(), vertexIndices, vertices, normals, UVs);

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO); // Becomes active VAO
    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).

    /*
    // Vertex VBO setup
    GLuint vertices_VBO;
    glGenBuffers(1, &vertices_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, vertices_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);
    glEnableVertexAttribArray(0);

    // Normals VBO setup
    GLuint normals_VBO;
    glGenBuffers(1, &normals_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, normals_VBO);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);
    glEnableVertexAttribArray(1);

    // UVs VBO setup
    GLuint uvs_VBO;
    glGenBuffers(1, &uvs_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, uvs_VBO);
    glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec2), &UVs.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid *)0);
    glEnableVertexAttribArray(2);
    */

    // Vertex VBO setup (location = 0)
    GLuint vertices_VBO;
    glGenBuffers(1, &vertices_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, vertices_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);
    glEnableVertexAttribArray(0);

    // Normals VBO setup (location = 3)
    GLuint normals_VBO;
    glGenBuffers(1, &normals_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, normals_VBO);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);
    glEnableVertexAttribArray(3);

    // UVs VBO setup (location = 2)
    GLuint uvs_VBO;
    glGenBuffers(1, &uvs_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, uvs_VBO);
    glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec2), &UVs.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);
    glEnableVertexAttribArray(2);

    // EBO setup
    GLuint EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertexIndices.size() * sizeof(int), &vertexIndices.front(), GL_STATIC_DRAW);

    glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO
    vertexCount = vertexIndices.size();
    return VAO;
}

void setProjectionMatrix(int shaderProgram, mat4 projectionMatrix)
{
    glUseProgram(shaderProgram);
    GLuint projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");
    glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
}

void setViewMatrix(int shaderProgram, mat4 viewMatrix)
{
    glUseProgram(shaderProgram);
    GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
}

void setWorldMatrix(int shaderProgram, mat4 worldMatrix)
{
    glUseProgram(shaderProgram);
    GLuint worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
}

struct SceneAssets
{
    // Textures
    GLuint grass, brick, cement, rock, space, brick2;
    GLuint earth, jupiter, mars, mercury, neptune, pluto, saturn, sun, uranus, venus, moon, rocketTexture, saturnRing;
    GLuint planetTextures[11];
    GLuint textureLocation;

    // VAOs and vertex counts
    GLuint cubeVAO,
        planetVAO, rocketVAO, saturnRingVao;

    // Mesh paths
    std::string cubePath, planetPath, rocketPath, saturnRingPath;

    int cubeVertices;
    int planet1Vertices;
    int rocketVertices;
    int saturnRingVertices;

    int colorShaderProgram;
    int texturedShaderProgram;
};

SceneAssets loadSceneAssets(bool renderingEBO)
{
    SceneAssets assets;

    // Textures
    assets.grass = loadTexture("./Textures/Grass001_1K-JPG_Color.jpg");
    assets.brick = loadTexture("./Textures/Bricks060_1K-JPG_Color.jpg");
    assets.cement = loadTexture("./Textures/cement.jpg");
    assets.rock = loadTexture("./Textures/Rock058_1K-JPG_Color.jpg");
    assets.space = loadTexture("./Textures/SPACE.png");
    assets.brick2 = loadTexture("./Textures/brick.jpg");

    assets.sun = loadTexture("./Textures/sunmap.jpg");
    assets.earth = loadTexture("./Textures/earthmap1k.jpg");
    assets.jupiter = loadTexture("./Textures/jupitermap.jpg");
    assets.mars = loadTexture("./Textures/mars_1k_color.jpg");
    assets.mercury = loadTexture("./Textures/mercurymap.jpg");
    assets.neptune = loadTexture("./Textures/neptunemap.jpg");
    assets.pluto = loadTexture("./Textures/plutomap1k.jpg");
    assets.saturn = loadTexture("./Textures/saturnmap.jpg");
    assets.uranus = loadTexture("./Textures/uranusmap.jpg");
    assets.venus = loadTexture("./Textures/venusmap.jpg");
    assets.saturnRing = loadTexture("./Textures/saturnringcolor.png");
    assets.moon = loadTexture("./Textures/moonmap1k.png");

    assets.rocketTexture = loadTexture("./Textures/Material.001_baseColor.png");

    assets.planetTextures[0] = assets.sun;
    assets.planetTextures[1] = assets.earth;
    assets.planetTextures[2] = assets.jupiter;
    assets.planetTextures[3] = assets.mars;
    assets.planetTextures[4] = assets.mercury;
    assets.planetTextures[5] = assets.neptune;
    assets.planetTextures[6] = assets.pluto;
    assets.planetTextures[7] = assets.saturn;
    assets.planetTextures[8] = assets.uranus;
    assets.planetTextures[9] = assets.venus;
    assets.planetTextures[10] = assets.moon;

    // Mesh paths
    assets.cubePath = "./Models/cube.obj";
    assets.planetPath = "./Models/Planet.obj";
    assets.rocketPath = "./Models/fixed_cube.obj";
    assets.saturnRingPath = "./Models/saturn_ring.obj";

    // Mesh loading
    if (renderingEBO)
    {
        assets.cubeVAO = setupModelEBO(assets.cubePath, assets.cubeVertices);
        assets.planetVAO = setupModelEBO(assets.planetPath, assets.planet1Vertices);
        assets.rocketVAO = setupModelEBO(assets.rocketPath, assets.rocketVertices);
        assets.saturnRingVao = setupModelEBO(assets.saturnRingPath, assets.saturnRingVertices);
    }
    else
    {
        assets.cubeVAO = setupModelVBO(assets.cubePath, assets.cubeVertices);
        assets.planetVAO = setupModelVBO(assets.planetPath, assets.planet1Vertices);
    }

    // Compile and link shaders here ...
    assets.colorShaderProgram = compileAndLinkShaders(getVertexShaderSource(), getFragmentShaderSource());
    assets.texturedShaderProgram = compileAndLinkShaders(getVertexShaderSource(), getFragmentShaderSource());
    assets.textureLocation = glGetUniformLocation(assets.texturedShaderProgram, "textureSampler");

    return assets;
}

void drawRocket(const SceneAssets &assets)
{
    setUVScale(assets.texturedShaderProgram, vec2(1.0f));

    float time = glfwGetTime();
    float orbitSpeed = 20.0f;
    float orbitRadius = 100.0f;
    vec3 orbitCenter = vec3(0.0f, 15.0f, 0.0f);

    float angleNow = radians(time * orbitSpeed);
    float angleAhead = radians((time + 1.0f) * orbitSpeed);

    vec3 posNow = orbitCenter + vec3(cos(angleNow), 0.0f, sin(angleNow)) * orbitRadius;
    vec3 posAhead = orbitCenter + vec3(cos(angleAhead), 0.0f, sin(angleAhead)) * orbitRadius;

    vec3 forward = normalize(posAhead - posNow);
    vec3 up = vec3(0.0f, 1.0f, 0.0f);
    vec3 right = normalize(cross(up, forward));
    vec3 correctedUp = cross(forward, right);

    mat4 rotationMatrix = mat4(
        vec4(right, 0.0f),
        vec4(correctedUp, 0.0f),
        vec4(forward, 0.0f),
        vec4(0.0f, 0.0f, 0.0f, 1.0f));

    mat4 model = mat4(1.0f);
    model = translate(model, posNow);
    model *= rotationMatrix;

    // Pre-rotate rocket from Y-up to Z-forward
    model *= rotate(mat4(1.0f), radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
    model *= rotate(mat4(1.0f), radians(180.0f), vec3(0.0f, 1.0f, 0.0f));

    model *= rotate(mat4(1.0f), radians(180.0f), vec3(1.0f, 0.0f, 0.0f));

    float scaleFactor = 4;
    model = scale(model, vec3(1, 1, 1) * scaleFactor); // scale after orientation

    setWorldMatrix(assets.texturedShaderProgram, model);

    mat3 normalMatrix = transpose(inverse(mat3(model)));
    GLuint normalLoc = glGetUniformLocation(assets.texturedShaderProgram, "normalMatrix");
    glUniformMatrix3fv(normalLoc, 1, GL_FALSE, &normalMatrix[0][0]);

    glBindTexture(GL_TEXTURE_2D, assets.rocketTexture);
    glBindVertexArray(assets.rocketVAO);
    glDrawElements(GL_TRIANGLES, assets.rocketVertices, GL_UNSIGNED_INT, 0);
}

void drawPlanets(SceneAssets assets)
{
    setUVScale(assets.texturedShaderProgram, vec2(0.8f));

    for (int i = 0; i < 11; ++i)
    {
        glBindTexture(GL_TEXTURE_2D, assets.planetTextures[i]);

        float time = glfwGetTime() + i * 3;
        float angle = radians(time * 10.0f + i * 36.0f);
        float spin = radians(time * 50.0f + i * 20.0f);

        float orbitRadius = (i == 0) ? 0 : 50.0f + i * 3.0f;
        vec3 orbitCenter = vec3(0.0f, 16.0f, -10.0f);

        mat4 model = mat4(1.0f);
        model = translate(model, orbitCenter);
        model = rotate(model, angle, vec3(0.0f, 1.0f, 0.0f));
        model = translate(model, vec3(orbitRadius, 0.0f, 0.0f));
        model = rotate(model, spin, vec3(0.0f, 1.0f, 0.0f));
        model = scale(model, vec3(6.0f));

        setWorldMatrix(assets.texturedShaderProgram, model);
        mat3 normalMatrix = transpose(inverse(mat3(model)));
        GLuint normalLoc = glGetUniformLocation(assets.texturedShaderProgram, "normalMatrix");
        glUniformMatrix3fv(normalLoc, 1, GL_FALSE, &normalMatrix[0][0]);

        glBindVertexArray(assets.planetVAO);
        glDrawElements(GL_TRIANGLES, assets.planet1Vertices, GL_UNSIGNED_INT, 0);

        if (i == 1)
        {
            float moonOrbitRadius = 2.0f;
            float moonOrbitSpeed = 25.0f;
            float moonSpinSpeed = 60.0f;
            float moonScale = 0.1f;
            float moonOrbitAngle = radians(time * moonOrbitSpeed);

            mat4 moonModel = model *
                             rotate(mat4(1.0f), moonOrbitAngle, vec3(0.0f, 1.0f, 0.0f)) *
                             translate(mat4(1.0f), vec3(moonOrbitRadius, 0.0f, 0.0f)) *
                             rotate(mat4(1.0f), radians(time * moonSpinSpeed), vec3(0.0f, 1.0f, 0.0f)) *
                             scale(mat4(1.0f), vec3(moonScale));

            setWorldMatrix(assets.texturedShaderProgram, moonModel);
            mat3 moonNormalMatrix = transpose(inverse(mat3(moonModel)));
            GLuint moonNormalLoc = glGetUniformLocation(assets.texturedShaderProgram, "normalMatrix");
            glUniformMatrix3fv(moonNormalLoc, 1, GL_FALSE, &moonNormalMatrix[0][0]);

            glBindTexture(GL_TEXTURE_2D, assets.moon);
            glBindVertexArray(assets.planetVAO);
            glDrawElements(GL_TRIANGLES, assets.planet1Vertices, GL_UNSIGNED_INT, 0);
        }

        if (i == 7) // saturn rings
        {
            mat4 ringModel = model *
                             rotate(mat4(1.0f), radians(90.0f), vec3(1.0f, 0.0f, 0.0f)) *
                             scale(mat4(1.0f), vec3(1)); // Optional scale tweak

            setWorldMatrix(assets.texturedShaderProgram, ringModel);
            mat3 ringNormalMatrix = transpose(inverse(mat3(ringModel)));
            GLuint ringNormalLoc = glGetUniformLocation(assets.texturedShaderProgram, "normalMatrix");
            glUniformMatrix3fv(ringNormalLoc, 1, GL_FALSE, &ringNormalMatrix[0][0]);

            glBindTexture(GL_TEXTURE_2D, assets.saturnRing);
            glBindVertexArray(assets.saturnRingVao);
            glDrawElements(GL_TRIANGLES, assets.saturnRingVertices, GL_UNSIGNED_INT, 0);
        }
    }
}

void drawCubeField(const SceneAssets &assets)
{
    setUVScale(assets.texturedShaderProgram, vec2(0.8f));

    auto drawCubes = [&](GLuint texture, float radius, int count)
    {
        for (int i = 0; i < count; ++i)
        {
            glBindTexture(GL_TEXTURE_2D, texture);

            float time = glfwGetTime();
            float angle = radians(time * 5.0f + i * 18.0f);
            float spin = radians(time * 10.0f);
            vec3 orbitCenter = vec3(0.0f, 0.0f, -10.0f);

            mat4 model = mat4(1.0f);
            model = translate(model, orbitCenter);
            model = rotate(model, angle, vec3(0.0f, 1.0f, 0.0f));
            model = translate(model, vec3(radius, 0.0f, 0.0f));
            model = rotate(model, spin, vec3(0.0f, 1.0f, 0.0f));
            model = scale(model, vec3(0.5f));

            setWorldMatrix(assets.texturedShaderProgram, model);
            mat3 normalMatrix = transpose(inverse(mat3(model)));
            GLuint normalLoc = glGetUniformLocation(assets.texturedShaderProgram, "normalMatrix");
            glUniformMatrix3fv(normalLoc, 1, GL_FALSE, &normalMatrix[0][0]);

            glBindVertexArray(assets.cubeVAO);
            glDrawElements(GL_TRIANGLES, assets.cubeVertices, GL_UNSIGNED_INT, 0);
        }
    };

    drawCubes(assets.brick, 30.0f, 40);
    drawCubes(assets.cement, 20.0f, 20);
    drawCubes(assets.rock, 10.0f, 20);
}

void drawSkybox(const SceneAssets &assets)
{
    glDisable(GL_CULL_FACE); // Show inside of the sphere
    glDepthMask(GL_FALSE);   // Prevent depth buffer overwrite

    glUseProgram(assets.texturedShaderProgram);
    setUVScale(assets.texturedShaderProgram, vec2(1.0f, 1.0f));

    glBindTexture(GL_TEXTURE_2D, assets.space);

    mat4 skyModel = translate(mat4(1.0f), vec3(0.0f, 16.0f, -10.0f)) *
                    scale(mat4(1.0f), vec3(-100.0f));

    setWorldMatrix(assets.texturedShaderProgram, skyModel);
    glBindVertexArray(assets.planetVAO);
    glDrawElements(GL_TRIANGLES, assets.planet1Vertices, GL_UNSIGNED_INT, 0);

    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);
}

void drawGround(const SceneAssets &assets)
{
    glUseProgram(assets.texturedShaderProgram);
    setUVScale(assets.texturedShaderProgram, vec2(300.0f, 300.0f));
    glBindTexture(GL_TEXTURE_2D, assets.grass);

    mat4 model = mat4(1.0f);
    model = translate(model, vec3(0.0f, -1.5f, 0.0f));
    model = scale(model, vec3(1000.0f, 0.02f, 1000.0f));

    setWorldMatrix(assets.texturedShaderProgram, model);

    mat3 normalMatrix = transpose(inverse(mat3(model)));
    GLuint normalLoc = glGetUniformLocation(assets.texturedShaderProgram, "normalMatrix");
    glUniformMatrix3fv(normalLoc, 1, GL_FALSE, &normalMatrix[0][0]);

    glBindVertexArray(assets.cubeVAO);
    glDrawElements(GL_TRIANGLES, assets.cubeVertices, GL_UNSIGNED_INT, 0);
}

struct CameraState
{
    // Position and orientation
    vec3 position; // Camera position
    vec3 lookAt;   // Camera forward direction
    vec3 up = vec3(0.0f, 1.0f, 0.0f);

    // Angles for rotation
    float horizontalAngle = 0.0f;
    float verticalAngle = 0.0f;

    // Movement speed
    float fastSpeed = 20.0f;
    float normalSpeed = 8.0f;

    // Camera mode
    bool firstPerson = true;

    // Mouse tracking
    double lastMouseX = 0.0;
    double lastMouseY = 0.0;
    int lastMouseLeftState = GLFW_RELEASE;

    // Timing
    float dt = 0.0f;
    float lastFrameTime = 0.0f;

    // Visual effect (e.g. rotating cube)
    void updateDeltaTime(float currentTime)
    {
        dt = currentTime - lastFrameTime;
        lastFrameTime = currentTime;
    }
};

void handleInput(GLFWwindow *window, SceneAssets &assets, CameraState &camera)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        camera.firstPerson = true;
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        camera.firstPerson = false;

    bool fast = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
    float speed = fast ? camera.fastSpeed : camera.normalSpeed;

    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    double dx = mouseX - camera.lastMouseX;
    double dy = mouseY - camera.lastMouseY;
    camera.lastMouseX = mouseX;
    camera.lastMouseY = mouseY;

    camera.horizontalAngle -= dx * 60.0f * camera.dt;
    camera.verticalAngle -= dy * 60.0f * camera.dt;
    camera.verticalAngle = std::clamp(camera.verticalAngle, -85.0f, 85.0f);

    float theta = radians(camera.horizontalAngle);
    float phi = radians(camera.verticalAngle);
    camera.lookAt = vec3(cosf(phi) * cosf(theta), sinf(phi), -cosf(phi) * sinf(theta));
    vec3 side = normalize(cross(camera.lookAt, camera.up));

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.position += camera.lookAt * camera.dt * speed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.position -= camera.lookAt * camera.dt * speed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.position += side * camera.dt * speed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.position -= side * camera.dt * speed;

    mat4 viewMatrix;
    if (camera.firstPerson)
    {
        viewMatrix = lookAt(camera.position, camera.position + camera.lookAt, camera.up);
    }
    else
    {
        float r = 5.0f;
        vec3 offset = vec3(r * cosf(phi) * cosf(theta), r * sinf(phi), -r * cosf(phi) * sinf(theta));
        viewMatrix = lookAt(camera.position - offset, camera.position, camera.up);
    }

    setViewMatrix(assets.colorShaderProgram, viewMatrix);
    setViewMatrix(assets.texturedShaderProgram, viewMatrix);

    camera.lastMouseLeftState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);

    camera.dt = glfwGetTime() - camera.lastFrameTime;
    camera.lastFrameTime = glfwGetTime();

    // Spinning cube at camera position

    // Draw avatar in view space for first person camera
    // and in world space for third person camera
    if (camera.firstPerson)
    {
        // Wolrd matrix is identity, but view transform like a world transform relative to camera basis
        // (1 unit in front of camera)
        //
        // This is similar to a weapon moving with camera in a shooter game

        setWorldMatrix(assets.colorShaderProgram, mat4(1.0f));
    }
    else
    {
    }
}

void updateLights(GLuint shader, const vec3 &lightPos1, const vec3 &lightPos2, const vec3 &viewPos)
{
    if (shader == 0)
    {
        std::cerr << "[updateLights] Warning: shader ID is 0. Skipping light update.\n";
        return;
    }
    glUseProgram(shader);
    glUniform3fv(glGetUniformLocation(shader, "lightPos1"), 1, &lightPos1[0]);
    glUniform3fv(glGetUniformLocation(shader, "lightPos2"), 1, &lightPos2[0]);
    glUniform3fv(glGetUniformLocation(shader, "viewPos"), 1, &viewPos[0]);
}

int main(int argc, char *argv[])
{

    bool renderingEBO = true; // enab;es EBO

    // Initialize GLFW and OpenGL version
    glfwInit();

#if defined(PLATFORM_OSX)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
    // On windows, we set OpenGL version to 2.1, to support more hardware
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#endif

    // Create Window and rendering context using GLFW, resolution is 800x600
    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;

    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Comp371 - project 01", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to create GLEW" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Load Textures
    SceneAssets assets = loadSceneAssets(renderingEBO);

    // Black background
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    // glClearColor(0.6f, 0.6f, 0.6f, 1.0f); // soft blue-gray

    // Spinning cube at camera position
    float spinningCubeAngle = 0.0f;

    // Set projection matrix for shader, this won't change
    mat4 projectionMatrix = glm::perspective(70.0f,           // field of view in degrees
                                             800.0f / 600.0f, // aspect ratio
                                             0.01f, 200.0f);  // near and far (near > 0)

    // setup camera input
    CameraState camera;

    // Set initial view matrix
    mat4 viewMatrix = lookAt(camera.position,                 // eye
                             camera.position + camera.lookAt, // center
                             camera.up);                      // up

    // Set View and Projection matrices on both shaders
    setViewMatrix(assets.colorShaderProgram, viewMatrix);
    setViewMatrix(assets.texturedShaderProgram, viewMatrix);

    setProjectionMatrix(assets.colorShaderProgram, projectionMatrix);
    setProjectionMatrix(assets.texturedShaderProgram, projectionMatrix);

    // Other OpenGL states to set once
    // Enable Backface culling
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    // set shader uniforms that don’t change every frame once before the main render loop begins.
    glUseProgram(assets.texturedShaderProgram);
    glUniform1i(glGetUniformLocation(assets.texturedShaderProgram, "textureSampler"), 0);

    // For frame time
    glfwGetCursorPos(window, &camera.lastMouseX, &camera.lastMouseY);

    camera.position = vec3(0, 10, 0);

    // Entering Main Loop
    while (!glfwWindowShouldClose(window))
    {
        // Frame time calculation
        camera.updateDeltaTime(glfwGetTime());

        // Each frame, reset color of each pixel to glClearColor
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // === LIGHTING ===
        float time = glfwGetTime();

        // Light 1 orbits the camera
        vec3 lightPos1 = camera.position + vec3(
                                               10.0f * cos(time), // bigger horizontal radius
                                               5.0f,              // higher vertical offset
                                               10.0f * sin(time)  // bigger horizontal radius
                                           );

        // Light 2 orbits the center of the scene at height 15
        vec3 lightPos2 = vec3(
            10.0f * cos(time * 0.5f),
            15.0f,
            10.0f * sin(time * 0.5f));

        updateLights(assets.texturedShaderProgram, lightPos1, lightPos2, camera.position);

        vec3 viewPos = camera.position; // assuming you track camera pos

        // Draw ground
        drawGround(assets);

        // draw sky
        drawSkybox(assets);

        // draw the rocket
        drawRocket(assets);

        // draw the planets
        drawPlanets(assets);

        // draw the field of cubes
        drawCubeField(assets);

        glUseProgram(assets.colorShaderProgram);

        handleInput(window, assets, camera);

        // End Frame
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}

const char *getVertexShaderSource()
{
    return "#version 330 core\n"
           "layout (location = 0) in vec3 aPos;\n"
           "layout (location = 1) in vec3 aColor;\n"
           "layout (location = 2) in vec2 aUV;\n"
           "layout (location = 3) in vec3 aNormal;\n"

           "uniform mat4 worldMatrix;\n"
           "uniform mat4 viewMatrix;\n"
           "uniform mat4 projectionMatrix;\n"
           "uniform vec2 uvScale;\n"
           "uniform mat3 normalMatrix;\n"

           "out vec3 FragPos;\n"
           "out vec3 Normal;\n"
           "out vec3 vertexColor;\n"
           "out vec2 vertexUV;\n"

           "void main() {\n"
           "    mat4 MVP = projectionMatrix * viewMatrix * worldMatrix;\n"
           "    gl_Position = MVP * vec4(aPos, 1.0);\n"
           "    FragPos = vec3(worldMatrix * vec4(aPos, 1.0));\n"
           "    Normal = normalize(normalMatrix * aNormal);\n"
           "    vertexColor = aColor;\n"
           "    vertexUV = aUV * uvScale;\n"
           "}\n";
}

const char *getFragmentShaderSource()
{
    std::string src =
        "#version 330 core\n"
        "in vec3 FragPos;\n"
        "in vec3 Normal;\n"
        "in vec2 vertexUV;\n"
        "uniform sampler2D textureSampler;\n"
        "uniform vec3 lightPos1;\n"
        "uniform vec3 lightPos2;\n"
        "uniform vec3 viewPos;\n"
        "out vec4 FragColor;\n"
        "void main() {\n"
        "    vec3 norm = normalize(Normal);\n"
        "    vec3 texColor = texture(textureSampler, vertexUV).rgb;\n"

        "    vec3 lightDir1 = normalize(lightPos1 - FragPos);\n"
        "    vec3 lightDir2 = normalize(lightPos2 - FragPos);\n"

        "    float dist1 = length(lightPos1 - FragPos);\n"
        "    float dist2 = length(lightPos2 - FragPos);\n"
        "    float atten1 = 100.0 / (dist1 * dist1);\n"
        "    float atten2 = 100.0 / (dist2 * dist2);\n"

        "    float diff1 = max(dot(norm, lightDir1), 0.0) * atten1;\n"
        "    float diff2 = max(dot(norm, lightDir2), 0.0) * atten2;\n"

        "    vec3 viewDir = normalize(viewPos - FragPos);\n"
        "    vec3 reflectDir1 = reflect(-lightDir1, norm);\n"
        "    vec3 reflectDir2 = reflect(-lightDir2, norm);\n"
        "    float spec1 = pow(max(dot(viewDir, reflectDir1), 0.0), 32.0) * atten1;\n"
        "    float spec2 = pow(max(dot(viewDir, reflectDir2), 0.0), 32.0) * atten2;\n"

        "    vec3 ambient = 0.6 * texColor;\n"
        "    vec3 diffuse = texColor * (diff1 + diff2) * 1.0;\n"
        "   vec3 specular = vec3(1.0) * (spec1 + spec2) * 1.0;\n "

        "    vec3 lighting = ambient + diffuse + specular;\n"
        "    FragColor = vec4(lighting, 1.0);\n"
        "}\n";

    return strdup(src.c_str());
}

int compileAndLinkShaders(const char *vertexShaderSource, const char *fragmentShaderSource)
{
    // compile and link shader program
    // return shader program id
    // ------------------------------------

    // vertex shader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    // fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    // link shaders
    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

GLuint loadTexture(const char *filename)
{
    // Step1 Load Textures with dimension data
    int width, height, nrChannels;
    unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0);
    if (!data)
    {
        std::cerr << "Error::Texture could not load texture file:" << filename << std::endl;
        return 0;
    }

    // Step2 Create and bind textures
    GLuint textureId = 0;
    glGenTextures(1, &textureId);
    assert(textureId != 0);

    glBindTexture(GL_TEXTURE_2D, textureId);

    // Step3 Set filter parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Step4 Upload the texture to the PU
    GLenum format = 0;
    if (nrChannels == 1)
        format = GL_RED;
    else if (nrChannels == 3)
        format = GL_RGB;
    else if (nrChannels == 4)
        format = GL_RGBA;

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height,
                 0, format, GL_UNSIGNED_BYTE, data);

    // Step5 Free resources
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
    return textureId;
}
