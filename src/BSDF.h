#ifndef _BSDF_H
#define _BSDF_H

#include "vec3.h"
#include "rng.h"
#include <cmath>
#include <algorithm>

using namespace std;
float fresnel(const vec3f &wo, const vec3f n, const float ior1, const float ior2)
{
    float f0 = pow((ior1 - ior2) / (ior1 + ior2), 2.0f);
    return f0 + (1.0f - f0) * pow(1.0 - abs(dot(wo, n)), 5.0f);
}
vec3f sampleHemisphere(float u, float v, float &pdf)
{
    const float theta = std::acos(clamp(1.0f - 2.0f * u, -1.0f, 1.0f)) / 2.0f;
    const float phi = 2.0f * M_PI * v;
    pdf = cos(theta) / M_PI;
    return vec3f(std::cos(phi) * std::sin(theta), std::cos(theta),
                 std::sin(phi) * std::sin(theta));
}

class BSDF
{
public:
    virtual vec3f reBSDF(const vec3f &wo, const vec3f &wi) { return 0; }
    virtual vec3f sampling(RNGrandom &rng, const vec3f &wo, vec3f &wi, float &pdf) { return 0; }
};

class Lambert : public BSDF
{
private:
    const vec3f rho;

public:
    Lambert(const vec3f rho) : rho(rho) {}

    vec3f reBSDF(const vec3f &wo, const vec3f &wi)
    {
        return rho / float(M_PI);
    }
    vec3f sampling(RNGrandom &rng, const vec3f &wo, vec3f &wi, float &pdf)
    {
        wi = sampleHemisphere(rng.getRandom(), rng.getRandom(), pdf);
        return rho / float(M_PI);
    }
};

class Mirror : public BSDF
{
private:
    const vec3f rho;

public:
    Mirror(const vec3f &rho) : rho(rho) {}

    vec3f reBSDF(const vec3f &wo, const vec3f &wi)
    {
        return vec3f(0);
    }

    vec3f sampling(RNGrandom &rng, const vec3f &wo, vec3f &wi, float &pdf)
    {
        wi = reflect(wo, vec3f(0, 1, 0));
        pdf = 1;
        vec3f bsdf = rho / abs(wi[1]);
        // cout << wi << endl;
        // cout << bsdf << endl;
        return bsdf;
    }
};

class Glass : public BSDF
{
private:
    const vec3f rho;
    const float ior;

public:
    Glass(const vec3f &rho, float ior) : rho(rho), ior(ior) {}
    vec3f reBSDF(const vec3f &wo, const vec3f &wi) const
    {
        return vec3f(0);
    }

    vec3f sampling(RNGrandom &rng, const vec3f &wo, vec3f &wi, float &pdf)
    {
        float ior1, ior2;
        vec3f n;
        //外部からの侵入
        if (wo[1] > 0)
        {
            ior1 = 1.0f;
            ior2 = ior;
            n = vec3f(0, 1, 0);
        }
        //内部からの反射
        else
        {
            ior1 = ior;
            ior2 = 1.0f;
            n = vec3f(0, -1, 0);
        }
        //フレネルは反射する光と屈折する光の割合を示すもの
        const float fr = fresnel(wo, n, ior1, ior2);
        //反射
        if (rng.getRandom() < fr)
        {
            wi = reflect(wo, n);
            pdf = 1;
            return rho / abs(wi[1]);
        }
        //屈折の場合
        else
        {
            vec3f t;
            if (refraction(wo, ior1, n, ior2, t))
            {
                wi = t;
                // cout << wo << " " << t << endl;
                pdf = 1;
                return rho / abs(wi[1]);
            }
            //全反射
            else
            {
                wi = reflect(wo, n);
                // if (n[1] < 0)
                //     cout << "全反射" << wi << endl;
                pdf = 1;
                return rho / abs(wi[1]);
            }
        }
    }
};
#endif