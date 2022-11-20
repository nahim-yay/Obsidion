
#pragma once

#include <iostream>
#include <vector>

#include <obs/mesh.h>
#include <obs/input.h>
#include <obs/camera_free.h>
// #include <obs/skybox.h>


class Application 
{
public:

    glm::mat4 view;
    glm::mat4 projection;


    // constructor/destructor
    Application(unsigned int width, unsigned int height);
    ~Application();

    // initialize 
    void Initialize();
    
    // game loop
    void Update(float dt, float u_time);


    std::vector<Vertex> vertices;
    std::vector<glm::mat4> mesh_data;
    unsigned int mesh_count;

    // std::vector<Vertex> sky_vertices;


    // Images that Make up the Skybox(Order is very important!)
    // char **faces = new char*[6]
    // {
    //     // X-AXIS
    //     "res/sky/right.jpg",
    //     "res/sky/left.jpg",

    //     // Y-AXIS
    //     "res/sky/top.jpg",
    //     "res/sky/bottom.jpg",

    //     // Z-AXIS
    //     "res/sky/front.jpg",
    //     "res/sky/back.jpg"
    // };


    // CubeMap cubemap = 
    // {
    //        nullptr, 0, 0, 0,

    //        faces
    // };





private:

    unsigned int SCREEN_WIDTH, SCREEN_HEIGHT;
    bool mouse_locked = false;

    Mesh *mesh;
    Mesh *second;
    Camera *camera;

    // Skybox *skybox;
};
