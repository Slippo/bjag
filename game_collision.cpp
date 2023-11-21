#include "game_collision.h"
#include <iostream>

namespace game
{
    GameCollision::GameCollision()
    {

    }

    void GameCollision::CollisionEventSceneNode(Camera* camera, SceneNode* obj)
    {

        if (glm::length(camera->GetPosition() - obj->GetPosition()) <= (camera->GetRadius() + obj->GetRadius()))
        {
            if (obj->GetType() == 1 && obj->GetCollision() == 1)
            {
                PlayerMachinePartCollision(camera, obj);
            }

            //else if (obj->GetType() == 1)
            //{
            //    PlayerEnemyCollision(camera, obj);
            //}

        }
    }

    void GameCollision::PlayerMachinePartCollision(Camera* camera, SceneNode* obj)
    {
        obj->SetCollision(2);
    }
}