#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <math.h>

#define WIDTH 1000
#define HEIGHT 1000
#define STRMAX 256
#define min(a,b) (((a) < (b)) ? (a) : (b))
struct camera {
    double x;
    double y;
    double height;
    double width;
};

struct render {
    double xMin;
    double xMax;

    double yMin;
    double yMax;

    struct camera pov;

    int height;
    int width;

    int maxIter;
    int radius;

    struct pixdiv *img;
    char basename[STRMAX];

};

struct pixdiv
{
    int iter;
    double x;
    double y;
};

double map(int v, int imin, int imax, double omin, double omax);
int render_init(struct render *set, int argc, char *argv[]);
void cam2rect(struct render *set, struct camera *pov);
void render_image(struct render *set);
int save_image_bw(struct render *set);
int save_image_alt(struct render *set);
int save_image_grey(struct render *set);
int save_image_grey_smoothed(struct render *set);
void print_render(struct render *set);

int main(int argc, char *argv[])
{       
    int error;
    struct render set;
    error = render_init(&set, argc, argv);
    if (error == 1){
        return 1;
    }
    render_image(&set);
    error = save_image_grey_smoothed(&set);
    return error;
}

double map(int v, int imin, int imax, double omin, double omax)
{
    return (double)(v - imin) / (imax - imin) * (omax - omin) + omin;
}

int endsWith(const char *str, const char *suffix) {
    const char *s = str;
    const char *suf = suffix;

    while (*s) s++;
    while (*suf) suf++;

    
    while (s >= str && suf >= suffix) {
        if (*s != *suf) return 0;
        s--;
        suf--;
    }

    return suf < suffix;
}

int render_init(struct render *set, int argc, char *argv[]){
    int iter = 100;
    double x = -0.76;
    double y = 0.0;
    double width_cam = 2.48;
    double height_cam = 2.48;
    int width_im = WIDTH;
    int height_im = HEIGHT;
    char basename[STRMAX];
    char *temp;
    char opt;
    double tab[20];
    int i = 0;
    struct camera pov;
    size_t length;

    strcpy(basename, "mandel");

    while ((opt = getopt(argc, argv, "s:n:f:c:")) != -1)
    {
        switch (opt)
        {
        case 's':
            i=0;
            temp = strtok(optarg, "x");
            while (temp != NULL){
                tab[i] = atol(temp);
                temp = strtok(NULL, "x");
                i++;
            }
            width_im = tab[0];
            height_im = tab[1];
            break;
        case 'n':
            iter = atol(optarg);
            break;
        case 'f':
            length = strlen(optarg);
            if (length>STRMAX){
                printf("Provided name for file is too long.");
                return 2;
            }
            strcpy(basename,optarg);
            break;
        case 'c':
            i = 0;
            temp = strtok(optarg, ",");
            while (temp != NULL && i<20){
                tab[i] = atof(temp);
                temp = strtok(NULL, ",");
                i++;
            }
            x = tab[0];
            y = tab[1];
            width_cam = tab[2];
            height_cam = tab[3];
            break;
        default:
            printf("Hello\n");
            break;
        }
    }
    
    pov.x = x;
    pov.y = y;
    pov.height = height_cam;
    pov.width = width_cam;
    set->pov = pov;

    cam2rect(set, &set->pov);

    set->height = height_im;
    set->width = width_im;
    set->maxIter = iter;
    set->radius = 2;
    set->img = malloc(set->height * set->width * sizeof(struct pixdiv));
    if (set->img == NULL) {
        fprintf(stderr, "Erreur : Allocation dynamique échouée pour img.\n");
        return 1;
    }

    strcpy(set->basename, basename);
    return 0;
}

void cam2rect(struct render *set, struct camera *pov){
    set->xMin = pov->x - pov->width/2;
    set->xMax = pov->x + pov->width/2;
    set->yMin = pov->y - pov->height/2;
    set->yMax = pov->y + pov->height/2;
}

void render_image(struct render *set){
    int px, py;
    struct pixdiv res;
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
            for(int _ = 0; _ <4; _++){
                temp = xn * xn - yn * yn + x0;
                yn = 2 * xn * yn + y0;
                xn = temp;
            }
            res.iter = iter;
            res.x = xn;
            res.y = yn;
            set->img[py*set->width + px] = res;
        }
    }
}

int save_image_bw(struct render *set){
    int px, py;
    int i;
    int iter;

    FILE *fout;
    char name[STRMAX];
    char *ext = ".pbm";

    strcpy(name, set->basename);
    if (endsWith(name, ext) != 1){
        strcat(name, ".pbm");
    }
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
            fprintf(fout, "%d ", set->img[py*set->width + px].iter == set->maxIter ? 0 : 1);
            if (iter++ == 70) {
                fprintf(fout, "\n");
                iter = 1;
            }
        }
    }

    fclose(fout);

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
            fprintf(fout, "%d ", set->img[py*set->width + px].iter == set->maxIter || set->img[py*set->width + px].iter%2 == 1 ? 0 : 1);
            if (iter++ == 70) {
                fprintf(fout, "\n");
                iter = 1;
            }
        }
    }

    fclose(fout);

    free(set->img);
    return 0;
}

int save_image_grey(struct render *set){
    int px, py;
    int i;
    int iter;

    FILE *fout;
    char name[STRMAX];
    char *ext = ".pgm";

    strcpy(name, set->basename);
    if (endsWith(name, ext) != 1){
        strcat(name, ".pgm");
    }
    fout = fopen(name, "w");
    if (fout == NULL) {
        fprintf(stderr, "Erreur : Impossible d'ouvrir le fichier.\n");
        return 1;
    }
    fprintf(fout, "P2\n");
    fprintf(fout, "#Image calculée avec %d itérations maximal.\n", set->maxIter);
    fprintf(fout, "#Visualisé dans l'intervale [%f, %f] en x et [%f, %f] en y\n", set->xMin, set->xMax, set->yMin, set->yMax);
    fprintf(fout, "%d %d\n", set->width, set->height);
    fprintf(fout, "255\n");

    iter = 1;
    for (py = 0; py < set->width; py++) {
        for (px = 0; px < set->width; px++) {
            fprintf(fout, "%d ", (int) map(set->img[py*set->width + px].iter, 0, set->maxIter, 0.0, 255.0));
            if (iter++ == 70) {
                fprintf(fout, "\n");
                iter = 1;
            }
        }
    }

    fclose(fout);

    free(set->img);
    return 0;
}

int save_image_grey_smoothed(struct render *set){
    int px, py;
    int i;
    double grey;
    double x;
    double y;
    int iter;

    FILE *fout;
    char name[STRMAX];
    char *ext = ".pgm";

    strcpy(name, set->basename);
    if (endsWith(name, ext) != 1){
        strcat(name, ".pgm");
    }
    fout = fopen(name, "w");
    if (fout == NULL) {
        fprintf(stderr, "Erreur : Impossible d'ouvrir le fichier.\n");
        return 1;
    }
    fprintf(fout, "P2\n");
    fprintf(fout, "#Image calculée avec %d itérations maximal.\n", set->maxIter);
    fprintf(fout, "#Visualisé dans l'intervale [%f, %f] en x et [%f, %f] en y\n", set->xMin, set->xMax, set->yMin, set->yMax);
    fprintf(fout, "%d %d\n", set->width, set->height);
    fprintf(fout, "255\n");

    i = 1;
    for (py = 0; py < set->width; py++) {
        for (px = 0; px < set->width; px++) {
            if (set->img[py*set->width + px].iter == set->maxIter){
                fprintf(fout, "255 ");
            }
            else{
                x = set->img[py*set->width + px].x;
                y = set->img[py*set->width + px].y;
                iter = set->img[py*set->width + px].iter;
                grey = 5 + iter - log(log(x*x + y*y)/log(2))/log(2);
                grey = min(floor(512*grey/set->maxIter), 255);

                fprintf(fout,"%d ", (int) grey);
            }
            if (i++ == 70) {
                fprintf(fout, "\n");
                i = 1;
            }
        }
    }

    fclose(fout);

    free(set->img);
    return 0;
}

void print_render(struct render *set){
        printf(
        "Render:\n"
        "xMin = %.2f, xMax = %.2f, yMin = %.2f, yMax = %.2f\n"
        "Camera: x = %.2f, y = %.2f\n"
        "Height = %d, Width = %d\n"
        "Max Iterations = %d, Radius = %d\n"
        "Basename = %s\n",
        set->xMin, set->xMax,
        set->yMin, set->yMax,
        set->pov.x, set->pov.y,
        set->height, set->width,
        set->maxIter, set->radius,
        set->basename
    );
}
