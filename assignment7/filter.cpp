#include "filter.h"
#include"vectors.h"
#include "film.h"
#include<iostream>
using std::cout;

float BoxFilter::getWeight(float x, float y)
{
    if (abs(x) <= coeff && abs(y) <= coeff)
    {
        return 1.0f;
    }
    return 0;
}
float TentFilter::getWeight(float x, float y)
{
    float distance = sqrtf(x * x + y * y);
    return max(0.0f, 1 - distance / coeff);
}
float GaussianFilter::getWeight(float x, float y)
{
    // if(x * x + y * y > 4 * coeff* coeff){
    //     return 0.0f;
    // }
    float weight = exp(-(x * x + y * y) / 2.0f / (coeff * coeff));
    return weight;
}

Vec3f Filter::getColor(int i, int j, Film *film)
{
    Vec3f result(0, 0, 0);
    float sum_weight = 0;
    // coeff *coeff * (2 * 3.14f);
    int support_radius = getSupportRadius();
    int pos_x_min = std::max<int>(i - support_radius, 0);
    int pos_x_max = std::min<int>(i + support_radius, film->getWidth() - 1);
    int pos_y_min = std::max<int>(j - support_radius, 0.0f);
    int pos_y_max = std::min<int>(j + support_radius, film->getHeight() - 1);

    for (int pos_x = (pos_x_min); pos_x <= pos_x_max; pos_x++)
    {
        for (int pos_y = (pos_y_min); pos_y <= pos_y_max; pos_y++)
        {
            for (int index = 0; index < film->getNumSamples(); index++)
            {
                Sample s = film->getSample(pos_x, pos_y, index);
                Vec2f offset(0,0);
                Vec2f::Sub(offset, s.getPosition(), {0.5f, 0.5f});
                // cout << offset.x() << ' ' << offset.y() << '\n';
                Vec2f::Add(offset,offset, Vec2f{pos_x - i, pos_y - j}); // 不同各自由不同的offset
                // cout << offset.x() << ' ' << offset.y() << "#\n";

                float weight = getWeight(offset.x(), offset.y());
                sum_weight += weight;
                result += Vec3f(weight, weight, weight) * s.getColor();
            }
        }
    }
    result.Divide(sum_weight, sum_weight, sum_weight);
    return result;
}