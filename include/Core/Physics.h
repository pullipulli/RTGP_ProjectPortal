/*
Physics class:
- initialization of the physics simulation using the Bullet library

The class sets up the collision manager and the resolver of the constraints, using basic general-purposes methods provided by the library. Advanced and multithread methods are available, please consult Bullet documentation and examples

createRigidBody method sets up a Box or Sphere Collision Shape. For other Shapes, you must extend the method.

author: Davide Gadia

Real-Time Graphics Programming - a.a. 2024/2025
Master degree in Computer Science
Universita' degli Studi di Milano
*/

#pragma once

#include <btBulletDynamicsCommon.h>
#include <memory>
#include <glm/glm.hpp>
#include <mutex>

//enum to identify the 2 considered Collision Shapes
enum shapes{ BOX, SPHERE};

///////////////////  Physics class ///////////////////////
class Physics
{
public:
    Physics(Physics &other) = delete;
    void operator=(const Physics& other) = delete;

    static Physics* GetInstance();

    std::unique_ptr<btDiscreteDynamicsWorld> dynamicsWorld; // the main physical simulation class
    btAlignedObjectArray<btCollisionShape*> collisionShapes; // a vector for all the Collision Shapes of the scene
    std::unique_ptr<btDefaultCollisionConfiguration> collisionConfiguration; // setup for the collision manager
    std::unique_ptr<btCollisionDispatcher> dispatcher; // collision manager
    std::unique_ptr<btBroadphaseInterface> overlappingPairCache; // method for the broadphase collision detection
    std::unique_ptr<btSequentialImpulseConstraintSolver> solver; // constraints solver

    //////////////////////////////////////////
    // Method for the creation of a rigid body, based on a Box or Sphere Collision Shape
    // The Collision Shape is a reference solid that approximates the shape of the actual object of the scene. The Physical simulation is applied to these solids, and the rotations and positions of these solids are used on the real models.
    btRigidBody* createRigidBody(int type, glm::vec3 pos, glm::vec3 size, glm::vec3 rot, float m, float friction , float restitution);

    //////////////////////////////////////////
    // We delete the data of the physical simulation when the program ends
    void Clear();
protected:
    Physics();
    ~Physics();
private:
    static Physics* instance;
    static std::mutex mutex;
};
