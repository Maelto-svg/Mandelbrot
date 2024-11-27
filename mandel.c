#include <stdio.h>

#define WIDTH 5000
#define HEIGHT 5000

double map (int v, int imin, int imax, double omin, double omax);

int main()
{
    int maxIter = 100;
    int iter;

    int radius = 2;

    int h = HEIGHT;
    int w = WIDTH;
    int px;
    int py;

    double xMin = -2.0;
    double xMax = 0.48;
    double xn;
    double x0;

    double yMin = -1.24;
    double yMax = 1.24;
    double yn;
    double y0;
    double temp;

    int img[HEIGHT][WIDTH];
    FILE * fout;

    for(py = 0; py<h; py++){
        for(px = 0; px<w; px++){
            x0 = map(px, 0, w, xMin, xMax);
            y0 = map(h-py, 0, h, yMin, yMax);
            xn = x0;
            yn = y0;
            iter = 0;

            while(xn*xn + yn*yn < radius && iter<maxIter){
                temp = xn*xn - yn*yn + x0;
                yn = 2*xn*yn +y0;
                xn = temp;
                iter++;
            }
            img[py][px] = iter;
        }
    }
    fout = fopen("mandel.pbm","w");
    if (fout == NULL){
        return 1;
    }
    fprintf(fout, "P1\n");
    fprintf(fout, "#Image calculé avec %d itérations maximal.\n", maxIter);
    fprintf(fout, "#Visualisé dans l'intervale [%f, %f] en x et [%f, %f] en y\n", xMin, xMax, yMin, yMax);
    fprintf(fout,"%d %d\n",WIDTH,HEIGHT);

    
    iter = 1;
    for(py = 0; py<h; py++){
        for(px = 0; px<w; px++){
            if (img[py][px] == maxIter){
                fprintf(fout, "0");
            }
            else{
                fprintf(fout, "1");
            }
            if (iter == 70){
                iter = 1;
                fprintf(fout, "\n");
            }
            else {
                fprintf(fout, " ");
                iter++;
            }
        }
    }

    fclose(fout);

    return 0;
}

double map (int v, int imin, int imax, double omin, double omax)
{
    return (double)(v-imin)/(imax-imin)*(omax-omin) + omin;
}