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
    for (int i=0; i<rows; i++) {
        array[i] = &(data[cols*i]);
    }
    return array;
}

void master(
    int width   ,
    int height  ,
    int itr     ,
    double minx ,
    double maxx ,
    double miny ,
    double maxy ,
    string filename
) {

    tStart = clock();

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
    printf("(E%d): Started.\n", MPI_RANK);


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
        printf("##\n##\tPARALLELO FRACTALLELO\n##\n");
        printf("Processors: %d\n", MPI_SIZE);
        
        printf("(MASTER): Started. Parameters\n");
        for (int i = 0; i <= argc; i++) {
            printf("%d - %s \n",i, argv[i]);
        }
        printf("\n");


        if (argc < 3) {
            fprintf(stderr,"Please supply 2 parameters. Input config file and output image name.\n");
            fprintf(stderr,"e.g. ./fractalize ./test1.txt ./fractal.tga\n");
            exit(-1);
        }

        FILE *newFile;
        char lineBuffer[80];
        //clrscr();

        if ((newFile = fopen (argv[1], "rt"))) {
            int    width  ;
            int    height ;
            int    itr    ;
            double minx;
            double maxx;
            double miny;
            double maxy;

            fgets(lineBuffer, 80, newFile);
            sscanf (lineBuffer, "%i",  &width );
            fgets(lineBuffer, 80, newFile);
            sscanf (lineBuffer, "%i",  &height);
            fgets(lineBuffer, 80, newFile);
            sscanf (lineBuffer, "%i",  &itr   );
            fgets(lineBuffer, 80, newFile);
            sscanf (lineBuffer, "%lf", &minx  );
            fgets(lineBuffer, 80, newFile);
            sscanf (lineBuffer, "%lf", &maxx  );
            fgets(lineBuffer, 80, newFile);
            sscanf (lineBuffer, "%lf", &miny  );
            fgets(lineBuffer, 80, newFile);
            sscanf (lineBuffer, "%lf", &maxy  );

            fclose(newFile);

            string filename(argv[2], 256);
            printf("(MASTER): Configuration loaded\n");

            master(width,height,itr,minx,maxx,miny,maxy,filename);            
        } else {
            printf("(MASTER): Error reading the file.\n");
            exit(-1);
        }
    } else {
        slave();
    }

    MPI_Finalize();

    return 0;
}

