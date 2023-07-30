#ifndef ATMOSPHERE_H
#define ATMOSPHERE_H
#include"vectors.h"

class Ray; 
class Atmosphere{
	
public:
    Atmosphere();
    Vec3f traceColor(const Ray& r);
private:
    float DistanceToTop(float r, float mu);
    float GetProfileDensity(float hDensity, float altitude, bool isOzone);
    float ComputeOpticalLengthToTopAtmosphereBoundary(float HDensity, float r, float mu, bool isOzone);
    Vec3f ComputeTransmittanceToTop(float r, float mu);
    float DistanceToBottom(float r, float mu);
    bool RayIntersectsGround(float r, float mu);
    Vec3f GetTransmittance(float r, float mu, float d, bool ray_intersect_ground);
    Vec3f GetTransmittanceToSun(
        float r, float mu_s);
    void ComputeSingleScatteringIntegrand(
        // image2D transmittance_texture,
        float r, float mu, float mu_s, float nu, float d,
        bool ray_r_mu_intersects_ground,
        Vec3f &rayleigh, Vec3f &mie);
    float DistanceToNearestAtmosphereBoundary(
        float r, float mu, bool ray_r_mu_intersects_ground);

    void ComputeSingleScattering(
        // image2D transmittance_texture,
        float r, float mu, float mu_s, float nu,
        bool ray_r_mu_intersects_ground,
        Vec3f &rayleigh, Vec3f &mie);

public:
    float solar_irradiance; //4
	float sun_angular_radius; //8
	float top_radius; //12
	float bottom_radius;//16
    float HDensityRayleigh; //20
	// DensityProfile mie_density; 
	float HDensityMie;//24
	float OzoneCenter;//28

	// g coefficient in phase function
	float mie_g; //32

	//scattering coefficient of air molecules
	Vec3f rayleigh_scattering;// 32 + 16 = 48
	Vec3f mie_scattering; // 48 + 16 = 64
	Vec3f mie_extinction;// 64 + 16 = 80

	// DensityProfile absorption_density; //ozone layer
	Vec3f absorption_extinction; // 80 + 16 = 96
	float OzoneWidth;//96 + 4 = 100

    float sunAngle; 
};

#endif // !ATMOSPHERE
