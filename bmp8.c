#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmp8.h"

t_bmp8 *bmp8_loadImage(const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        printf("Erreur : impossible d'ouvrir %s\n", filename);
        return NULL;
    }

    t_bmp8 *img = malloc(sizeof(t_bmp8));
    if (!img) {
        fclose(fp);
        printf("Erreur : échec d'allocation mémoire pour l'image.\n");
        return NULL;
    }


    fread(img->header, 1, 54, fp);


    img->width = *(unsigned int*)&img->header[18];
    img->height = *(unsigned int*)&img->header[22];
    img->colorDepth = *(unsigned short*)&img->header[28];
    img->dataSize = *(unsigned int*)&img->header[34];


    if (img->colorDepth != 8) {
        printf("Erreur : format d'image non supporté (%d bits).\n", img->colorDepth);
        bmp8_free(img);
        fclose(fp);
        return NULL;
    }

    fread(img->colorTable, 1, 1024, fp);

    int padding = 0;
    if (img->width % 4 != 0) {
        padding = 4 - (img->width % 4);
    }

    img->data = malloc(img->width * img->height);
    if (!img->data) {
        printf("Erreur : échec d'allocation mémoire pour les pixels.\n");
        bmp8_free(img);
        fclose(fp);
        return NULL;
    }

    for (int y = 0; y < img->height; y++) {
        fread(img->data + (y * img->width), 1, img->width, fp);
        fseek(fp, padding, SEEK_CUR);
    }

    fclose(fp);
    printf("Image %s chargée avec succès.\n", filename);
    return img;
}

void bmp8_saveImage(const char *filename, t_bmp8 *img) {
    if (!img) {
        printf("Erreur : image invalide.\n");
        return;
    }

    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        printf("Erreur : impossible de créer %s\n", filename);
        return;
    }


    int padding = 0;
    if (img->width % 4 != 0) {
        padding = 4 - (img->width % 4);
    }
    unsigned int newDataSize = (img->width + padding) * img->height;


    *(unsigned int*)&img->header[2] = 54 + 1024 + newDataSize;
    *(unsigned int*)&img->header[34] = newDataSize;
    *(unsigned int*)&img->header[18] = img->width;
    *(unsigned int*)&img->header[22] = img->height;


    fwrite(img->header, 1, 54, fp);
    fwrite(img->colorTable, 1, 1024, fp);


    unsigned char paddingBytes[4] = {0};
    for (int y = 0; y < img->height; y++) {
        fwrite(img->data + (y * img->width), 1, img->width, fp);
        fwrite(paddingBytes, 1, padding, fp); // Ajout du padding
    }

    fclose(fp);
    printf("Image sauvegardée sous %s\n", filename);
}

void bmp8_free(t_bmp8 *img) {
    if (!img) {
        return;
    }

    if (img->data) {
        free(img->data);
    }

    free(img);
    printf("Mémoire de l'image libérée.\n");
}

void bmp8_printInfo(t_bmp8 *img) {
    if (!img) {
        printf("Erreur : image invalide.\n");
        return;
    }

    printf("Informations de l'image :\n");
    printf("- Dimensions : %u x %u\n", img->width, img->height);
    printf("- Taille des données : %u octets\n", img->dataSize);
    printf("- Profondeur de couleur : %u bits\n", img->colorDepth);
}

void bmp8_negative(t_bmp8 *img) {
    if (!img) {
        printf("Erreur : image invalide.\n");
        return;
    }
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            int index = y * img->width + x;
            img->data[index] = 255 - img->data[index];
        }
    }
}
void bmp8_brightness(t_bmp8 *img, int value) {
    if (!img) {
        printf("Erreur : image invalide.\n");
        return;
    }

    if (!img->data) {
        printf("Erreur : données image invalides.\n");
        return;
    }

    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            int index = y * img->width + x;
 int newValue = img->data[index] + value;

            if (newValue > 255) {
                newValue = 255;
            } else if (newValue < 0) {
                newValue = 0;
            }

            img->data[index] = (unsigned char)newValue;
        }
    }
}
void bmp8_threshold(t_bmp8 *img, int threshold) {
    if (!img) {
        printf("Erreur : image invalide.\n");
        return;
    }

    if (!img->data) {
        printf("Erreur : données image invalides.\n");
        return;
    }

    for (int y = 0; y < img->height; y++) {
        for  (int x = 0; x < img->width; x++) {
            int index = y * img->width + x;

            if (img->data[index] >= threshold) {
                img->data[index] = 255;
            } else {
                img->data[index] = 0;
            }
        }
    }
}
void bmp8_applyFilter(t_bmp8 *img, float kernel[3][3], int kernelSize) {
    if (!img) {
        return;
    }

    if (!img->data) {
        return;
    }

     int n = kernelSize / 2;
    int width = img->width;
     int height = img->height;

     unsigned char *newData = malloc(width * height);
    if (newData == NULL) {
        return;
    }

     for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
             float sum = 0.0f;

            for (int j = -n; j <= n; j++) {
                 for (int i = -n; i <= n; i++) {
                    int pixelValue = img->data[(y + j) * width + (x + i)];
                    float kernelValue = kernel[j + n][i + n];
                    sum += pixelValue * kernelValue;
                }
            }

              if (sum < 0) {
                sum = 0;
            }
            if (sum > 255) {
                sum = 255;
            }

            newData[y * width + x] = (unsigned char)sum;
        }
    }

         for (int y = 1; y < height - 1; y++) {
                for (int x = 1; x < width - 1; x++) {
            img->data[y * width + x] = newData[y * width + x];
        }
    }

    free(newData);
}

