#include "camera.h"
#include"filter.h"
#include "glCanvas.h"
#include "grid.h"
#include "group.h"
#include "hit.h"
#include "image.h"
#include "light.h"
#include "material.h"
#include "rayTree.h"
#include "raytracer.h"
#include "raytracing_stats.h"
#include "scene_parser.h"
#include "vectors.h"
#include "atmosphere.h"
#include <GL/freeglut.h>
#include <cstring>
#include <memory>

using std::cout;

char *input_file = NULL;
int width = 100;
int height = 100;
char *output_file = NULL;
float depth_min = 0;
float depth_max = 1;
char *depth_file = NULL;
char *normals_file = NULL;
bool shade_back = false;
bool enable_depth = false;
bool enable_normal = false;
bool enable_gui = false;
int tessx = 20;
int tessy = 20;
bool enable_gouraud = false;
bool shadows = false;
int max_bounces = 10;
float cutoff_weight = 0.001f;
int nx, ny, nz;
int random_samples = 0;
int uniform_samples = 0;
int jittered_samples = 0;
char *sample_file = nullptr;
int zoom_factor = 0;

char *render_filter_file = nullptr;
int filter_zoom_factor = 0;

float box_filter_radius = 0, tent_filter_radius = 0, gaussian_filter_radius = 0;

bool enable_grid = false;
bool visualize_grid = false;
bool stats = false;
SceneParser *p_parser;
RayTracer *p_tracer;

#include "film.h"
#include "sampler.h"

void renderFunction()
{

	Camera *camera = p_parser->getCamera();
	Group *group = p_parser->getGroup();
	Vec3f background = p_parser->getBackgroundColor();
	Sampler *sampler = nullptr;
	Filter *filter = nullptr;
	Film *film = nullptr;
	int samples = 1; 
	if(random_samples){
		sampler = new RandomSampler(random_samples);
		samples = random_samples;
	}
	else if (uniform_samples){
		sampler = new UniformSampler(uniform_samples);
		samples = uniform_samples;
	}
	else if (jittered_samples){
		sampler = new JitteredSampler(jittered_samples);
		samples = jittered_samples;
	}

	film = new Film(width, height, samples);

	if(box_filter_radius){
		filter = new BoxFilter(box_filter_radius);
	}
	else if (tent_filter_radius){
		filter = new TentFilter(tent_filter_radius);
	}
	else if (gaussian_filter_radius){
		filter = new GaussianFilter(gaussian_filter_radius);
	}

	Image image(width, height);

	int numlight = p_parser->getNumLights();
	Grid *g = p_tracer->grid;

	RayTracingStats::Initialize(width, height, group->getBoundingBox(), g->nx, g->ny, g->nz);
	int sz = std::max(width, height);

	for (int _i = 0; _i < height; _i++)
	{
		int i = _i; 
		if(height < width){
			i += (width - height) / 2; 
		}
		for (int _j = 0; _j < width; _j++)
		{
			int j = _j; 
			if(width < height){
				j += (width - height) / 2;
			}
			Vec2f coordinate(j * 1.0f / sz, i * 1.0f / sz);
			Vec3f Color(0,0,0); 
			for (int s = 0; s < samples; ++s)
			{
				Vec2f _coord = coordinate;
				Vec2f offset(0, 0);
				if(sampler){
					offset = sampler->getSamplePosition(s);
				}
				Vec2f _offset = offset; 
				offset.Divide(width, height);
				Vec2f::Add(_coord, coordinate, offset);

				Hit h;
				h.set(INFINITY, nullptr, {}, {});

				Ray r = camera->generateRay(_coord);

				Color = p_tracer->traceRay(r, 0.0f, 0, 1.0f, 1.0f, h);

				film->setSample(_j, _i, s, _offset, Color);
				// debug
				// image.SaveTGA(output_file);
			}

		}
	}

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if(filter){
				Vec3f resultColor = filter->getColor(j, i, film);
				image.SetPixel(j, i, resultColor);
			}
			else{
				Vec3f Color = film->getSample(j, i, 0).getColor();
				image.SetPixel(j, i, Color);
			}
		}
	}
	if (stats)
	{
		RayTracingStats::PrintStatistics();
	}

	if (output_file)
	{
		char out_prefix[100] = "./result/";
		image.SaveTGA(strcat(out_prefix, output_file));
	}
	if (sample_file)
	{
		char rff[100] = "./result/";
		strcat(rff, sample_file);
		film->renderSamples(rff, zoom_factor);
	}
	if(render_filter_file){
		char rff[100] = "./result/";
		strcat(rff, render_filter_file);
		film->renderFilter(rff, filter_zoom_factor, filter);
	}
	if(sampler){
		delete sampler; 
	}
	if(filter){
		delete filter;
	}
	if(film){
		delete film; 
	}
}

void traceRayFunction(float x, float y)
{
	Camera *camera = p_parser->getCamera();
	Ray r = camera->generateRay(Vec2f(x, y));
	Hit hit{INFINITY, nullptr, {}};
	p_tracer->traceSingleRay(r, camera->getTMin(), 0, cutoff_weight, 1.0f, hit);

	RayTree::SetMainSegment(r, 0, hit.getT());
}

int main(int argc, char **argv)
{

	// sample command line:
	// raytracer -input scene1_1.txt -size 200 200 -output output1_1.tga -depth 9 10 depth1_1.tga
	for (int i = 1; i < argc; i++)
	{
		if (!strcmp(argv[i], "-input"))
		{
			i++;
			assert(i < argc);
			input_file = argv[i];
		}
		else if (!strcmp(argv[i], "-size"))
		{
			i++;
			assert(i < argc);
			width = atoi(argv[i]);
			i++;
			assert(i < argc);
			height = atoi(argv[i]);
		}
		else if (!strcmp(argv[i], "-output"))
		{
			i++;
			assert(i < argc);
			output_file = argv[i];
		}
		else if (!strcmp(argv[i], "-depth"))
		{
			enable_depth = true;
			i++;
			assert(i < argc);
			depth_min = atof(argv[i]);
			i++;
			assert(i < argc);
			depth_max = atof(argv[i]);
			i++;
			assert(i < argc);
			depth_file = argv[i];
		}
		else if (!strcmp(argv[i], "-normals"))
		{
			enable_normal = true;
			i++;
			assert(i < argc);
			normals_file = argv[i];
		}
		else if (!strcmp(argv[i], "-shade_back"))
		{
			assert(i < argc);
			shade_back = true;
		}
		else if (!strcmp(argv[i], "-gui"))
		{
			assert(i < argc);
			enable_gui = true;
		}
		else if (!strcmp(argv[i], "-tessellation"))
		{
			++i;
			assert(i < argc);
			tessx = atof(argv[i]);
			++i;
			tessy = atof(argv[i]);
		}
		else if (!strcmp(argv[i], "-gouraud"))
		{
			enable_gouraud = true;
		}
		else if (!strcmp(argv[i], "-shadows"))
		{
			shadows = true;
		}
		else if (!strcmp(argv[i], "-bounces"))
		{
			++i;
			max_bounces = atof(argv[i]);
		}
		else if (!strcmp(argv[i], "-weight"))
		{
			++i;
			cutoff_weight = atof(argv[i]);
		}
		else if (!strcmp(argv[i], "-grid"))
		{
			enable_grid = true;
			++i;
			nx = atof(argv[i]);
			++i;
			ny = atof(argv[i]);
			++i;
			nz = atof(argv[i]);
		}
		else if (!strcmp(argv[i], "-visualize_grid"))
		{
			visualize_grid = true;
		}
		else if (!strcmp(argv[i], "-stats"))
		{
			stats = true;
		}
		else if (!strcmp(argv[i], "-random_samples"))
		{
			++i;
			random_samples = atof(argv[i]);
		}
		else if (!strcmp(argv[i], "-render_samples"))
		{
			++i;
			sample_file = argv[i];
			++i;
			zoom_factor = atof(argv[i]);
		}
		else if (!strcmp(argv[i], "-uniform_samples"))
		{
			++i;
			uniform_samples = atoi(argv[i]);
		}
		else if (!strcmp(argv[i], "-jittered_samples"))
		{
			++i;
			jittered_samples = atoi(argv[i]);
		}
		else if (!strcmp(argv[i], "-box_filter"))
		{
			++i;
			box_filter_radius = atof(argv[i]);
		}
		else if (!strcmp(argv[i], "-tent_filter"))
		{
			++i;
			tent_filter_radius = atof(argv[i]);
		}
		else if (!strcmp(argv[i], "-gaussian_filter"))
		{
			++i;
			gaussian_filter_radius = atof(argv[i]);
		}
		else if (!strcmp(argv[i], "-render_filter")){
			++i;
			render_filter_file = argv[i];
			++i;
			filter_zoom_factor = atoi(argv[i]);
		}
		else
		{
			printf("whoops error with command line argument %d: '%s'\n", i, argv[i]);
			assert(0);
		}
	}

	if (!input_file)
	{
		return 0;
	}
	char prefix[100] = "./scene/";
	SceneParser parser(strcat(prefix, input_file));
	p_parser = &parser;
	RayTracer tracer(p_parser, max_bounces, cutoff_weight, shadows);
	p_tracer = &tracer;
	if (enable_gui)
	{
		glutInit(&argc, argv);
		GLCanvas canvas;
		canvas.initialize(&parser, renderFunction, traceRayFunction, p_tracer->grid, visualize_grid);
	}
	else
	{
		renderFunction();
	}

	return 0;
}