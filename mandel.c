#include <stdio.h>

double map (int v, int imin, int imax, double omin, double omax);

int main()
{
    int maxIter = 100;
    int iter;
    int radius = 2;
    int h = 50;
    int w = 160;
    int px;
    int py;
    double xn;
    double x0;
    double yn;
    double y0;
    double temp;

    for(py = 0; py<h; py++){
        for(px = 0; px<w; px++){
            x0 = map(px, 0, w, -2.0, 0.48);
            y0 = map(h-py, 0, h, -1.24, 1.24);
            xn = x0;
            yn = y0;
            iter = 0;

            while(xn*xn + yn*yn < radius && iter<maxIter){
                temp = xn*xn - yn*yn + x0;
                yn = 2*xn*yn +y0;
                xn = temp;
                iter++;
            }
            if (iter == maxIter){
                printf("*");
            }
            else{
                printf(" ");
            }
        }
        printf("\n");
    }

    return 0;
}

double map (int v, int imin, int imax, double omin, double omax)
{
    return (double)(v-imin)/(imax-imin)*(omax-omin) + omin;
}