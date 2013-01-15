//
//  Fractal.h
//  parallel fractallel
//
//  Created by danone on 1/4/13.
//  Copyright (c) 2013 danone. All rights reserved.
//

#ifndef __parallel_fractallel__Fractal__
#define __parallel_fractallel__Fractal__

#include <iostream>
#include <cmath>

#include "ComplexNumber.h"
// #include "Image.h"

class Fractal {
    
public:
    static ComplexNumber* c;

    Fractal() {
        c = new ComplexNumber(-0.220, 0.735);
    }
    
    
    static double testPoint(ComplexNumber cn, ComplexNumber constant, int iterations) {
        for (int i = 0; i < iterations; i++) {
            //printf("%f %f -> %f \n", cn.a, cn.b, cn.magnitude());
            cn = cn.square().add(constant);
        }
        return cn.magnitude();
    }
    
    Colour** render(int w, int h, int itr, float minx, float maxx, float miny, float maxy) {
        Colour** image = (Colour **)malloc(sizeof(Colour*)*w);
        for (int i = 0; i < w; i++) {
            image[i] = (Colour *)malloc(sizeof(Colour)*h);
        }
        double pixel_size_x = ((double)(maxx - minx) / (double)(w));
        double pixel_size_y = ((double)(maxy - miny) / (double)(h));

        for(int x=0; x<w; x++) {
            for(int y=0; y<h; y++) {
                double re = ((long double)x * pixel_size_x) + (double)minx;
                double im = ((long double)y * pixel_size_y) + (double)miny;
                int magnitude = abs((int)(testPoint(ComplexNumber(re, im), *c, itr))%255);
                //printf("%f\n", testPoint(ComplexNumber(re, im), *c, itr));
                image[x][y] = ColourConverter::hsvToRgb(magnitude, magnitude, magnitude);
                image[x][y].a = 255;
                
            }
        }

        return image;
    }
};

#endif /* defined(__parallel_fractallel__Fractal__) */
