#ifndef MANIPULATOR_H
#define MANIPULATOR_H

#include "game.h"

/*
The Manipulator class has two primary functions:
(1) Constructs hierarchical objects
(2) Animates hierarchical objects
*/

namespace game {
	class Manipulator {
		public:
			Manipulator();
			~Manipulator();

			// (1) Construct hierarchical objects
			CompositeNode* ConstructKelp(ResourceManager* resman_, std::string name_, int branch_complexity = 4, glm::vec3 position_ = glm::vec3(0.0, 0.0, 0.0));
			CompositeNode* ConstructStalagmite(ResourceManager* resman_, std::string name_, glm::vec3 position_ = glm::vec3(0.0, 0.0, 0.0));
			CompositeNode* ConstructSubmarine(ResourceManager* resman_, std::string name_, glm::vec3 position_ = glm::vec3(0.0, 0.0, 0.0));
			CompositeNode* ConstructCoral(ResourceManager* resman_, std::string name_, glm::vec3 position_ = glm::vec3(0.0, 0.0, 0.0));
			CompositeNode* ConstructSeaweed(ResourceManager* resman_, std::string name_, int length_complexity = 4, glm::vec3 position_ = glm::vec3(0.0, 0.0, 0.0));

			// (2) Animate hierarchical objects
			void AnimateAll(SceneGraph* scene_, double time_, float theta_);
			void AnimateKelp(CompositeNode* node_, double time_, float theta_);
			void AnimateSeaweed(CompositeNode* node_, double time_, float theta_);
	
		private:
			// Copied from game.cpp
			SceneNode* CreateSceneNodeInstance(std::string entity_name, std::string object_name, std::string material_name, ResourceManager* resman_);

	};
}

#endif