#include<memory>
#include"scene_parser.h"
#include"image.h"
#include"vectors.h"
#include"group.h"
#include"camera.h"
#include "hit.h"
#include"material.h"
#include"shading.h"
#include<cstring>


using std::cout;


int main(int argc, char** argv){
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
	  } else {
		printf ("whoops error with command line argument %d: '%s'\n",i,argv[i]);
		assert(0);
	  }
	}

	assert(input_file != nullptr && output_file != nullptr && (enable_depth == false || depth_file != nullptr) && (enable_normal==false || normals_file != nullptr ));

	char prefix[100] = "./scene/";
	SceneParser parser(strcat(prefix, input_file));

    Camera *camera = parser.getCamera();
    Group *group = parser.getGroup();
    Vec3f background = parser.getBackgroundColor();

	int w = width, h = height;
    Image image(w, h);
	Image depth_image(w, h);
	Image normal_image(w, h);

	std::shared_ptr<Shading> normal_shading(new NormalShading());
	std::shared_ptr<Shading> diffuseShading(new DiffuseShading());

	for (int i = 0; i < h;i++){
        for (int j = 0; j < w;j++){
            Vec2f coordinate(j * 1.0f / w, i * 1.0f / h);
            Hit h;
			h.set(1e6, nullptr, {}, {});

			Ray r = camera->generateRay(coordinate);
			bool isIntersect = group->intersect(r, h, camera->getTMin());

			if(!isIntersect){
                image.SetPixel(j, i, background);
				depth_image.SetPixel(j, i, { 0,0,0 });
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

				Vec3f diffuseColor = diffuseShading->exec(mat, h, parser);
				image.SetPixel(j, i, diffuseColor);

				if(enable_depth){
					float depth = h.getT();
					float gray_scale = (depth_max - depth) * 1.0f / (depth_max - depth_min);
					depth_image.SetPixel(j, i, { gray_scale,gray_scale,gray_scale });
				}

				if(enable_normal){
					Vec3f normalColor = normal_shading->exec(mat, h, parser);
					normal_image.SetPixel(j, i, normalColor);
				}
            }
        }
    }

	char out_prefix[100] = "./result/";
	image.SaveTGA(strcat(out_prefix, output_file)); 
	char out_depth_prefix[100] = "./result/";

	if(enable_depth){
		depth_image.SaveTGA(strcat(out_depth_prefix, depth_file));
	}

	char out_normal_prefix[100] = "./result/";
	if(enable_normal){
		normal_image.SaveTGA(strcat(out_normal_prefix, normals_file));
	}

    return 0;
}