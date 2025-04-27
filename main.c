
#include <stdio.h>
#include <stdlib.h>
#include "bmp8.h"
#include "bmp24.h"

float boxBlurKernel[3][3] = {
    {1.0f/9, 1.0f/9, 1.0f/9},
    {1.0f/9, 1.0f/9, 1.0f/9},
    {1.0f/9, 1.0f/9, 1.0f/9}
};

float gaussianBlurKernel[3][3] = {
    {1.0f/16, 2.0f/16, 1.0f/16},
    {2.0f/16, 4.0f/16, 2.0f/16},
    {1.0f/16, 2.0f/16, 1.0f/16}
};

float outlineKernel[3][3] = {
    {-1, -1, -1},
    {-1, 8, -1},
    {-1, -1, -1}
};

float embossKernel[3][3] = {
    {-2, -1, 0},
    {-1, 1, 1},
    {0, 1, 2}
};

float sharpenKernel[3][3] = {
    {0, -1, 0},
    {-1, 5, -1},
    {0, -1, 0}
};

int main() {
    int choix;
    printf("Menu Principal:\n");
    printf("1. Ouvrir une image 8 bits (niveaux de gris)\n");
    printf("2. Ouvrir une image 24 bits (couleur)\n");
    printf("3. Quitter\n");
    printf("> Votre choix : ");
    scanf("%d", &choix);

    if (choix == 1) {
        char chemin[256];
        printf("Chemin de l'image 8 bits : ");
        scanf("%s", chemin);

        t_bmp8 *image8 = bmp8_loadImage(chemin);
        if (!image8) {
            return 1;
        }

        bmp8_printInfo(image8);

        int traitement;
        printf("\nQue voulez-vous faire ?\n");
        printf("1. Appliquer un négatif\n");
        printf("2. Modifier la luminosité\n");
        printf("3. Appliquer un seuillage (threshold)\n");
        printf("4. Appliquer un filtre (Box Blur, etc.)\n");
        printf("Votre choix : ");
        scanf("%d", &traitement);

        switch (traitement) {
            case 1:
                bmp8_negative(image8);
                break;
            case 2: {
                int val;
                printf("Entrez la valeur de luminosité (-255 à 255) : ");
                scanf("%d", &val);
                bmp8_brightness(image8, val);
                break;
            }
            case 3: {
                int seuil;
                printf("Entrez la valeur du seuil (0 à 255) : ");
                scanf("%d", &seuil);
                bmp8_threshold(image8, seuil);
                break;
            }
            case 4: {
                int filtre;
                printf("\nChoisissez un filtre :\n");
                printf("1. Box Blur\n");
                printf("2. Gaussian Blur\n");
                printf("3. Outline\n");
                printf("4. Emboss\n");
                printf("5. Sharpen\n");
                printf("Votre choix : ");
                scanf("%d", &filtre);
                switch (filtre) {
                    case 1:
                        bmp8_applyFilter(image8, boxBlurKernel, 3);
                        break;
                    case 2:
                        bmp8_applyFilter(image8, gaussianBlurKernel, 3);
                        break;
                    case 3:
                        bmp8_applyFilter(image8, outlineKernel, 3);
                        break;
                    case 4:
                        bmp8_applyFilter(image8, embossKernel, 3);
                        break;
                    case 5:
                        bmp8_applyFilter(image8, sharpenKernel, 3);
                        break;
                    default:
                        printf("Choix invalide.\n");
                }
                break;
            }
            default:
                printf("Choix invalide.\n");
        }

        bmp8_saveImage("image8bitsmodifie.bmp", image8);
        bmp8_free(image8);

    } else if (choix == 2) {
        char chemin[256];
        printf("Chemin de l'image 24 bits : ");
        scanf("%s", chemin);

        t_bmp24 *image24 = bmp24_loadImage(chemin);
        if (!image24) {
            return 1;
        }

        int traitement;
        printf("\nQue voulez-vous faire ?\n");
        printf("1. Appliquer un négatif\n");
        printf("2. Passer en niveaux de gris\n");
        printf("3. Modifier la luminosité\n");
        printf("Votre choix : ");
        scanf("%d", &traitement);

        switch (traitement) {
            case 1:
                bmp24_negative(image24);
                break;
            case 2:
                bmp24_grayscale(image24);
                break;
            case 3: {
                int val;
                printf("Entrez la valeur de luminosité (-255 à 255) : ");
                scanf("%d", &val);
                bmp24_brightness(image24, val);
                break;
            }
            default:
                printf("Choix invalide.\n");
        }

        bmp24_saveImage("resulta_24bits.bmp", image24);
        bmp24_free(image24);

    } else {
        printf("Au revoir !\n");
    }

    return 0;
}
