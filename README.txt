COMP 3501 Assignment 3: Tree

Author: George Power
ID: 101181367

Creates a small floral plant with a function called CreateKelp(). The number of branches can be changed. The plant is animated by applying a transformation to the root node, and so the rest of the nodes follow along.

__Files__
camera.cpp/h, CMakeLists.txt, CMakeSettings.json, composite_node.cpp/h, game.cpp/h, kelp_material(_fp/_vp).glsl, main.cpp/h, material(_fp/_vp).glsl, path_config.h, README.txt, resource.cpp/h, resource_manager.cpp/h, scene_graph.cpp/h, scene_node.cpp/h

__Changes__
- composite_node.cpp/h
	Class describing a std::vector of SceneNodes
	This is done to encapsulate individual models

- game.cpp/h
	CreateKelp() function used to generation a new CompositeNode for a tree
	The "Kelp" composite node is animated in the main() function

- kelp_material_vp.glsl
	Custom shader to make the "Kelp" appear like stems and flowers

- scene_graph.cpp/h
	Changed the vector to store CompositeNode instead of SceneNode

- scene_node.cpp/h
	Added orbit_ and pivot_ so nodes can orbit around a given pivot point
	Added Orbit() method to update orbit_
	Nodes have a vector of SceneNode pointers to their children
	Post-transformation, the node passes it's translation matrix (transf) to it's children
