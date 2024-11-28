#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WIDTH 1000
#define HEIGHT 1000
#define STRMAX 256

struct render {
    double xMin;
    double xMax;

    double yMin;
    double yMax;

    int height;
    int width;

    int maxIter;
    int radius;

    int **img;
    char basename[STRMAX];

};

double map(int v, int imin, int imax, double omin, double omax);
int render_init(struct render *set);
void render_image(struct render *set);
int save_image_bw(struct render *set);
int save_image_alt(struct render *set);

int main()
{       
    int error;
    struct render set;
    error = render_init(&set);
    if (error == 1){
        return 1;
    }
    render_image(&set);
    error = save_image(&set);
    return error;
}

double map(int v, int imin, int imax, double omin, double omax)
{
    return (double)(v - imin) / (imax - imin) * (omax - omin) + omin;
}

int render_init(struct render *set){
    int i;
    set->xMin = -2.0;
    set->xMax = 0.48;
    set->yMin = -1.24;
    set->yMax = 1.24;

    set->height = HEIGHT;
    set->width = WIDTH;
    set->maxIter = 100;
    set->radius = 2;
    set->img = malloc(set->height * sizeof(int *));
    if (set->img == NULL) {
        fprintf(stderr, "Erreur : Allocation dynamique échouée pour img.\n");
        return 1;
    }
    for (i = 0; i < set->height; i++) {
        set->img[i] = malloc(set->width * sizeof(int));
        if (set->img[i] == NULL) {
            fprintf(stderr, "Erreur : Allocation dynamique échouée pour img[%d].\n", i);
            return 1;
        }
    }
    strcpy(set->basename, "mandel");
    return 0;
}

void render_image(struct render *set){
    int px, py;

    double xn, x0;
    double yn, y0;
    double temp;

    int iter;
    for (py = 0; py < set->height; py++) {
        for (px = 0; px < set->width; px++) {
            printf("calcul de la ligne %d\r",py+1);

            x0 = map(px, 0, set->width, set->xMin, set->xMax);
            y0 = map(set->height - py, 0, set->height, set->yMin, set->yMax);
            xn = x0;
            yn = y0;
            iter = 0;

            while (xn * xn + yn * yn < set->radius * set->radius && iter < set->maxIter) {
                temp = xn * xn - yn * yn + x0;
                yn = 2 * xn * yn + y0;
                xn = temp;
                iter++;
            }
            set->img[py][px] = iter;
        }
    }
}

int save_image_bw(struct render *set){
    int px, py;
    int i;
    int iter;

    FILE *fout;
    char name[STRMAX];

    strcpy(name, set->basename);
    strcat(name, ".pbm");
    fout = fopen(name, "w");
    if (fout == NULL) {
        fprintf(stderr, "Erreur : Impossible d'ouvrir le fichier.\n");
        return 1;
    }
    fprintf(fout, "P1\n");
    fprintf(fout, "#Image calculée avec %d itérations maximal.\n", set->maxIter);
    fprintf(fout, "#Visualisé dans l'intervale [%f, %f] en x et [%f, %f] en y\n", set->xMin, set->xMax, set->yMin, set->yMax);
    fprintf(fout, "%d %d\n", set->width, set->height);

    iter = 1;
    for (py = 0; py < set->width; py++) {
        for (px = 0; px < set->width; px++) {
            fprintf(fout, "%d ", set->img[py][px] == set->maxIter ? 0 : 1);
            if (iter++ == 70) {
                fprintf(fout, "\n");
                iter = 1;
            }
        }
    }

    fclose(fout);

    for (i = 0; i < set->height; i++) {
        free(set->img[i]);
    }
    free(set->img);
    return 0;
}

int save_image_alt(struct render *set){
int px, py;
    int i;
    int iter;

    FILE *fout;
    char name[STRMAX];

    strcpy(name, set->basename);
    strcat(name, ".pbm");
    fout = fopen(name, "w");
    if (fout == NULL) {
        fprintf(stderr, "Erreur : Impossible d'ouvrir le fichier.\n");
        return 1;
    }
    fprintf(fout, "P1\n");
    fprintf(fout, "#Image calculée avec %d itérations maximal.\n", set->maxIter);
    fprintf(fout, "#Visualisé dans l'intervale [%f, %f] en x et [%f, %f] en y\n", set->xMin, set->xMax, set->yMin, set->yMax);
    fprintf(fout, "%d %d\n", set->width, set->height);

    iter = 1;
    for (py = 0; py < set->width; py++) {
        for (px = 0; px < set->width; px++) {
            fprintf(fout, "%d ", set->img[py][px] == set->maxIter || set->img[py][px]%2 == 1 ? 0 : 1);
            if (iter++ == 70) {
                fprintf(fout, "\n");
                iter = 1;
            }
        }
    }

    fclose(fout);

    for (i = 0; i < set->height; i++) {
        free(set->img[i]);
    }
    free(set->img);
    return 0;
}
