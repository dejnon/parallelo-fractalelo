#include "Image.h"
#include "Fractal.h"
#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sstream>

clock_t tStart; 
clock_t tFinish; 

int MPI_RANK, MPI_SIZE;

int **alloc_2d_int(int rows, int cols) {
    int *data = (int *)malloc(rows*cols*sizeof(int));
    int **array= (int **)malloc(rows*sizeof(int*));
    for (int i=0; i<rows; i++)
        array[i] = &(data[cols*i]);

    return array;
}

void master() {
    tStart = clock();

    int width   =  5000;
    int height  =  5000;
    int itr     =  40;//120;//505;
    double minx   =  0.13772053;
    double maxx   =  0.000492212651;
    double miny   =  0.000442666778;
    double maxy   =  0.16108669;

    int splice_itr    = itr;
    int splice_width  = (width /(MPI_SIZE-1));
    int splice_height = height;
    
    for(int partner = 1; partner < MPI_SIZE; partner++) {
        double splice_size   = (maxx-minx)/(double)(MPI_SIZE-1);
        double splice_minx   = (splice_size*(double)(partner-1))+minx;
        double splice_maxx   = splice_minx+splice_size;
        double splice_miny   = miny;
        double splice_maxy   = maxy;

        printf("(MASTER): Sending initial data to processor %d\n", partner);
        MPI_Send(&splice_itr    ,1, MPI_INT, partner, 0, MPI_COMM_WORLD);
        MPI_Send(&splice_width  ,1, MPI_INT, partner, 0, MPI_COMM_WORLD);
        MPI_Send(&splice_height ,1, MPI_INT, partner, 0, MPI_COMM_WORLD);
        MPI_Send(&splice_minx   ,1, MPI_DOUBLE, partner, 0, MPI_COMM_WORLD);
        MPI_Send(&splice_maxx   ,1, MPI_DOUBLE, partner, 0, MPI_COMM_WORLD);
        MPI_Send(&splice_miny   ,1, MPI_DOUBLE, partner, 0, MPI_COMM_WORLD);
        MPI_Send(&splice_maxy   ,1, MPI_DOUBLE, partner, 0, MPI_COMM_WORLD);
    }

    TGAImage *img = new TGAImage(width,height);
    string filename = "./test.tga";
    //         for(int x=0; x<width; x++) {
    //     for(int y=0; y<height; y++) {
    //         img->setPixel(image[x][y],x,y);
    //     }
    // }
    // img->WriteImage(filename);

    MPI_Status status;
    int** image = alloc_2d_int(splice_width, splice_height);

    for(int partner = 1; partner < MPI_SIZE; partner++) {
        MPI_Recv(&(image[0][0]), splice_width*splice_height, MPI_INT, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("(MASTER): Got result from: %d\n", partner);
        int x_shift = (splice_width*(partner-1));
        for(int x=0; x<splice_width; x++) {
            for(int y=0; y<splice_height; y++) {    
                img->setPixel(
                    ColourConverter::hsvToRgb(
                        image[x][y],
                        image[x][y],
                        image[x][y]
                    ),
                    (x_shift+x),
                    y
                );
            }
        }
    }
    img->WriteImage(filename);
    printf("(MASTER): Finished! File saved! Total execution time: %.2fs\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);

}

void slave() {
    tStart = clock();

    int splice_itr   ;
    int splice_width ;
    int splice_height;
    double splice_minx;
    double splice_maxx;
    double splice_miny;
    double splice_maxy;

    MPI_Recv(&splice_itr    ,1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&splice_width  ,1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&splice_height ,1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&splice_minx   ,1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&splice_maxx   ,1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&splice_miny   ,1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&splice_maxy   ,1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    printf("(E%d): Got all the parameters. Starts computing.\n", MPI_RANK);

    // Fractal* f = new Fractal();
    // short** image = f->getValues(
    //     splice_width, 
    //     splice_height, 
    //     splice_itr, 
    //     splice_minx, 
    //     splice_maxx, 
    //     splice_miny, 
    //     splice_maxy
    // );
    ComplexNumber* c = new ComplexNumber(-0.220, 0.735);
    double pixel_size_x = ((double)(splice_maxx - splice_minx) / (double)(splice_width));
    double pixel_size_y = ((double)(splice_maxy - splice_miny) / (double)(splice_height));

    int** image = alloc_2d_int(splice_width, splice_height);
    for(int x=0; x<splice_width; x++) {
        for(int y=0; y<splice_height; y++) {
            double re = ((long double)x * pixel_size_x) + (double)splice_minx;
            double im = ((long double)y * pixel_size_y) + (double)splice_miny;
            int magnitude = abs(
                (int)(Fractal::testPoint(ComplexNumber(re, im), *c, splice_itr)*100) % 255
            );
            image[x][y] = magnitude;
        }
    }

    printf("(E%d): Calculation finished! \n", MPI_RANK);
    tFinish = clock();

    MPI_Send(&(image[0][0]), splice_width*splice_height, MPI_INT, 0, 0, MPI_COMM_WORLD);
    

    printf("(E%d): Finished! Total execution time: %.2fs (data transfer: %.2fs)\n", MPI_RANK, (double)(clock() - tStart)/CLOCKS_PER_SEC, (double)(clock() - (tFinish-tStart))/CLOCKS_PER_SEC);

    // TGAImage *img = new TGAImage(splice_width,splice_height);

    // int a = MPI_RANK;
    // stringstream ss;
    // ss << a;
    // string str = ss.str();
    // string filename = "./test"+str+".tga";
    // for(int x=0; x<splice_width; x++) {
    //     for(int y=0; y<splice_height; y++) {
    //         img->setPixel(ColourConverter::hsvToRgb(image[x][y],image[x][y],image[x][y]) ,x,y);
    //     }
    // }
    // img->WriteImage(filename);

}

int main(int argc, char **argv)
{

	MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &MPI_RANK);
    MPI_Comm_size(MPI_COMM_WORLD, &MPI_SIZE);

    if (MPI_SIZE < 2) {
        fprintf(stderr,"Requires at least two processes.\n");
        exit(-1);
    }

    if (MPI_RANK == 0) {
        master();
    } else {
        slave();
    }

    MPI_Finalize();

    return 0;
}

