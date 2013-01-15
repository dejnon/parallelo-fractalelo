//includes
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <cmath>

using namespace std;

//data structures
struct Colour {
	unsigned char r,g,b,a;
};

class ColourConverter {
public:
    static Colour hslToRgb(int h, int s, int l){
        Colour c = Colour();
        
        int q = l < 0.5 ? l * (1 + s) : l + s - l * s;
        int p = 2 * l - q;
        c.r = hue2rgb(p, q, h + 1/3) * 255;
        c.g = hue2rgb(p, q, h) * 255;
        c.b = hue2rgb(p, q, h - 1/3) * 255;
        
        return c;
    }
    static Colour hsvToRgb(int h, int s, int v){
        Colour c = Colour();
        
        int i = floor(h * 6);
        int f = h * 6 - i;
        int p = v * (1 - s);
        int q = v * (1 - f * s);
        int t = v * (1 - (1 - f) * s);
        
        switch(i % 6){
            case 0: c.r = v, c.g = t, c.b = p; break;
            case 1: c.r = q, c.g = v, c.b = p; break;
            case 2: c.r = p, c.g = v, c.b = t; break;
            case 3: c.r = p, c.g = q, c.b = v; break;
            case 4: c.r = t, c.g = p, c.b = v; break;
            case 5: c.r = v, c.g = p, c.b = q; break;
        }
        
        return c;
    }
private:
    static int hue2rgb(int p, int q, int t){
        if(t < 0) t += 1;
        if(t > 1) t -= 1;
        if(t < 1/6) return p + (q - p) * 6 * t;
        if(t < 1/2) return q;
        if(t < 2/3) return p + (q - p) * (2/3 - t) * 6;
        return p;
    }
};



class TGAImage {

public:

	//Constructor
	TGAImage();

	//Overridden Constructor
	TGAImage(int width, int height) {
        m_width = width;
        m_height = height;
        m_pixels = new Colour[m_width*m_height];
    }

	//Set all pixels at once
	void setAllPixels(Colour *pixels){
        m_pixels = pixels;
    };
	//set individual pixels
	void setPixel(Colour inputcolor, int x, int y) {
        m_pixels[convert2dto1d(x,y)] = inputcolor;
    }

	void WriteImage(string filename) {

    //Error checking
    if (m_width <= 0 || m_height <= 0)
    {
        cout << "Image size is not set properly" << endl;
        return;
    }

    ofstream o(filename.c_str(), ios::out | ios::binary);

    //Write the header
    o.put(0);
    o.put(0);
    o.put(2);                         /* uncompressed RGB */
    o.put(0);       o.put(0);
    o.put(0);   o.put(0);
    o.put(0);
    o.put(0);   o.put(0);           /* X origin */
    o.put(0);   o.put(0);           /* y origin */
    o.put((m_width & 0x00FF));
    o.put((m_width & 0xFF00) / 256);
    o.put((m_height & 0x00FF));
    o.put((m_height & 0xFF00) / 256);
    o.put(32);                        /* 24 bit bitmap */
    o.put(0);
    
    //Write the pixel data
    for (int i=0;i<m_height*m_width;i++) {
        o.put(m_pixels[i].b);
        o.put(m_pixels[i].g);
        o.put(m_pixels[i].r);
        o.put(m_pixels[i].a);
    }   
    
    //close the file
    o.close();
    }

//General getters and setters

	void setWidth(int width);
	void setHeight(int height);

	int getWidth();
	int getHeight();

private:

	//store the pixels
	Colour *m_pixels;

	short m_height;
	short m_width;

	//convert 2D to 1D indexing
	int convert2dto1d(int x, int y){
        return m_width * x + y;
    }
	

};