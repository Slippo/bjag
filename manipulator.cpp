#include "manipulator.h"
#include <iostream>

namespace game {
	
	Manipulator::Manipulator() {}
	Manipulator::~Manipulator() {}

	// (1) Construct hierarchical objects
	CompositeNode* Manipulator::ConstructKelp(ResourceManager* resman_, int branch_complexity, glm::vec3 position_) {
        
        CompositeNode* kelp = new CompositeNode("Kelp");

        // Create root node
        SceneNode* root = CreateSceneNodeInstance("Root", "Cylinder", "KelpMaterial", resman_);
        root->SetPosition(position_);
        root->SetPivot(position_ + glm::vec3(0, -1, 0));
        root->SetType(SceneNode::Type::Stem);
        kelp->AddNode(root);

        // Create branches and sub-branches   
        float theta = 2 * glm::pi<float>();
        float offset = 0.2f;
        float stem_len = 2.0f;
        // CREATE BRANCHES CONNECTED TO THE ROOT NODE
        for (int i = 0; i < branch_complexity + 1; i++) {
            float frac = float(i) / (branch_complexity + 1);
            SceneNode* branch = CreateSceneNodeInstance("Branch", "Cylinder", "KelpMaterial", resman_);
            branch->SetType(SceneNode::Type::Stem); // type specified for shader
            branch->SetPosition(glm::vec3(0, 2, 0)); // 2 units above the root
            branch->SetPivot(glm::vec3(0, -stem_len / 2, 0));
            // Orbit about the pivot to set the starting orientation
            branch->Orbit(glm::angleAxis(theta / 12, glm::vec3(cos(theta * frac), 0, sin(theta * frac))));
            root->AddChild(branch);
            kelp->AddNode(branch);

            // CREATE LEAVES FOR THE CURRENT BRANCH
            // THE BRANCH HAS LOWER LEAVES AND HIGH LEAVES
            for (int j = 0; j < branch_complexity; j++) {
                float frac = float(j) / (branch_complexity);
                SceneNode* high_leaf = CreateSceneNodeInstance("Leaf", "Sphere", "KelpMaterial", resman_);
                high_leaf->SetType(SceneNode::Type::Leaf);
                high_leaf->SetScale(0.25f * glm::vec3(1.0f + abs(cos(theta * frac)), 0.2f, 1.0f + abs(sin(theta * frac))));
                high_leaf->Scale(glm::vec3(0.75f));
                high_leaf->SetPosition(glm::vec3(offset * cos(theta * frac), 1, offset * sin(theta * frac)));
                high_leaf->SetPivot(glm::vec3(0, -1, 0));
                SceneNode* low_leaf = CreateSceneNodeInstance("SubSubBranch", "Sphere", "KelpMaterial", resman_);
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
                SceneNode* sub_branch = CreateSceneNodeInstance("SubBranch", "Cylinder", "KelpMaterial", resman_);
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
                    SceneNode* leaf = CreateSceneNodeInstance("Leaf", "Sphere", "KelpMaterial", resman_);
                    leaf->SetType(SceneNode::Type::Leaf);
                    leaf->SetScale(0.25f * glm::vec3(1.0f + abs(cos(theta * frac)), 0.2f, 1.0f + abs(sin(theta * frac))));
                    leaf->SetPosition(glm::vec3(offset * cos(theta * frac), 1, offset * sin(theta * frac)));
                    leaf->SetPivot(glm::vec3(0, -stem_len / 2, 0));
                    sub_branch->AddChild(leaf);
                    kelp->AddNode(leaf);
                }
            }
        }

        return kelp;
	}

	// (2) Animate hierarchical objects
    void Manipulator::AnimateAll(SceneGraph* scene_, double time_, float theta_) {
        for (int i = 0; i < scene_->GetSize(); i++) {
            if (scene_->GetNode(i)->GetName() == "Kelp") {
                SceneNode* root = scene_->GetNode(i)->GetNode("Root");
                root->Orbit(glm::angleAxis(theta_, glm::vec3(0.1 * sin(time_), 0, 0.05 * sin(1 - time_))));
                for (int x = 0; x < root->GetChildCount(); x++) {
                    root->GetChild(x)->Orbit(glm::angleAxis(theta_, glm::vec3(0.1 * sin(time_), 0, 0.05 * sin(1 - time_))));
                }
            }
        }
    }

	void Manipulator::AnimateKelp(CompositeNode* root_) {

	}

	// Copied from game.cpp
	SceneNode* Manipulator::CreateSceneNodeInstance(std::string entity_name, std::string object_name, std::string material_name, ResourceManager* resman_) {

		// Get resources
		Resource* geom = resman_->GetResource(object_name);
		if (!geom) {
			throw(GameException(std::string("Could not find resource \"") + object_name + std::string("\"")));
		}

		Resource* mat = resman_->GetResource(material_name);
		if (!mat) {
			throw(GameException(std::string("Could not find resource \"") + material_name + std::string("\"")));
		}

		// Create object instance
		SceneNode* obj = new SceneNode(entity_name, geom, mat, 0);
		return obj;
	}
}