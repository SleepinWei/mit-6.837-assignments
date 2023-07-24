#include<GL/freeglut.h>
#include<memory>
#include"scene_parser.h"
#include"image.h"
#include"vectors.h"
#include"group.h"
#include"light.h"
#include"camera.h"
#include "hit.h"
#include"material.h"
#include"glCanvas.h"
#include<cstring>


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
SceneParser* p_parser; 

void renderFunction(){
	Camera *camera = p_parser->getCamera();
    Group *group = p_parser->getGroup();
    Vec3f background = p_parser->getBackgroundColor();

	int w = width, h = height;
    Image image(w, h);

	int numlight = p_parser->getNumLights();

	for (int i = 0; i < height;i++){
        for (int j = 0; j < w;j++){
            Vec2f coordinate(j * 1.0f / w, i * 1.0f / height);
            Hit h;
			h.set(1e6, nullptr, {}, {});

			Ray r = camera->generateRay(coordinate);
			bool isIntersect = group->intersect(r, h, camera->getTMin());

			if(!isIntersect){
                image.SetPixel(j, i, background);
            }
            else{
                // shading 
                Material* mat = h.getMaterial();
				Vec3f normal = h.getNormal();
				// back facing 
				if(normal.Dot3(r.getDirection()) > 0.0f){
					if(!shade_back)
						continue;
					normal.Set(-normal.x(), -normal.y(), -normal.z());
					h.setNormal(normal);
				}


				Vec3f Color = p_parser->getAmbientLight() * h.getMaterial()->getDiffuseColor();
				for (int i = 0; i < numlight;i++){
					auto light = p_parser->getLight(i);
					Vec3f dirToLight;
					Vec3f lightColor;
					Vec3f intersection = h.getIntersectionPoint();
					float distanceToLight; 
					light->getIllumination(intersection, dirToLight, lightColor,distanceToLight);
					Color += mat->Shade(r, h, dirToLight, lightColor);
				}

				image.SetPixel(j, i, Color);
            }
        }
    }

	char out_prefix[100] = "./result/";
	image.SaveTGA(strcat(out_prefix, output_file)); 
	char out_depth_prefix[100] = "./result/";
}

int main(int argc, char** argv){

	// sample command line:
	// raytracer -input scene1_1.txt -size 200 200 -output output1_1.tga -depth 9 10 depth1_1.tga
	for (int i = 1; i < argc; i++) {
	  if (!strcmp(argv[i],"-input")) {
		i++; assert (i < argc); 
		input_file = argv[i];
	  } else if (!strcmp(argv[i],"-size")) {
		i++; assert (i < argc); 
		width = atoi(argv[i]);
		i++; assert (i < argc); 
		height = atoi(argv[i]);
	  } else if (!strcmp(argv[i],"-output")) {
		i++; assert (i < argc); 
		output_file = argv[i];
	  } else if (!strcmp(argv[i],"-depth")) {
		enable_depth = true; 
		i++; assert (i < argc); 
		depth_min = atof(argv[i]);
		i++; assert (i < argc); 
		depth_max = atof(argv[i]);
		i++; assert (i < argc); 
		depth_file = argv[i];
	  } else if (!strcmp(argv[i],"-normals")){
		enable_normal = true;
		i++;
		assert(i < argc);
		normals_file = argv[i];
	  } else if (!strcmp(argv[i],"-shade_back")){
		assert(i < argc);
		shade_back = true; 
	  } 
	   else if (!strcmp(argv[i],"-gui")){
		assert(i < argc);
		enable_gui = true; 
	   }
	  else {
		printf ("whoops error with command line argument %d: '%s'\n",i,argv[i]);
		assert(0);
	  }
	}

	assert(input_file != nullptr && output_file != nullptr && (enable_depth == false || depth_file != nullptr) && (enable_normal==false || normals_file != nullptr ));

	char prefix[100] = "./scene/";
	SceneParser parser(strcat(prefix, input_file));
	p_parser = &parser;

	if(enable_gui){
		glutInit(&argc, argv);
		GLCanvas canvas;
		canvas.initialize(&parser,renderFunction);
	}
	else{
		renderFunction();
	}

    return 0;
}