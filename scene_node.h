/*
    
    All nodes now have a vector of SceneNode pointers. This stores references to all their child nodes
    Nodes also have a pivot_ for orbits and SetParentTransf() for hierarchical transformations

*/

#ifndef SCENE_NODE_H_
#define SCENE_NODE_H_

#include <string>
#include <vector>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>


#include "resource.h"
#include "camera.h"

namespace game {

    // Class that manages one object in a scene 
    class SceneNode {

        public:

            typedef enum Type { Stem, Leaf } NodeType;
            // Create scene node from given resources
            SceneNode(const std::string name, const Resource *geometry, const Resource *material, int collision);

            // Destructor
            ~SceneNode();
            
            // Get name of node
            const std::string GetName(void) const;

            // Handle child nodes
            SceneNode* GetChild(int n) const;
            void AddChild(SceneNode* child);
            int GetChildCount(void) const;

            // Get node attributes
            glm::vec3 GetPosition(void) const;
            glm::quat GetOrientation(void) const;
            glm::vec3 GetScale(void) const;
            glm::vec3 GetPivot(void) const;
            std::vector<SceneNode*>::const_iterator begin() const;
            std::vector<SceneNode*>::const_iterator end() const;

            // Set node attributes
            void SetPosition(glm::vec3 position);
            void SetOrientation(glm::quat orientation);
            void SetScale(glm::vec3 scale);
            void SetCollision(int collision);
            void SetPivot(glm::vec3 pivot);
            void SetParentTransf(glm::mat4 transf);
            void SetType(Type type);
            
            // Perform transformations on node
            void Translate(glm::vec3 trans);
            void Rotate(glm::quat rot);
            void Orbit(glm::quat rot);
            void Scale(glm::vec3 scale);

            // Draw the node according to scene parameters in 'camera'
            // variable
            virtual void Draw(Camera *camera);

            // Update the node
            virtual void Update(Camera *camera);

            // OpenGL variables
            GLenum GetMode(void) const;
            GLuint GetArrayBuffer(void) const;
            GLuint GetElementArrayBuffer(void) const;
            GLsizei GetSize(void) const;
            GLuint GetMaterial(void) const;
            int GetCollision(void) const;

            // Setter for geometry to change models
            void SetGeometry(const Resource *geometry);

        private:
            std::string name_; // Name of the scene node
            std::vector<SceneNode*> children_;
            GLuint array_buffer_; // References to geometry: vertex and array buffers
            GLuint element_array_buffer_;
            GLenum mode_; // Type of geometry
            GLsizei size_; // Number of primitives in geometry
            GLuint material_; // Reference to shader program
            glm::vec3 position_; // Position of node
            glm::quat orientation_; // Orientation of node
            glm::mat4 orbit_; // orbit motion + rotation
            glm::vec3 scale_; // Scale of node
            int collision_; // Collision state of the node (0 = no collision, 1 = collidable, 2 = has been collided with)
            glm::vec3 pivot_; // the point at which the node orbits (locally)
            glm::mat4 parent_transf_ = glm::mat4(1.0f);
            Type t_; // for use in shader. Types allow for differentiation between stems and leaves

            // Set matrices that transform the node in a shader program
            virtual void SetupShader(GLuint program);

    }; // class SceneNode

} // namespace game

#endif // SCENE_NODE_H_
