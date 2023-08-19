#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "opengl.h"

unsigned int loadBMP(const char *path)
{
    unsigned char header[54];
    unsigned int position;
    unsigned int width, height;
    unsigned int image_size;
    unsigned char *image;


    FILE *file = fopen(path, "rb");
    if ( !file )
    {
        printf("unable to open file %s\n", path);
        return 0;
    }

    if ( fread(header, 1, 54, file) != 54)
    {
        printf("Incorrect type of file, not BMP\n");
        return 0;
    }

    if ( header[0] == 'B' && header[1] == 'M' )
    {
        position    = *(int*)&(header[0x0A]);
        image_size  = *(int*)&(header[0x22]);
        width       = *(int*)&(header[0x12]);
        height      = *(int*)&(header[0x16]);

        // handling errors in misformatting
        if ( image_size == 0 )
        {
            image_size = width*height*3;
        }

        if ( position == 0 )
        {
            position = 54;
        }

        image = malloc(image_size * sizeof(unsigned char));

        fread(image, 1, image_size, file);

        fclose(file);

        unsigned int texture;
        glGenTextures(1, &texture);

        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, image);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        free(image);

        printf("COMPLETED ASSIGNING IMAGE\n");

        return texture;
    }
    else
    {
        printf("INCORRECT FILE FORMAT\n");
        return 0;
    }

    return 0;
}