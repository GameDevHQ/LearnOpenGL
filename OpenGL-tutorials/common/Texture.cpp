#include "Texture.h"


GLuint loadBMP_custom(const char * imagepath)
{
    // Data read from the header of the BMP file
    unsigned char header[54]; // Each BMP file begins by a 54-bytes header
    unsigned int dataPos;     // Position in the file where the actual data begins
    unsigned int width, height;
    unsigned int imageSize;   // = width*height*3

    // Actual RGB data
    unsigned char * data;

    FILE * file = fopen(imagepath,"rb");
    if (!file)
    {
        std::cout << "Image could not be opened." << std::endl;
        return 0;
    }

    if ((fread(header, 1, 54, file) != 54) || (header[0] != 'B' || header[1] != 'M'))
    {
        std::cout << "Not a correct BMP file." << std::endl;
        return 0;
    }

    dataPos   = *(int*)&(header[0x0A]);
    imageSize = *(int*)&(header[0x22]);
    width     = *(int*)&(header[0x12]);
    height    = *(int*)&(header[0x16]);

    // Some BMP files are misformatted, guess missing information
    // 3: one byte for each Red, Green and Blue component
    if (imageSize == 0) imageSize = width * height * 3;
    // The BMP header is done that way
    if (dataPos == 0)   dataPos = 54;

    // Create a buffer
    data = new unsigned char [imageSize];

    // Read the actual data from the file into the buffer
    fread(data,1,imageSize,file);

    //Everything is in memory now, the file can be closed
    fclose(file);

    // Create one OpenGL texture
    GLuint textureID;
    glGenTextures(1, &textureID);

    // "Bind" the newly created texture: all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Give the image to OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    return textureID;
}


GLuint loadDDS(const char * imagepath) {

    unsigned char header[124];
    FILE *fp;

    // Try to open the file.
    fp = fopen(imagepath, "rb");
    if (fp == NULL)
        return 0;

    // Verify the type of file.
    char filecode[4];
    fread(filecode, 1, 4, fp);
    if (strncmp(filecode, "DDS ", 4) != 0) {
        fclose(fp);
        return 0;
    }

    // Get the surface desc.
    fread(&header, 124, 1, fp);

    unsigned int height      = *(unsigned int*)&(header[8 ]);
    unsigned int width       = *(unsigned int*)&(header[12]);
    unsigned int linearSize  = *(unsigned int*)&(header[16]);
    unsigned int mipMapCount = *(unsigned int*)&(header[24]);
    unsigned int fourCC      = *(unsigned int*)&(header[80]);

    unsigned char * buffer;
    unsigned int bufsize;
    // How big is it going to be including all mipmaps?
    bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;
    buffer = (unsigned char*)malloc(bufsize * sizeof(unsigned char));
    fread(buffer, 1, bufsize, fp);

    // Close the file pointer.
    fclose(fp);

    unsigned int components  = (fourCC == FOURCC_DXT1) ? 3 : 4;
    unsigned int format;
    switch(fourCC)
    {
        case FOURCC_DXT1:
            format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
            break;
        case FOURCC_DXT3:
            format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
            break;
        case FOURCC_DXT5:
            format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
            break;
        default:
            free(buffer);
            return 0;
    }

    // Create one OpenGL texture.
    GLuint textureID;
    glGenTextures(1, &textureID);

    // "Bind" the newly created texture: all future texture functions will modify this texture.
    glBindTexture(GL_TEXTURE_2D, textureID);

    unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
    unsigned int offset = 0;

    // Load the mipmaps.
    for (unsigned int level = 0; level < mipMapCount && (width || height); ++level)
    {
        if(width < 1)
            width = 1;
        if(height < 1)
            height = 1;

        unsigned int size = ((width+3)/4) * ((height+3) / 4) * blockSize;
        glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height, 0, size, buffer + offset);

        offset += size;
        width  /= 2;
        height /= 2;
    }
    free(buffer);

    return textureID;
}