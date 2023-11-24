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
			CompositeNode* ConstructSeaweed(ResourceManager* resman_, std::string name_, int length_ = 4, glm::vec3 position_ = glm::vec3(0.0, 0.0, 0.0));
			void ConstructSeaweedPatch(ResourceManager* resman_, SceneGraph* scene_, int num_strands, int length, int width, glm::vec3 position_ = glm::vec3(0.0, 0.0, 0.0)); // Makes a group of seaweed objects based on the given parameters
			CompositeNode* ConstructPart(ResourceManager* resman_, std::string name_, glm::vec3 position_ = glm::vec3(0.0, 0.0, 0.0));
			CompositeNode* ConstructAnemonie(ResourceManager* resman_, std::string name_, glm::vec3 position_ = glm::vec3(0.0, 0.0, 0.0));
			CompositeNode* ConstructSeaweed(ResourceManager* resman_, std::string name_, int length_complexity = 4, glm::vec3 position_ = glm::vec3(0.0, 0.0, 0.0));

			// Create the sand floor
			CompositeNode* ConstructPlane(ResourceManager* resman_);
			// Create the stone boundary/wall
			CompositeNode* ConstructBoundary(ResourceManager* resman_);
      // Create light source
			CompositeNode* ConstructSun(ResourceManager* resman, glm::vec3 position_ = glm::vec3(0.0, 20.0, 0.0));


			// (2) Animate hierarchical objects
			void AnimateAll(SceneGraph* scene_, double time_, float theta_);
			void AnimateKelp(CompositeNode* node_, double time_, float theta_);
			void AnimateSeaweed(CompositeNode* node_, double time_, float theta_);
			void AnimateSubmarine(CompositeNode* node_, double time_, float theta_);
	
		private:
			// Copied from game.cpp
			SceneNode* CreateSceneNodeInstance(std::string entity_name, std::string object_name, std::string material_name, std::string texture_name, ResourceManager* resman_);

	};
}

#endif