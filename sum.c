#include <stdio.h>
#include <windows.h>
#include <math.h>
#pragma warning(disable :4996)
//#define EXPORT extern "C" _declspec(dllexport)
#define nsize 800

//#EXPORT void sum(int *binmatrix);

struct params {
    int iterations;
    double tofvector[21];
    double radiusvector[21];
    double energymatrix[21 * 21];
    double thetamatrix[21 * 21];
    double xvector[21];
    double yvector[21];
    double xmatrix[21 * 21];
    double ymatrix[21 * 21];
    int t0;
    double centerE;
    double beginE;
    double endE;
    int binsize[3];
    char filedirectory[300];
};

double vectorpol(double * vector, double val) {
    if ((val < vector[0]) && (val > vector[20])) {
        return 20 * (val - vector[0]) / (vector[20] - vector[0]);
    }
    else {
        return -1;
    }
}

double matpol(double* matrix, double x, double y) {
    int xi = (int)x;
    int yi = (int)y;
    double xr = fmod(x, 1.0);
    double yr = fmod(y, 1.0);

    return matrix[xi + yi * 21] * (1 - xr) * (1 - yr)
        + matrix[(xi + 1) + yi * 21] * (xr) * (1 - yr)
        + matrix[xi + (yi + 1) * 21] * (1 - xr) * (yr)
        + matrix[(xi + 1) + (yi + 1) * 21] * (xr) * (yr);
    
}


_declspec(dllexport) int cal(double* binmatrix, struct params *p) {

    double x1 = 0, y1 = 0, t1 = 0, r = 0, energy = 0, theta = 0, phi = 0, x2 = 0, y2 = 0;
    char direc[10];
    char fulldirec[300];
    for (int iters = 0; iters < p->iterations; iters++) { //file iterations
        

        strcpy(fulldirec, p->filedirectory);
        
        sprintf(direc, "%d_0", iters);
        strcat(fulldirec, direc);
        
        FILE* fp;
        fp = fopen(fulldirec, "rb");
        if (fp == NULL) {
            printf("file open error %d\n", iters);
            continue;
        }
        
        fseek(fp, 0, SEEK_END);
        int size = ftell(fp); //check file size

        fseek(fp, 0, SEEK_SET); //go to zero pointer

        int buffer[3] = { 0, }; //init buffer

        for (int i = 0; i < size / 12; i++) { //file stream, int -> 4 byte, 3 ints for an electron

            fread(buffer, 4, 3, fp);

            x1 = matpol(p->xmatrix, vectorpol(p->xvector, buffer[0]), vectorpol(p->yvector, buffer[1]));
            y1 = matpol(p->ymatrix, vectorpol(p->xvector, buffer[0]), vectorpol(p->yvector, buffer[1]));
            t1 = (buffer[2] - p->t0) * 25 * 1E-12 * sqrt(p->centerE / 10);
            r = sqrt(x1 * x1 + y1 * y1);

            energy = matpol(p->energymatrix, vectorpol(p->tofvector, t1), vectorpol(p->radiusvector, r));;
            theta = matpol(p->thetamatrix, vectorpol(p->tofvector, t1), vectorpol(p->radiusvector, r));;

            phi = atan2(y1, x1);

            x2 = theta * cos(phi) * 180 / 3.14159265;
            y2 = theta * sin(phi) * 180 / 3.14159265;

            if (((buffer[0] * buffer[0] + buffer[1] * buffer[1]) < nsize * nsize) && (energy > p->beginE) && (energy < p->endE)) {

                double x2bin = (x2 + 22) * p->binsize[0] / 44;
                double y2bin = (y2 + 22) * p->binsize[1] / 44;
                double ebin = (energy - p->beginE) / (p->endE - p->beginE) * p->binsize[2];


                int nxi = (int)floor(x2bin);
                int nyi = (int)floor(y2bin);
                int nzi = (int)floor(ebin);
                double nxr = x2bin - (double)nxi;
                double nyr = y2bin - (double)nyi;
                double nzr = ebin - (double)nzi;

                //printf("%f, %f, %f, %d\n", x2, y2, energy, buffer[2]);
                //printf("%d, %d, %d\n", nxi, nyi, nzi);
                if ((nxi >= 0) && (nxi < p->binsize[0]) && (nyi >= 0) && (nyi < p->binsize[1]) && (nzi >= 0) && (nzi < p->binsize[2])) {
                    
                    binmatrix[nxi * p->binsize[1] * p->binsize[2] + nyi * p->binsize[2] + nzi] += (1 - nxr) * (1 - nyr) * (1 - nzr);
                    binmatrix[nxi * p->binsize[1] * p->binsize[2] + nyi * p->binsize[2] + nzi + 1] += (1 - nxr) * (1 - nyr) * (nzr);

                    binmatrix[(nxi + 1) * p->binsize[1] * p->binsize[2] + nyi * p->binsize[2] + nzi] += (nxr) * (1 - nyr) * (1 - nzr);
                    binmatrix[(nxi + 1) * p->binsize[1] * p->binsize[2] + nyi * p->binsize[2] + nzi] += (nxr) * (1 - nyr) * (nzr);

                    binmatrix[nxi * p->binsize[1] * p->binsize[2] + (nyi + 1) * p->binsize[2] + nzi] += (1 - nxr) * (nyr) * (1 - nzr);
                    binmatrix[nxi * p->binsize[1] * p->binsize[2] + (nyi + 1) * p->binsize[2] + nzi] += (1 - nxr) * (nyr) * (nzr);

                    binmatrix[(nxi + 1) * p->binsize[1] * p->binsize[2] + (nyi + 1) * p->binsize[2] + nzi] += (nxr) * (nyr) * (1 - nzr);
                    binmatrix[(nxi + 1) * p->binsize[1] * p->binsize[2] + (nyi + 1) * p->binsize[2] + nzi] += (nxr) * (nyr) * (nzr);

                    
                }

            }
        }
        fclose(fp);
    }
    return 0;
}

