#ifndef _IMAGE_H
#define _IMAGE_H
#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <string>
#include "vec3.h"
#define rep(i, N) for (int i = 0; i < (N); ++i)
using namespace std;
class Image
{
private:
    unsigned int width;
    unsigned int height;
    float *pixel;

public:
    Image(unsigned int w, unsigned int h) : width(w), height(h)
    {
        pixel = new float[3 * width * height];
        rep(i, width)
        {
            rep(j, height)
            {
                const int index = 3 * i + 3 * width * j;
                pixel[index] = 0;
                pixel[index + 1] = 0;
                pixel[index + 2] = 0;
            }
        }
    }
    ~Image()
    {
        delete[] pixel;
    }

    unsigned int getWidth() const
    {
        return width;
    }
    unsigned int getHeight() const
    {
        return height;
    }
    vec3f getPixel(unsigned int u, unsigned int v)
    {
        if (u >= width || u < 0 || v >= height || v < 0)
        {
            cout << "uvが正規化されていません" << endl;
            return vec3f(0);
        }
        return vec3f(pixel[3 * u + 3 * width * v], pixel[3 * u + 3 * width * v + 1], pixel[3 * u + 3 * width * v + 2]);
    }

    void SetPixel(unsigned int i, unsigned int j, const vec3<float> &RGB)
    {
        const int idx = 3 * i + 3 * width * j;
        pixel[idx] = RGB[0];
        pixel[idx + 1] = RGB[1];
        pixel[idx + 2] = RGB[2];
    }

    void writePPM(const string &filename) const
    {
        ofstream file(filename);
        if (!file)
        {
            cerr << "filed to open" << filename << endl;
            return;
        }
        file << "P3" << endl;
        file << width << " " << height << endl;
        file << "255" << endl;
        rep(i, width)
        {
            rep(j, height)
            {
                const int idx = 3 * i + 3 * width * j;
                float R = pixel[idx];
                float G = pixel[idx + 1];
                float B = pixel[idx + 2];

                file << static_cast<unsigned int>(clamp(255.0f * R, 0.0f, 255.0f)) << " ";
                file << static_cast<unsigned int>(clamp(255.0f * G, 0.0f, 255.0f)) << " ";
                file << static_cast<unsigned int>(clamp(255.0f * B, 0.0f, 255.0f)) << endl;
            }
        }
        file.close();
    }

    float gammaCorrection(const float color)
    {
        return pow(color, 1 / 2.2f);
    }
};

#endif