#include "composite_node.h"
#include <iostream>
namespace game {

    CompositeNode::CompositeNode(std::string name) {
        name_ = name;
    }

    CompositeNode::~CompositeNode(){}

    void CompositeNode::AddNode(SceneNode* node) {
        root_->AddChild(node);
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

    SceneNode* CompositeNode::GetRoot() const {
        if (root_ != nullptr) {
            return root_;
        }
        else { return nullptr; }
    }

    // iterators for the vector
    std::vector<SceneNode*>::const_iterator CompositeNode::begin() const { return node_.begin(); }
    std::vector<SceneNode*>::const_iterator CompositeNode::end() const { return node_.end(); }

    void CompositeNode::SetRoot(SceneNode* root) {
        root_ = root;
        root_->SetRadius(0.9f);
    }

    void CompositeNode::ClearNodes()
    {
        node_.clear();
    }

    // Transformations
    void CompositeNode::SetPosition(glm::vec3 pos) {
        root_->SetPosition(pos);
    }

    void CompositeNode::Translate(glm::vec3 trans) {
        root_->Translate(trans);
    }
    void CompositeNode::Rotate(glm::quat rot) {
        root_->Rotate(rot);
    }
    void CompositeNode::Orbit(glm::quat rot) {
        root_->Orbit(rot);
    }
    void CompositeNode::Scale(glm::vec3 scale) {
        root_->Scale(scale);
    }
    
    void CompositeNode::Draw(Camera* camera, SceneNode* light) {
        // Draw all scene nodes
        root_->Draw(camera, light);
        for (int i = 0; i < node_.size(); i++) {
            node_[i]->Draw(camera, light);
        }
    }

    // Update all nodes and their respective children
    int CompositeNode::Update(Camera* camera) {
        root_->Update(camera);
        for (int i = 0; i < node_.size(); i++) {
            node_[i]->Update(camera);
        }
        return 0;
    }
}