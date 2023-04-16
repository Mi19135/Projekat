
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


#include <cstdlib>
#include <stb_image.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cmath>


#include <learnopengl/filesystem.h>
#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void mouse_callback(GLFWwindow *window, double xpos, double ypos);

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

void processInput(GLFWwindow *window);

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

unsigned int loadTexture(const char *path);
unsigned int loadCubemap(vector<std::string> faces);

void moonMovement(Camera_Movement direction);
bool go_left=false;
bool go_right=false;
bool front=false;
bool back=true;

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera=(glm::vec3(10.0f,0.0f,0.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;


bool blinn = false;
bool blinnKeyPressed = false;

glm::vec3 moonPosition(glm::vec3(0.0f, 25.0f, 20.0f));


// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;




glm::vec3 lightPos(1.2f, 1.0f, 2.0f);


int main() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", nullptr, nullptr);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }



    glfwMakeContextCurrent(window);


    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    Shader skyboxShader("resources/shaders/skybox.vs", "resources/shaders/skybox.fs");
    Shader shaderBlending("resources/shaders/blending.vs", "resources/shaders/blending.fs");
    Shader shaderBlinn("resources/shaders/advanced_lighting.vs", "resources/shaders/advanced_lighting.fs");

    Shader shader("resources/shaders/instancing.vs", "resources/shaders/instancing.fs");





    float skyboxVertices[] = {
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f,  1.0f
    };


    float planeVertices[] = {
            // positions            // normals         // texcoords
            10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
            -10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
            -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,

            10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
            -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
            10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f
    };

    float transparentVertices[] = {
            // positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
            0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
            0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
            1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

            0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
            1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
            1.0f,  0.5f,  0.0f,  1.0f,  0.0f
    };




    //skybox - arrays & buffers
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // plane VAO
    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glBindVertexArray(0);

    // transparent VAO
    unsigned int transparentVAO, transparentVBO;
    glGenVertexArrays(1, &transparentVAO);
    glGenBuffers(1, &transparentVBO);
    glBindVertexArray(transparentVAO);
    glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);




    vector<std::string> faces= {
            FileSystem::getPath("resources/textures/skybox/right.jpg"),
            FileSystem::getPath("resources/textures/skybox/left.jpg"),
            FileSystem::getPath("resources/textures/skybox/top.jpg"),
            FileSystem::getPath("resources/textures/skybox/bottom.jpg"),
            FileSystem::getPath("resources/textures/skybox/front.jpg"),
            FileSystem::getPath("resources/textures/skybox/back.jpg")
    };




    unsigned int cubemapTexture = loadCubemap(faces);


    unsigned int floorTexture = loadTexture(FileSystem::getPath("resources/textures/grass1.jpg").c_str());
    unsigned int specularMap = loadTexture(FileSystem::getPath("resources/textures/grassSpecular.png").c_str());


    shader.use();
    shader.setInt("material.diffuse", 0);
    shader.setInt("material.specular", 1);



    unsigned int transparentTexture = loadTexture(FileSystem::getPath("resources/textures/p4.png").c_str());


    vector<glm::vec3> vegetation
            {
                    glm::vec3(-300.5f, 50.0f, -350.5f),
                    glm::vec3( -100.5f, 50.0f, -350.51f),
                    glm::vec3( 100.0f, 50.0f, -350.7f),
                    glm::vec3(300.3f, 50.0f, -350.3f),
                    glm::vec3 (500.5f, 50.0f, -350.6f)
            };
    shader.use();
    shader.setInt("texture1", 0);

    //
    // load models
    // -----------

    Model leptir(FileSystem::getPath("resources/objects/leptir/butterfly.obj"));
    leptir.SetShaderTextureNamePrefix("material.");


    Model cvet(FileSystem::getPath("resources/objects/cvet/cvet.obj"));
    cvet.SetShaderTextureNamePrefix("material.");


    Model moon(FileSystem::getPath("resources/objects/moon/moon.obj"));
    moon.SetShaderTextureNamePrefix("material.");



   float time=glfwGetTime();

    // generate a large list of semi-random model transformation matrices
    // ------------------------------------------------------------------
    unsigned int amount = 100;
    glm::mat4* modelMatrices;
    modelMatrices = new glm::mat4[amount];
    srand(static_cast<unsigned int>(glfwGetTime())); // initialize random seed
    float radius = 50.0;
    float offset = 1.5f;
    for (unsigned int i = 0; i < amount; i++)
    {
        glm::mat4 model = glm::mat4(1.0f);
        // 1. translation: displace along circle with 'radius' in range [-offset, offset]
        float angle = (float)i / (float)amount * 360.0f;
        float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float x = sin(angle) * radius + displacement;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float y = displacement * 0.4f; // keep height of asteroid field smaller compared to width of x and z
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float z = cos(angle) * radius + displacement;
        model = glm::translate(model, glm::vec3(x, y, z));

        // 2. scale: Scale between 0.05 and 0.25f
        float scale = static_cast<float>((rand() % 20) / 100.0 + 0.05);
        model = glm::scale(model, glm::vec3(scale));

        // 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
        float rotAngle = static_cast<float>((rand() % 360));
        model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

        // 4. now add to list of matrices
        modelMatrices[i] = model;
    }

    shaderBlinn.use();
    shaderBlinn.setInt("texture1", 0);

    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);



    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);


        // render
        // ------
        glClearColor(0.1f,0.1f,0.1f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        shader.setVec3("viewPos", camera.Position);
        shader.setVec3("light.position", lightPos);
        shader.setVec3("light.direction", camera.Front);
        shader.setFloat("light.cutOff", glm::cos(glm::radians(12.5f)));
        shader.setFloat("light.outerCutOff", glm::cos(glm::radians(17.5f)));



        shader.setVec3("light.specular", 1.0f,1.0f,1.0f);
        shader.setVec3("light.ambient", 0.2f,0.2f,0.2f);
        shader.setVec3("light.diffuse",0.7f,0.7f,0.7f);


        shader.setFloat("light.constant", 1.0f);
        shader.setFloat("light.linear", 0.3f);
        shader.setFloat("light.quadratic", 0.1f);


        shader.setFloat("material.shininess", 32.0f);

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);
        //shader.use();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);

        // world transformation
        shader.setMat4("model", model);


       shader.setInt("blinn", blinn);



        glEnable(GL_TEXTURE_2D);
        // bind diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        // bind specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        // floor

        glBindVertexArray(planeVAO);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 15.0f, 0.0f));
        model = glm::scale(model, glm::vec3(100.0f, 60.0f, 100.0f));

        // model = glm::rotate(model, glm::radians(90.0f), glm::vec3(100, 100, 0));
        shader.setMat4("model", model);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);

        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_CULL_FACE);



        // draw flower
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -20.0f, 15.0f));
        model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(-3.5, 1, 1));
        shader.setMat4("model", model);
        cvet.Draw(shader);



        // draw butterflies

        for (unsigned int i = 0; i < amount; i++)
        {
            shader.setMat4("model", modelMatrices[i]);
            leptir.Draw(shader);
        }


         //draw moon

        model = glm::mat4(1.0f);
        model = glm::translate(model, moonPosition);
        if (front) {
            model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
        }
        if (go_left) {
            model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0, 1.0, 0.0));
        }
        if (go_right) {
            model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
        }


        model = glm::scale(model, glm::vec3(6.0f, 6.0f, 6.0f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(-3.5, 1, 1));
        shader.setMat4("model", model);
        shader.setVec3("lightColor", (glm::vec3(5.0f,   5.0f,  5.0f)));

        moon.Draw(shader);





        // vegetation
        shaderBlending.use();
        shaderBlending.setMat4("view", view);
        shaderBlending.setMat4("projection", projection);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glBindVertexArray(transparentVAO);
        glBindTexture(GL_TEXTURE_2D, transparentTexture);
        for (unsigned int i = 0; i < vegetation.size(); i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, vegetation[i]);
            model = glm::scale(model, glm::vec3(210.0f, 210.0f, 210.0f));

            shaderBlending.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        glDisable(GL_CULL_FACE);






        // skybox shader setup
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);




        // render skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);


       // std::cout << (blinn ? "Blinn-Phong" : "Phong") << std::endl;


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------

        glfwSwapBuffers(window);
        glfwPollEvents();

    }

     glDeleteVertexArrays(1,&skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);
    glDeleteBuffers(1, &planeVBO);

    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);


    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        front = true;
        back = go_right = go_left = false;
        moonMovement(FORWARD);
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        back = true;
        front = go_left = go_right = false;
        moonMovement(BACKWARD);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        go_left = true;
        go_right = front = back = false;
        moonMovement(LEFT);
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        go_right=true;
        go_left= front= back=false;
        moonMovement(RIGHT);
    }



    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !blinnKeyPressed)
    {
        blinn = !blinn;
        blinnKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE)
    {
        blinnKeyPressed = false;
    }

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

   // if (programState->CameraMouseMovementUpdateEnabled)
        camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
   camera.ProcessMouseScroll(yoffset);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_F1 && action == GLFW_PRESS) {
         glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    } else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

}

unsigned int loadTexture(char const * path){
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data){
        GLenum format =GL_RGB;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, (GLint)format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else{
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}



unsigned int loadCubemap(vector<std::string> faces){
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++){
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data){
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else{
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

void moonMovement(Camera_Movement direction){
    float v = 1.8f * deltaTime;
    glm::vec3 s(1.0f, 0.0f, 1.0f);

    if (direction == FORWARD)
        moonPosition += camera.Front * v * s;
    if (direction == BACKWARD)
        moonPosition-= camera.Front * v * s;
    if (direction == LEFT)
        moonPosition -= camera.Right * v * s;
    if (direction == RIGHT)
        moonPosition += camera.Right * v * s;
}