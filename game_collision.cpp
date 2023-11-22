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

    void GameCollision::CollisionEventCompositeNode(Camera* camera, CompositeNode* obj)
    {
        if (glm::length(camera->GetPosition() - obj->GetRoot()->GetPosition()) <= (camera->GetRadius() + obj->GetRoot()->GetRadius()))
        {
            if (obj->GetType() == 7 && obj->GetRoot()->GetCollision() == 1)
            {
                PlayerSeaweedCollision(camera, obj);
            }
        }
    }

    void GameCollision::PlayerSeaweedCollision(Camera* camera, CompositeNode* obj)
    {
        obj->GetRoot()->SetCollision(2);
    }

    void GameCollision::PlayerMachinePartCollision(Camera* camera, SceneNode* obj)
    {
        obj->SetCollision(2);
    }
}