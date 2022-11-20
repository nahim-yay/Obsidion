

#include "Bots.h"




Bots::Bots(unsigned int width, unsigned int height)
    : State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{}


void Bots::Init()
{

    // Initialise Projection Matrix
    projection = glm::perspective(glm::radians(60.0f), (float)Bots::Width / (float)Bots::Height, 0.01f, 1000.0f);

    light = new Light(glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));




    // Initialise skybox
    skybox = Skybox(view, projection, faces);

    floor = PlaneGround(shine_diffuse, shine_normal);
    floor.setMatrices(projection);
    floor.scale(100, 100, 100);

    dome = Dome(view, projection);
    dome.setMatrices(projection);
    dome.setColor(glm::vec4(0.5f, 0.0f, 0.0f, 1.0f));
    dome.setMaterial(glm::vec3(1.0f, 1.0f, 1.0f), 32.0f);

    monkey = new ObjModel("res/models/monkey3.obj", "res/textures/Suzanne_BaseColor.png");
    monkey->setMatrices(projection);
    monkey->rotate(0, 180, 0);
    // monkey->translate(0,20,0);

    player = Player(monkey);

    model = gltfModel(gltf_cube);
    model.setMatrices(projection);
    model.translate(0, 0, -5);
    // Initialise Camera
    camera = Camera(&player, 10, 3);
}


void Bots::Update(float dt, float uTime)
{
    // dt: milliseconds passed since last frame
    // uTime: milliseconds passed since window initialisation
    float deltaTime = dt/1000;    // seconds passed since last frame
    float fps = 1000/dt;          // frames per second


    // Set View Matrix
    view = camera.GetViewMatrix();    


    Input::Update();

    player.move(deltaTime);

    camera.update(uTime);

    cout << fps << " FPS" << "\n";
}

void Bots::Render(float dt, float uTime)
{
    glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    skybox.Draw(camera);

    floor.Draw(camera, light, ambience);

    // dome.Draw(camera, light, ambience);

    monkey->Draw(camera, light, ambience);

    model.draw(camera);
}







void Bots::ProcessInput(float dt)
    {}

Bots::~Bots()
{}









void Bots::InitPhysics()
    {}