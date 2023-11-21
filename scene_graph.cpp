#include <stdexcept>
#include <iostream>
#include <fstream>
#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "scene_graph.h"
#include "camera.h"
namespace game {

SceneGraph::SceneGraph(void){

    background_color_ = glm::vec3(0.0, 0.0, 0.0);
}


SceneGraph::~SceneGraph(){
}


void SceneGraph::SetBackgroundColor(glm::vec3 color){

    background_color_ = color;
}


glm::vec3 SceneGraph::GetBackgroundColor(void) const {

    return background_color_;
}

void SceneGraph::AddNode(CompositeNode *node){

    node_.push_back(node);
}


CompositeNode *SceneGraph::GetNode(std::string node_name) const {

    // Find node with the specified name
    for (int i = 0; i < node_.size(); i++){
        if (node_[i]->GetName() == node_name){
            return node_[i];
        }
    }
    return NULL;

}
  
CompositeNode* SceneGraph::GetNode(int index) const {
    
    if (index >= 0 && index < node_.size()) {
        return node_[index];
    }
    return NULL;
}

std::vector<CompositeNode *>::const_iterator SceneGraph::begin() const { return node_.begin(); }


std::vector<CompositeNode *>::const_iterator SceneGraph::end() const { return node_.end(); }


void SceneGraph::Draw(Camera *camera, SceneNode* light){

    // Clear background
    glClearColor(background_color_[0], 
                 background_color_[1],
                 background_color_[2], 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw all scene nodes
    for (int i = 0; i < node_.size(); i++){
        node_[i]->Draw(camera, light);
    }
}

// Handles movement, collisions, and geometric changes
int SceneGraph::Update(Camera* camera, ResourceManager* resman) {
    int val = 0;
    for (int i = 0; i < node_.size(); i++) {
        node_[i]->Update(camera);
    }
    return 0;
}
} // namespace game
