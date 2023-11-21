#include "composite_node.h"
#include <iostream>
namespace game {

    CompositeNode::CompositeNode(std::string name) {
        name_ = name;
    }

    CompositeNode::~CompositeNode(){}

    void CompositeNode::AddNode(SceneNode* node) {

        node_.push_back(node);
    }

    SceneNode* CompositeNode::GetNode(std::string node_name) const {

        // Find node with the specified name
        for (int i = 0; i < node_.size(); i++) {
            if (node_[i]->GetName() == node_name) {
                return node_[i];
            }
        }
        return NULL;
    }

    // iterators for the vector
    std::vector<SceneNode*>::const_iterator CompositeNode::begin() const { return node_.begin(); }
    std::vector<SceneNode*>::const_iterator CompositeNode::end() const { return node_.end(); }

    void CompositeNode::Draw(Camera* camera, SceneNode* light) {
        // Draw all scene nodes
        for (int i = 0; i < node_.size(); i++) {
            node_[i]->Draw(camera, light);
        }
    }

    // Update all nodes and their respective children
    int CompositeNode::Update(Camera* camera) {
        for (int i = 0; i < node_.size(); i++) {
            node_[i]->Update(camera);
        }
        return 0;
    }
}