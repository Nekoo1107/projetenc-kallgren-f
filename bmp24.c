#include <stdio.h>
#include <stdint.h>
#include "bmp24.h"

#include <stdio.h>
#include <stdlib.h>

t_bmp24 *bmp24_loadImage(const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) return NULL;

    t_bmp24 *img = malloc(sizeof(t_bmp24));
    if (!img) {
        fclose(fp);
        return NULL;
    }

    fread(&(img->header.type), sizeof(uint16_t), 1, fp);
    fread(&(img->header.size), sizeof(uint32_t), 1, fp);
    fread(&(img->header.reserved1), sizeof(uint16_t), 1, fp);
    fread(&(img->header.reserved2), sizeof(uint16_t), 1, fp);
    fread(&(img->header.offset), sizeof(uint32_t), 1, fp);

    fread(&(img->header_info.size), sizeof(uint32_t), 1, fp);
    fread(&(img->header_info.width), sizeof(int32_t), 1, fp);
    fread(&(img->header_info.height), sizeof(int32_t), 1, fp);
    fread(&(img->header_info.planes), sizeof(uint16_t), 1, fp);
    fread(&(img->header_info.bits), sizeof(uint16_t), 1, fp);
    fread(&(img->header_info.compression), sizeof(uint32_t), 1, fp);
    fread(&(img->header_info.imagesize), sizeof(uint32_t), 1, fp);
    fread(&(img->header_info.xresolution), sizeof(int32_t), 1, fp);
    fread(&(img->header_info.yresolution), sizeof(int32_t), 1, fp);
    fread(&(img->header_info.ncolors), sizeof(uint32_t), 1, fp);
    fread(&(img->header_info.importantcolors), sizeof(uint32_t), 1, fp);

    img->width = img->header_info.width;
    img->height = img->header_info.height;
    img->colorDepth = img->header_info.bits;

    if (img->colorDepth != 24) {
        printf("Erreur : image non 24 bits (%d bits).\n", img->colorDepth);
        free(img);
        fclose(fp);
        return NULL;
    }

    img->data = malloc(img->height * sizeof(t_pixel *));
    if (!img->data) {
        free(img);
        fclose(fp);
        return NULL;
    }

    for (int y = 0; y < img->height; y++) {
        img->data[y] = malloc(img->width * sizeof(t_pixel));
        if (!img->data[y]) {
            for (int i = 0; i < y; i++) {
                free(img->data[i]);
            }
            free(img->data);
            free(img);
            fclose(fp);
            return NULL;
        }
    }

    fseek(fp, img->header.offset, SEEK_SET);

    int padding = (4 - (img->width * 3) % 4) % 4;

    for (int y = img->height - 1; y >= 0; y--) {
        for (int x = 0; x < img->width; x++) {
            fread(&(img->data[y][x].blue), 1, 1, fp);
            fread(&(img->data[y][x].green), 1, 1, fp);
            fread(&(img->data[y][x].red), 1, 1, fp);
        }
        fseek(fp, padding, SEEK_CUR);
    }

    fclose(fp);
    return img;
}

void bmp24_saveImage(const char *filename, t_bmp24 *img) {
    if (!img || !img->data) return;

    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        printf("Erreur ouverture fichier sauvegarde\n");
        return;
    }

    int padding = (4 - (img->width * 3) % 4) % 4;
    unsigned int newDataSize = (img->width * 3 + padding) * img->height;
    unsigned int newFileSize = 54 + newDataSize;

    uint16_t bfType = 0x4D42; // "BM"
    uint32_t bfSize = newFileSize;
    uint16_t bfReserved1 = 0;
    uint16_t bfReserved2 = 0;
    uint32_t bfOffBits = 54;

    uint32_t biSize = 40;
    int32_t biWidth = img->width;
    int32_t biHeight = img->height;
    uint16_t biPlanes = 1;
    uint16_t biBitCount = 24;
    uint32_t biCompression = 0;
    uint32_t biSizeImage = newDataSize;
    int32_t biXPelsPerMeter = 0;
    int32_t biYPelsPerMeter = 0;
    uint32_t biClrUsed = 0;
    uint32_t biClrImportant = 0;

    fwrite(&bfType, sizeof(uint16_t), 1, fp);
    fwrite(&bfSize, sizeof(uint32_t), 1, fp);
    fwrite(&bfReserved1, sizeof(uint16_t), 1, fp);
    fwrite(&bfReserved2, sizeof(uint16_t), 1, fp);
    fwrite(&bfOffBits, sizeof(uint32_t), 1, fp);

    fwrite(&biSize, sizeof(uint32_t), 1, fp);
    fwrite(&biWidth, sizeof(int32_t), 1, fp);
    fwrite(&biHeight, sizeof(int32_t), 1, fp);
    fwrite(&biPlanes, sizeof(uint16_t), 1, fp);
    fwrite(&biBitCount, sizeof(uint16_t), 1, fp);
    fwrite(&biCompression, sizeof(uint32_t), 1, fp);
    fwrite(&biSizeImage, sizeof(uint32_t), 1, fp);
    fwrite(&biXPelsPerMeter, sizeof(int32_t), 1, fp);
    fwrite(&biYPelsPerMeter, sizeof(int32_t), 1, fp);
    fwrite(&biClrUsed, sizeof(uint32_t), 1, fp);
    fwrite(&biClrImportant, sizeof(uint32_t), 1, fp);

    unsigned char pad[3] = {0, 0, 0};
    for (int y = img->height - 1; y >= 0; y--) {
        for (int x = 0; x < img->width; x++) {
            fwrite(&(img->data[y][x].blue), 1, 1, fp);
            fwrite(&(img->data[y][x].green), 1, 1, fp);
            fwrite(&(img->data[y][x].red), 1, 1, fp);
        }
        fwrite(pad, 1, padding, fp);
    }

    fclose(fp);
}
void bmp24_free(t_bmp24 *img) {
    if (!img) return;

    for (int y = 0; y < img->height; y++) {
        free(img->data[y]);
    }
    free(img->data);
    free(img);
}

void bmp24_negative(t_bmp24 *img) {
    if (!img) return;

    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            img->data[y][x].red = 255 - img->data[y][x].red;
            img->data[y][x].green = 255 - img->data[y][x].green;
            img->data[y][x].blue = 255 - img->data[y][x].blue;
        }
    }
}

void bmp24_grayscale(t_bmp24 *img) {
    if (!img || !img->data) return;

    for (int y = 0; y < img->height; y++) {
        if (!img->data[y]) continue;

        for (int x = 0; x < img->width; x++) {
            t_pixel *p = &img->data[y][x];
            uint8_t gray = (p->red + p->green + p->blue) / 3;
            p->red = p->green = p->blue = gray;
        }
    }
}



void bmp24_brightness(t_bmp24 *img, int value) {
    if (!img) return;

    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            int r = img->data[y][x].red + value;
            int g = img->data[y][x].green + value;
            int b = img->data[y][x].blue + value;

            if (r > 255) r = 255;
            if (g > 255) g = 255;
            if (b > 255) b = 255;
            if (r < 0) r = 0;
            if (g < 0) g = 0;
            if (b < 0) b = 0;

            img->data[y][x].red = (uint8_t)r;
            img->data[y][x].green = (uint8_t)g;
            img->data[y][x].blue = (uint8_t)b;
        }
    }
}
