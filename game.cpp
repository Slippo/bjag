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
    CreateKelp();
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
                
                CompositeNode* a_kelp = scene_.GetNode("Kelp");
                SceneNode* root = a_kelp->GetNode("Root");
                root->Orbit(glm::angleAxis(mytheta, glm::vec3(0.1 * sin(current_time), 0, 0.05 * sin(1 - current_time))));
                for (int x = 0; x < root->GetChildCount(); x++) {
                    root->GetChild(x)->Orbit(glm::angleAxis(mytheta, glm::vec3(0.1 * sin(current_time), 0, 0.05 * sin(1-current_time))));
                }
                
                last_time = current_time;
            }
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
    float trans_factor = 0.2; // amount the ship steps forward per keypress
    // Look up/down
    if (key == GLFW_KEY_UP){
        game->camera_.Pitch(rot_factor);
    }
    if (key == GLFW_KEY_DOWN){
        game->camera_.Pitch(-rot_factor);
    }
    // Turn left/right
    if (key == GLFW_KEY_LEFT) {
        game->camera_.Yaw(rot_factor);
    }
    if (key == GLFW_KEY_RIGHT) {
        game->camera_.Yaw(-rot_factor);
    }
    
    //forward backward side movement (strafe)
    if (key == GLFW_KEY_A){
      
        game->camera_.Translate(-glm::vec3(game->camera_.GetSide().x, 0.0, game->camera_.GetSide().z) * trans_factor);
    }
    if (key == GLFW_KEY_D){
        
        game->camera_.Translate(glm::vec3(game->camera_.GetSide().x, 0.0, game->camera_.GetSide().z) * trans_factor);
    }
   
    // Accelerate and break
    if (key == GLFW_KEY_W){
      
        /* //old architecture from acceleration based model
        float new_speed = game->camera_.GetSpeed() + 0.005f;
        
        if (new_speed < game->camera_.GetMaxSpeed()) {
            game->camera_.SetSpeed(new_speed);
        }
        else {
            game->camera_.SetSpeed(game->camera_.GetMaxSpeed());
        }*/

        game->camera_.Translate(glm::vec3(game->camera_.GetForward().x, 0.0, game->camera_.GetForward().z) * trans_factor);
    }
    if (key == GLFW_KEY_S){
     
        /* //old architecture from acceleration based model
        float new_speed = game->camera_.GetSpeed() - 0.05f;
        if (new_speed > game->camera_.GetMinSpeed()) {
            game->camera_.SetSpeed(new_speed);
        }
        else {
            game->camera_.SetSpeed(0.0f);
        }*/

        game->camera_.Translate(-glm::vec3(game->camera_.GetForward().x, 0.0, game->camera_.GetForward().z) * trans_factor);
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

// Create a floral shrub with many nodes
// branch_complexity changes the number of overall branches
void Game::CreateKelp(int branch_complexity) {
    CompositeNode* kelp = new CompositeNode("Kelp");
    
    // Create root node
    SceneNode* root = CreateSceneNodeInstance("Root", "Cylinder", "KelpMaterial");
    root->SetPosition(glm::vec3(0,1,0));
    root->SetPivot(glm::vec3(0, -1, 0));
    root->SetType(SceneNode::Type::Stem);
    kelp->AddNode(root);

    // Create branches and sub-branches   
    float theta = 2 * glm::pi<float>();
    float offset = 0.2f;
    float stem_len = 2.0f;
    // CREATE BRANCHES CONNECTED TO THE ROOT NODE
    for (int i = 0; i < branch_complexity+1; i++) {
        float frac = float(i) / (branch_complexity+1);
        SceneNode* branch = CreateSceneNodeInstance("Branch", "Cylinder", "KelpMaterial");
        branch->SetType(SceneNode::Type::Stem); // type specified for shader
        branch->SetPosition(glm::vec3(0,2,0)); // 2 units above the root
        branch->SetPivot(glm::vec3(0, -stem_len/2, 0));
        // Orbit about the pivot to set the starting orientation
        branch->Orbit(glm::angleAxis(theta / 12, glm::vec3(cos(theta * frac), 0, sin(theta * frac))));
        root->AddChild(branch);
        kelp->AddNode(branch);
        
        // CREATE LEAVES FOR THE CURRENT BRANCH
        // THE BRANCH HAS LOWER LEAVES AND HIGH LEAVES
        for (int j = 0; j < branch_complexity; j++) {
            float frac = float(j) / (branch_complexity);
            SceneNode* high_leaf = CreateSceneNodeInstance("Leaf", "Sphere", "KelpMaterial");
            high_leaf->SetType(SceneNode::Type::Leaf);
            high_leaf->SetScale(0.25f * glm::vec3(1.0f + abs(cos(theta * frac)), 0.2f, 1.0f + abs(sin(theta * frac))));
            high_leaf->Scale(glm::vec3(0.75f));
            high_leaf->SetPosition(glm::vec3(offset * cos(theta * frac), 1, offset * sin(theta * frac)));
            high_leaf->SetPivot(glm::vec3(0, -1, 0));
            SceneNode* low_leaf = CreateSceneNodeInstance("SubSubBranch", "Sphere", "KelpMaterial");
            low_leaf->SetType(SceneNode::Type::Leaf);
            low_leaf->SetScale(0.25f * glm::vec3(1.0f + abs(cos(theta * frac)), 0.2f, 1.0f + abs(sin(theta * frac))));
            low_leaf->SetPosition(glm::vec3(offset * cos(theta * frac), 0, offset * sin(theta * frac)));
            low_leaf->SetPivot(glm::vec3(0, -stem_len / 2, 0));
            low_leaf->Scale(glm::vec3(0.5f));
            
            branch->AddChild(low_leaf);
            kelp->AddNode(low_leaf);
            branch->AddChild(high_leaf);
            kelp->AddNode(high_leaf);
        }
        // CREATE SUB BRANCHES FOR CURRENT BRANCH
        for (int k = 0; k < branch_complexity; k++) {
            float frac = float(k) / branch_complexity;
            SceneNode* sub_branch = CreateSceneNodeInstance("SubBranch", "Cylinder", "KelpMaterial");
            sub_branch->SetType(SceneNode::Type::Stem);
            sub_branch->SetPosition(glm::vec3(0, stem_len, 0));
            sub_branch->SetPivot(glm::vec3(0, -stem_len / 2, 0));
            sub_branch->Orbit(glm::angleAxis(-theta / 16.0f, glm::vec3(cos(theta * frac), 0, sin(theta * frac))));
            branch->AddChild(sub_branch);
            kelp->AddNode(sub_branch);

            // CREATE LEAVES FOR CURRENT SUB BRANCH
            for (int j = 0; j < branch_complexity; j++) {
                float theta = 2.0f * glm::pi<float>();
                float frac = float(j) / (branch_complexity);
                SceneNode* leaf = CreateSceneNodeInstance("Leaf", "Sphere", "KelpMaterial");
                leaf->SetType(SceneNode::Type::Leaf);
                leaf->SetScale(0.25f * glm::vec3(1.0f + abs(cos(theta* frac)), 0.2f, 1.0f + abs(sin(theta* frac))));
                leaf->SetPosition(glm::vec3(offset * cos(theta* frac), 1, offset * sin(theta* frac)));
                leaf->SetPivot(glm::vec3(0, -stem_len / 2, 0));
                sub_branch->AddChild(leaf);
                kelp->AddNode(leaf);
            }
        }
    }
    scene_.AddNode(kelp);
}
} // namespace game
