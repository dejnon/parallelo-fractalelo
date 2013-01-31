#ifndef __parallel_fractallel__ComplexNumber__
#define __parallel_fractallel__ComplexNumber__

#include <iostream>
#include <math.h>

class ComplexNumber {
public:
    long double a, b;

    ComplexNumber(long double re, long double im){
        a = re;
        b = im;
    }

    ComplexNumber square(){
        return ComplexNumber(a*a - b*b, 2*a*b);
    }
    

    ComplexNumber add(ComplexNumber cn){
        return ComplexNumber(a+cn.a, b+cn.b);
    }
    // Method for calculating magnitude^2 (how close the number is to infinity)
    double magnitude(){
        return a*a+b*b;
    }
    
    ComplexNumber multiply(ComplexNumber cn) {
        double re = a*cn.a - b *cn.b;
        double im = a*cn.a + b *cn.b;
        return ComplexNumber(re, im);
    }
};

#endif /* defined(__parallel_fractallel__ComplexNumber__) */
