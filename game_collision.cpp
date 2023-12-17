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

            //if (obj->GetType() == 1 && obj->GetCollision() == 1)
            //{
                //PlayerMachinePartCollision(camera, obj);
            //}

            //else if (obj->GetType() == 1)
            //{
            //    PlayerEnemyCollision(camera, obj);
            //}    
        }
    }

    void GameCollision::CollisionEventCompositeNode(Camera* camera, CompositeNode* obj)
    {

        glm::vec3 position_ = obj->GetRoot()->GetPosition();

        if (glm::length(camera->GetPosition() - position_) <= (camera->GetRadius() + obj->GetRoot()->GetRadius()))
        {
            // std::cout << "Collision detected" << std::endl;
            if (obj->GetType() == 5 && obj->GetRoot()->GetCollision() == 1)
            {
                // PlayerSeaweedCollision(camera, obj);
                PlayerMachinePartCollision(camera, obj);
                return;
            }

            if (obj->GetType() == CompositeNode::Type::Vent && obj->GetCollision() == 1) { // If player is hit by hydrothermal vent stream
                camera->DecreaseTimer(1);
            }
            
        }

        if (obj->hitboxes_.size() != 0) { // If the object has a hitbox

            for (int i = 0; i < obj->hitboxes_.size(); i++) { // For each hitbox

                glm::vec3 hitbox_pos = obj->GetRoot()->GetPosition() + obj->hitboxes_[i]->GetPosition(); // Calculate "canonical" position of hitbox

                if (glm::length(camera->GetPosition() - hitbox_pos) <= (camera->GetRadius() + obj->hitboxes_[i]->GetRadius())) { // Sphere-to-sphere collision

                    if (obj->GetType() == CompositeNode::Type::Stalagmite) {
                        camera->DecreaseTimer(1);
                    }
                }
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