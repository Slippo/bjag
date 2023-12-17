#include "game_collision.h"
#include <iostream>

namespace game
{
    GameCollision::GameCollision()
    {
        prev_collision_ = -1.0;
    }
    void GameCollision::CollisionEventCompositeNode(Camera* camera, CompositeNode* obj)
    {

        bool h = false;
        glm::vec3 position_ = glm::vec3(obj->GetRoot()->GetPosition().x, camera->GetPosition().y, obj->GetRoot()->GetPosition().z);

        if (glm::length(camera->GetPosition() - position_) <= (camera->GetRadius() + obj->GetRoot()->GetRadius()))
        {
            // If we collided with a machine part
            if (obj->GetType() == 5 && obj->GetRoot()->GetCollision() == 1)
            {
                PlayerMachinePartCollision(camera, obj);
                return;
            }

            if (obj->GetType() == CompositeNode::Type::Vent && obj->GetCollision() == 1) { // If player is hit by hydrothermal vent stream
                camera->DecreaseTimer(1);
                prev_collision_ = glfwGetTime();
            }
            
        }

        if (obj->hitboxes_.size() != 0) { // If the object has a hitbox (for spikes)

            for (int i = 0; i < obj->hitboxes_.size(); i++) { // For each hitbox

                glm::vec3 hitbox_pos = obj->GetRoot()->GetPosition() + obj->hitboxes_[i]->GetPosition(); // Calculate "canonical" position of hitbox

                if (glm::length(camera->GetPosition() - hitbox_pos) <= (camera->GetRadius() + obj->hitboxes_[i]->GetRadius())) { // Sphere-to-sphere collision

                    if (obj->GetType() == CompositeNode::Type::Stalagmite) {
                        camera->DecreaseTimer(1);
                        prev_collision_ = glfwGetTime();
                    }
                }
                
            }
        }

        if (glfwGetTime() - prev_collision_ <= 0.05 && prev_collision_ >= 0.0)
        {
            camera->SetHurt(true);
        }
        else
        {
            camera->SetHurt(false);
        }
        
    }

    // If player collides with machine part, increase the number of parts we've collected
    void GameCollision::PlayerMachinePartCollision(Camera* camera, CompositeNode* obj)
    {
        obj->GetRoot()->SetCollision(2);
        camera->IncreaseNumParts();
    }
}