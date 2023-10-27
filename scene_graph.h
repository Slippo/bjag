#ifndef SCENE_GRAPH_H_
#define SCENE_GRAPH_H_

#include <string>
#include <vector>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "scene_node.h"
#include "composite_node.h"
#include "resource.h"
#include "resource_manager.h"
#include "camera.h"

namespace game {

    // Class that manages all the objects in a scene
    class SceneGraph {

        private:
            // Background color
            glm::vec3 background_color_;

            // Scene nodes to render (now only accepts composite nodes)
            std::vector<CompositeNode *> node_;

        public:
            // Constructor and destructor
            SceneGraph(void);
            ~SceneGraph();
            // Getters
            int GetSize() { return node_.size(); }
            CompositeNode* GetNode(std::string node_name) const;
            CompositeNode* GetNode(int index) const;

            // Background color
            void SetBackgroundColor(glm::vec3 color);
            glm::vec3 GetBackgroundColor(void) const;
            
            // Add an already-created node
            void AddNode(CompositeNode *node);
            // Get node const iterator
            std::vector<CompositeNode *>::const_iterator begin() const;
            std::vector<CompositeNode *>::const_iterator end() const;

            // Draw the entire scene
            void Draw(Camera *camera);

            // Update entire scene
            int Update(Camera *camera, ResourceManager *resman);

    }; // class SceneGraph

} // namespace game

#endif // SCENE_GRAPH_H_
