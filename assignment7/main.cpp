#include "camera.h"
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
float zoom_factor;

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
	Sampler *sampler = new RandomSampler();

	int w = width, h = height;
	Image image(w, h);
	Film film(width, height, random_samples);

	int numlight = p_parser->getNumLights();
	Grid *g = p_tracer->grid;

	RayTracingStats::Initialize(width, height, group->getBoundingBox(), g->nx, g->ny, g->nz);

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < w; j++)
		{
			Vec2f coordinate(j * 1.0f / w, i * 1.0f / height);
			for (int s = 0; s < random_samples; ++s)
			{
				Vec2f _coord;
				Vec2f offset = sampler->getSamplePosition(s);
				Vec2f::Add(_coord, coordinate, offset);

				Hit h;
				h.set(INFINITY, nullptr, {}, {});

				Ray r = camera->generateRay(_coord);

				Vec3f Color = p_tracer->traceRay(r, 0.0f, 0, 1.0f, 1.0f, h);

				image.SetPixel(j, i, Color);

				// debug
				// image.SaveTGA(output_file);
				film.setSample(j, i, s, offset, Color);
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
		char out_depth_prefix[100] = "./result/";
	}

	if (sample_file)
	{
		//
		film.renderSamples(sample_file, zoom_factor);
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
			uniform_samples = atof(argv[i]);
		}
		else if (!strcmp(argv[i], "-jittered_samples"))
		{
			++i;
			jittered_samples = atof(argv[i]);
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