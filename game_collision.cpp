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
                //PlayerMachinePartCollision(camera, obj);
            }

            //else if (obj->GetType() == 1)
            //{
            //    PlayerEnemyCollision(camera, obj);
            //}

        }
    }

    void GameCollision::CollisionEventCompositeNode(Camera* camera, CompositeNode* obj)
    {

        glm::vec3 position_ = glm::vec3(obj->GetRoot()->GetPosition().x, camera->GetPosition().y, obj->GetRoot()->GetPosition().z);

        if (glm::length(camera->GetPosition() - position_) <= (camera->GetRadius() + obj->GetRoot()->GetRadius()))
        {
            // std::cout << "Collision detected" << std::endl;
            if (obj->GetType() == 5 && obj->GetRoot()->GetCollision() == 1)
            {
                // PlayerSeaweedCollision(camera, obj);
                PlayerMachinePartCollision(camera, obj);
                return;
            }
        }
       
    }

    void GameCollision::PlayerSeaweedCollision(Camera* camera, CompositeNode* obj)
    {
        obj->GetRoot()->SetCollision(2);
    }

    void GameCollision::PlayerMachinePartCollision(Camera* camera, CompositeNode* obj)
    {
        obj->GetRoot()->SetCollision(2);
    }
}