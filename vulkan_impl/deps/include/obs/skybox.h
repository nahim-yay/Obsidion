
#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>


#include <jvk/vertex.h>
#include <jvk/cubemap.h>


// #include <glm/glm.hpp>

// struct Vertex
// {
    // glm::vec3 position;
    // glm::vec3 color;
    // glm::vec2 tex_coord;
    // glm::vec3 normal;
    // glm::vec3 tangent;
    // glm::vec3 bitangent;
// };


class Skybox 
{
public:

    unsigned int index = 0;

    CubeMap cubemap;


    // -Z, +Z, -X, +X, +Y, -Y
    const std::vector<Vertex> m_vertices =
    {
            // POSITIONS                // COLORS               // TEX_COORDS


            // FRONT
        {{-1.0f, -1.0f, -1.0f},     { 1.0f,  1.0f,  1.0f},      {0.0f, 0.0f}},
        {{ 1.0f, -1.0f, -1.0f},     { 0.0f,  0.0f,  0.0f},      {1.0f, 0.0f}},
        {{ 1.0f,  1.0f, -1.0f},     { 0.0f,  1.0f,  1.0f},      {1.0f, 1.0f}},
        {{ 1.0f,  1.0f, -1.0f},     { 0.0f,  1.0f,  1.0f},      {1.0f, 1.0f}},
        {{-1.0f,  1.0f, -1.0f},     { 1.0f,  0.0f,  0.0f},      {0.0f, 1.0f}},
        {{-1.0f, -1.0f, -1.0f},     { 1.0f,  1.0f,  1.0f},      {0.0f, 0.0f}},

            // BACK
        {{-1.0f, -1.0f,  1.0f},     { 0.0f,  0.0f,  1.0f},      {0.0f, 0.0f}},
        {{ 1.0f, -1.0f,  1.0f},     { 1.0f,  1.0f,  0.0f},      {1.0f, 0.0f}},
        {{ 1.0f,  1.0f,  1.0f},     { 1.0f,  0.0f,  1.0f},      {1.0f, 1.0f}},
        {{ 1.0f,  1.0f,  1.0f},     { 1.0f,  0.0f,  1.0f},      {1.0f, 1.0f}},
        {{-1.0f,  1.0f,  1.0f},     { 0.0f,  1.0f,  0.0f},      {0.0f, 1.0f}},
        {{-1.0f, -1.0f,  1.0f},     { 0.0f,  0.0f,  1.0f},      {0.0f, 0.0f}},

            // LEFT
        {{-1.0f,  1.0f,  1.0f},     { 0.0f,  1.0f,  0.0f},      {1.0f, 0.0f}},
        {{-1.0f,  1.0f, -1.0f},     { 1.0f,  0.0f,  0.0f},      {1.0f, 1.0f}},
        {{-1.0f, -1.0f, -1.0f},     { 1.0f,  1.0f,  1.0f},      {0.0f, 1.0f}},
        {{-1.0f, -1.0f, -1.0f},     { 1.0f,  1.0f,  1.0f},      {0.0f, 1.0f}},
        {{-1.0f, -1.0f,  1.0f},     { 0.0f,  0.0f,  1.0f},      {0.0f, 0.0f}},
        {{-1.0f,  1.0f,  1.0f},     { 0.0f,  1.0f,  0.0f},      {1.0f, 0.0f}},

            // RIGHT
        {{ 1.0f,  1.0f,  1.0f},     { 1.0f,  0.0f,  1.0f},      {1.0f, 0.0f}},
        {{ 1.0f,  1.0f, -1.0f},     { 0.0f,  1.0f,  1.0f},      {1.0f, 1.0f}},
        {{ 1.0f, -1.0f, -1.0f},     { 0.0f,  0.0f,  0.0f},      {0.0f, 1.0f}},
        {{ 1.0f, -1.0f, -1.0f},     { 0.0f,  0.0f,  0.0f},      {0.0f, 1.0f}},
        {{ 1.0f, -1.0f,  1.0f},     { 1.0f,  1.0f,  0.0f},      {0.0f, 0.0f}},
        {{ 1.0f,  1.0f,  1.0f},     { 1.0f,  0.0f,  1.0f},      {1.0f, 0.0f}},

            // TOP
        {{-1.0f, -1.0f, -1.0f},     { 1.0f,  0.0f,  0.0f},      {0.0f, 1.0f}},
        {{ 1.0f, -1.0f, -1.0f},     { 0.0f,  1.0f,  1.0f},      {1.0f, 1.0f}},
        {{ 1.0f, -1.0f,  1.0f},     { 1.0f,  0.0f,  1.0f},      {1.0f, 0.0f}},
        {{ 1.0f, -1.0f,  1.0f},     { 1.0f,  0.0f,  1.0f},      {1.0f, 0.0f}},
        {{-1.0f, -1.0f,  1.0f},     { 0.0f,  1.0f,  0.0f},      {0.0f, 0.0f}},
        {{-1.0f, -1.0f, -1.0f},     { 1.0f,  0.0f,  0.0f},      {0.0f, 1.0f}},

            // BUTTOM
        {{-1.0f,  1.0f, -1.0f},     { 1.0f,  1.0f,  1.0f},      {0.0f, 1.0f}},
        {{ 1.0f,  1.0f, -1.0f},     { 0.0f,  0.0f,  0.0f},      {1.0f, 1.0f}},
        {{ 1.0f,  1.0f,  1.0f},     { 1.0f,  1.0f,  0.0f},      {1.0f, 0.0f}},
        {{ 1.0f,  1.0f,  1.0f},     { 1.0f,  1.0f,  0.0f},      {1.0f, 0.0f}},
        {{-1.0f,  1.0f,  1.0f},     { 0.0f,  0.0f,  1.0f},      {0.0f, 0.0f}},
        {{-1.0f,  1.0f, -1.0f},     { 1.0f,  1.0f,  1.0f},      {0.0f, 1.0f}}
    };


    Skybox(CubeMap l_cubemap)
    {
        cubemap = l_cubemap;
    }

    ~Skybox()
    {

    }



    void push_data(std::vector<Vertex> &vertices)
    {

        // PUSH VERTICES TO JVK ENGINE
        for (size_t i = 0; i < m_vertices.size(); i++)
        {
            vertices.push_back(m_vertices[i]);
        }
    }
};