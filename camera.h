#ifndef CAMERA_H_
#define CAMERA_H_

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <iostream>


namespace game {

    // Abstraction of a camera
    class Camera {

        public:
            Camera(void);
            ~Camera();

            enum CameraState { walking = 0, jumping = 1, at_rest = 2 };
 
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
            float jump_ = 0.0;
            float base_y_position_ = 0.0;
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
            CameraState state_;

            // For collision
            float radius_;
            bool dead_;

            // Win condition
            bool win_condition_ = false;

            // Create view matrix from current camera parameters
            void SetupViewMatrix(void);

    }; // class Camera

} // namespace game

#endif // CAMERA_H_
