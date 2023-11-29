#ifndef GAME_COLLISION
#define GAME_COLLISION

#include "camera.h"
#include "scene_node.h"
#include "composite_node.h"

namespace game
{
	// Game collision handler for the game
	class GameCollision
	{
	public:
		// Constructor
		GameCollision();

		// Handler in the event the camera (player) collides with any object
		void CollisionEventSceneNode(Camera* camera, SceneNode* obj);


		void CollisionEventCompositeNode(Camera* camera, CompositeNode* obj);

		void PlayerSeaweedCollision(Camera* camera, CompositeNode* obj);
		// Handles collision if the player collides with a beacon (the beacon disappears if it's active)
		void PlayerMachinePartCollision(Camera* camera, CompositeNode* obj);
	};
}

#endif