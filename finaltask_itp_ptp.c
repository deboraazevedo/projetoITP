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
    unsigned char r, g, b;
    FILE* arquivo;
    Imagem* img;

    arquivo = fopen(nome_arquivo, "r");

    fscanf(arquivo, "%s", P3);
    fscanf(arquivo, "%i %i", &width, &height);
    fscanf(arquivo, "%i", &max_value);
    img = criar_imagem(width, height);

    if (P3[1] == '3') {
        for (i = 0; i < height; ++i) {
            for (j = 0; j < width; ++j) {
                fscanf(arquivo, "%i %i %i", &r, &g, &b);
                img->pixels[i][j].r = r;
                img->pixels[i][j].g = g;
                img->pixels[i][j].b = b;
            }
        }
    } else if (P3[1] == '6') {
        fscanf(arquivo, "%c", &r);
        for (i = 0; i < height; ++i) {
            for (j = 0; j < width; ++j) {
                fscanf(arquivo, "%c%c%c", &r, &g, &b);
                img->pixels[i][j].r = (unsigned int) r;
                img->pixels[i][j].g = (unsigned int) g;
                img->pixels[i][j].b = (unsigned int) b;
            }
        }
    } else {
        printf("Erro: formato de imagem desconhecida\n");
        return NULL;
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

Imagem* binarizacao_imagem(Imagem* padrao, int limiar) {
    Imagem* copia = criar_imagem(padrao->width, padrao->height);
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

    return copia;
}

void calculo_blur(Imagem* cop, Imagem* img, int i, int j) {
    int acr, acb, acg;
    int k;
    int m;
    int ii;
    int jj;
    int produto;
    float filtro[3][3];

    filtro[0][0] = 1/9.0;
    filtro[0][1] = 1/9.0;
    filtro[0][2] = 1/9.0;

    filtro[1][0] = 1/9.0;
    filtro[1][1] = 1/9.0;
    filtro[1][2] = 1/9.0;

    filtro[2][0] = 1/9.0;
    filtro[2][1] = 1/9.0;
    filtro[2][2] = 1/9.0;

    acr = 0;
    acg = 0;
    acb = 0;

    k = i - 1;
    ii = 0;

    while (k <= i + 1) {
       m = j - 1;
       jj = 0;

       while (m <= j + 1) {
         acr += img->pixels[k][m].r * filtro[ii][jj];
         acg += img->pixels[k][m].g * filtro[ii][jj];
         acb += img->pixels[k][m].b * filtro[ii][jj];
         ++m;
         ++jj;
       }

       ++k;
       ++ii;
    }

    cop->pixels[i][j].r = sat(acr);
    cop->pixels[i][j].g = sat(acg);
    cop->pixels[i][j].b = sat(acb);
}

void calculo_sharpen(Imagem* cop, Imagem* img, int i, int j) {
    int acr, acb, acg;
    int k;
    int m;
    int ii;
    int jj;
    int produto;
    float filtro[3][3];

    filtro[0][0] = 0;
    filtro[0][1] = -1;
    filtro[0][2] = 0;

    filtro[1][0] = -1;
    filtro[1][1] = 5;
    filtro[1][2] = -1;

    filtro[2][0] = 0;
    filtro[2][1] = -1;
    filtro[2][2] = 0;

    acr = 0;
    acg = 0;
    acb = 0;

    k = i - 1;
    ii = 0;

    while (k <= i + 1) {
       m = j - 1;
       jj = 0;

       while (m <= j + 1) {
         acr += img->pixels[k][m].r * filtro[ii][jj];
         acg += img->pixels[k][m].g * filtro[ii][jj];
         acb += img->pixels[k][m].b * filtro[ii][jj];
         ++m;
         ++jj;
       }

       ++k;
       ++ii;
    }

    cop->pixels[i][j].r = sat(acr);
    cop->pixels[i][j].g = sat(acg);
    cop->pixels[i][j].b = sat(acb);
}

void calculo_borda(Imagem* cop, Imagem* img, int i, int j) {
    int acr, acb, acg;
    int k;
    int m;
    int ii;
    int jj;
    int produto;
    float filtro[3][3];

    filtro[0][0] = -1;
    filtro[0][1] = -1;
    filtro[0][2] = -1;

    filtro[1][0] = -1;
    filtro[1][1] = 8;

    filtro[1][2] = -1;

    filtro[2][0] = -1;
    filtro[2][1] = -1;
    filtro[2][2] = -1;

    acr = 0;
    acg = 0;
    acb = 0;

    k = i - 1;
    ii = 0;

    while (k <= i + 1) {
       m = j - 1;
       jj = 0;

       while (m <= j + 1) {
         acr += img->pixels[k][m].r * filtro[ii][jj];
         acg += img->pixels[k][m].g * filtro[ii][jj];
         acb += img->pixels[k][m].b * filtro[ii][jj];
         ++m;
         ++jj;
       }

       ++k;
       ++ii;
    }

    cop->pixels[i][j].r = sat(acr);
    cop->pixels[i][j].g = sat(acg);
    cop->pixels[i][j].b = sat(acb);
}

Imagem* blur(Imagem* original) {
    Imagem* modificada = criar_imagem(original->width, original->height);
    int width;
    int height;
    int i;
    int j;
    int k;
    int m;
    int r;
    int g;
    int b;
    int acc;

    height = original->height;
    width = original->width;

    for (i = 1; i < height - 1; ++i) {
        for (j = 1; j < width - 1; ++j) {
            calculo_blur(modificada, original, i, j);
        }
    }

    return modificada;
}

Imagem* sharpen(Imagem* original) {
    Imagem* modificada = criar_imagem(original->width, original->height);
    int width;
    int height;
    int i;
    int j;
    int k;
    int m;
    int r;
    int g;
    int b;
    int acc;

    height = original->height;
    width = original->width;

    for (i = 1; i < height - 1; ++i) {
        for (j = 1; j < width - 1; ++j) {
            calculo_sharpen(modificada, original, i, j);
        }
    }

    return modificada;
}

Imagem* borda(Imagem* original) {
    Imagem* modificada = criar_imagem(original->width, original->height);
    int width;
    int height;
    int i;
    int j;
    int k;
    int m;
    int r;
    int g;
    int b;
    int acc;

    height = original->height;
    width = original->width;

    for (i = 1; i < height - 1; ++i) {
        for (j = 1; j < width - 1; ++j) {
            calculo_borda(modificada, original, i, j);
        }
    }

    return modificada;
}




void calculo_gaussianblur(Imagem* cop, Imagem* img, int i, int j) {
    int acr, acb, acg;
    int k;
    int m;
    int ii;
    int jj;
    int produto;
    float filtro[3][3];

    filtro[0][0] = 1/16.0 * 1.0;
    filtro[0][1] = 1/16.0 * 2.0;
    filtro[0][2] = 1/16.0 * 1.0;

    filtro[1][0] = 1/16.0 * 2.0;
    filtro[1][1] = 1/16.0 * 4.0;
    filtro[1][2] = 1/16.0 * 2.0;

    filtro[2][0] = 1/16.0 * 1.0;
    filtro[2][1] = 1/16.0 * 2.0;
    filtro[2][2] = 1/16.0 * 1.0;



    acr = 0;
    acg = 0;
    acb = 0;

    k = i - 1;
    ii = 0;

    while (k <= i + 1) {
       m = j - 1;
       jj = 0;

       while (m <= j + 1) {
         acr += img->pixels[k][m].r * filtro[ii][jj];
         acg += img->pixels[k][m].g * filtro[ii][jj];
         acb += img->pixels[k][m].b * filtro[ii][jj];
         ++m;
         ++jj;
       }

       ++k;
       ++ii;
    }

    cop->pixels[i][j].r = sat(acr);
    cop->pixels[i][j].g = sat(acg);
    cop->pixels[i][j].b = sat(acb);
}

Imagem* gaussianblur(Imagem* original) {
    Imagem* modificada = criar_imagem(original->width, original->height);
    int width;
    int height;
    int i;
    int j;
    int k;
    int m;
    int r;
    int g;
    int b;
    int acc;

    height = original->height;
    width = original->width;

    for (i = 1; i < height - 1; ++i) {
        for (j = 1; j < width - 1; ++j) {
            calculo_gaussianblur(modificada, original, i, j);
        }
    }

    return modificada;
}



Imagem* rotacionar90(Imagem* original) {
    int i;
    int j;
    int w = original->width;
    int h = original->height;

    Imagem* nova = criar_imagem(h, w);

    for (i = 0; i < h; ++i) {
        for (j = 0; j < w; ++j) {
            nova->pixels[j][(h - 1) - i] = original->pixels[i][j];
        }
    }

    return nova;
}






Imagem* rotacionar180(Imagem* original){
	return rotacionar90(rotacionar90(original));
}

Imagem* rotacionar270(Imagem* original){
	return rotacionar180(rotacionar90(original));
}


Pixel calculo_media(Pixel a, Pixel b) {
    Pixel tmp;

    tmp.r = (a.r + b.r) / 2;
    tmp.g = (a.g + b.g) / 2;
    tmp.b = (a.b + b.b) / 2;

    return tmp;
}

Pixel rgb(int r, int g, int b) {
    Pixel p;

    p.r = r;
    p.g = g;
    p.b = b;

    return p;
}

void limpar(Imagem* img, Pixel p) {
    int i;
    int j;

    for (i = 0; i < img->height; ++i) {
        for (j = 0; j < img->width; ++j) {
            img->pixels[i][j] = p;

        }
    }
}




Pixel calculo_media4(Pixel* a, Pixel* b, Pixel* c, Pixel* d) {
    Pixel tmp;

    tmp.r = (a->r + b->r + c->r + d->r) / 4;
    tmp.g = (a->g + b->g + c->g + d->g) / 4;
    tmp.b = (a->b + b->b + c->b + d->b) / 4;

    return tmp;
}

Imagem* zoom2x(Imagem* org) {
    int i;
    int j;
    Pixel p, a, b, d, e, c;
    Imagem* nova;

    nova = criar_imagem(org->width * 2, org->height * 2);
    limpar(nova, rgb(255, 255, 255));

    for (i = 0; i < org->height; ++i) {
        for (j = 0; j < org->width; ++j) {
            nova->pixels[i*2][j*2] = org->pixels[i][j];
        }
    }

    for (i = 0; i < org->height; ++i) {
        for (j = 0; j < org->width; ++j) {
            if (j+1 < org->width) {
                a = org->pixels[i][j];
                b = org->pixels[i][j+1];
                p = calculo_media(a, b);
            }

            if (j != 0) {
                nova->pixels[i*2][j*2-1] = p;
            }

            if (i+1 < org->height) {
                a = org->pixels[i][j];
                b = org->pixels[i+1][j];
                p = calculo_media(a, b);
            }

            if (i != 0) {
                nova->pixels[i*2-1][j*2] = p;
            }
        }
    }

    for (i = 0; i < nova->height - 2; i += 2) {
        for (j = 0; j < nova->width - 2; j += 2) {
            a = nova->pixels[i][j+1];
            b = nova->pixels[i+1][j];
            d = nova->pixels[i+1][j+2];
            e = nova->pixels[i+2][j+1];
            c = calculo_media4(&a, &b, &d, &e);
            nova->pixels[i+1][j+1] = c;
        }
    }

    return nova;
}



Imagem* zoom4x(Imagem* org) {
    zoom2x(zoom2x(org));
}



Imagem* reduzir2x(Imagem* org) {

    int i;
    int j;
    Pixel p, a, b, d, e, c;
    Imagem* nova;


    nova = criar_imagem(org->width / 2, org->height / 2);
    limpar(nova, rgb(255, 255, 255));

    for (i = 0; i < org->height - 2; i += 2) {
        for (j = 0; j < org->width - 2; j += 2) {
            a = org->pixels[i][j];
            b = org->pixels[i][j+1];
            c = org->pixels[i+1][j];
            d = org->pixels[i+1][j+1];
            p = calculo_media4(&a, &b, &c, &d);
            nova->pixels[i/2][j/2] = p;
        }
    }

    return nova;
}



int mostrar_menu() {
    int opcao;

    printf("1 - Ler arquivo\n");
    printf("2 - Blur\n");
    printf("3 - Sharpen\n");
    printf("4 - Deteccao de Borda\n");
    printf("5 - Binarizacao\n");
    printf("6 - Rotacionar\n");
    printf("7 - Zoom AUMENTAR\n");
    printf("8 - Zoom REDUZIR\n");
    printf("9 - Gaussian blur\n");
    printf("10 - Salvar\n");
    printf("0 - Sair\n");

    printf("Opcao: ");
    scanf("%i", &opcao);

    return opcao;
}

int main() {
    Imagem* img;
    int opcao = 1;
    int i=0,j=0;
    int limiar=-1;
    char nome[255];
    int grau=0;


    while (opcao != 0) {
        opcao = mostrar_menu();

        if (opcao == 1) {
            printf("Nome do arquivo: ");
            scanf("%s", nome);
            img = ler_imagem(nome);
        } else if (opcao == 2) {
            img = blur(img);
        } else if (opcao == 3) {
            img = sharpen(img);
    	} else if(opcao == 4){
			img = borda(img);
		}else if(opcao == 5){
		    printf("Digite o limiar: ");
            scanf("%i", &limiar);
            img = binarizacao_imagem(img, limiar);
		}else if(opcao == 6){
			printf("Grau de rotacao (90, 180 ou 270º): ");
			scanf("%i", &grau);
			if(grau == 90){
				img = rotacionar90(img);
			}else if(grau == 180){
				img = rotacionar180(img);
			}else if(grau == 270){
				img = rotacionar270(img);
			}else{
				printf("O grau informado nao e permitido.");
			}
		}else if(opcao == 7){
			img = zoom2x(img);
		}else if(opcao == 8){
			img = reduzir2x(img);
		}else if(opcao == 9){
			img = gaussianblur(img);
		}else if (opcao == 10){
		    printf("Nome do arquivo: ");
            scanf("%s", nome);
            salvar_imagem(img, nome);
        }else{
        	printf("Opção inválida.");
		}
    }

    return 0;
}

