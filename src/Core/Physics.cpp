//
// Created by Andrea Pullia on 28/06/2026.
//

#include "Core/Physics.h"

#include <iostream>

Physics* Physics::instance{nullptr};
std::mutex Physics::mutex{};

Physics::Physics()
{
    // Collision configuration, to be used by the collision detection class
    // collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
    this->collisionConfiguration = std::make_unique<btDefaultCollisionConfiguration>();

    // default collision dispatcher (= collision detection method). For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
    this->dispatcher = std::make_unique<btCollisionDispatcher>(this->collisionConfiguration.get());

    // btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
    this->overlappingPairCache = std::make_unique<btDbvtBroadphase>();

    // we set a ODE solver, which considers forces, constraints, collisions etc., to calculate positions and rotations of the rigid bodies.
    // the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
    this->solver = std::make_unique<btSequentialImpulseConstraintSolver>();

    //  DynamicsWorld is the main class for the physical simulation
    this->dynamicsWorld = std::make_unique<btDiscreteDynamicsWorld>(this->dispatcher.get() ,this->overlappingPairCache.get(), this->solver.get(), this->collisionConfiguration.get());

    // we set the gravity force
    this->dynamicsWorld->setGravity(btVector3(0.0f,-9.82f,0.0f));

    std::cout << "Created Singleton Physics!";
}

Physics * Physics::GetInstance()
{
    std::lock_guard lock(mutex);

    if (instance == nullptr)
    {
        instance = new Physics();
    }

    return instance;
}

btRigidBody* Physics::createRigidBody(int type, glm::vec3 pos, glm::vec3 size, glm::vec3 rot, float m, float friction , float restitution)
{
    btCollisionShape* cShape = NULL;

    // we convert the glm vector to a Bullet vector
    btVector3 position = btVector3(pos.x,pos.y,pos.z);

    // we set a quaternion from the Euler angles passed as parameters
    btQuaternion rotation;
    rotation.setEuler(rot.x,rot.y,rot.z);

    // Box Collision shape
    if (type == BOX)
    {
        // we convert the glm vector to a Bullet vector
        btVector3 dim = btVector3(size.x,size.y,size.z);
        // BoxShape
        cShape = new btBoxShape(dim);
    }
    // Sphere Collision Shape (in this case we consider only the first component)
    else if (type == SPHERE)
        cShape = new btSphereShape(size.x);

    // we add this Collision Shape to the vector
    this->collisionShapes.push_back(cShape);

    // We set the initial transformations
    btTransform objTransform;
    objTransform.setIdentity();
    objTransform.setRotation(rotation);
    // we set the initial position (it must be equal to the position of the corresponding model of the scene)
    objTransform.setOrigin(position);

    // if objects has mass = 0 -> then it is static (it does not move and it is not subject to forces)
    btScalar mass = m;
    bool isDynamic = (mass != 0.0f);

    // if it is dynamic (mass > 0) then we calculates local inertia
    btVector3 localInertia(0.0f,0.0f,0.0f);
    if (isDynamic)
        cShape->calculateLocalInertia(mass,localInertia);

    // we initialize the Motion State of the object on the basis of the transformations
    // using the Motion State, the physical simulation will calculate the positions and rotations of the rigid body
    btDefaultMotionState* motionState = new btDefaultMotionState(objTransform);

    // we set the data structure for the rigid body
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,motionState,cShape,localInertia);
    // we set friction and restitution
    rbInfo.m_friction = friction;
    rbInfo.m_restitution = restitution;

    // if the Collision Shape is a sphere
    if (type == SPHERE){
        // the sphere touches the plane on the plane on a single point, and thus the friction between sphere and the plane does not work -> the sphere does not stop
        // to avoid the problem, we apply the rolling friction together with an angular damping (which applies a resistence during the rolling movement), in order to make the sphere to stop after a while
        rbInfo.m_angularDamping = 0.3f;
        rbInfo.m_rollingFriction = 0.3f;
    }

    // we create the rigid body
    btRigidBody* body = new btRigidBody(rbInfo);

    //add the body to the dynamics world
    this->dynamicsWorld->addRigidBody(body);

    // the function returns a pointer to the created rigid body
    // in a standard simulation (e.g., only objects falling), it is not needed to have a reference to a single rigid body, but in some cases (e.g., the application of an impulse), it is needed.
    return body;
}

void Physics::Clear()
{
    //we remove the rigid bodies from the dynamics world and delete them
    for (int i=this->dynamicsWorld->getNumCollisionObjects()-1; i>=0 ;i--)
    {
        // we remove all the Motion States
        btCollisionObject* obj = this->dynamicsWorld->getCollisionObjectArray()[i];
        // we upcast in order to use the methods of the main class RigidBody
        btRigidBody* body = btRigidBody::upcast(obj);
        if (body && body->getMotionState())
        {
            delete body->getMotionState();
        }
        this->dynamicsWorld->removeCollisionObject( obj );
        delete obj;
    }

    this->collisionShapes.clear();

    instance = nullptr;
}

Physics::~Physics()
{
    Clear();
}
