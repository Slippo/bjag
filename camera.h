#ifndef CAMERA_H_
#define CAMERA_H_

#define GLEW_STATIC
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <vector>


namespace game {

    // Abstraction of a camera
    class Camera {

        public:
            Camera(void);
            ~Camera();

            enum CameraState { walking = 0, jumping = 1, at_rest = 2 }; // at_rest is never used
 
            // Get global camera attributes
            glm::vec3 GetPosition(void) const;
            glm::quat GetOrientation(void) const;

            // Set global camera attributes
            void SetPosition(glm::vec3 position);
            void SetOrientation(glm::quat orientation);
            void SetForwardSpeed(float speed);
            void SetSideSpeed(float speed);
            void SetMaxSpeed(float speed);
            void SetRadius(float r);
            void SetTimer(float t);
            void IncreaseTimer(float t);
            void DecreaseTimer(float t);
            void AddPart(); // Increase num_parts_
            void SetDead(bool d);
            inline void SetState(CameraState t) { state_ = t; }
            void SetHeightMap(std::vector<float> h, std::vector<float> height_boundary);
            void SetDimensions(int x, int z, int width, int height);
            float CalculateSlope(float h);
            void UpdateForwardVelocity(float backwards);
            void UpdateSideVelocity(float left);
            void Update(float delta_time);
            
            // Perform global transformations of camera
            void Translate(glm::vec3 trans);
            void Rotate(glm::quat rot);

            // Get relative attributes of camera
            glm::vec3 GetForward(void) const;
            glm::vec3 GetSide(void) const;
            glm::vec3 GetUp(void) const;
            glm::vec3 GetForwardMovement(void) const;
            glm::vec3 GetSideMovement(void) const;
            float GetForwardSpeed(void) const;
            float GetSideSpeed(void) const;
            float GetMaxSpeed(void) const;
            float GetMinSpeed(void) const;
            float GetRadius(void) const;
            float GetTimer(void) const;
            int GetNumParts(void) const;
            bool IsDead(void) const;
            bool CheckWinCondition(void) const;
            inline CameraState GetState() { return state_; }

            // Perform relative transformations of camera
            void Pitch(float angle);
            void Yaw(float angle);
            void Roll(float angle);

            void Jump();

            // Set the view from camera parameters: initial position of camera,
            // point looking at, and up vector
            // Resets the current orientation and position of the camera
            void SetView(glm::vec3 position, glm::vec3 look_at, glm::vec3 up);
            // Set projection from frustum parameters: field-of-view,
            // near and far planes, and width and height of viewport
            void SetProjection(GLfloat fov, GLfloat near, GLfloat far, GLfloat w, GLfloat h);
            // Set all camera-related variables in shader program
            void SetupShader(GLuint program);

        private:
            float forward_speed_; // Current speed factor
            float side_speed_;
            float max_speed_ = 6.0f; // Maximum speed factor
            float min_speed_ = -6.0f; // Minimum speed factor
            float jump_limit_ = 60.0f;
            float timer_ = 240.0; // Oxygen / health / game time
            int num_parts_ = 0; // Game win condition, goes up when submarine part is collected
            glm::vec3 position_; // Position of camera
            //glm::quat orientation_; // Orientation of camera
            glm::quat orientation_;
            glm::quat movement_orientation_;
            glm::vec3 forward_; // Initial forward vector (-Z)
            glm::vec3 side_; // Initial side vector (+X)
            glm::vec3 up_; // Initial up vector (+Y)
            glm::vec3 movement_forward_;
            glm::vec3 movement_side_;
            glm::mat4 view_matrix_; // View matrix
            glm::mat4 projection_matrix_; // Projection matrix
            glm::vec3 base_position_;
            CameraState state_;

            std::vector<float> height_map_;
            std::vector<float> slope_map_;
            int offsetX;
            int offsetZ;
            int width;
            int height;

            // Physics values
            float base_vel = 10.0f;
            float jump_height = 10.0f;
            float gravity = 5.8f;
            float t_;

            // For collision
            float radius_; // IDK when this will be used
            
            // Win/loss conditions
            bool dead_ = false;
            bool win_condition_ = false;

            // Create view matrix from current camera parameters
            void SetupViewMatrix(void);

    }; // class Camera

} // namespace game

#endif // CAMERA_H_
