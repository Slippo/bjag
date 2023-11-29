#ifndef GAME_H_
#define GAME_H_

#include "imgui/imgui.h"

#include <exception>
#include <string>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <fstream>
#include <set>

#include "scene_graph.h"
#include "resource_manager.h"
#include "camera.h"
#include "composite_node.h"
#include "manipulator.h"
#include "game_collision.h"

namespace game {

    // Exception type for the game
    class GameException: public std::exception
    {
        private:
            std::string message_;
        public:
            GameException(std::string message) : message_(message) {};
            virtual const char* what() const throw() { return message_.c_str(); };
            virtual ~GameException() throw() {};
    };

    // Game application
    class Game {

        public:
            // Constructor and destructor
            Game(void);
            ~Game();
            // Call Init() before calling any other method
            void Init(void); 
            // Set up resources for the game
            void SetupResources(void);
            // Set up initial scene
            void SetupScene(void);
            // Run the game: keep the application active
            void MainLoop(void); 

        private:

            // GLFW window
            GLFWwindow* window_;

            // Scene graph containing all nodes to render
            SceneGraph scene_;

            // Resources available to the game
            ResourceManager resman_;

            // Camera abstraction
            Camera camera_;

            // Collision handler
            GameCollision collision_;

            // Flag to turn animation on/off
            bool animating_;

            bool moving_;

            std::set<int> pressed_;

            // Size of both planes
            const glm::ivec2 plane_size_ = glm::ivec2(200,200);
            std::vector<float> height_map_; // height map for the floor
            std::vector<float> height_map_boundary_; // height map for the boundary (stone walls)

            // ImGui io (to retain data)
            ImGuiIO imgui_io_;

            // Methods to initialize the game
            void InitWindow(void);
            void InitView(void);
            void InitEventHandlers(void);
 
            // Methods to handle events
            static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
            static void ResizeCallback(GLFWwindow* window, int width, int height);
            static void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);

            // UI
            void UpdateHUD();

            // Kelp tree/bush nodes
            // The sphere used to make leaves
            // The scene node instance is used to make the cylinders (stems/branches)
            SceneNode* CreateSphereInstance(std::string entity_name, std::string object_name, std::string material_name);
            SceneNode* CreateSceneNodeInstance(std::string entity_name, std::string object_name, std::string material_name);



    }; // class Game

} // namespace game

#endif // GAME_H_
