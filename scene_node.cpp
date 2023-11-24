#include <stdexcept>
#define GLM_FORCE_RADIANS
#include <iostream>
#include <time.h>

#include "scene_node.h"

namespace game {

SceneNode::SceneNode(const std::string name, const Resource *geometry, const Resource *material, const Resource* texture, int collision){

    // Set name of scene node
    name_ = name;

    // Set geometry
    if (geometry->GetType() == PointSet){
        mode_ = GL_POINTS;
    } else if (geometry->GetType() == Mesh){
        mode_ = GL_TRIANGLES;
    } else {
        throw(std::invalid_argument(std::string("Invalid type of geometry")));
    }

    array_buffer_ = geometry->GetArrayBuffer();
    element_array_buffer_ = geometry->GetElementArrayBuffer();
    size_ = geometry->GetSize();

    // Set material (shader program)
    if (material->GetType() != Material){
        throw(std::invalid_argument(std::string("Invalid type of material")));
    }

    material_ = material->GetResource();

    // Set texture
    if (texture) {
        texture_ = texture->GetResource();
    }
    else {
        texture_ = 0;
    }

    // Other attributes
    scale_ = glm::vec3(1.0, 1.0, 1.0);
    orbit_ = glm::mat4(1.0f);
    pivot_ = position_;
    collision_ = collision;
}


SceneNode::~SceneNode(){
}


const std::string SceneNode::GetName(void) const {
    return name_;
}

SceneNode* SceneNode::GetChild(int n) const {
    return children_.at(n);
};

void SceneNode::AddChild(SceneNode* child) {
    children_.push_back(child);
};

int SceneNode::GetChildCount(void) const {
    return children_.size();
}

glm::vec3 SceneNode::GetPosition(void) const {
    return position_;
}


glm::quat SceneNode::GetOrientation(void) const {
    return orientation_;
}


glm::vec3 SceneNode::GetScale(void) const {
    return scale_;
}

glm::vec3 SceneNode::GetPivot(void) const {
    return pivot_;
}

glm::vec3 SceneNode::GetColor(void) const
{
    return color_;
}

void SceneNode::SetPosition(glm::vec3 position){   
    pivot_ = pivot_ + (position - position_);
    position_ = position;
}

void SceneNode::SetOrientation(glm::quat orientation){ // Does not work yet, use Rotate instead
    orientation_ = orientation;
}


void SceneNode::SetScale(glm::vec3 scale){

    scale_ = scale;
    pivot_ *= scale;
}

void SceneNode::SetPivot(glm::vec3 pivot) {
    pivot_ = position_ + (pivot * orientation_); // Important note: SetPivot() already considers position / orientation, do not include them again when using this function.
}

void SceneNode::SetParentTransf(glm::mat4 transf) {
    parent_transf_ = transf;
}

void SceneNode::SetType(Type type) {
    t_ = type;
}

void SceneNode::SetColor(glm::vec3 color) {
    color_ = color;
}

SceneNode::Type SceneNode::GetType()
{
    return t_;
}
  
void SceneNode::SetTileCount(int count) {
    tile_count_ = count;
}

void SceneNode::SetLambertianCoefficient(float coefficient) {
    lambertian_coefficient_ = coefficient;
}

void SceneNode::SetSpecularCoefficient(float coefficient) {
    specular_coefficient_ = coefficient;
}

void SceneNode::SetSpecularPower(float power) {
    specular_power_ = power;
}

void SceneNode::SetAmbientLighting(float ambient) {
    ambient_lighting_ = ambient;
}

void SceneNode::Translate(glm::vec3 trans){
    position_ += trans;
    pivot_ += trans;
}

void SceneNode::Rotate(glm::quat rot){
    orientation_ *= rot;
    orientation_ = glm::normalize(orientation_);
    pivot_ = rot * pivot_ * -rot;
}

void SceneNode::Orbit(glm::quat rot) {
    glm::vec3 trans = glm::vec3(pivot_ - position_);
    glm::mat4 rot_mat = glm::mat4_cast(glm::normalize(rot));
    orbit_ *= glm::translate(glm::mat4(1.0f), trans) * rot_mat * glm::translate(glm::mat4(1.0f), -trans);
}


void SceneNode::Scale(glm::vec3 scale){

    scale_ *= scale;
    pivot_ *= scale;
}


GLenum SceneNode::GetMode(void) const {

    return mode_;
}


GLuint SceneNode::GetArrayBuffer(void) const {

    return array_buffer_;
}


GLuint SceneNode::GetElementArrayBuffer(void) const {

    return element_array_buffer_;
}


GLsizei SceneNode::GetSize(void) const {

    return size_;
}


GLuint SceneNode::GetMaterial(void) const {

    return material_;
}

void SceneNode::SetGeometry(const Resource *geometry) {
    // Set geometry
    if (geometry->GetType() == PointSet) {
        mode_ = GL_POINTS;
    }
    else if (geometry->GetType() == Mesh) {
        mode_ = GL_TRIANGLES;
    }
    else {
        throw(std::invalid_argument(std::string("Invalid type of geometry")));
    }

    array_buffer_ = geometry->GetArrayBuffer();
    element_array_buffer_ = geometry->GetElementArrayBuffer();
    size_ = geometry->GetSize();
}

void SceneNode::Draw(Camera *camera, SceneNode* light){

    // Select proper material (shader program)
    glUseProgram(material_);

    // Set geometry to draw
    glBindBuffer(GL_ARRAY_BUFFER, array_buffer_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_array_buffer_);

    // Set globals for camera
    camera->SetupShader(material_);

    // Set world matrix and other shader input variables
    SetupShader(material_, camera, light);

    // Draw geometry
    if (mode_ == GL_POINTS){
        glDrawArrays(mode_, 0, size_);
    } else {
        glDrawElements(mode_, size_, GL_UNSIGNED_INT, 0);
    }
}


void SceneNode::Update(Camera *camera){

    // Do nothing for this generic type of scene node
}


void SceneNode::SetupShader(GLuint program, Camera* camera, SceneNode* light){

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Set attributes for shaders
    GLint vertex_att = glGetAttribLocation(program, "vertex");
    glVertexAttribPointer(vertex_att, 3, GL_FLOAT, GL_FALSE, 11*sizeof(GLfloat), 0);
    glEnableVertexAttribArray(vertex_att);

    GLint normal_att = glGetAttribLocation(program, "normal");
    glVertexAttribPointer(normal_att, 3, GL_FLOAT, GL_FALSE, 11*sizeof(GLfloat), (void *) (3*sizeof(GLfloat)));
    glEnableVertexAttribArray(normal_att);

    GLint color_att = glGetAttribLocation(program, "color");
    glVertexAttribPointer(color_att, 3, GL_FLOAT, GL_FALSE, 11*sizeof(GLfloat), (void *) (6*sizeof(GLfloat)));
    glEnableVertexAttribArray(color_att);

    GLint tex_att = glGetAttribLocation(program, "uv");
    glVertexAttribPointer(tex_att, 2, GL_FLOAT, GL_FALSE, 11*sizeof(GLfloat), (void *) (9*sizeof(GLfloat)));
    glEnableVertexAttribArray(tex_att);

    // World transformation
    glm::mat4 scaling = glm::scale(glm::mat4(1.0), scale_);
    glm::mat4 rotation = glm::mat4_cast(orientation_);
    glm::mat4 orbit = orbit_;
    glm::mat4 translation = glm::translate(glm::mat4(1.0), position_);
    glm::mat4 transf = parent_transf_ * translation * orbit * rotation * scaling; // why this sequence?

    for (int i = 0; i < children_.size(); i++) {
        children_[i]->SetParentTransf(transf);
    }

    GLint world_mat = glGetUniformLocation(program, "world_mat");
    glUniformMatrix4fv(world_mat, 1, GL_FALSE, glm::value_ptr(transf));
    
    // Normal matrix
    glm::mat4 normal_matrix = glm::transpose(glm::inverse(transf));
    GLint normal_mat = glGetUniformLocation(program, "normal_mat");
    glUniformMatrix4fv(normal_mat, 1, GL_FALSE, glm::value_ptr(normal_matrix));
    
    // Texture
    if (texture_) {
        GLint tex = glGetUniformLocation(program, "texture_map");
        glUniform1i(tex, 0); // Assign the first texture to the map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_); // First texture we bind
        // Define texture interpolation
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
    }

    // Timer
    GLint timer_var = glGetUniformLocation(program, "timer");
    double current_time = glfwGetTime();
    glUniform1f(timer_var, (float) current_time);

    // Collision
    GLint collision_var = glGetUniformLocation(program, "collision");
    glUniform1i(collision_var, collision_);

    // Type of node
    GLint type_var = glGetUniformLocation(program, "node_type");
    glUniform1i(type_var, t_);

    // View position
    GLint view_pos_var = glGetUniformLocation(program, "view_pos");
    glUniform3fv(view_pos_var, 1, glm::value_ptr(camera->GetPosition()));

    // Light position
    GLint light_pos_var = glGetUniformLocation(program, "light_pos");
    glUniform3fv(light_pos_var, 1, glm::value_ptr(light->GetPosition()));

    // Object color
    GLint object_color_var = glGetUniformLocation(program, "object_color");
    glUniform3fv(object_color_var, 1, glm::value_ptr(color_));

    // Tile count
    GLint tile_count_var = glGetUniformLocation(program, "tile_count");
    glUniform1i(tile_count_var, tile_count_);

    // Lighting
    GLint lambertian_coefficient_var = glGetUniformLocation(program, "lambertian_coefficient");
    glUniform1f(lambertian_coefficient_var, lambertian_coefficient_);

    GLint specular_coefficient_var = glGetUniformLocation(program, "specular_coefficient");
    glUniform1f(specular_coefficient_var, specular_coefficient_);

    GLint specular_power_var = glGetUniformLocation(program, "specular_power");
    glUniform1f(specular_power_var, specular_power_);

    GLint ambient_lighting_var = glGetUniformLocation(program, "ambient_lighting");
    glUniform1f(ambient_lighting_var, ambient_lighting_);
}

int SceneNode::GetCollision(void) const {
    return collision_;
}
void SceneNode::SetCollision(int collision) {
    collision_ = collision;
}

float SceneNode::GetRadius(void) const {
    return radius_;
}

void SceneNode::SetRadius(float r)
{
    radius_ = r;
}

} // namespace game;
