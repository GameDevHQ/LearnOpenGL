#ifndef TEXT2D_H
#define TEXT2D_H

void initText2D(const char * texturePath,
                const char * vertex_filepath,
                const char * fragment_filepath,
                const char * uniform_id);
void printText2D(const char * text, int x, int y, int size);
void cleanupText2D();

#endif
