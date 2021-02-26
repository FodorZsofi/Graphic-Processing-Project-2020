#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm/glm.hpp> //core glm functionality
#include <glm/glm/gtc/matrix_transform.hpp> //glm extension for generating common transformation matrices
#include <glm/glm/gtc/matrix_inverse.hpp> //glm extension for computing inverse matrices
#include <glm/glm/gtc/type_ptr.hpp> //glm extension for accessing the internal data structure of glm types

#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"
#include "SkyBox.hpp"

#include <iostream>



// window
gps::Window myWindow;
typedef glm::mat4(*modelMatrix)();
// matrices
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
glm::mat3 normalMatrix;

// light parameters
glm::vec3 lightDir;
glm::vec3 lightColor;
glm::mat3 lightDirMatrix;

// shader uniform locations
GLuint modelLoc;
GLuint viewLoc;
GLuint projectionLoc;
GLuint normalMatrixLoc;
GLuint lightDirLoc;
GLuint lightColorLoc;
GLuint lightDirMatrixLoc;

GLfloat lightAngle;

int retina_width = 1024, retina_height = 648;
float dogX = -0.95;
float dogY = -1.21;
float dogZ = -1.1;

float butterflyX = -0.5;
float butterflyY = -0.5;
float butterflyZ = -1.36;

float delta = 0.0;

float catX = -1.05f;
float catY = -1.2099f;
float catZ = -0.9f;

float lampX = 0.27f;
float lampY = -1.22;
float lampZ = -0.53;

float cubeX = dogX, cubeY = dogY, cubeZ = dogZ;

float sceneX = -0.7f, sceneY = -1.01f, sceneZ = -1.7f;

double currentTimeStamp;
double elapsedTime;
double lastTimeStamp = glfwGetTime();

// camera
gps::Camera myCamera(
    glm::vec3(-0.92f, -1.15f, 0.42f),
    glm::vec3(0.0f, -0.5f, -10.0f),
    glm::vec3(0.0f, 1.0f, 0.0f));

GLfloat cameraSpeed = 0.3f;

GLboolean pressedKeys[1024];
float movementSpeed = 20.0;

typedef glm::mat4(*modelMatrix)();


// models
gps::Model3D cat;
gps::Model3D butterfly;
gps::Model3D dog;
gps::Model3D scene;
gps::Model3D lamp;
gps::Model3D lightCube;
gps::Model3D flower;
GLfloat angle;


glm::mat4 butterflyModel;
glm::mat4 catModel;
glm::mat4 dogModel;
glm::mat4 lampModel;
glm::mat4 sceneModel;
glm::mat4 cubeModel;
glm::mat4 flowerModlel;

float flowerX = 0.001f, flowerY = 0.001f, flowerZ = 0.001f;

bool openGate = false;
float degreeGate = 41.85f;
int j = 0;


gps::SkyBox mySkyBox;
// shaders
gps::Shader myCustomShader;
gps::Shader lightShader;
gps::Shader screenQuadShader;
gps::Shader depthMapShader;

gps::Shader skyboxShader;
std::vector<const GLchar*> faces;


GLuint textureID;
const unsigned int SHADOW_WIDTH = 10048;
const unsigned int SHADOW_HEIGHT = 10048;
GLuint shadowMapFBO;
GLuint depthMapTexture;
GLFWwindow* glWindow = NULL;

GLenum glCheckError_(const char* file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR) {
        std::string error;
        switch (errorCode) {
        case GL_INVALID_ENUM:
            error = "INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            error = "INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            error = "INVALID_OPERATION";
            break;
        case GL_STACK_OVERFLOW:
            error = "STACK_OVERFLOW";
            break;
        case GL_STACK_UNDERFLOW:
            error = "STACK_UNDERFLOW";
            break;
        case GL_OUT_OF_MEMORY:
            error = "OUT_OF_MEMORY";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            error = "INVALID_FRAMEBUFFER_OPERATION";
            break;
        }
        //std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height)
{
    fprintf(stdout, "Window resized! New width: %d , and height: %d\n", width, height);
    //TODO
    glfwGetFramebufferSize(myWindow.getWindow(), &retina_width, &retina_height);


    projection = glm::perspective(glm::radians(45.0f),
        (float)width / (float)height / 2,
        0.1f, 800.0f);
    projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
    // send projection matrix to shader
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glViewport(0, 0, retina_width, retina_height);
}




void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            pressedKeys[key] = true;
        }
        else if (action == GLFW_RELEASE) {
            pressedKeys[key] = false;
        }
    }
}




double yaw = -90.0f, pitch = 0.0f;
double yaw2 = -90.0f, pitch2 = 0.0f;

bool firstCursorChange = true;
double xdiff = 0;
double ydiff = 0;
double lastX = 400;
double lastY = 300;
float mouseSensitivity = 0.05;

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    //TODO
    if (firstCursorChange) {
        xdiff = 0;
        ydiff = 0;
        firstCursorChange = false;
    }
    else {
        xdiff = xpos - lastX;
        ydiff = lastY - ypos;
    }

    lastX = xpos;
    lastY = ypos;

    xdiff *= mouseSensitivity;
    ydiff *= mouseSensitivity;

    yaw += xdiff;
    pitch += ydiff;

    if (pitch > 89.0f) {
        pitch = 89.0f;
    }

    if (pitch < -89.0f) {
        pitch = -89.0f;
    }

    myCamera.rotate(pitch, yaw);
    view = myCamera.getViewMatrix();
    myCustomShader.useShaderProgram();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    //std::cout << myCamera.getCameraTarget().x << " " << myCamera.getCameraTarget().y << " " << myCamera.getCameraTarget().z << "  Cam target " << std::endl;
}



float fov = 45.0f;
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (fov >= 1.0f && fov <= 45.0f)
        fov -= yoffset;
    if (fov <= 1.0f)
        fov = 1.0f;
    if (fov >= 45.0f)
        fov = 45.0f;
}

bool lightOn = false;
//GLFWwindow* glWindow = NULL;
bool flowerAppear = true;
bool willRot = false;
float flowerAngle = -90.0f;

bool rotateCamera = true;
void processMovement() {


    if (pressedKeys[GLFW_KEY_R])
    {
        gps::Camera cam(
            glm::vec3(0.0f, -0.5f, 3.0f),
            glm::vec3(0.0f, -0.5f, -10.0f),
            glm::vec3(0.0f, 1.0f, 0.0f));
        myCamera = cam;
    }
    

    if (pressedKeys[GLFW_KEY_W]) {
        myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myCustomShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_S]) {
        myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myCustomShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_A]) {
        myCamera.move(gps::MOVE_LEFT, cameraSpeed);
        view = myCamera.getViewMatrix();
        myCustomShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_D]) {
        myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myCustomShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_R])
    {
        // applyLight();
    }

    if (pressedKeys[GLFW_KEY_Q]) {
        angle -= 1.0f;
        // update model matrix for teapot
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        // update normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_E]) {
        angle += 1.0f;
        // update model matrix for teapot
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        // update normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_X])
    {
        if (dogX < -0.75) 
        {
            dogX += 0.02;
            catX -= 0.02;
        }
    }

    if (pressedKeys[GLFW_KEY_Z])
    {
        if (dogX > -0.95)
        {
            dogX -= 0.02;
            catX += 0.02;
        }
    }

    if (pressedKeys[GLFW_KEY_C]) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    if (pressedKeys[GLFW_KEY_V]) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    if (pressedKeys[GLFW_KEY_B]) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    }


    if (pressedKeys[GLFW_KEY_M]) {
        lightOn = !lightOn;
    }

    if (pressedKeys[GLFW_KEY_J]) {

        lightAngle += 1.0f;
        if (lightAngle > 360.0f)
            lightAngle -= 360.0f;
        glm::vec3 lightDirTr = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(lightDir, 1.0f));
        myCustomShader.useShaderProgram();
        glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDirTr));
    }

    if (pressedKeys[GLFW_KEY_H]) {
        lightAngle -= 1.0f;
        if (lightAngle < 0.0f)
            lightAngle += 360.0f;
        glm::vec3 lightDirTr = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(lightDir, 1.0f));
        myCustomShader.useShaderProgram();
        glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDirTr));
    }

    if (pressedKeys[GLFW_KEY_O] && !willRot) {
        if (flowerX > 0.004)
            willRot = true;

        else {
            flowerX += 0.001;
        }
    }

    if (pressedKeys[GLFW_KEY_L]) {

        sceneY += 0.1;
    }

    if (pressedKeys[GLFW_KEY_P]) {
        sceneZ += 0.1;
    }
    if (pressedKeys[GLFW_KEY_LEFT]) {
        sceneX -= 0.01;
    }

    if (pressedKeys[GLFW_KEY_RIGHT]) {
        sceneY -= 0.1;
    }

    if (pressedKeys[GLFW_KEY_UP]) {
        sceneZ -= 0.1;
    }
}

void initOpenGLWindow() {
    myWindow.Create(1024, 648, "OpenGL Project Core");
}

void setWindowCallbacks() {
    glfwSetWindowSizeCallback(myWindow.getWindow(), windowResizeCallback);
    glfwSetKeyCallback(myWindow.getWindow(), keyboardCallback);
    glfwSetCursorPosCallback(myWindow.getWindow(), mouseCallback);
}

void initOpenGLState() {
    glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
    glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
    glEnable(GL_CULL_FACE); // cull face
    glCullFace(GL_BACK); // cull back face
    glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
}



void initModels() {


    cat.LoadModel("objects/Cat_v1_L3.123cb1b1943a-2f48-4e44-8f71-6bbe19a3ab64/12221_Cat_v1_l3.obj");
    scene.LoadModel("objects/project.obj");
    dog.LoadModel("models/dog/13466_Canaan_Dog_v1_L3.obj");
    butterfly.LoadModel("objects/butterfly/untitled.obj");
    lamp.LoadModel("models/lamp/streetlamp.obj");
    lightCube.LoadModel("objects/cube/cube.obj");
    flower.LoadModel("objects/crocus/12974_crocus_flower_v1_l3.obj");

    mySkyBox.Load(faces);

}

void initShaders() {

    myCustomShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
    myCustomShader.useShaderProgram();
    depthMapShader.loadShader("shaders/depthMapShader.vert", "shaders/depthMapShader.frag");
    lightShader.loadShader("shaders/lightCube.vert", "shaders/lightCube.frag");
    skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
    skyboxShader.useShaderProgram();
}



GLuint lightSpaceTrMatrixLoc;
//GLuint lightDirMatrixLoc;

void initUniforms()
{
    lightSpaceTrMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightSpaceTrMatrix");
    lightDirMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDirMatrix");
    myCustomShader.useShaderProgram();


    modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");

    // get view matrix for current camera
    view = myCamera.getViewMatrix();
    viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
    // send view matrix to shader
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    // compute normal matrix for teapot
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");

    // create projection matrix
    projection = glm::perspective(glm::radians(45.0f),
        (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
        0.1f, 1000.0f);
    projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
    // send projection matrix to shader
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    //set the light direction (direction towards the light)
    lightDir = glm::vec3(0.0f, 1.0f, 1.0f);
    lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
    // send light dir to shader
    glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

    //set light color
    lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
    lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
    // send light color to shader
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
    lightShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
}

void initFBO() {
    //TODO - Create the FBO, the depth texture and attach the depth texture to the FBO

    //generate FBO ID
    glGenFramebuffers(1, &shadowMapFBO);
    glGenTextures(1, &depthMapTexture);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void initSkyBox() {
    faces.push_back("textures/skybox/right.tga");
    faces.push_back("textures/skybox/left.tga");
    faces.push_back("textures/skybox/top.tga");
    faces.push_back("textures/skybox/bottom.tga");
    faces.push_back("textures/skybox/back.tga");
    faces.push_back("textures/skybox/front.tga");

    mySkyBox.Load(faces);
}

void drawSkyBox() {
    skyboxShader.useShaderProgram();

    mySkyBox.Draw(skyboxShader, view, projection);
}

glm::mat4 computeLightSpaceTrMatrix() {

    const GLfloat near_plane = 1.0f, far_plane = 8.0f;
    glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);

    glm::vec3 lightDirTr = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(lightDir, 1.0f));

    glm::mat4 lightView = glm::lookAt(lightDirTr, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    return lightProjection * lightView;
}


int flag = 0;


glm::mat4 renderLamp()
{
    lampModel = model;

    lampModel = glm::translate(glm::mat4(1.0f), glm::vec3(lampX, lampY, lampZ));
    lampModel = glm::scale(lampModel, glm::vec3(0.05f, 0.05f, 0.05f));


    return lampModel;
}

float rotateCow = 0.0f;
int flag3 = 0;
float prevAngle3 = 0.0;
glm::mat4 renderButterfly()
{
    float rotateAngle = delta * 1.3;
    while (rotateAngle > 360)
    {
        rotateAngle -= 360;
    }
    butterflyModel = glm::mat4(1.0f);


    butterflyModel = glm::translate(glm::mat4(1.0f), glm::vec3(butterflyX, butterflyY, butterflyZ));
    butterflyModel = glm::scale(butterflyModel, glm::vec3(0.005f, 0.005f, 0.005f));

    butterflyModel = glm::rotate(butterflyModel, glm::radians(rotateAngle), glm::vec3(0.0f, 1.0f, 0.0f));



    butterflyModel = glm::rotate(butterflyModel, glm::radians(180.0f), glm::vec3(0.0f, 12.0f, 0.0f));

    return butterflyModel;
}

glm::mat4 renderCat()
{
    catModel = model;

    catModel = glm::translate(glm::mat4(1.0f), glm::vec3(catX, catY, catZ));
    catModel = glm::scale(catModel, glm::vec3(0.002f, 0.002f, 0.002f));
    catModel = glm::rotate(catModel, glm::radians(-180.0f), glm::vec3(0, 0, 1));
    catModel = glm::rotate(catModel, glm::radians(50.0f), glm::vec3(0, 1, 0));
    catModel = glm::rotate(catModel, glm::radians(90.0f), glm::vec3(1, 0, 0));

    return catModel;
}


glm::mat4 renderDog()
{
    dogModel = model;
    dogModel = glm::translate(glm::mat4(1.0f), glm::vec3(dogX, dogY, dogZ));
    dogModel = glm::scale(dogModel, glm::vec3(0.004f, 0.004f, 0.004f));
    dogModel = glm::rotate(dogModel, glm::radians(-90.0f), glm::vec3(1, 0, 0));
    dogModel = glm::rotate(dogModel, glm::radians(-50.0f), glm::vec3(0, 0, 1));


    return dogModel;
}


glm::mat4 renderFlower()
{
    flowerModlel = model;
    if (willRot)
    {
        flowerAngle += 10.0f;
        if (flowerAngle == 0.0f)
            flowerAppear = false;
    }
    flowerModlel = glm::translate(glm::mat4(1.0f), glm::vec3(-2.05, -1.21, -1.2));
    flowerModlel = glm::scale(flowerModlel, glm::vec3(flowerX, flowerX, flowerX));
    flowerModlel = glm::rotate(flowerModlel, glm::radians(flowerAngle), glm::vec3(1, 0, 0));
    return flowerModlel;
}




glm::mat4 renderObjScene()
{
    sceneModel = model;
    sceneModel = glm::translate(glm::mat4(1.0f), glm::vec3(sceneX, sceneY, sceneZ));
    sceneModel = glm::scale(sceneModel, glm::vec3(0.2f, 0.2f, 0.2f));
    sceneModel = glm::rotate(sceneModel, glm::radians(-90.0f), glm::vec3(0, 1, 0));

    return sceneModel;
}



void renderObj(gps::Model3D obj, glm::mat4 func, gps::Shader shader)
{
    model = func;
    shader.useShaderProgram();
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));


    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    obj.Draw(shader);
}

void renderShadowsInit()
{
    depthMapShader.useShaderProgram();

    glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"),
        1,
        GL_FALSE,
        glm::value_ptr(computeLightSpaceTrMatrix()));
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
}



glm::vec4  lightSource1;
GLuint lightSourceLoc1;


void drawObjectShadow(gps::Model3D modelToDraw, glm::mat4 func)
{
    model = func;


    glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),
        1,
        GL_FALSE,
        glm::value_ptr(model));
    modelToDraw.Draw(depthMapShader);
}


glm::vec3 lightPos;// = glm::vec3(0.27f, -1.22f, 0.53f);
glm::vec3 greenLight = glm::vec3(0.0f, 255.0f, 0.0f);
glm::vec3 whiteLight = glm::vec3(255.0f, 255.0f, 230.0f);
bool reseted = false;
glm::mat4 lightRotation;
bool notrotated = true;
bool firstIteration = true;
void renderScene()
{
    view = myCamera.getViewMatrix();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));


    dogModel = renderDog();
    sceneModel = renderObjScene();
    catModel = renderCat();
    butterflyModel = renderButterfly();
    lampModel = renderLamp();
    flowerModlel = renderFlower();
    if (rotateCamera == true)
    {
        gps::Camera cam(
            glm::vec3(-1.32f, -1.15f, -1.08f),
            glm::vec3(0.0f, -0.5f, -10.0f),
            glm::vec3(0.0f, 1.0f, 0.0f));
        myCamera = cam;
        myCamera.rotate(pitch2, yaw2);
         yaw2 += 10.0f;
        if (yaw2 >= 360.0f)
        {
            
            rotateCamera = false;
            yaw2 = 0.0f;
        }
    }
    else if (reseted == false)
    {
        gps::Camera cam(
            glm::vec3(-0.92f, -1.15f, 0.42f),
            glm::vec3(0.0f, -0.5f, -10.0f),
            glm::vec3(0.0f, 1.0f, 0.0f));
        myCamera = cam;
        reseted = true;
        //myCamera.rotate(90.0, yaw);
    }

    /*if (notrotated && rotateCamera)
    {
        gps::Camera cam(
            glm::vec3(-1.90342f, -1.1619f, -1.29255f),
            glm::vec3(-1.22492f, -0.70015f, -0.721208f),
            glm::vec3(0.13f, 0.94f, -0.31f));
        myCamera = cam;
        notrotated = false;
    }

    if(rotateCamera) {
        myCamera.rotate(pitch, yaw);
        yaw += 10.0f;
        if (yaw >= 360.0f)
        {
            rotateCamera = false;
            gps::Camera cam(
                glm::vec3(0.0f, -5.0f, 3.0f),
                glm::vec3(0.0f, -1.0f, -10.0f),
                glm::vec3(0.05f, 0.9f, -0.27f));
            myCamera = cam;
            yaw = 0.0f;
        }
    }*/


   // std::cout << pitch << " " << yaw << std::endl;


    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderShadowsInit();
    drawObjectShadow(dog, dogModel);
    drawObjectShadow(cat, catModel);
    drawObjectShadow(scene, sceneModel);
    drawObjectShadow(butterfly,butterflyModel);
    drawObjectShadow(lamp, lampModel);
    if (flowerAppear) {
        drawObjectShadow(flower, flowerModlel);
    }


    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // get current time
    double currentTimeStamp = glfwGetTime();
    delta = delta + movementSpeed * (currentTimeStamp - lastTimeStamp);
    lastTimeStamp = currentTimeStamp;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (lightOn)
    {
        lightPos = glm::vec3(0.27f, -1.22f, 0.53f);
        myCustomShader.useShaderProgram();
        glUniform3fv(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[0].position"), 1, glm::value_ptr(lightPos));
        glUniform3fv(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[0].color"), 1, glm::value_ptr(greenLight));
    }
    else
    {
        lightPos = glm::vec3(-3.27f, -10.22f, 7.53f);
        myCustomShader.useShaderProgram();
        glUniform3fv(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[0].position"), 1, glm::value_ptr(lightPos));
        glUniform3fv(glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[0].color"), 1, glm::value_ptr(greenLight));
    }




    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    myCustomShader.useShaderProgram();

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);
    glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "shadowMap"), 3);

    glUniformMatrix4fv(lightSpaceTrMatrixLoc, 1, GL_FALSE, glm::value_ptr(computeLightSpaceTrMatrix()));
    view = myCamera.getViewMatrix();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    lightDirMatrix = glm::mat3(glm::inverseTranspose(view));
    glUniformMatrix3fv(lightDirMatrixLoc, 1, GL_FALSE, glm::value_ptr(lightDirMatrix));


    glViewport(0, 0, retina_width + 1, retina_height + 1);

    lightShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

    model = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, lightDir);
    model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
    glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    lightCube.Draw(lightShader);
    //renderModelsInit();
    renderObj(lamp, lampModel, myCustomShader);
    renderObj(dog, dogModel, myCustomShader);
    renderObj(scene, sceneModel, myCustomShader);
    renderObj(cat, catModel, myCustomShader);
    renderObj(butterfly, butterflyModel, myCustomShader);
    if (flowerAppear) {
        renderObj(flower, flowerModlel, myCustomShader);
    }
    else
    {
        flowerAppear = !flowerAppear;
        willRot = !willRot;
        flowerAngle = -90.0f;
        flowerX = 0.001;
    }
    drawSkyBox();


   

}

void cleanup() {
    myWindow.Delete();
    //cleanup code for your own data
}

int main(int argc, const char* argv[]) {

    try {
        initOpenGLWindow();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }


    initOpenGLState();
    initModels();
    initShaders();
    initUniforms();
    initSkyBox();
    initFBO();
    setWindowCallbacks();
    glCheckError();
    // application loop
    while (!glfwWindowShouldClose(myWindow.getWindow())) {
        processMovement();
        renderScene();

        glfwPollEvents();
        glfwSwapBuffers(myWindow.getWindow());
    }

    cleanup();

    return EXIT_SUCCESS;
}
