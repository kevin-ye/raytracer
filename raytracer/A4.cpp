#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <ctime>

#include "cs488-framework/MathUtils.hpp"

#include "A4.hpp"
#include "Tray.hpp"

using namespace std;
using namespace glm;

void A4_Render(
		// What to render
		SceneNode * root,

		// Image to write to, set to a given width and height
		Image & image,

		// Viewing parameters
		const vec3 & eye,
		const vec3 & view,
		const vec3 & up,
		double fovy,

		// Lighting parameters
		const vec3 & ambient,
		const list<Light *> & lights
) {

  // Fill in raytracing code here...

  cout << "Calling A4_Render(\n" <<
		  "\t" << *root <<
          "\t" << "Image(width:" << image.width() << ", height:" << image.height() << ")\n"
          "\t" << "Antialiasing:  x" << root->aasize << endl <<
          "\t" << "eye:  " << to_string(eye) << endl <<
		  "\t" << "view: " << to_string(view) << endl <<
		  "\t" << "up:   " << to_string(up) << endl <<
		  "\t" << "fovy: " << fovy << endl <<
          "\t" << "ambient: " << to_string(ambient) << endl <<
		  "\t" << "lights{" << endl;

	for(const Light * light : lights) {
		cout << "\t\t" <<  *light << endl;
	}
	cout << "\t}" << endl;
	 cout <<")" << endl;

	size_t h = image.height();
	size_t w = image.width();
	double pro = 0;

	srand(time(NULL));

	vec3 background(double(123) / 255, double(104) / 255, double(238) / 255);

	vec3 **temper_img = new vec3*[w];
	for (int ix = 0; ix < w; ++ix)
	{
		temper_img[ix] = new vec3[h];
	}

	cout << "Progress: 0%" << endl;

	for (uint y = 0; y < h; ++y) {
		for (uint x = 0; x < w; ++x) {
			// construct a primary ray through current pixel
			
			// intersect scene
			vec3 currentPixel(0, 0, 0);
			for (int cp = 0; cp < root->aasize; ++cp)
			{
				for (int cq = 0; cq < root->aasize; ++cq)
				{
					int randx = rand() % 100 + 1;
					int randy = rand() % 100 + 1;
					double dx = 1 / double(randx);
					double dy = 1 / double(randy);
					Tray aaray(eye, view, up, fovy, h, w,
						x, y, cp + dx, cq + dy, ambient, lights);
					vec3 aacolor = aaray.intersect(root);
					if (aacolor.x != -1)
					{
						currentPixel += aacolor;
					} else {
						currentPixel += (background  + ((vec3(1, 1, 1) - background) * (double(y) / double(h))));
					}
				}
			}
			// write pixel
			currentPixel = currentPixel / (root->aasize * root->aasize);
			temper_img[x][y] = currentPixel;
		}
		if ((pro + 10) <= (100 * double(y) / double(h)))
		{
			pro = (100 * double(y) / double(h));
			cout << "Progress: " << int(pro) << "%" << endl;
		}
	}

	// remap to lower RGB value in temper_img
	for (uint y = 0; y < h; ++y) {
		for (uint x = 0; x < w; ++x) {

			if (temper_img[x][y][0] > 1)
			{
				temper_img[x][y][0] = 1;
			}

			if (temper_img[x][y][1] > 1)
			{
				temper_img[x][y][1] = 1;
			}

			if (temper_img[x][y][2] > 1)
			{
				temper_img[x][y][2] = 1;
			}


			image(x, y, 0) = temper_img[x][y][0];
			image(x, y, 1) = temper_img[x][y][1];
			image(x, y, 2) = temper_img[x][y][2];

			//cout << temper_img[x][y][0] / maxRed << ","
			//<< temper_img[x][y][1] / maxGreen << ","
			//<< temper_img[x][y][2] / maxBlue << endl;
		}
	}

	for (int ix = 0; ix < w; ++ix)
	{
		delete [] temper_img[ix];
	}

	delete [] temper_img;

	cout << "Progress: 100%" << endl;
}
