#include <bits/stdc++.h>
using namespace std;

#include "uint.h"
#include "vec3.hpp"
#include "image.hpp"
#include "timer.hpp"
#include "color.hpp"
#include "imagewnd.hpp"
#include "fastmath.hpp"
#include "random.hpp"
#include "material.hpp"
#include "triangle.hpp"
#include "spherical.hpp"
#include "scene.hpp"
#include "pathtracer.hpp"
#include "renderer.hpp"
#include "testscene.hpp"
#include "video.hpp"

int main(int argc, char *argv[])
{
	fastmath::presolve();

	Scene scene;
	TestScene test_scene;
	test_scene.Load(scene);

	for (auto &i : scene.triangles)
		i.auto_normal();

	int img_siz_x = 1024;
	double img_aspect = 2.39;
	int img_siz_y = (int)(img_siz_x / img_aspect) / 2 * 2;
	int spp = 1;
	vec3 cam_dir = (vec3){0.8, 1, 0}.unit();
	vec3 cam_pos = {-3, -6.5, 1};
	vec3 cam_top = {0, 0, 1};
	double focal = 24;
	double near_clip = 0.1;

	int count = 0;

	int img_width = img_siz_x;
	int img_height = img_siz_y;

	int screen_w = img_siz_x;
	int screen_h = img_siz_y;
	int image_w = img_siz_x;
	int image_h = img_siz_y;

	Timer timer;
	int frame_count = 0;

	VideoEncoder video_encoder("output.mp4", "libx264", img_siz_x, img_siz_y, 20000000, 24, 10);

	for (int i = 1; i <= 24; i++)
	{
		cout << "Frame Begin" << endl;
		Image image(img_siz_x, img_siz_y);

		count++;
		cam_pos.y = -7 + 0.03 * i;

		RenderMain(img_siz_x, img_siz_y, spp, cam_pos, cam_dir, cam_top, focal, near_clip, image, scene);
		cout << "Render End" << endl;

		image.Clamp();
		image.FilpV();
		video_encoder.NewFrame(image);
		image.FilpV();
		frame_count++;
		cout << "Frame " << frame_count << "  AvgFPS " << fixed << setprecision(2) << 1.0 * frame_count / timer.Current() << endl;
	}

	for (int i = 1; i <= 24; i++)
	{
		Image image(img_siz_x, img_siz_y);
		video_encoder.NewFrame(image);
	}

	video_encoder.End();

	return 0;
}
