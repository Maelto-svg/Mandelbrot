#include <stdio.h>
#include <stdlib.h>

#define WIDTH 1000
#define HEIGHT 1000

double map(int v, int imin, int imax, double omin, double omax);

int main()
{
    int maxIter = 100;
    int iter;

    int radius = 2;

    int h = HEIGHT;
    int w = WIDTH;
    int px, py;

    double xMin = -2.0;
    double xMax = 0.48;
    double xn, x0;

    double yMin = -1.24;
    double yMax = 1.24;
    double yn, y0;
    double temp;

    FILE *fout;
    int **img = malloc(h * sizeof(int *));
    int i;
    if (img == NULL) {
        fprintf(stderr, "Erreur : Allocation dynamique échouée pour img.\n");
        return 1;
    }
    for (i = 0; i < h; i++) {
        img[i] = malloc(w * sizeof(int));
        if (img[i] == NULL) {
            fprintf(stderr, "Erreur : Allocation dynamique échouée pour img[%d].\n", i);
            return 1;
        }
    }

    for (py = 0; py < h; py++) {
        for (px = 0; px < w; px++) {
            x0 = map(px, 0, w, xMin, xMax);
            y0 = map(h - py, 0, h, yMin, yMax);
            xn = x0;
            yn = y0;
            iter = 0;

            while (xn * xn + yn * yn < radius * radius && iter < maxIter) {
                temp = xn * xn - yn * yn + x0;
                yn = 2 * xn * yn + y0;
                xn = temp;
                iter++;
            }
            img[py][px] = iter;
        }
    }

    fout = fopen("mandel.pbm", "w");
    if (fout == NULL) {
        fprintf(stderr, "Erreur : Impossible d'ouvrir le fichier.\n");
        return 1;
    }
    fprintf(fout, "P1\n");
    fprintf(fout, "#Image calculée avec %d itérations maximal.\n", maxIter);
    fprintf(fout, "#Visualisé dans l'intervale [%f, %f] en x et [%f, %f] en y\n", xMin, xMax, yMin, yMax);
    fprintf(fout, "%d %d\n", w, h);

    iter = 1;
    for (py = 0; py < h; py++) {
        for (px = 0; px < w; px++) {
            fprintf(fout, "%d ", img[py][px] == maxIter ? 0 : 1);
            if (iter++ == 70) {
                fprintf(fout, "\n");
                iter = 1;
            }
        }
    }

    fclose(fout);

    for (i = 0; i < h; i++) {
        free(img[i]);
    }
    free(img);

    return 0;
}

double map(int v, int imin, int imax, double omin, double omax)
{
    return (double)(v - imin) / (imax - imin) * (omax - omin) + omin;
}
