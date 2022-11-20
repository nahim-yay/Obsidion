

#pragma once



//  Mathematics library
#include <glm/glm.hpp>


//  Engine Headers
#include <obs/Camera.h>
#include <obs/input.h>
#include <obs/geometry/PlaneGround.h>
#include <obs/geometry/Terrain.h>
#include <obs/geometry/Sphere.h>
#include <obs/geometry/Dome.h>
#include <obs/Skybox.h>
#include <obs/Light.h>
#include <obs/Shader.h>
#include <obs/ObjModel.h>
#include <obs/Player.h>
#include <obs/Object.h>
#include <obs/gltfModel.h>

#include "Game.h"


// Represents the current state of the game
enum GameState
{
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};


// Holds all game-related state and functionality.
// Combines all game-related data into a single class for
// easy access to each of the components and manageability.
class Bots
{
    public:

        float ambience = 0.5f;

        const char* shine_diffuse = "res/textures/shine_diffuse.jpg";
        const char* shine_normal = "res/textures/shine_normal.jpg";

        const char* gltf_cube = "gltf/skinning/Woman.gltf";

        // // TODO this is horribly inefficient. Fix this.
        Shader blurShader;
        Shader basicerShader;
        Shader sceneDrawShader;
        Shader simpleDepthShader;
        Shader playerShader;

        


        glm::mat4 view;
        glm::mat4 projection;


        // constructor/destructor
        Bots(unsigned int width, unsigned int height);
        ~Bots();

        // initialize game state (load all shaders/textures/levels)
        void Init();
        
        void InitPhysics();
        
        // game loop
        void ProcessInput(float dt);
        void Update(float dt, float uTime);
        void Render(float dt, float uTime);






        // game state
        GameState               State;

        bool                    Keys[1024];
        unsigned int            Width, Height;


        // Images that Make up the Skybox(Order is very important!)
        vector<std::string> faces
        {
            // X-AXIS
            "res/sky/right.jpg",
            "res/sky/left.jpg",

            // Y-AXIS
            "res/sky/top.jpg",
            "res/sky/bottom.jpg",

            // Z-AXIS
            "res/sky/front.jpg",
            "res/sky/back.jpg"
        };

        Skybox skybox;

        // camera
        Camera camera;

        Light *light;

        PlaneGround floor;
        Sphere sphere;
        Sphere lamp;
        Dome dome;

        ObjModel *monkey;
        Player player;


        gltfModel model;



protected:

};
