#include <iostream>
#include <time.h>
#include <sstream>

#include "game.h"
#include "path_config.h"

namespace game {

    // Some configuration constants
    // They are written here as global variables, but ideally they should be loaded from a configuration file

    // Main window settings
    const std::string window_title_g = "Bjag";
    const unsigned int window_width_g = 1280;
    const unsigned int window_height_g = 720;
    const bool window_full_screen_g = false;

    // Viewport and camera settings
    float camera_near_clip_distance_g = 0.01;
    float camera_far_clip_distance_g = 1000.0;
    float camera_fov_g = 60.0; // Field-of-view of camera (degrees)
    const glm::vec3 viewport_background_color_g(0.5, 0.5, 1.0);
    glm::vec3 camera_position_g(0.0, 5.0, 8.0);
    glm::vec3 camera_look_at_g(0.0, 2.5, 0.0);
    glm::vec3 camera_up_g(0.0, 1.0, 0.0);

    // Materials 
    const std::string material_directory_g = MATERIAL_DIRECTORY;

    // Manipulator
    Manipulator* manipulator = new Manipulator();

    Game::Game(void) {

        // Don't do work in the constructor, leave it for the Init() function
    }


    void Game::Init(void) {

        // Run all initialization steps
        InitWindow();
        InitView();
        InitEventHandlers();

        // Set variables
        animating_ = true;
    }


    void Game::InitWindow(void) {

        // Initialize the window management library (GLFW)
        if (!glfwInit()) {
            throw(GameException(std::string("Could not initialize the GLFW library")));
        }

        // Create a window and its OpenGL context
        if (window_full_screen_g) {
            window_ = glfwCreateWindow(window_width_g, window_height_g, window_title_g.c_str(), glfwGetPrimaryMonitor(), NULL);
        }
        else {
            window_ = glfwCreateWindow(window_width_g, window_height_g, window_title_g.c_str(), NULL, NULL);
        }
        if (!window_) {
            glfwTerminate();
            throw(GameException(std::string("Could not create window")));
        }

        // Make the window's context the current 
        glfwMakeContextCurrent(window_);

        // Initialize the GLEW library to access OpenGL extensions
        // Need to do it after initializing an OpenGL context
        glewExperimental = GL_TRUE;
        GLenum err = glewInit();
        if (err != GLEW_OK) {
            throw(GameException(std::string("Could not initialize the GLEW library: ") + std::string((const char*)glewGetErrorString(err))));
        }
    }


    void Game::InitView(void) {
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
        // Hide mouse
        glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    void Game::InitEventHandlers(void) {

        // Set event callbacks
        glfwSetKeyCallback(window_, KeyCallback);
        glfwSetFramebufferSizeCallback(window_, ResizeCallback);
        glfwSetCursorPosCallback(window_, CursorPosCallback);

        // Set pointer to game object, so that callbacks can access it
        glfwSetWindowUserPointer(window_, (void*)this);
    }


    void Game::SetupResources(void) {

        // POPULATE HEIGHT MAP ARRAY
        std::ifstream height_file;
        try {
            height_file.open(material_directory_g + "\\height_map.pgm");
            if (!height_file.is_open()) {
                throw std::ios_base::failure("Error opening height_map.pgm");
            }
        }
        catch (const std::ios_base::failure& e) {
            std::cout << e.what();
            std::exit(1);
        }

        // CHECK FORMATTING; ONLY ACCEPT PGM FILES
        // PGM FILES START WITH P2 AND ARE FOLLOWED BY THEIR WIDTH x HEIGHT DIMENSIONS
        // THE REST OF THE FILE CONTAINS THE VERTEX DATA
        std::string magic_number;
        std::string comment;
        int width, height, max_value;
        height_file >> magic_number >> width >> height >> max_value;
        if (magic_number != "P2" || width != plane_size_.x || height != plane_size_.y || max_value != 255) {
            throw std::invalid_argument("Invalid PGM file format or dimensions");
        }

        height_map_.reserve(width * height);
        height_map_boundary_.reserve(width * height);

        int offsetX = plane_size_.x / 2;
        int offsetZ = plane_size_.y / 2;
        srand(3535);
        // Generate random starting values
        
        for (int z = 0; z < height_map_.capacity() && z < height_map_boundary_.capacity(); z++) {
            height_map_.insert(height_map_.end(), rand() / (float)(RAND_MAX / 0.5)); // Random height between 0 to 2.0
            height_map_boundary_.insert(height_map_boundary_.end() , -0.1f - (float)(rand() / (RAND_MAX))); // -0.1 to -1.1
        }
    height_map_.resize(height_map_.capacity());
    height_map_boundary_.resize(height_map_boundary_.capacity());
    // Set heights
    
    float h = 0;
    for (int z = 0; z < height_map_boundary_.size() / width; ++z) {
        for (int x = 0; x < height_map_boundary_.size() / height; ++x) {
            height_file >> h;
            height_map_boundary_[x + width*z] += h/8; // max height is 16 (255/16) ~= 15.9
        }
    }
    height_file.close();
  
    // SHAPES
    // Basic
    resman_.CreateCylinder("Cylinder", 2, 0.15);
    resman_.CreateSphere("Sphere", 1.0f, 90, 45);
    // Stalagmite
    resman_.CreateCylinder("StalagmiteBase", 3.0, 3.0, 30, 15);
    resman_.CreateCone("StalagmiteSpike", 1.0, 0.6, 30, 15);
    resman_.CreateSphere("SubmarineBase", 10.0, 90, 45);
    // Coral
    resman_.CreateCylinder("FatStem", 2.0, 0.6, 30, 30);
    resman_.CreateCylinder("LongStem", 5.0, 0.6, 30, 30);
    resman_.CreateCylinder("SuperLongStem", 10.0, 0.6, 30, 30);
    resman_.CreateCylinder("Branch", 3.0, 0.6, 30, 30);
    resman_.CreateSphere("Tip", 0.6, 90, 45);
    //mechanical part
    resman_.CreateCylinder("MainBody", 10, 2);
    resman_.CreateCylinder("Exhaust", 2.5, 0.5);
    //sea anemonie
    resman_.CreateCylinder("Base", 0.5, 1);
    resman_.CreateSphere("Middle", 1.0, 30);
    resman_.CreateCylinder("Tentacle", 0.5, 0.1);
    // Seaweed
    resman_.CreateCylinder("LowPolyCylinder", 1.0, 0.6, 10, 9);
    // Plane
    resman_.CreatePlane("Plane", height_map_, height_map_.size() / width, height_map_.size()/height, offsetX, offsetZ);
    resman_.CreatePlane("Boundary", height_map_boundary_, height_map_boundary_.capacity() / width, height_map_boundary_.capacity() / height, offsetX, offsetZ);
    std::string filename = std::string(MATERIAL_DIRECTORY) + std::string("/normal_map");
    resman_.LoadResource(Material, "NormalMapMaterial", filename.c_str());

    // TEXTURES
    filename = std::string(MATERIAL_DIRECTORY) + std::string("/kelp_material");
    resman_.LoadResource(Material, "KelpMaterial", filename.c_str());

    filename = std::string(MATERIAL_DIRECTORY) + std::string("/material");
    resman_.LoadResource(Material, "ObjectMaterial", filename.c_str());

    // Load texture to be used in normal mapping
    filename = std::string(MATERIAL_DIRECTORY) + std::string("/nm_sand.png");
    resman_.LoadResource(Texture, "NormalMapSand", filename.c_str());

    filename = std::string(MATERIAL_DIRECTORY) + std::string("/nm_stone.png");
    resman_.LoadResource(Texture, "NormalMapStone", filename.c_str());

    filename = std::string(MATERIAL_DIRECTORY) + std::string("/nm_grass2.png");
    resman_.LoadResource(Texture, "NormalMapGrass", filename.c_str());

    filename = std::string(MATERIAL_DIRECTORY) + std::string("/nm_scales.png");
    resman_.LoadResource(Texture, "NormalMapScales", filename.c_str());

    filename = std::string(MATERIAL_DIRECTORY) + std::string("/nm_glass.png");
    resman_.LoadResource(Texture, "NormalMapGlass", filename.c_str());

    filename = std::string(MATERIAL_DIRECTORY) + std::string("/nm_metal.png");
    resman_.LoadResource(Texture, "NormalMapMetal", filename.c_str());
  
    resman_.CreateCone("MachinePart", 2.0, 1.0, 10, 10);
    filename = std::string(MATERIAL_DIRECTORY) + std::string("/material");
    resman_.LoadResource(Material, "ObjectMaterial", filename.c_str());
}

void Game::SetupScene(void){    

    camera_.SetTimer(480); // Starting player time limit / oxygen

    
    scene_.SetBackgroundColor(viewport_background_color_g);
    
    // Floor of the game (sand)
    scene_.AddNode(manipulator->ConstructPlane(&resman_)); // name is "Plane"
    //scene_.GetNode("Plane")

    // Boundary "walls" (stone)
    scene_.AddNode(manipulator->ConstructBoundary(&resman_));

    // Light source ("sun")
    scene_.AddNode(manipulator->ConstructSun(&resman_, glm::vec3(0,100,0)));
  
    //scene_.AddNode(manipulator->ConstructStalagmite(&resman_, "Stalagmite1", glm::vec3(10, 0, -10)));
    //scene_.GetNode("Stalagmite1")->Rotate(glm::angleAxis(glm::pi<float>(), glm::vec3(0, 0, 1)));
    
    scene_.AddNode(manipulator->ConstructSubmarine(&resman_, "Submarine", glm::vec3(-17, 7.5, -33)));
    //scene_.GetNode("Submarine")->Rotate(glm::angleAxis(glm::pi<float>(), glm::vec3(1, 1, 1)));

    //scene_.AddNode(manipulator->ConstructPart(&resman_, "Mechanical_Part", glm::vec3(0, 4, 0)));

    //scene_.AddNode(manipulator->ConstructAnemonie(&resman_, "Anemonie", glm::vec3(0, 2, 0)));


    //scene_.AddNode(manipulator->ConstructCoral(&resman_, "Coral1", glm::vec3(-8.0, 5.0, -20.0)));
    //scene_.GetNode("Coral1")->Scale(glm::vec3(2,5, 2));

    //scene_.AddNode(manipulator->ConstructSeaweed(&resman_, "Seaweed1", 4, glm::vec3(0, 0, -5)));

    //scene_.AddNode(manipulator->ConstructKelp(&resman_, "Kelp1", 4, glm::vec3(0.0, 0.0, -5.0))); // Example on how to make object
    //scene_.GetNode("Kelp1")->Scale(glm::vec3(1,2,1)); // Example on how to transform object after creation

    // Seaweed instancer call, can generate random seaweed using given dimensions / density
    manipulator->ConstructSeaweedPatch(&resman_, &scene_, 10, 50, 50, glm::vec3(0, 0, -5));
}

void Game::MainLoop(void){
    // Loop while the user did not close the window
    while (!glfwWindowShouldClose(window_)){

        SceneNode* world_light = scene_.GetNode("Sphere")->GetRoot();

        // Animate the scene
        if (animating_){
            static double last_time = 0;
            double current_time = glfwGetTime();
            float mytheta = glm::pi<float>() / 64;
            if ((current_time - last_time) > 0.05){
                camera_.DecreaseTimer(current_time - last_time); // Decrease remaining player time limit / oxygen
                scene_.Update(&camera_, &resman_);
                manipulator->AnimateAll(&scene_, current_time, mytheta);
                last_time = current_time;
                camera_.Update();

                //std::cout << camera_.GetPosition().x << ", " << camera_.GetPosition().y << ", " << camera_.GetPosition().z << std::endl;

            }
        }

        // Process camera/player forward movement
        //camera_.Translate(camera_.GetForward() * camera_.GetSpeed());
        //if (camera_.GetSpeed() > 0) {
        //    camera_.SetSpeed(camera_.GetSpeed() * 0.98);

        //}
        // Draw the scene
        scene_.Draw(&camera_, world_light);

        // HUD
        UpdateHUD();

        // Push buffer drawn in the background onto the display
        glfwSwapBuffers(window_);

        // Update other events like input handling
        glfwPollEvents();

        // Win condition
        if (camera_.CheckWinCondition() == true) {
            glfwSetWindowShouldClose(window_, true);
        }
    }
}

void Game::CursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    // Get user data with a pointer to the game class
    void* ptr = glfwGetWindowUserPointer(window);
    Game* game = (Game*)ptr;
    int width = 0;
    int height = 0;
    glfwGetWindowSize(window, &width, &height);
    
    glm::vec2 dir = glm::vec2(xpos,ypos) - glm::vec2(width / 2, height / 2); // current direction of the cursor
    dir = glm::normalize(dir);
    float sens = 0.025;

    game->camera_.Yaw(sens * -dir.x);
    game->camera_.Pitch(sens * -dir.y);
    
    //game->camera_.Roll(0);

    //std::cout << glm::to_string(dir) << std::endl;

    glfwSetCursorPos(window, width / 2, height / 2); // center the cursor
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
    if (key == GLFW_KEY_E && action == GLFW_PRESS){
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
    float trans_factor = 0.7f; // amount the ship steps forward per keypress
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
      
        //game->camera_.UpdateVelocity(1);
        //game->camera_.Translate(-glm::vec3(game->camera_.GetSide().x, 0.0, game->camera_.GetSide().z) * trans_factor);
    }
    if (key == GLFW_KEY_D)
    {
        //game->camera_.UpdateVelocity(-1);
        //game->camera_.Translate(glm::vec3(game->camera_.GetSide().x, 0.0, game->camera_.GetSide().z) * trans_factor);
    }

    if (key == GLFW_KEY_SPACE)
    {
        //game->camera_.SetState(0);
        game->camera_.Jump();
        //game->camera_.Translate(glm::vec3(0.0, game->camera_.GetUp(), 0.0) * trans_factor);
    }
   
    if (key == GLFW_KEY_W && action == GLFW_RELEASE) {
        game->camera_.SetVelocity(0.0);
    }
    // Accelerate and break
    if (key == GLFW_KEY_W){
        
   
        game->camera_.UpdateVelocity(1);
        
    }
    
    if (key == GLFW_KEY_S && action == GLFW_RELEASE) {
        game->camera_.SetVelocity(0.0);
        
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
 
        game->camera_.UpdateVelocity(-1);
        //game->camera_.Translate(-glm::vec3(game->camera_.GetForward().x, 0.0, game->camera_.GetForward().z) * trans_factor);
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

void Game::UpdateHUD() {
    // Oxygen timer
    DisplayText(glm::vec2(-0.95, 0.9), glm::vec3(0, 0, 0), GLUT_BITMAP_TIMES_ROMAN_24, ("OXYGEN REMAINING: " + (std::to_string((int)camera_.GetTimer()))).c_str());
    // Mechanical part counter
    DisplayText(glm::vec2(-0.95, 0.8), glm::vec3(1.0, 0.0, 0.0), GLUT_BITMAP_TIMES_ROMAN_24, ("PARTS COLLECTED: " + (std::to_string((int)camera_.GetTimer()))).c_str());
}

void Game::DisplayText(glm::vec2 position, glm::vec3 colour, void* font, const char* text) {
    // *NOTE: For some reason, the text is being dynamically lit... enabling GL_FOG at least makes the text black, for now.
    glEnable(GL_FOG);
    glRasterPos2f(position.x, position.y); // Set text position
    glColor3fv(glm::value_ptr(colour));
    //glColor3f(1, 1, 1);
    for (int i = 0; i < (int)strlen(text); i++) { // "Print" each character of text to window
        glutBitmapCharacter(font, text[i]);
    }
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
    SceneNode* sphere = new SceneNode(entity_name, geom, mat, NULL, 1); //collision on for color change
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
    SceneNode* node = new SceneNode(entity_name, geom, mat, NULL, 0);
    return node;
}
} // namespace game
