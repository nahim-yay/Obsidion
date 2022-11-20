
#pragma once



struct CubeMap
{
    void *texture_data[6];

    unsigned int width;
    unsigned int height;
    unsigned int channels;

    char **faces;
};