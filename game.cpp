#include <iostream>
#include <time.h>
#include <sstream>

#include "game.h"
#include "path_config.h"

namespace game {

// Some configuration constants
// They are written here as global variables, but ideally they should be loaded from a configuration file

// Main window settings
const std::string window_title_g = "COMP 3501 Assignment 3: Tree";
const unsigned int window_width_g = 800;
const unsigned int window_height_g = 600;
const bool window_full_screen_g = false;

// Viewport and camera settings
float camera_near_clip_distance_g = 0.01;
float camera_far_clip_distance_g = 1000.0;
float camera_fov_g = 60.0; // Field-of-view of camera (degrees)
const glm::vec3 viewport_background_color_g(0.0, 0.0, 0.0);
glm::vec3 camera_position_g(0.0, 5.0, 8.0);
glm::vec3 camera_look_at_g(0.0, 2.5, 0.0);
glm::vec3 camera_up_g(0.0, 1.0, 0.0);

// Materials 
const std::string material_directory_g = MATERIAL_DIRECTORY;

// Manipulator
Manipulator* manipulator = new Manipulator();


Game::Game(void){

    // Don't do work in the constructor, leave it for the Init() function
}


void Game::Init(void){

    // Run all initialization steps
    InitWindow();
    InitView();
    InitEventHandlers();

    // Set variables
    animating_ = true;
}

       
void Game::InitWindow(void){

    // Initialize the window management library (GLFW)
    if (!glfwInit()){
        throw(GameException(std::string("Could not initialize the GLFW library")));
    }

    // Create a window and its OpenGL context
    if (window_full_screen_g){
        window_ = glfwCreateWindow(window_width_g, window_height_g, window_title_g.c_str(), glfwGetPrimaryMonitor(), NULL);
    } else {
        window_ = glfwCreateWindow(window_width_g, window_height_g, window_title_g.c_str(), NULL, NULL);
    }
    if (!window_){
        glfwTerminate();
        throw(GameException(std::string("Could not create window")));
    }

    // Make the window's context the current 
    glfwMakeContextCurrent(window_);

    // Initialize the GLEW library to access OpenGL extensions
    // Need to do it after initializing an OpenGL context
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK){
        throw(GameException(std::string("Could not initialize the GLEW library: ")+std::string((const char *) glewGetErrorString(err))));
    }
}


void Game::InitView(void){

    // Set up z-buffer
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Set viewport
    int width, height;
    glfwGetFramebufferSize(window_, &width, &height);
    glViewport(0, 0, width, height);

    // Set up camera
    // Set current view
    camera_.SetView(camera_position_g, camera_look_at_g, camera_up_g);
    // Set projection
    camera_.SetProjection(camera_fov_g, camera_near_clip_distance_g, camera_far_clip_distance_g, width, height);
    // Set acceleration
    camera_.SetSpeed(0.0f);
}


void Game::InitEventHandlers(void){

    // Set event callbacks
    glfwSetKeyCallback(window_, KeyCallback);
    glfwSetFramebufferSizeCallback(window_, ResizeCallback);

    // Set pointer to game object, so that callbacks can access it
    glfwSetWindowUserPointer(window_, (void *) this);
}


void Game::SetupResources(void){
    // STEMS AND LEAVES
    resman_.CreateCylinder("Cylinder", 2, 0.15);
    std::string filename = std::string(MATERIAL_DIRECTORY) + std::string("/kelp_material");
    resman_.LoadResource(Material, "KelpMaterial", filename.c_str());

    // Sphere for leaves
    resman_.CreateSphere("Sphere", 1.0f, 90, 45);
    filename = std::string(MATERIAL_DIRECTORY) + std::string("/kelp_material");
    resman_.LoadResource(Material, "KelpMaterial", filename.c_str());
}


void Game::SetupScene(void){
    
    scene_.SetBackgroundColor(viewport_background_color_g);
    
    scene_.AddNode(manipulator->ConstructKelp(&resman_, 4, glm::vec3(0.0, 0.0, -5.0)));
    scene_.AddNode(manipulator->ConstructKelp(&resman_, 4, glm::vec3(-5.0, 0.0, -5.0)));
}

void Game::MainLoop(void){
    // Loop while the user did not close the window
    while (!glfwWindowShouldClose(window_)){

        // Animate the scene
        if (animating_){
            static double last_time = 0;
            double current_time = glfwGetTime();
            float mytheta = glm::pi<float>() / 64;
            if ((current_time - last_time) > 0.05){
                scene_.Update(&camera_, &resman_);
                manipulator->AnimateAll(&scene_, current_time, mytheta);
                last_time = current_time;
            }
        }

        // Process camera/player forward movement
        camera_.Translate(camera_.GetForward() * camera_.GetSpeed());
        if (camera_.GetSpeed() > 0) {
            camera_.SetSpeed(camera_.GetSpeed() * 0.98);

        }

        // Draw the scene
        scene_.Draw(&camera_);

        // Push buffer drawn in the background onto the display
        glfwSwapBuffers(window_);

        // Update other events like input handling
        glfwPollEvents();
    }
}


void Game::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods){

    // Get user data with a pointer to the game class
    void* ptr = glfwGetWindowUserPointer(window);
    Game *game = (Game *) ptr;

    // Quit game if 'q' is pressed
    if (key == GLFW_KEY_Q && action == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }

    // Stop animation if space bar is pressed
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS){
        game->animating_ = (game->animating_ == true) ? false : true;
    }

    // Print debugging information when 'p' is pressed
    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        std::string pos = glm::to_string(game->camera_.GetPosition());
        std::string forw = glm::to_string(game->camera_.GetForward());
        std::string side = glm::to_string(game->camera_.GetSide());
        std::string up = glm::to_string(game->camera_.GetUp());
        //glm::quat ori = game->camera_.GetOrientation();
        std::cout << "\nPOS: " << pos
                  << "\nFOR: " << forw
                  << "\nSID: " << side
                  << "\n UP: " << up
                  << "\nSPD: " << game->camera_.GetSpeed() << std::endl;
        
    }

    // View control
    float rot_factor(2 * glm::pi<float>() / 180); // amount the ship turns per keypress (DOUBLE)
    float trans_factor = 1.0; // amount the ship steps forward per keypress
    // Look up/down
    if (key == GLFW_KEY_UP){
        game->camera_.Pitch(rot_factor);
    }
    if (key == GLFW_KEY_DOWN){
        game->camera_.Pitch(-rot_factor);
    }
    // Turn left/right
    if (key == GLFW_KEY_A){
        game->camera_.Yaw(rot_factor);
    }
    if (key == GLFW_KEY_D){
        game->camera_.Yaw(-rot_factor);
    }
    // Roll anticlockwise/clockwise
    if (key == GLFW_KEY_LEFT){
        game->camera_.Roll(-rot_factor);
    }
    if (key == GLFW_KEY_RIGHT){
        game->camera_.Roll(rot_factor);
    }
    // Accelerate and break
    if (key == GLFW_KEY_W){
        //game->camera_.Translate(game->camera_.GetForward()*trans_factor);
        float new_speed = game->camera_.GetSpeed() + 0.005f;
        
        if (new_speed < game->camera_.GetMaxSpeed()) {
            game->camera_.SetSpeed(new_speed);
        }
        else {
            game->camera_.SetSpeed(game->camera_.GetMaxSpeed());
        }
    }
    if (key == GLFW_KEY_S){
        //game->camera_.Translate(-game->camera_.GetForward()*trans_factor);
        float new_speed = game->camera_.GetSpeed() - 0.05f;
        if (new_speed > 0.0f) {
            game->camera_.SetSpeed(new_speed);
        }
        else {
            game->camera_.SetSpeed(0.0f);
        }
    }
    // Strafe left/right
    if (key == GLFW_KEY_J){
        game->camera_.Translate(-game->camera_.GetSide()*trans_factor);
    }
    if (key == GLFW_KEY_L){
        game->camera_.Translate(game->camera_.GetSide()*trans_factor);
    }
    // Travel up/down
    if (key == GLFW_KEY_I){
        game->camera_.Translate(game->camera_.GetUp()*trans_factor);
    }
    if (key == GLFW_KEY_K){
        game->camera_.Translate(-game->camera_.GetUp()*trans_factor);
    }
}


void Game::ResizeCallback(GLFWwindow* window, int width, int height){

    // Set up viewport and camera projection based on new window size
    glViewport(0, 0, width, height);
    void* ptr = glfwGetWindowUserPointer(window);
    Game *game = (Game *) ptr;
    game->camera_.SetProjection(camera_fov_g, camera_near_clip_distance_g, camera_far_clip_distance_g, width, height);
}


Game::~Game(){
    
    glfwTerminate();
}

SceneNode* Game::CreateSphereInstance(std::string entity_name, std::string object_name, std::string material_name) {
    // Get resources
    Resource* geom = resman_.GetResource(object_name);
    if (!geom) {
        throw(GameException(std::string("Could not find resource \"") + object_name + std::string("\"")));
    }
    Resource* mat = resman_.GetResource(material_name);
    if (!mat) {
        throw(GameException(std::string("Could not find resource \"") + material_name + std::string("\"")));
    }
    // Create stem instance
    SceneNode* sphere = new SceneNode(entity_name, geom, mat,1); //collision on for color change
    return sphere;
}

// Create an object
SceneNode* Game::CreateSceneNodeInstance(std::string entity_name, std::string object_name, std::string material_name) {
    // Get resources
    Resource* geom = resman_.GetResource(object_name);
    if (!geom) {
        throw(GameException(std::string("Could not find resource \"") + object_name + std::string("\"")));
    }
    Resource* mat = resman_.GetResource(material_name);
    if (!mat) {
        throw(GameException(std::string("Could not find resource \"") + material_name + std::string("\"")));
    }
    // Create stem instance
    SceneNode* node = new SceneNode(entity_name, geom, mat, 0);
    return node;
}

} // namespace game
