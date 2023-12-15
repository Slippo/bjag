#include <iostream>
#include <time.h>
#include <sstream>
#include "game.h"
#include "path_config.h"

namespace game {
    // Configuration constants
    // Main window settings
    const std::string window_title_g = "Bjag";
    const unsigned int window_width_g = 1280;
    const unsigned int window_height_g = 720;
    const bool window_full_screen_g = false;
    // Viewport and camera settings
    float camera_near_clip_distance_g = 0.05;
    float camera_far_clip_distance_g = 1000.0;
    float camera_fov_g = 60.0; // degrees
    const glm::vec3 viewport_background_color_g(0.5, 0.5, 1.0);
    glm::vec3 camera_position_g(0.0, 5.0, 8.0);
    glm::vec3 camera_look_at_g(0.0, 2.5, 0.0);
    glm::vec3 camera_up_g(0.0, 1.0, 0.0);

    // Materials 
    const std::string material_directory_g = MATERIAL_DIRECTORY;

    Manipulator* manipulator = new Manipulator();

    Game::Game(void) {}


    void Game::Init(void) {

        // Run all initialization steps
        InitWindow();
        InitView();
        InitEventHandlers();

        // Set variables
        animating_ = true;
        moving_ = false;
        state_ = start;
        std::cout << "initialized!" << std::endl;
        SoundEngine = irrklang::createIrrKlangDevice();
        SoundEngine->play2D((MATERIAL_DIRECTORY + std::string("\\audio\\breakout.mp3")).c_str(), true);

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

        // Initialize ImGui to window
        ImGui::CreateContext();
        imgui_io_ = ImGui::GetIO();
        imgui_io_.Fonts->AddFontFromFileTTF((MATERIAL_DIRECTORY + std::string("\\PixelBug.otf")).c_str(), 30); // Load custom font from file
        ImGui::StyleColorsDark(); // Window style
        ImGui_ImplGlfw_InitForOpenGL(window_, true);
        ImGui_ImplOpenGL3_Init("#version 130");
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
        camera_.SetForwardSpeed(0.0f);
        camera_.SetSideSpeed(0.0f);
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

        std::ifstream collision_file;
        try {
            collision_file.open(material_directory_g + "\\collision_map.pgm");
            if (!collision_file.is_open())
            {
                throw std::ios_base::failure("Error opening collision_map.pgm");
            }
        }
        catch (const std::ios_base::failure& e) {
            std::cout << e.what();
            std::exit(1);
        }
        

        // Setup drawing to texture
        scene_.SetupDrawToTexture();

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

        collision_file >> magic_number >> width >> height >> max_value;

        if (magic_number != "P2" || width != plane_size_.x || height != plane_size_.y || max_value != 255) {
            throw std::invalid_argument("Invalid PGM file format or dimensions");
        }

        height_map_.reserve(width * height);
        height_map_boundary_.reserve(width * height);
        height_map_collision_.reserve(width * height);

        int offsetX = plane_size_.x / 2;
        int offsetZ = plane_size_.y / 2;
        srand(3535);
        // Generate random starting values
        
        for (int z = 0; z < height_map_.capacity() && z < height_map_boundary_.capacity(); z++) {
            height_map_.insert(height_map_.end(), rand() / (float)(RAND_MAX / 0.5)); // Random height between 0 to 2.0
            height_map_boundary_.insert(height_map_boundary_.end() , -0.1f - (float)(rand() / (RAND_MAX))); // -0.1 to -1.1
            height_map_collision_.insert(height_map_collision_.end(), -0.1f - (float)(rand() / (RAND_MAX)));
        }
    height_map_.resize(height_map_.capacity());
    height_map_boundary_.resize(height_map_boundary_.capacity());
    height_map_collision_.resize(height_map_collision_.capacity());
    // Set heights
    
    float h = 0;
    for (int z = 0; z < height_map_boundary_.size() / width; ++z) {
        for (int x = 0; x < height_map_boundary_.size() / height; ++x) {
            height_file >> h;
            height_map_boundary_[x + width*z] += h/8; // max height is 16 (255/16) ~= 15.9
        }
    }
    height_file.close();

    for (int z = 0; z < height_map_collision_.size() / width; ++z)
    {
        for (int x = 0; x < height_map_boundary_.size() / height; ++x) {
            collision_file >> h;
            height_map_collision_[x + width * z] += h / 8;
        }
    }
     
    camera_.SetDimensions(offsetX, offsetZ, width, height);
    camera_.SetHeightMap(height_map_, height_map_collision_);
    
  
    // SHAPES
    // Basic
    resman_.CreateCylinder("Cylinder", 2, 0.15);
    resman_.CreateSphere("Sphere", 1.0f, 90, 45);
    // Stalagmite
    resman_.CreateCylinder("StalagmiteBase", 3.0, 3.0, 10, 9);
    resman_.CreateCone("StalagmiteSpike", 1.0, 0.6, 10, 9);
    resman_.CreateSphere("SubmarineBase", 10.0, 90, 45);
    // Coral
    resman_.CreateCylinder("FatStem", 2.0, 0.6, 30, 30);
    resman_.CreateCylinder("LongStem", 5.0, 0.6, 30, 30);
    resman_.CreateCylinder("SuperLongStem", 10.0, 0.6, 30, 30);
    resman_.CreateCylinder("Branch", 3.0, 0.6, 30, 30);
    resman_.CreateSphere("Tip", 0.6, 90, 45);
    //mechanical part
    resman_.CreateCylinder("MainBody", 10, 2, 20, 20);
    resman_.CreateCylinder("Exhaust", 2.5, 0.5, 10, 10);
    //sea anemonie
    resman_.CreateCylinder("Base", 0.5, 1);
    resman_.CreateSphere("Middle", 1.0, 30);
    resman_.CreateCylinder("Tentacle", 0.5, 0.1);
    // Rock
    resman_.CreateSphere("Rock_Sphere", 2, 40, 20);
    // Seaweed
    resman_.CreateCylinder("LowPolyCylinder", 1.0, 0.6, 10, 9);
    // Skybox
    resman_.CreateInvertedSphere("SkyBox", 700, 300, 150);
    // Plane
    resman_.CreatePlane("Plane", height_map_, height_map_.size() / width, height_map_.size() / height, offsetX, offsetZ);
    resman_.CreatePlane("Boundary", height_map_boundary_, height_map_boundary_.capacity() / width, height_map_boundary_.capacity() / height, offsetX, offsetZ);

    std::string filename = std::string(MATERIAL_DIRECTORY) + std::string("/normal_map");
    resman_.LoadResource(Material, "NormalMapMaterial", filename.c_str());

    // SCREENSPACE
    filename = std::string(MATERIAL_DIRECTORY) + std::string("/screen_space");
    resman_.LoadResource(Material, "ScreenSpaceMaterial", filename.c_str());

    // TEXTURES
    filename = std::string(MATERIAL_DIRECTORY) + std::string("/kelp_material");
    resman_.LoadResource(Material, "KelpMaterial", filename.c_str());

    filename = std::string(MATERIAL_DIRECTORY) + std::string("/material");
    resman_.LoadResource(Material, "ObjectMaterial", filename.c_str());

    filename = std::string(MATERIAL_DIRECTORY) + std::string("/invisible.png");
    resman_.LoadResource(Texture, "InvisibleTexture", filename.c_str());

    // Load texture to be used in normal mapping
    filename = std::string(MATERIAL_DIRECTORY) + std::string("/nm_sand.png");
    resman_.LoadResource(Texture, "NormalMapSand", filename.c_str());

    filename = std::string(MATERIAL_DIRECTORY) + std::string("/nm_stone.png");
    resman_.LoadResource(Texture, "NormalMapStone", filename.c_str());

    //normal map for rock
    filename = std::string(MATERIAL_DIRECTORY) + std::string("/Pebbles_026_normal.png");
    resman_.LoadResource(Texture, "NormalMapRock", filename.c_str());
    
    // shader for 3-term lighting and texture combined effect
    filename = std::string(MATERIAL_DIRECTORY) + std::string("/combined");
    resman_.LoadResource(Material, "CombinedMaterial", filename.c_str());

    // Load texture to be used on rocks
    filename = std::string(MATERIAL_DIRECTORY) + std::string("/rocky.png");
    resman_.LoadResource(Texture, "RockyTexture", filename.c_str());

    // Load texture to be used on skybox
    filename = std::string(MATERIAL_DIRECTORY) + std::string("/sky-box-2.png");
    resman_.LoadResource(Texture, "SkyBoxTexture", filename.c_str());

    //metal
    //filename = std::string(MATERIAL_DIRECTORY) + std::string("/Metal_Corrugated_0111_.png");
    filename = std::string(MATERIAL_DIRECTORY) + std::string("/Metal_Corrugated_0111_.png");
    resman_.LoadResource(Texture, "MetalTexture", filename.c_str());

    filename = std::string(MATERIAL_DIRECTORY) + std::string("/nm_grass2.png");
    resman_.LoadResource(Texture, "NormalMapGrass", filename.c_str());

    filename = std::string(MATERIAL_DIRECTORY) + std::string("/nm_scales.png");
    resman_.LoadResource(Texture, "NormalMapScales", filename.c_str());

    filename = std::string(MATERIAL_DIRECTORY) + std::string("/nm_glass.png");
    resman_.LoadResource(Texture, "NormalMapGlass", filename.c_str());

    filename = std::string(MATERIAL_DIRECTORY) + std::string("/nm_metal.png");
    resman_.LoadResource(Texture, "NormalMapMetal", filename.c_str());

    filename = std::string(MATERIAL_DIRECTORY) + std::string("/nm_coral.png");
    resman_.LoadResource(Texture, "NormalMapCoral", filename.c_str());

    filename = std::string(MATERIAL_DIRECTORY) + std::string("/coral_tex.png");
    resman_.LoadResource(Texture, "CoralTexture", filename.c_str());

    filename = std::string(MATERIAL_DIRECTORY) + std::string("/anemone_tex_yellow.png");
    resman_.LoadResource(Texture, "YellowAnemoneTexture", filename.c_str());

    filename = std::string(MATERIAL_DIRECTORY) + std::string("/Bubble.png");
    resman_.LoadResource(Texture, "BubbleTexture", filename.c_str());

    filename = std::string(MATERIAL_DIRECTORY) + std::string("/Gear.png");
    resman_.LoadResource(Texture, "GearTexture", filename.c_str());
  
    // PARTICLE FX
    filename = std::string(MATERIAL_DIRECTORY) + std::string("/environment");
    resman_.LoadResource(Material, "ParticleGeyserMaterial", filename.c_str());

    //bubble material
    filename = std::string(MATERIAL_DIRECTORY) + std::string("/particle_vent");
    resman_.LoadResource(Material, "ParticleVentMaterial", filename.c_str());

    //star material
    filename = std::string(MATERIAL_DIRECTORY) + std::string("/star");
    resman_.LoadResource(Material, "ParticleStarMaterial", filename.c_str());

    //player bubbles material
    filename = std::string(MATERIAL_DIRECTORY) + std::string("/particle_bubbles");
    resman_.LoadResource(Material, "ParticleBubbleMaterial", filename.c_str());

    //skybox material
    filename = std::string(MATERIAL_DIRECTORY) + std::string("/skybox");
    resman_.LoadResource(Material, "SkyBoxMaterial", filename.c_str());
    
    // Load house smoke texture
    filename = std::string(MATERIAL_DIRECTORY) + std::string("/smoke.png");
    resman_.LoadResource(Texture, "SmokeTexture", filename.c_str());

    // Load house Star texture
    filename = std::string(MATERIAL_DIRECTORY) + std::string("/stars.png");
    resman_.LoadResource(Texture, "StarTexture", filename.c_str());

    // Create particles
    resman_.CreateSphereParticles("SphereParticles");
    resman_.CreateSphereParticles("SphereParticlesBubbles", 10);


    /*resman_.CreateCone("MachinePart", 2.0, 1.0, 10, 10);
    filename = std::string(MATERIAL_DIRECTORY) + std::string("/material");
    resman_.LoadResource(Material, "ObjectMaterial", filename.c_str());*/
}

void Game::SetupScene(void)
{    
    if (state_ == start)
    {
        SetupStartScreen();
    }
}

void Game::PopulateWorld(void) {
    //scene_.AddNode(manipulator->ConstructStalagmite(&resman_, "Stalagmite1", glm::vec3(10, 0, -10)));
    //scene_.GetNode("Stalagmite1")->Rotate(glm::angleAxis(glm::pi<float>(), glm::vec3(0, 0, 1)));

    scene_.AddNode(manipulator->ConstructSubmarine(&resman_, "Submarine", glm::vec3(-17, 7.5, -33)));
    scene_.GetNode("Submarine")->Rotate(glm::angleAxis(glm::pi<float>(), glm::vec3(1, 1, 1)));

    //CREATE COLLECTIBLE MECHANICAL PARTS
    scene_.AddNode(manipulator->ConstructPart(&resman_, "Mechanical_Part1", glm::vec3(-23.5, 15.9, -73.3)));
    scene_.AddNode(manipulator->ConstructPart(&resman_, "Mechanical_Part2", glm::vec3(-74.2, 5.0, 89.2)));
    scene_.AddNode(manipulator->ConstructPart(&resman_, "Mechanical_Part3", glm::vec3(-74.07, 5.0, -75.85)));
    scene_.AddNode(manipulator->ConstructPart(&resman_, "Mechanical_Part4", glm::vec3(19.44, 17.85, 83.95)));
    scene_.AddNode(manipulator->ConstructPart(&resman_, "Mechanical_Part5", glm::vec3(81.98, 5.0, -26.343)));
  /*
    // Light source ("sun")
    scene_.AddNode(manipulator->ConstructSun(&resman_, glm::vec3(0,100,0)));
  
    //scene_.GetNode("Stalagmite1")->Rotate(glm::angleAxis(glm::pi<float>(), glm::vec3(0, 0, 1)));
    
    scene_.AddNode(manipulator->ConstructSubmarine(&resman_, "Submarine", glm::vec3(-17, 7.5, -33)));
    //scene_.GetNode("Submarine")->Rotate(glm::angleAxis(glm::pi<float>(), glm::vec3(1, 1, 1)));
    //scene_.AddNode(manipulator->ConstructPart(&resman_, "Mechanical_Part1", glm::vec3(0, 3, 0)));
    */
    

   //scene_.AddNode(manipulator->ConstructAnemonie(&resman_, "Anemonie", glm::vec3(0, 2, 0)));

  // scene_.AddNode(manipulator->ConstructAnemonie(&resman_, "Anemonie", glm::vec3(0, 2, 0)));

    //scene_.AddNode(manipulator->ConstructRock(&resman_, "ROCK", glm::vec3(0, 0, 0)));
    //scene_.GetNode("ROCK")->Scale(glm::vec3(0.8, 0.5, 0.5));


    //scene_.AddNode(manipulator->ConstructSeaweed(&resman_, "Seaweed1", 4, glm::vec3(0, 0, -5)));

    //PLACE GEORGE'S PLANT
    scene_.AddNode(manipulator->ConstructKelp(&resman_, "Kelp3", 4, glm::vec3(4.9, 0.0, 14.03)));
    /*
    scene_.AddNode(manipulator->ConstructKelp(&resman_, "Kelp1", 4, glm::vec3(3.84, 0.0, -38.37))); 
    scene_.AddNode(manipulator->ConstructKelp(&resman_, "Kelp2", 4, glm::vec3(-42.6, 0.0, -13.96)));
    
    
    scene_.AddNode(manipulator->ConstructKelp(&resman_, "Kelp4", 4, glm::vec3(74.54, 0.0, -86.53)));
    scene_.AddNode(manipulator->ConstructKelp(&resman_, "Kelp5", 4, glm::vec3(73.77, 0.0, 87.39)));
    scene_.AddNode(manipulator->ConstructKelp(&resman_, "Kelp6", 4, glm::vec3(87.15, 0.0, 32.46)));
    scene_.AddNode(manipulator->ConstructKelp(&resman_, "Kelp7", 4, glm::vec3(-17.5, 0.0, 47.9)));
    scene_.AddNode(manipulator->ConstructKelp(&resman_, "Kelp8", 4, glm::vec3(10.09, 0.0, -62.77)));
    scene_.AddNode(manipulator->ConstructKelp(&resman_, "Kelp9", 4, glm::vec3(-69.82, 0.0, -35.23)));
    */
    //PLACE CORAL
    scene_.AddNode(manipulator->ConstructCoral(&resman_, "Coral1", glm::vec3(3.67, 1.6, 0.343))); //-31.67, 1.6, 0.343
    /*
    scene_.AddNode(manipulator->ConstructCoral(&resman_, "Coral2", glm::vec3(-11.26, 1.6, 9.43)));
    scene_.AddNode(manipulator->ConstructCoral(&resman_, "Coral3", glm::vec3(62.44, 1.6, 62.33)));
    scene_.AddNode(manipulator->ConstructCoral(&resman_, "Coral4", glm::vec3(49.37, 1.6, 18.47)));
    scene_.AddNode(manipulator->ConstructCoral(&resman_, "Coral5", glm::vec3(73.73, 1.6, -57.04)));
    scene_.AddNode(manipulator->ConstructCoral(&resman_, "Coral6", glm::vec3(-20.59, 1.6, 87.86)));
    scene_.AddNode(manipulator->ConstructCoral(&resman_, "Coral7", glm::vec3(-81.3, 1.6, -6.87)));
    */
    //PLACE ANEMONIES
    
    scene_.AddNode(manipulator->ConstructAnemonie(&resman_, "Anemonie1", glm::vec3(1.49, 0, 3.57))); // 14.49, 0, -33.57
    scene_.AddNode(manipulator->ConstructSeaweed(&resman_, "Seaweed1", 4, glm::vec3(-3,0,6)));
    /*
    scene_.AddNode(manipulator->ConstructAnemonie(&resman_, "Anemonie2", glm::vec3(-39.42, 0, 24.37)));
    scene_.AddNode(manipulator->ConstructAnemonie(&resman_, "Anemonie3", glm::vec3(6.47, 0, 36.26)));
    scene_.AddNode(manipulator->ConstructAnemonie(&resman_, "Anemonie4", glm::vec3(-40.20, 0, 74.84)));
    scene_.AddNode(manipulator->ConstructAnemonie(&resman_, "Anemonie5", glm::vec3(-73.08, 0, 52.47)));
    scene_.AddNode(manipulator->ConstructAnemonie(&resman_, "Anemonie6", glm::vec3(-61.06, 0, 4.78)));
    scene_.AddNode(manipulator->ConstructAnemonie(&resman_, "Anemonie7", glm::vec3(19.41, 0, -86.87)));
    scene_.AddNode(manipulator->ConstructAnemonie(&resman_, "Anemonie8", glm::vec3(88.29, 0, 74.85)));
    scene_.AddNode(manipulator->ConstructAnemonie(&resman_, "Anemonie9", glm::vec3(86.76, 0, 70.89)));
    scene_.AddNode(manipulator->ConstructAnemonie(&resman_, "Anemonie10", glm::vec3(53.37, 0, 85.80)));
    scene_.AddNode(manipulator->ConstructAnemonie(&resman_, "Anemonie11", glm::vec3(20.32, 0, -32.43)));
    scene_.AddNode(manipulator->ConstructAnemonie(&resman_, "Anemonie12", glm::vec3(9.75, 0, -34.95)));
    scene_.AddNode(manipulator->ConstructAnemonie(&resman_, "Anemonie13", glm::vec3(-36.17, 1, -24.05)));
    */
    //PLACE SEAWEED PATCHES
    // Seaweed instancer call, can generate random seaweed using given dimensions / density
    //manipulator->ConstructSeaweedPatch(&resman_, &scene_, 10, 20, 20, glm::vec3(70.58, 0, -5.64));
    //manipulator->ConstructSeaweedPatch(&resman_, &scene_, 10, 20, 20, glm::vec3(-47.98, 0, 17.74));
    //manipulator->ConstructSeaweedPatch(&resman_, &scene_, 10, 20, 20, glm::vec3(40.87, 0, -55.81));
    
    // Create particles
    //scene_.AddNode(manipulator->ConstructParticleSystem(&resman_, "SphereParticles", "ParticleInstance1", "ParticleVentMaterial", "BubbleTexture", glm::vec3(0, 0, 0)));

    /*
    //PARTICLE SYSTEM FOR MECHANICAL PARTS
    scene_.AddNode(manipulator->ConstructParticleSystem(&resman_, "SphereParticles", "ParticleStarInstance1", "ParticleStarMaterial", "StarTexture", glm::vec3(-23.5, 15.9, -73.3)));
    scene_.AddNode(manipulator->ConstructParticleSystem(&resman_, "SphereParticles", "ParticleStarInstance2", "ParticleStarMaterial", "StarTexture", glm::vec3(-74.2, 5.0, 89.2)));
    scene_.AddNode(manipulator->ConstructParticleSystem(&resman_, "SphereParticles", "ParticleStarInstance3", "ParticleStarMaterial", "StarTexture", glm::vec3(-74.07, 5.0, -75.85)));
    scene_.AddNode(manipulator->ConstructParticleSystem(&resman_, "SphereParticles", "ParticleStarInstance4", "ParticleStarMaterial", "StarTexture", glm::vec3(19.44, 17.85, 83.95)));
    scene_.AddNode(manipulator->ConstructParticleSystem(&resman_, "SphereParticles", "ParticleStarInstance5", "ParticleStarMaterial", "StarTexture", glm::vec3(81.98, 5.0, -26.343)));
    //scene_.AddNode(manipulator->ConstructParticleSystem(&resman_, "SphereParticles", "ParticleInstance3", "ParticleGeyserMaterial", "SmokeTexture", glm::vec3(-3, 2, 0)));

    //scene_.AddNode(manipulator->ConstructParticleSystem(&resman_, "SphereParticlesBubbles", "BubbleParticles", "ParticleBubbleMaterial", "BubbleTexture", glm::vec3(0, 3, 0)));
    */
}

void Game::SetupGameScreen(void)
{

    camera_.SetTimer(500); // Starting player time limit / oxygen

    scene_.SetBackgroundColor(viewport_background_color_g);
    scene_.AddNode(manipulator->ConstructSkyBox(&resman_, "Sky_Box", glm::vec3(0, 3, 0)));

    scene_.AddNode(manipulator->ConstructPlane(&resman_)); // "Plane" | sandy floor

    scene_.AddNode(manipulator->ConstructBoundary(&resman_)); // "Boundary" | stone walls

    scene_.AddNode(manipulator->ConstructSun(&resman_, glm::vec3(0, 100, 0))); // "Sun"

    PopulateWorld();

    //scene_.AddNode(manipulator->ConstructParticleSystem(&resman_, "SphereParticlesBubbles", "BubbleParticles", "ParticleBubbleMaterial", "BubbleTexture", glm::vec3(0, 3, 0)));


    /*/ Hydrothermal vents
    scene_.AddNode(manipulator->ConstructParticleSystem(&resman_, "SphereParticles", "Vent1", "ParticleGeyserMaterial", "BubbleTexture", glm::vec3(-71, 0, -23)));
    scene_.GetNode("Vent1")->Scale(glm::vec3(15.0, 1.0, 15.0));
    scene_.GetNode("Vent1")->SetType(CompositeNode::Type::Vent);
    scene_.AddNode(manipulator->ConstructVentBase(&resman_, "VentBase1", glm::vec3(-71, 0, -23)));

    scene_.AddNode(manipulator->ConstructParticleSystem(&resman_, "SphereParticles", "Vent2", "ParticleGeyserMaterial", "BubbleTexture", glm::vec3(-79.7, 0, -23)));
    scene_.GetNode("Vent2")->Scale(glm::vec3(15.0, 1.0, 15.0));
    scene_.GetNode("Vent2")->SetType(CompositeNode::Type::Vent);
    scene_.AddNode(manipulator->ConstructVentBase(&resman_, "VentBase2", glm::vec3(-79.7, 0, -23)));

    scene_.AddNode(manipulator->ConstructParticleSystem(&resman_, "SphereParticles", "Vent3", "ParticleGeyserMaterial", "BubbleTexture", glm::vec3(-79.6, 0, -44.2)));
    scene_.GetNode("Vent3")->Scale(glm::vec3(15.0, 1.0, 15.0));
    scene_.GetNode("Vent3")->SetType(CompositeNode::Type::Vent);
    scene_.AddNode(manipulator->ConstructVentBase(&resman_, "VentBase3", glm::vec3(-79.6, 0, -44.2)));

    scene_.AddNode(manipulator->ConstructParticleSystem(&resman_, "SphereParticles", "Vent4", "ParticleGeyserMaterial", "BubbleTexture", glm::vec3(-70, 0, -44.2)));
    scene_.GetNode("Vent4")->Scale(glm::vec3(15.0, 1.0, 15.0));
    scene_.GetNode("Vent4")->SetType(CompositeNode::Type::Vent);
    scene_.AddNode(manipulator->ConstructVentBase(&resman_, "VentBase4", glm::vec3(-70, 0, -44.2)));

    scene_.AddNode(manipulator->ConstructParticleSystem(&resman_, "SphereParticles", "Vent5", "ParticleGeyserMaterial", "BubbleTexture", glm::vec3(-69.3, 0, -61.7)));
    scene_.GetNode("Vent5")->Scale(glm::vec3(15.0, 1.0, 15.0));
    scene_.GetNode("Vent5")->SetType(CompositeNode::Type::Vent);
    scene_.AddNode(manipulator->ConstructVentBase(&resman_, "VentBase5", glm::vec3(-69.3, 0, -61.7)));

    scene_.AddNode(manipulator->ConstructParticleSystem(&resman_, "SphereParticles", "Vent6", "ParticleGeyserMaterial", "BubbleTexture", glm::vec3(-80.8, 0, -61.5)));
    scene_.GetNode("Vent6")->Scale(glm::vec3(15.0, 1.0, 15.0));
    scene_.GetNode("Vent6")->SetType(CompositeNode::Type::Vent);
    scene_.AddNode(manipulator->ConstructVentBase(&resman_, "VentBase6", glm::vec3(-80.8, 0, -61.5)));

    // Stalagmites
    scene_.AddNode(manipulator->ConstructStalagmite(&resman_, "Stalagmite1", glm::vec3(85.2, 0, 19.2)));
    scene_.GetNode("Stalagmite1")->Scale(glm::vec3(0.7, 0.9, 0.7));

    scene_.AddNode(manipulator->ConstructStalagmite(&resman_, "Stalagmite2", glm::vec3(88.6, 0, 3.9)));
    scene_.GetNode("Stalagmite2")->Scale(glm::vec3(0.8, 0.8, 0.8));

    scene_.AddNode(manipulator->ConstructStalagmite(&resman_, "Stalagmite3", glm::vec3(79.5, 0, -1.5)));
    scene_.GetNode("Stalagmite3")->Scale(glm::vec3(0.7, 0.7, 0.7));

    scene_.AddNode(manipulator->ConstructStalagmite(&resman_, "Stalagmite4", glm::vec3(80.5, 0, 7.5)));
    scene_.GetNode("Stalagmite4")->Scale(glm::vec3(0.7, 0.9, 0.7));

    scene_.AddNode(manipulator->ConstructStalagmite(&resman_, "Stalagmite5", glm::vec3(75.5, 0, 13.5)));
    scene_.GetNode("Stalagmite5")->Scale(glm::vec3(0.6, 1.0, 0.7));

    scene_.AddNode(manipulator->ConstructStalagmite(&resman_, "Stalagmite6", glm::vec3(78.5, 0, 22.5)));
    scene_.GetNode("Stalagmite6")->Scale(glm::vec3(0.7, 0.7, 0.7));

    scene_.AddNode(manipulator->ConstructStalagmite(&resman_, "Stalagmite7", glm::vec3(87.5, 0, -4.0)));
    scene_.GetNode("Stalagmite7")->Scale(glm::vec3(0.7, 0.7, 0.7));
  */
}

void Game::SetupStartScreen(void)
{
    animating_ = false;
    UpdateStartHUD();
}

void Game::MainLoop(void){
    double current_time = 0.0f;
    float last_time = 0.0f;
    float delta_time = 0.0f;
    float mytheta = glm::pi<float>() / 64;
    while (!glfwWindowShouldClose(window_)){

        double current_time = glfwGetTime();
        if (state_ == start)
        {
            // scene_.SetBackgroundColor(glm::vec3(1.0, 0.0, 0.1));
            UpdateStartHUD();
        } 
        else if (state_ == win)
        {
            scene_.Draw();
            UpdateWinHUD();
        }
        else if (state_ == lose)
        {
            scene_.Draw();

            UpdateLoseHUD();
        }

        else if (state_ == ingame)
        {
            SceneNode* world_light = scene_.GetNode("Sphere")->GetRoot();
            float delta_time = 0.0f;
            // Animate the scene

            if (animating_) {

                float mytheta = glm::pi<float>() / 64;
                
                delta_time = current_time - last_time_;

                //if ((current_time - last_time_) > 0.05) {

                camera_.DecreaseTimer(current_time - last_time_); // Decrease remaining player time limit / oxygen

                scene_.Update(&camera_, &resman_);
                manipulator->AnimateAll(&scene_, current_time, mytheta);


                camera_.Update(delta_time);

                scene_.GetNode("BubbleParticles")->SetPosition(camera_.GetPosition() + glm::vec3(0, -0.5, 0.08)); // Make passive bubble particles follow player

                for (std::vector<CompositeNode*>::const_iterator iterator = scene_.begin(); iterator != scene_.end(); iterator++) {
                    collision_.CollisionEventCompositeNode(&camera_, *iterator);

                    if (camera_.GetNumParts() != last_num_machine_parts_)
                    {
                        SoundEngine->play2D((MATERIAL_DIRECTORY + std::string("\\audio\\ring.mp3")).c_str(), false);
                        last_num_machine_parts_ = camera_.GetNumParts();
                    }
                }

                std::cout << "Is being hurt" << camera_.IsBeingHurt() << std::endl;


                scene_.DrawToTexture(&camera_, world_light);

                scene_.DisplayTexture(resman_.GetResource("ScreenSpaceMaterial")->GetResource());

                // Update ImGui UI
                UpdateHUD();

                if (camera_.GetNumParts() == 5)
                {
                    animating_ = false;
                    scene_.ClearObj();
                    state_ = win;
                }

                else if (camera_.GetTimer() <= 0)
                {
                    animating_ = false;
                    scene_.ClearObj();
                    state_ = lose;
                }
                //std::cout << camera_.GetPosition().x << ", " << camera_.GetPosition().y << ", " << camera_.GetPosition().z << std::endl;

                // Hydrothermal vent collision switch
                /*
                if (int(current_time) % 6 == 0) { // On
                    for (int i = 1; i < 7; i++) {
                        scene_.GetNode("Vent" + std::to_string(i))->SetCollision(1);
                    }
                }
                else if (int(current_time) % 6 == 3) { // Off
                    for (int i = 1; i < 7; i++) {
                        scene_.GetNode("Vent" + std::to_string(i))->SetCollision(0);
                    }
                }
                */
            }


            // Process camera/player forward movement
            //camera_.Translate(camera_.GetForward() * camera_.GetSpeed());
            //if (camera_.GetSpeed() > 0) {
            //    camera_.SetSpeed(camera_.GetSpeed() * 0.98);

            //}
            // Draw the scene
            //scene_.Draw(&camera_, world_light);

        }

        // Process camera/player forward movement
        //camera_.Translate(camera_.GetForward() * camera_.GetSpeed());
        //if (camera_.GetSpeed() > 0) {
        //    camera_.SetSpeed(camera_.GetSpeed() * 0.98);

        //}
        // Draw the scene
        //scene_.Draw(&camera_, world_light);

        // Push buffer drawn in the background onto the display
        glfwSwapBuffers(window_);

        // Win condition
        //if (camera_.CheckWinCondition() == true) {

            //glfwSetWindowShouldClose(window_, true);

        //}

        last_time_ = current_time;

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
    float sens = 0.05;

    game->camera_.Yaw(sens * -dir.x);
    game->camera_.Pitch(sens * -dir.y);
    
    glfwSetCursorPos(window, width / 2, height / 2); // center the cursor
}

void Game::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods){
    void* ptr = glfwGetWindowUserPointer(window);
    Game *game = (Game *) ptr;

    if (game->state_ == start)
    {
        if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
        {
            game->animating_ = true;
            game->state_ = ingame;
            game->SetupGameScreen();

        }
    }
    else
    {

        // Quit game if 'q' is pressed
        if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }


        // Stop animation if space bar is pressed
        if (key == GLFW_KEY_E && action == GLFW_PRESS) {
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
                << "\nSPD: " << game->camera_.GetForwardSpeed() << std::endl;

        }

        // View control
        float rot_factor(2 * glm::pi<float>() / 180); // amount the ship turns per keypress (DOUBLE)
        float trans_factor = 0.7f; // amount the ship steps forward per keypress
        // Look up/down
        if (key == GLFW_KEY_UP) {
            game->camera_.Pitch(rot_factor);
        }
        if (key == GLFW_KEY_DOWN) {
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
        if (key == GLFW_KEY_A) {

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

        // Accelerate and break
        if (key == GLFW_KEY_W && glfwGetKey(window, key) == GLFW_PRESS) {
            /* //old architecture from acceleration based model
            float new_speed = game->camera_.GetSpeed() + 0.005f;

            if (new_speed < game->camera_.GetMaxSpeed()) {
                game->camera_.SetSpeed(new_speed);
            }
            else {
                game->camera_.SetSpeed(game->camera_.GetMaxSpeed());
            }*/
            game->pressed_.insert(key);
            game->camera_.UpdateForwardVelocity(1);
            //game->camera_.Translate(glm::vec3(game->camera_.GetForward().x, 0.0, game->camera_.GetForward().z) * trans_factor);
        }
        else if (key == GLFW_KEY_W && action == GLFW_RELEASE)
        {
            game->pressed_.erase(key);
        }

        if (key == GLFW_KEY_S && glfwGetKey(window, key) == GLFW_PRESS) {

            /* //old architecture from acceleration based model
            float new_speed = game->camera_.GetSpeed() - 0.05f;
            if (new_speed > game->camera_.GetMinSpeed()) {
                game->camera_.SetSpeed(new_speed);
            }
            else {
                game->camera_.SetSpeed(0.0f);
            }*/

            game->pressed_.insert(key);
            game->camera_.UpdateForwardVelocity(-1);
            //game->camera_.Translate(-glm::vec3(game->camera_.GetForward().x, 0.0, game->camera_.GetForward().z) * trans_factor);
        }

        else if (key == GLFW_KEY_S && action == GLFW_RELEASE)
        {
            game->pressed_.erase(key);
        }

        if (key == GLFW_KEY_D && glfwGetKey(window, key) == GLFW_PRESS)
        {

            game->pressed_.insert(key);
            game->camera_.UpdateSideVelocity(1);
        }
        else if (key == GLFW_KEY_D && action == GLFW_RELEASE)
        {
            game->pressed_.erase(key);
        }

        if (key == GLFW_KEY_A && glfwGetKey(window, key) == GLFW_PRESS)
        {
            game->pressed_.insert(key);
            game->camera_.UpdateSideVelocity(-1);
        }
        else if (key == GLFW_KEY_A && action == GLFW_RELEASE)
        {
            game->pressed_.erase(key);
        }


        if (game->pressed_.find(GLFW_KEY_W) == game->pressed_.end() && game->pressed_.find(GLFW_KEY_S) == game->pressed_.end())
        {
            game->camera_.SetForwardSpeed(0);
        }

        if (game->pressed_.find(GLFW_KEY_D) == game->pressed_.end() && game->pressed_.find(GLFW_KEY_A) == game->pressed_.end())
        {
            game->camera_.SetSideSpeed(0);
        }
    }
    if (game->pressed_.find(GLFW_KEY_D) == game->pressed_.end() && game->pressed_.find(GLFW_KEY_A) == game->pressed_.end()) {
        game->camera_.SetSideSpeed(0);
    }

    if (key == GLFW_KEY_F) {
        std::cout << "Current Position: " << glm::to_string(game->camera_.GetPosition()) << std::endl;
    }

}

void Game::ResizeCallback(GLFWwindow* window, int width, int height){

    // Set up viewport and camera projection based on new window size
    glViewport(0, 0, width, height);
    void* ptr = glfwGetWindowUserPointer(window);
    Game *game = (Game *) ptr;
    game->camera_.SetProjection(camera_fov_g, camera_near_clip_distance_g, camera_far_clip_distance_g, width, height);
}

void Game::UpdateStartHUD()
{
    // Generate new frame for OpenGl, glfw, and ImGui respectively
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    //ImGui::SetNextWindowSize(ImVec2(350, 100)); // Next window size
    ImVec2 screenSize = ImGui::GetIO().DisplaySize;
    ImVec2 titleSize = ImGui::CalcTextSize("The Submarine Game");
    ImVec2 pressEnterSize = ImGui::CalcTextSize("Press Enter to start");

    ImVec2 titlePos = ImVec2((screenSize.x - titleSize.x) * 0.4f, screenSize.y * 0.35f);
    ImVec2 pressEnterPos = ImVec2((screenSize.x - pressEnterSize.x) * 0.4f, screenSize.y * 0.5f);

    ImGui::SetNextWindowBgAlpha(0.6f); // Next window background alpha
    ImGui::Begin("UI", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground); // A bunch of flags to style window

    // Draw "The Submarine Game" text
    ImGui::SetCursorPos(titlePos);
    ImGui::Text("The Submarine Game");

    // Draw "Press Enter to start" text
    ImGui::SetCursorPos(pressEnterPos);
    ImGui::Text("Press Enter to start");

    // End GUI effect ------------------------
    ImGui::End();
    ImGui::Render();
    ImGui::EndFrame(); // <-- End GUI effect
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); // Render
}

void Game::UpdateWinHUD()
{
    // Generate new frame for OpenGl, glfw, and ImGui respectively
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    //ImGui::SetNextWindowSize(ImVec2(350, 100)); // Next window size
    ImVec2 screenSize = ImGui::GetIO().DisplaySize;
    ImVec2 titleSize = ImGui::CalcTextSize("You Win!");

    ImVec2 titlePos = ImVec2((screenSize.x - titleSize.x) * 0.4f, screenSize.y * 0.35f);

    ImGui::SetNextWindowBgAlpha(0.6f); // Next window background alpha
    ImGui::Begin("UI", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground); // A bunch of flags to style window

    // Draw "The Submarine Game" text
    ImGui::SetCursorPos(titlePos);
    ImGui::Text("You Win!");

    // End GUI effect ------------------------
    ImGui::End();
    ImGui::Render();
    ImGui::EndFrame(); // <-- End GUI effect
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); // Render
}

void Game::UpdateLoseHUD()
{
    // Generate new frame for OpenGl, glfw, and ImGui respectively
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    //ImGui::SetNextWindowSize(ImVec2(350, 100)); // Next window size
    ImVec2 screenSize = ImGui::GetIO().DisplaySize;
    ImVec2 titleSize = ImGui::CalcTextSize("You ran out of oxygen and died!");

    ImVec2 titlePos = ImVec2((screenSize.x - titleSize.x) * 0.4f, screenSize.y * 0.35f);

    ImGui::SetNextWindowBgAlpha(0.6f); // Next window background alpha
    ImGui::Begin("UI", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground); // A bunch of flags to style window

    // Draw "The Submarine Game" text
    ImGui::SetCursorPos(titlePos);
    ImGui::Text("You ran out of oxygen and died!");

    // End GUI effect ------------------------
    ImGui::End();
    ImGui::Render();
    ImGui::EndFrame(); // <-- End GUI effect
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); // Render
}

void Game::UpdateHUD() {

    // Generate new frame for OpenGl, glfw, and ImGui respectively
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    //ImGui::SetNextWindowSize(ImVec2(350, 100)); // Next window size
    ImGui::SetNextWindowBgAlpha(0.6f); // Next window background alpha
    ImGui::Begin("UI", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoScrollbar); // A bunch of flags to style window
    // Start GUI effect ----------------------

    // First row
    ImGui::Image((void*)resman_.GetResource("BubbleTexture")->GetResource(), ImVec2(50, 50)); // Bubble icon
    ImGui::SameLine();
    ImGui::SetCursorPosY(ImGui::GetWindowSize().y * 0.15); // Text offset
    ImGui::Text("OXYGEN: %i", (int)camera_.GetTimer());

    // Second row
    ImGui::Image((void*)resman_.GetResource("GearTexture")->GetResource(), ImVec2(50, 50)); // Gear icon
    ImGui::SameLine();
    ImGui::SetCursorPosY(ImGui::GetWindowSize().y * 0.62); // Text offset
    ImGui::Text("PARTS: %i / 5", camera_.GetNumParts(), camera_.GetNumParts());

    // End GUI effect ------------------------
    ImGui::End();
    ImGui::Render();
    ImGui::EndFrame(); // <-- End GUI effect
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); // Render
}


Game::~Game(){
    
    // Free ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Close glfw
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
