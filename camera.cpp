#include <stdexcept>
#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "camera.h"

namespace game {

//define constant acceleration factor
    float base_vel = 5.0f;
    float jump_height = 4.0f;
    float gravity = 1.8f;
    float t_;

Camera::Camera(void){
    state_ = walking;
    jump_ = 0.0;
    radius_ = 1.0f;

    max_y_ = 15.5;
    ground_height_ = 3.4f;
}

Camera::~Camera(){}

glm::vec3 Camera::GetPosition(void) const {
    return position_;
}

glm::quat Camera::GetOrientation(void) const {
    return orientation_;
}

void Camera::SetPosition(glm::vec3 position){
    position_ = position;
}

void Camera::SetOrientation(glm::quat orientation){
    orientation_ = orientation;
}

void Camera::SetForwardSpeed(float speed) {
    forward_speed_ = speed;
}

void Camera::SetSideSpeed(float speed)
{
    side_speed_ = speed;
}

void Camera::SetMaxSpeed(float speed) {
    max_speed_ = speed;
}

void Camera::SetRadius(float r)
{
    radius_ = r;
}

void Camera::SetTimer(float t) {
    timer_ = t;
}

float Camera::CalculateSlope(float h)
{
    float curr_height = position_.y;
    float slope = -100;
    
    slope = abs(curr_height - (h));

    if (h <= ground_height_ + 0.4)
    {
        slope = 0;
    }

    return slope;


}

void Camera::SetHeightMap(std::vector<float> h, std::vector<float> height_boundary)
{

    for (int i = 0; i < h.size(); i++)
    {
        height_map_.push_back(std::max(h[i], height_boundary[i]));
    }

    float slopes[4];
    int index = 0;
    // z
    for (int i = 0; i < height - 1; i++)
    {
        // x
        for (int j = 0; j < width - 1; j++)
        {
            float heightA = height_map_[j + (width -1)* i];
            float heightB = height_map_[j + 1 + (width - 1) * i];
            float heightC = height_map_[j + (width - 1) * (i + 1)];
            float heightD = height_map_[(j + 1) + (width - 1) * (i + 1)];

            //AB
            slopes[0] = heightB - heightA;

            //BD
            slopes[1] = heightD - heightB;

            //CD
            slopes[2] = heightD - heightC;

            //AC
            slopes[3] = heightC - heightA;
            for (int i = 1; i < 4; i++)
            {
                if (abs(slopes[index]) < abs(slopes[i]))
                {
                    index = i;
                }
            }

            slope_map_.push_back(slopes[index]);
            index = 0;
        }
    }

}

void Camera::SetDimensions(int x, int z, int w, int h)
{
    offsetX = x;
    offsetZ = z;
    width = w;
    height = h;

    slope_map_.reserve((width-1) * (height-1));
    height_map_.reserve(width * height);
}

void Camera::IncreaseTimer(float t) {
    timer_ += t;
}

void Camera::DecreaseTimer(float t) {
    timer_ -= t;
}

void Camera::AddPart() {
    num_parts_++;
    if (num_parts_ >= 5) {
        win_condition_ = true;
    }
}

void Camera::SetDead(bool d)
{
    dead_ = d;
}

void Camera::SetHurt(bool h)
{
    hurt_ = h;
}

void Camera::Translate(glm::vec3 trans){
    position_ += trans;
}

void Camera::Rotate(glm::quat rot){
    // apply this rotation to the camera's current orientation
    orientation_ = rot * orientation_;
    orientation_ = glm::normalize(orientation_);
}

glm::vec3 Camera::GetForward(void) const {
    glm::vec3 current_forward = orientation_ * forward_;
    return -current_forward; // Return -forward since the camera coordinate system points in the opposite direction
}

glm::vec3 Camera::GetForwardMovement(void) const {
    glm::vec3 current_forward = movement_orientation_ * movement_forward_;
    return -current_forward;
}

glm::vec3 Camera::GetSideMovement(void) const
{
    glm::vec3 current_side = movement_orientation_ * movement_side_;
    return current_side;
}

glm::vec3 Camera::GetSide(void) const {
    glm::vec3 current_side = orientation_ * side_;
    return current_side;
}

glm::vec3 Camera::GetUp(void) const {
    glm::vec3 current_up = orientation_ * up_;
    return current_up;
}

float Camera::GetForwardSpeed(void) const {
    return forward_speed_;
}

float Camera::GetSideSpeed(void) const
{
    return side_speed_;
}

float Camera::GetMaxSpeed(void) const {
    return max_speed_;
}

float Camera::GetMinSpeed(void) const {
    return min_speed_;
}

float Camera::GetRadius(void) const
{
    return radius_;
}

float Camera::GetTimer(void) const {
    return timer_;
}

int Camera::GetNumParts(void) const {
    return num_parts_;
}

bool Camera::IsBeingHurt(void) const {
    return hurt_;
}

void Camera::IncreaseNumParts(void) {
    num_parts_ += 1;
}

bool Camera::IsDead(void) const
{
    return dead_;
}

bool Camera::CheckWinCondition(void) const {
    return win_condition_;
}

void Camera::Pitch(float angle){
    glm::quat rotation = glm::angleAxis(angle, GetSide());
    orientation_ = rotation * orientation_;
    orientation_ = glm::normalize(orientation_);
}

void Camera::Yaw(float angle){
    //glm::quat rotation = glm::angleAxis(angle, GetUp());
    glm::quat rotation = glm::angleAxis(angle, glm::vec3(0, 1, 0));
    orientation_ = rotation * orientation_;
    orientation_ = glm::normalize(orientation_);

    movement_orientation_ = rotation * movement_orientation_;
    movement_orientation_ = glm::normalize(movement_orientation_);
}

void Camera::Roll(float angle){
    glm::quat rotation = glm::angleAxis(angle,GetForward());
    orientation_ = rotation * orientation_;
    orientation_ = glm::normalize(orientation_);
}

void Camera::Jump()
{
    if (state_ == walking)
    {
        base_y_position_ = position_.y;
        old_position_ = position_;
        state_ = jumping;
        t_ = 0.0; //set timer to zero just to make sure
     
    }
}
void Camera::Update(float delta_time)
{
    glm::vec3 oldPos = position_;
    glm::vec3 tempPos = position_ + ((GetForwardMovement() * forward_speed_ * delta_time)) + (GetSideMovement() * side_speed_ * delta_time);
    //position_ += (GetForwardMovement() * forward_speed_ * delta_time);
    //position_ += (GetSideMovement() * side_speed_ * delta_time);    

    float oldY = 0.0;
    

    int coordXMin = floor(tempPos.x + offsetX);
    int coordXMax = ceil(tempPos.x + offsetX);

    int coordZMin = floor(tempPos.z + offsetZ);
    int coordZMax = ceil(tempPos.z + offsetZ);

    float s = tempPos.x + offsetX - coordXMin;
    float t = tempPos.z + offsetZ - coordZMin;

    int x = coordXMin - (1 - round(s));
    int z = coordZMin - (1 - round(t));


    float a = (height_map_)[coordXMin + (width) * coordZMin];
    float b = (height_map_)[coordXMax + (width) * coordZMin];
    float c = (height_map_)[coordXMin + (width) * coordZMax];
    float d = (height_map_)[coordXMax + (width) * coordZMax];

    float interpolation = ((1 - t) * ((1 - s) * a + s * b) + t * ((1 - s) * c + s * d)) + 3.0;
    if (state_ == jumping)
    {
        //y position is calculated using kinematic equation of vertical motion, factoring in gravity, base y position,
        //basevelocity, and time (jump height is also specified here)
        if (position_.y <= ground_height_ + 0.2)
        {
            if (abs(slope_map_[x + width * z]) >= 1.1 || x == 0 || x == height - 2 || z == 0 || z == width - 2)
            {
                state_ = walking;
                return;
            }
        }
        /*else
        {
            float slope = CalculateSlope(interpolation);
            if (slope >= 1.1 || x == 0 || x == height - 2 || z == 0 || z == width - 2)
            {
                state_ = falling;
            }

        }*/
        
        position_ = tempPos;
        position_.y = base_y_position_ + (0.5 * (jump_height + (base_vel - (gravity * t_))) * t_);

        //timer is increemented here, 0.1 for each recorded frame
        t_ += delta_time;

        //distance from base y position to current y is calculated here
        float distance = position_.y - interpolation;
        
        //if the timer has been going for a while (to account for the initial push-off from the ground), and the distance
        //from the ground is small enough, set state to walking and reset timer to 0
        if (t_ >= 2.0 && abs(distance) <= 0.3)
        {
            tempPos.y = interpolation;
            //float slope = CalculateSlope(interpolation);
            t_ = 0.0;
            state_ = walking;

            /*if (slope >= 1.1 || x == 0 || x == height - 2 || z == 0 || z == width - 2)
            {
                std::cout << "Hello" << std::endl;
                position_.x = oldPos.x;
                position_.z = oldPos.z;
                position_.y = oldY;
                return;
            }*/
            position_ = tempPos;
        }
        else {
            position_ = position_ - (glm::vec3(0.0, 0.4f, 0.0) * delta_time);
        }        

        if (position_.y <= 3.0)
        {
            position_ = old_position_;
            state_ = walking;
            t_ = 0.0;
        }

    }
    else
    {
        if (abs(slope_map_[x + width * z]) >= 1.0 || x == 0 || x == height - 2 || z == 0 || z == width - 2)
        {
            return;
        }

        tempPos.y = interpolation;
        position_ = tempPos;
        oldY = interpolation;

    }

}
void Camera::UpdateForwardVelocity(float backwards)
{
    forward_speed_ = max_speed_ * backwards;
    /*if (forward_speed_ >= GetMaxSpeed())
    {
        forward_speed_ = max_speed_;
    }
    if (forward_speed_ <= GetMinSpeed()) {
        forward_speed_ = min_speed_;
    }*/
}

void Camera::UpdateSideVelocity(float left)
{
    side_speed_ = max_speed_ * left;
    /*if (side_speed_ >= GetMaxSpeed())
    {
        side_speed_ = max_speed_;
    }
    if (side_speed_ <= GetMinSpeed()) {
        side_speed_ = min_speed_;
    }*/
}

void Camera::SetView(glm::vec3 position, glm::vec3 look_at, glm::vec3 up) {
    // Store initial forward and side vectors
    // See slide in "Camera control" for details
    // probably will not be much used (directly setting view a rare occurrence in games)
    forward_ = look_at - position;
    forward_ = -glm::normalize(forward_);
    side_ = glm::cross(up, forward_);
    side_ = glm::normalize(side_);
    up_ = glm::normalize(up);

    movement_forward_ = glm::vec3(forward_.x, 0.0, forward_.z);
    movement_side_ = glm::cross(glm::vec3(0, 1, 0), movement_forward_);
    movement_side_ = glm::normalize(movement_side_);

    // Reset orientation and position of camera
    position_ = position;
    orientation_ = glm::quat();
    movement_orientation_ = glm::quat();
}

void Camera::SetProjection(GLfloat fov, GLfloat near, GLfloat far, GLfloat w, GLfloat h){
    // Set projection based on field-of-view
    float top = tan((fov/2.0)*(glm::pi<float>()/180.0))*near;
    float right = top * w/h;
    projection_matrix_ = glm::frustum(-right, right, -top, top, near, far);
}

void Camera::SetupShader(GLuint program){
    // Update view matrix
    SetupViewMatrix();

    // Set view matrix in shader
    GLint view_mat = glGetUniformLocation(program, "view_mat");
    glUniformMatrix4fv(view_mat, 1, GL_FALSE, glm::value_ptr(view_matrix_));
    
    // Set projection matrix in shader
    GLint projection_mat = glGetUniformLocation(program, "projection_mat");
    glUniformMatrix4fv(projection_mat, 1, GL_FALSE, glm::value_ptr(projection_matrix_));
}

void Camera::SetupViewMatrix(void){
    // Get current vectors of coordinate system
    // [side, up, forward]
    // See slide in "Camera control" for details
    glm::vec3 current_forward = orientation_ * forward_;
    glm::vec3 current_side = orientation_ * side_;
    glm::vec3 current_up = glm::cross(current_forward, current_side);
    current_up = glm::normalize(current_up);

    // Initialize the view matrix as an identity matrix
    view_matrix_ = glm::mat4(1.0); 

    // Copy vectors to matrix
    // Add vectors to rows, not columns of the matrix, so that we get
    // the inverse transformation
    // Note that in glm, the reference for matrix entries is of the form
    // matrix[column][row]
    view_matrix_[0][0] = current_side[0]; // First row
    view_matrix_[1][0] = current_side[1];
    view_matrix_[2][0] = current_side[2];
    view_matrix_[0][1] = current_up[0]; // Second row
    view_matrix_[1][1] = current_up[1];
    view_matrix_[2][1] = current_up[2];
    view_matrix_[0][2] = current_forward[0]; // Third row
    view_matrix_[1][2] = current_forward[1];
    view_matrix_[2][2] = current_forward[2];

    // Create translation to camera position
    glm::mat4 trans = glm::translate(glm::mat4(1.0), -position_);

    // Combine translation and view matrix in proper order
    view_matrix_ *= trans;
}
} // namespace game
