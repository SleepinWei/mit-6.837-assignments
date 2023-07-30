#include "atmosphere.h"
#include "ray.h"

Atmosphere::Atmosphere(){
    solar_irradiance = 10.0f; // solar irradiance
	sun_angular_radius = 0.005f;//sun angular radius
	top_radius = 6460.0f;// top_radius
	bottom_radius = 6360.0f;// bottom_radius

	HDensityRayleigh = 8.0f; //hdensity rayleigh
	HDensityMie = 1.2f; // hdensity mie
	OzoneCenter = 25.0f; //ozoneCenter
	mie_g = 0.8;//mig_g
	rayleigh_scattering = Vec3f(5.802e-3, 13.558e-3, 33.1e-3);//ray_scat
	// here 1 means 1km, thus we make scattering & absorption 1000 times larger
	mie_scattering = Vec3f(3.996e-3, 3.996e-3, 3.996e-3);//sattering
	mie_extinction = Vec3f(4.40e-3, 4.40e-3, 4.40e-3);//mie_absorption(extinction)
	mie_extinction = mie_extinction + mie_scattering;
	absorption_extinction = Vec3f(0.650e-3, 1.881e-3, 0.085e-3);// ozone absorption
	OzoneWidth = 15.0f; //ozone thickness
}

float Atmosphere::DistanceToTop(float r, float mu)
{
    // mu = cos theta
    // r = polar radius
    if (r > top_radius)
        return -1;
    // if(mu < -1.0 || mu > 1.0)
    // return -1;
    float discriminant = r * r * (mu * mu - 1.0f) + top_radius * top_radius;
    if (discriminant < 0)
        return -1;
    return (-r * mu + sqrt(discriminant));
}

float Atmosphere::DistanceToBottom(float r, float mu)
{
    // mu = cos theta
    // r = polar radius
    if (r < bottom_radius)
        return -1;
    if (mu < -1.0 || mu > 1.0)
        return -1;
    float discriminant = r * r * (mu * mu - 1.0f) + bottom_radius * bottom_radius;
    if (discriminant < 0)
        return -1;
    return (-r * mu - sqrt(discriminant));
}

bool Atmosphere::RayIntersectsGround(float r, float mu)
{
    return mu < 0.0 && r * r * (mu * mu - 1.0) +
                               bottom_radius * bottom_radius >=
                           0.0;
}

float Atmosphere::GetProfileDensity(float hDensity, float altitude, bool isOzone)
{
    float density = 0.0f;
    if (isOzone)
    {
        density = std::max<float>(0, 1 - abs(altitude - OzoneCenter) / OzoneWidth);
    }
    else
    {
        density = exp(-altitude / hDensity);
    }
    return std::min(std::max(density, 0.0f), 1.0f);
}

float Atmosphere::ComputeOpticalLengthToTopAtmosphereBoundary(float HDensity, float r, float mu, bool isOzone)
{
    float SAMPLES = 40.0f;
    float dx = DistanceToTop(r, mu) / float(SAMPLES);
    float result = 0.0;
    for (int i = 0; i <= SAMPLES; i++)
    {
        float d_i = float(i) * dx;
        // current sample point to planet center
        float r_i = sqrt(d_i * d_i + 2.0 * r * mu * d_i + r * r);
        // density at given height, dimensionless.
        // float y_i = GetProfileDensity(profile,r_i-atmosphere.bottom_radius);
        float y_i = GetProfileDensity(HDensity, r_i - bottom_radius, isOzone);
        // sample weight
        // trapezoidal rule
        float weight_i = (i == 0 || i == SAMPLES ? 0.5f : 1.0f);
        result += y_i * weight_i * dx; // compute integral
    }
    return result;
}

Vec3f Atmosphere::ComputeTransmittanceToTop(float r, float mu)
{
    Vec3f lightDistance =
        rayleigh_scattering *
            ComputeOpticalLengthToTopAtmosphereBoundary(
                //   atmosphere, atmosphere.rayleigh_density, r, mu) +
                HDensityRayleigh, r, mu, false)
        // ;
        +
        mie_extinction *
            ComputeOpticalLengthToTopAtmosphereBoundary(
                // atmosphere, atmosphere.mie_density, r, mu) +
                HDensityMie, r, mu, false) +
        absorption_extinction *
            ComputeOpticalLengthToTopAtmosphereBoundary(
                //   atmosphere, atmosphere.absorption_density, r, mu)
                0.0f, r, mu, true);
    return Vec3f{exp(-lightDistance.x()), exp(-lightDistance.y()), exp(-lightDistance.z())};
}

Vec3f Atmosphere::GetTransmittance(
    float r, float mu, float d, bool ray_intersect_ground)
{
    float r_d = sqrt(d * d + 2.0 * r * mu * d + r * r);
    float mu_d = (r * mu + d) / r_d;

    if (ray_intersect_ground)
    {
        Vec3f tran = ComputeTransmittanceToTop(
                         r_d, -mu_d) /
                     ComputeTransmittanceToTop(
                         r, -mu);
        return tran;
    }
    else
    {
        Vec3f tran = ComputeTransmittanceToTop(
                         r, mu) /
                     ComputeTransmittanceToTop(
                         // transmittanceTexture,
                         r_d, mu_d);
        return tran;
    }
}

float clamp(float v, float _min, float _max)
{
    return std::min(std::max(_min, v), _max);
}

float smoothstep(float t1, float t2, float x)
{
    x = clamp((x - t1) / (t2 - t1), 0.0, 1.0);
    // Evaluate polynomial
    return x * x * (3 - 2 * x);
}

Vec3f Atmosphere::GetTransmittanceToSun(
    float r, float mu_s)
{
    float sin_theta_h = bottom_radius / r;
    float cos_theta_h = -sqrt(max(1.0 - sin_theta_h * sin_theta_h, 0.0));
    return ComputeTransmittanceToTop(
               r, mu_s) *
           smoothstep(-sin_theta_h * sun_angular_radius,
                      sin_theta_h * sun_angular_radius,
                      mu_s - cos_theta_h);
}

void Atmosphere::ComputeSingleScatteringIntegrand(
    // image2D transmittance_texture,
    float r, float mu, float mu_s, float nu, float d,
    bool ray_r_mu_intersects_ground,
    Vec3f &rayleigh, Vec3f &mie)
{
    float r_d = sqrt(d * d + 2.0 * r * mu * d + r * r);
    float mu_s_d = (r * mu_s + d * nu) / r_d;
    Vec3f transmittance =
        GetTransmittance(
            r, mu, d,
            ray_r_mu_intersects_ground) *
        GetTransmittanceToSun(
            r_d, mu_s_d);
    rayleigh = transmittance * GetProfileDensity(
                                   HDensityRayleigh, r_d - bottom_radius, false);
    mie = transmittance * GetProfileDensity(
                              // atmosphere.mie_density, r_d - atmosphere.bottom_radius);
                              HDensityMie, r_d - bottom_radius, false);
}
float Atmosphere::DistanceToNearestAtmosphereBoundary(
    float r, float mu, bool ray_r_mu_intersects_ground)
{
    if (ray_r_mu_intersects_ground)
    {
        return DistanceToBottom(r, mu);
    }
    else
    {
        return DistanceToTop(r, mu);
    }
}

void Atmosphere::ComputeSingleScattering(
    float r, float mu, float mu_s, float nu,
    bool ray_r_mu_intersects_ground,
    Vec3f &rayleigh, Vec3f &mie)
{
    // assume sun light is parallel
    //   assert(r >= atmosphere.bottom_radius && r <= atmosphere.top_radius);
    //   assert(mu >= -1.0 && mu <= 1.0);
    //   assert(mu_s >= -1.0 && mu_s <= 1.0);
    //   assert(nu >= -1.0 && nu <= 1.0);

    // Number of intervals for the numerical integration.
    const int SAMPLE_COUNT = 30;
    // The integration step, i.e. the length of each integration interval.
    float dx =
        DistanceToNearestAtmosphereBoundary(r, mu,
                                            ray_r_mu_intersects_ground) /
        float(SAMPLE_COUNT);
    // Integration loop.
    Vec3f rayleigh_sum(0, 0, 0);
    Vec3f mie_sum(0, 0, 0);
    for (int i = 0; i <= SAMPLE_COUNT; ++i)
    {
        float d_i = float(i) * dx;
        // The Rayleigh and Mie single scattering at the current sample point
        Vec3f rayleigh_i;
        Vec3f mie_i;
        ComputeSingleScatteringIntegrand(
            // transmittance_texture,
            r, mu, mu_s, nu, d_i, ray_r_mu_intersects_ground, rayleigh_i, mie_i);
        // Sample weight (from the trapezoidal rule).
        float weight_i = (i == 0 || i == SAMPLE_COUNT) ? 0.5 : 1.0;
        rayleigh_sum += rayleigh_i * weight_i;
        mie_sum += mie_i * weight_i;
    }
    rayleigh = rayleigh_sum * dx * solar_irradiance *
               rayleigh_scattering;
    mie = mie_sum * dx * solar_irradiance * mie_scattering;
}

#define PI 3.1415926
float RayleighPhaseFunction(float nu)
{
    float k = 3.0 / (16.0 * PI);
    return k * (1.0 + nu * nu);
}

float MiePhaseFunction(float g, float nu)
{
    float k = 3.0 / (8.0 * PI) * (1.0 - g * g) / (2.0 + g * g);
    return k * (1.0 + nu * nu) / pow(1.0 + g * g - 2.0 * g * nu, 1.5);
}

Vec3f Atmosphere::traceColor(const Ray &ray)
{
    float sun_rad = sunAngle / 180.0f * PI;
    Vec3f dir = ray.getDirection();
    Vec3f sunDir(
        0.0f,
        sin(sun_rad),
        -cos(sun_rad));
    float r, mu, mu_s, nu;
    Vec3f up(0.0f, 1.0f, 0.0f);
    r = bottom_radius + 1.0f;
    mu = up.Dot3(dir);
    nu = dir.Dot3(sunDir);
    mu_s = up.Dot3(sunDir);
    bool intersect_ground = RayIntersectsGround(r, mu);

    Vec3f rayleigh(0,0,0);
    Vec3f mie(0,0,0);

    ComputeSingleScattering(
        // transmittanceTexture,
        r, mu, mu_s, nu, intersect_ground, rayleigh, mie);
    Vec3f color = rayleigh * RayleighPhaseFunction(nu) + mie * MiePhaseFunction(mie_g, nu);
    return color;
}