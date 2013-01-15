//
//  ComplexNumber.h
//  parallel fractallel
//
//  Created by danone on 1/4/13.
//  Copyright (c) 2013 danone. All rights reserved.
//

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
//class ComplexNumber {
//public:
//    double Re;
//    double Im;
//    
//    ComplexNumber() {
//        
//    };
//    ComplexNumber(double re, double im){
//        Re = re;
//        Im = im;
//    }
//    
//    void add(ComplexNumber b) {
//        Re += b.Re;
//        Im += b.Im;
//    }
//    
//    void multiply(ComplexNumber b) {
//        double re = Re*b.Re - Im*b.Im;
//        double im = Re*b.Re + Im*b.Im;
//        Re = re;
//        Im = im;
//    }
//    
//    void square() {
//        Re = (Re * Re) - (Im * Im);
//        Im = 2*Re*Im;
//    }
//    
//    double magnitude() {
//        return sqrt(Re*Re+Im*Im);
//    }
//    
//    double magnitude2() {
//        return (Re*Re+Im*Im); 
//    }
//    
//    
//    static ComplexNumber* add(ComplexNumber a, ComplexNumber b) {
//        ComplexNumber* c = new ComplexNumber(a.Re, a.Im);
//        c->add(b);
//        return c;
//    }
//    
//    static ComplexNumber* multiply(ComplexNumber a, ComplexNumber b) {
//        ComplexNumber* c = new ComplexNumber(a.Re, a.Im);
//        c->multiply(b);
//        return c;
//    }
//
//    static ComplexNumber* square(ComplexNumber a) {
//        ComplexNumber* c = new ComplexNumber(a.Re, a.Im);
//        c->square();
//        return c;
//    }
//    
//    
//
//};


#endif /* defined(__parallel_fractallel__ComplexNumber__) */
