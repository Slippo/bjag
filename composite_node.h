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


		typedef enum Type { None, Kelp, Coral, Stalagmite, Submarine, Part , Anemonie, Seaweed, Rock, ParticleSystem, VentBase, Vent} NodeType; // Object types list

		// Create a named composite node
		CompositeNode(const std::string name);
		~CompositeNode();
		
		// Methods
		// Add an already-created node
		void AddNode(SceneNode* node);
		
		// Getters
		SceneNode* GetNode(std::string node_name) const;
		SceneNode* GetNode(int index) const;
		inline std::vector<SceneNode*> GetAllNodes() const { return node_; }
		inline std::string GetName(void) const { return name_; }
		SceneNode* GetRoot() const;
		Type GetType() const { return t_; }
		std::vector<SceneNode*>::const_iterator begin() const;
		std::vector<SceneNode*>::const_iterator end() const;

		void ClearNodes();

		// Setters
		void SetRoot(SceneNode* root);
		void SetType(Type type) { t_ = type; }

		// Collision
		void SetCollision(int val);
		int GetCollision();

		// Transformations
		void SetPosition(glm::vec3 pos);
		void Translate(glm::vec3 trans);
		void Rotate(glm::quat rot);
		void Orbit(glm::quat rot);
		void Scale(glm::vec3 scale);

		// Draw nodes
		void Draw(Camera* camera, SceneNode* light);

		// Update all nodes
		int Update(Camera* camera);

		std::vector<SceneNode*> hitboxes_;

	private:
		std::string name_;
		std::vector<SceneNode*> node_;
		SceneNode* root_ = nullptr;
		Type t_ = None; // Object type
		int collision_ = 0;
	};

}

#endif // COMPOSITE_NODE_H