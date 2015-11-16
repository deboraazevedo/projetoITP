#include <stdio.h>
#include <stdlib.h>

typedef struct Pixel {
    int r;
    int g;
    int b;
} Pixel;

typedef struct Imagem {
    int width;
    int height;
    Pixel** pixels;
} Imagem;

Imagem* criar_imagem(int width, int height) {
    int i;
    Imagem* nova_img;

    nova_img = malloc(sizeof(Imagem));

    nova_img->width = width;
    nova_img->height = height;

    nova_img->pixels =
        malloc(sizeof(Pixel*) * height);

    for (i = 0; i < nova_img->height; ++i) {
        nova_img->pixels[i] =
            malloc(sizeof(Pixel) * width);
    }

    return nova_img;
}

Imagem* ler_imagem(char* nome_arquivo) {
    char P3[3];
    int width;
    int height;
    int max_value;
    int i;
    int j;
    int r, g, b;
    FILE* arquivo;
    Imagem* img;

    arquivo = fopen(nome_arquivo, "r");

    fscanf(arquivo, "%s", P3);
    fscanf(arquivo, "%i %i", &width, &height);
    fscanf(arquivo, "%i", &max_value);

    img = criar_imagem(width, height);

    for (i = 0; i < height; ++i) {
        for (j = 0; j < width; ++j) {
            fscanf(arquivo, "%i %i %i", &r, &g, &b);
            img->pixels[i][j].r = r;
            img->pixels[i][j].g = g;
            img->pixels[i][j].b = b;
        }
    }

    fclose(arquivo);
    return img;
}


void salvar_imagem(Imagem* img, char* nome_arquivo) {
    int i;
    int j;
    int r;
    int g;
    int b;
    FILE* arquivo;

    arquivo = fopen(nome_arquivo, "w");

    fprintf(arquivo, "P3\n");
    fprintf(arquivo, "%i %i\n", img->width, img->height);
    fprintf(arquivo, "255\n");

    for (i = 0; i < img->height; ++i) {
        for (j = 0; j < img->width; ++j) {
            r = img->pixels[i][j].r;
            g = img->pixels[i][j].g;
            b = img->pixels[i][j].b;
            fprintf(arquivo, "%i %i %i\n", r, g, b);
        }
    }

    fclose(arquivo);
}

int sat(int x) {
    if (x < 0) {
        return 0;
    } else if (x > 255) {
        return 255;
    } else {
        return x;
    }
}

void binarizacao_imagem(Imagem* padrao, Imagem* copia, int limiar){
    int i;
    int j;
    int esc_C, a=0;

    for (i = 0; i < padrao->height; i++) {
        for (j = 0; j < padrao->width;j++) {
            esc_C = 0;

            esc_C = esc_C + padrao->pixels[i][j].r;
            esc_C = esc_C + padrao->pixels[i][j].g;
            esc_C = esc_C + padrao->pixels[i][j].b;

            esc_C = sat(esc_C/3);
            if(esc_C > limiar){
                a=000;
            }else{
                a=255;
            }

            copia->pixels[i][j].r = a;
            copia->pixels[i][j].g = a;
            copia->pixels[i][j].b = a;
        }
    }
}

int main() {
    Imagem* img;
    Imagem* cop;
    int i;
    int limiar=-1;

    img = ler_imagem("lena.ppm");
    cop = criar_imagem(img->width, img->height);

    while(limiar <0 || limiar >255){
        printf("Digite seu limiar: ");
        scanf("%i", &limiar);
    }

    binarizacao_imagem(img, cop, limiar);
    salvar_imagem(cop, "okokok.ppm");

    return 0;
}





