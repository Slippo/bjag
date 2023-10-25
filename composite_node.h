/*
 *
 * A composite object for multiple nodes, ideally those of a singular object (i.e. all connected).
 * Any transformations done to a node should affect all child nodes equally.
 *
 * Author: George Power
 * Student ID: 101181367
 *
 */
#ifndef COMPOSITE_NODE_H
#define COMPOSITE_NODE_H

#include "scene_node.h"
#include <vector>

namespace game {

	// Class that manages multiple objects in a scene
	// Supports hierarchical transformations
	class CompositeNode {
	public:
		// Create a named composite node
		CompositeNode(const std::string name);
		~CompositeNode();
		
		// Methods
		// Add an already-created node
		void AddNode(SceneNode* node);
		
		// Getters
		SceneNode* GetNode(std::string node_name) const;
		inline std::string GetName(void) const { return name_; }
		std::vector<SceneNode*>::const_iterator begin() const;
		std::vector<SceneNode*>::const_iterator end() const;

		// Draw nodes
		void Draw(Camera* camera);

		// Update all nodes
		int Update(Camera* camera);

	private:
		std::string name_;
		std::vector<SceneNode*> node_;
		
		glm::vec3 background_color_;
	};

}

#endif // COMPOSITE_NODE_H