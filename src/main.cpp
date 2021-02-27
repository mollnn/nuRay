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

const double pi = acos(-1);
const double eps = 1e-6;

random_device global_random_device;
uniform_int_distribution<int> global_uniform_int_distribution(0, RAND_MAX);

struct Material
{
	vec3 diffuse, emission;
};

struct Triangle
{
	vec3 p0, p1, p2;
	Material mat;
	vec3 n;
	vec3 math_normal() { return ((p1 - p0).cross(p2 - p0)).unit(); }
	vec3 normal() { return n; }
	void auto_normal() { n = math_normal(); }
	std::pair<double, vec3> intersect(vec3 pos, vec3 dir)
	{
		vec3 tpos = p0, tdir = normal();
		if (fabs(tdir.dot(dir)) < eps)
			return {-1, vec3()};
		if (tdir.dot(dir) < 0)
			tdir = -1 * tdir;
		double x = (tpos - pos).dot(tdir) / (tdir.dot(dir));
		if (x < 0)
			return {-1, vec3()};
		vec3 ph = pos + x * dir;
		double dis = (ph - p0).dot(normal());
		vec3 q0 = (p1 - p0).cross(ph - p0);
		vec3 q1 = (p2 - p1).cross(ph - p1);
		vec3 q2 = (p0 - p2).cross(ph - p2);
		if (q0.dot(q1) > 0 && q1.dot(q2) > 0 && q2.dot(q0) > 0)
		{
			return {x, ph};
		}
		else
			return {-1, vec3()};
	}
};

std::tuple<double, vec3, Triangle *> intersect(std::vector<Triangle> &triangles, vec3 pos, vec3 dir)
{
	double hitdis = 2e18;
	vec3 hitpos;
	Triangle *hitobj = NULL;
	for (auto &triangle : triangles)
	{
		auto [thdis, thpos] = triangle.intersect(pos, dir);
		if (thdis > 0 && thdis < hitdis)
		{
			hitdis = thdis;
			hitpos = thpos;
			hitobj = &triangle;
		}
	}
	if (hitdis > 1e18)
		return {-1, hitpos, hitobj};
	return {hitdis, hitpos, hitobj};
}

vec3 PathTrace(vec3 raypos, vec3 raydir, int depth, std::vector<Triangle> &triangles)
{
	if (depth > 10)
		return {0, 0, 0};
	auto [hitdis, hitpos, hitobj] = intersect(triangles, raypos, raydir);
	if (hitdis < 0)
		return {0, 0, 0};
	double r2 = randf();
	double phi = randf() * 2 * pi;
	double sqr2 = sqrt(r2);
	// double cosphi = cos(phi);
	double cosphi = fastmath::get_cos_l(phi);
	double sinphi = sqrt(1 - cosphi * cosphi) * (phi < pi ? 1 : -1);
	double du = sqr2 * cosphi;
	double dv = sqr2 * sinphi;
	double dw = sqrt(1 - r2);
	vec3 normal = hitobj->normal();
	if (raydir.dot(normal) > 0)
		normal = -1 * normal;
	vec3 ew = normal;
	vec3 eu = ((vec3){randf(), randf(), randf()}).unit().cross(normal);
	vec3 ev = eu.cross(ew);
	vec3 difdir = du * eu + dv * ev + dw * ew;
	return hitobj->mat.emission + hitobj->mat.diffuse * PathTrace(hitpos + eps * difdir, difdir, depth + 1, triangles);
}

void render_thread(int img_siz_x, int img_siz_y, int spp, vec3 cam_pos, vec3 cam_dir, vec3 cam_top, double focal, double near_clip,
				   Image &image, vector<Triangle> &scene, double fp_siz_x, double fp_siz_y, vec3 fp_e_x, vec3 fp_e_y,
				   int img_y_min, int img_y_max)
{
	for (int img_y = img_y_min; img_y <= img_y_max; img_y++)
	{
		for (int img_x = 0; img_x < img_siz_x; img_x++)
		{
			for (int t = 0; t < spp; t++)
			{
				double rx = (-1 + 2 * randf());
				double ry = (-1 + 2 * randf());
				double x = img_x + 0.5 * rx;
				double y = img_y + 0.5 * ry;
				vec3 focus_pos = cam_pos + (cam_dir + (x / img_siz_x - 0.5) * fp_siz_x * fp_e_x + (y / img_siz_y - 0.5) * fp_siz_y * fp_e_y) * near_clip;
				vec3 raypos = focus_pos;
				vec3 raydir = (focus_pos - cam_pos).unit();
				vec3 radiance = PathTrace(raypos, raydir, 0, scene);

				image.Add(img_x, img_y, radiance / (1.0 * spp));
			}
		}
	}
}

void render(int img_siz_x, int img_siz_y, int spp, vec3 cam_pos, vec3 cam_dir, vec3 cam_top, double focal, double near_clip,
			Image &image, vector<Triangle> &scene)
{
	double fov = 2 * atan(36 / 2 / focal);
	double fp_siz_x = 2 * tan(fov / 2);
	double fp_siz_y = fp_siz_x * img_siz_y / img_siz_x;
	vec3 fp_e_y = cam_top;
	vec3 fp_e_x = cam_dir.cross(fp_e_y);
	int img_y_step = 32;

	vector<thread *> thread_list;
	for (int img_y = 0; img_y < img_siz_y; img_y += img_y_step)
	{
		int img_y_min = img_y, img_y_max = min(img_y + img_y_step, img_siz_y) - 1;
		thread *th = new thread(render_thread, img_siz_x, img_siz_y, spp, cam_pos, cam_dir, cam_top, focal, near_clip,
								ref(image), ref(scene), fp_siz_x, fp_siz_y, fp_e_x, fp_e_y, img_y_min, img_y_max);
		thread_list.push_back(th);
	}
	for (auto th : thread_list)
	{
		th->join();
	}
	for (auto th : thread_list)
	{
		delete th;
	}

	// 单线程，用于测速参考
	// int img_y_min = 0, img_y_max = img_siz_y - 1;
	// render_thread(img_siz_x, img_siz_y, spp, cam_pos, cam_dir, cam_top, focal, near_clip,
	// 			  ref(image), ref(scene), fp_siz_x, fp_siz_y, fp_e_x, fp_e_y, img_y_min, img_y_max);
}

int main(int argc, char *argv[])
{
	fastmath::presolve();

	std::vector<Triangle> scene;

	scene.push_back({{-0.7, 0, 0}, {0.7, 0, 0}, {0, 0, 1.8}, {{0.8, 0.8, 0.8}, {0, 0, 0}}});	  // 演员
	scene.push_back({{-5, -7, 0}, {5, -7, 0}, {0, -7, 5}, {{0, 0, 0}, {4.5, 2, 0}}});			  // 射灯
	scene.push_back({{-1e2, 1e2, 0}, {1e2, 1e2, 0}, {0, -1e2, 0}, {{0.3, 0.3, 0.3}, {0, 0, 0}}}); // 地板
	scene.push_back({{-10, -10, 3}, {10, -10, 3}, {0, -5, 5}, {{0, 0, 0}, {1, 1.5, 2.2}}});		  // 背景灯
	scene.push_back({{-1e2, 2, 0}, {1e2, 2, 0}, {0, 2, 1e3}, {{0.5, 0.5, 0.5}, {0, 0, 0}}});	  // 幕布

	for (auto &i : scene)
		i.auto_normal();

	int img_siz_x = 1024;
	double img_aspect = 2.39;
	int img_siz_y = img_siz_x / img_aspect;
	int spp = 1;
	vec3 cam_dir = (vec3){0.8, 1, 0.14}.unit();
	vec3 cam_pos = {-3, -6, 0.5};
	vec3 cam_top = {0, 0, 1};
	double focal = 24;
	double near_clip = 0.1;

	Image image(img_siz_x, img_siz_y);

	int count = 0;

	int img_width = image.size_x;
	int img_height = image.size_y;

	SDL_Window *screen;
	SDL_Renderer *sdl_renderer;
	SDL_Texture *sdl_texture;
	SDL_Rect sdl_rect;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER))
	{
		printf("Could not initialize SDL - %s\n", SDL_GetError());
		return -1;
	}

	int screen_w = img_siz_x;
	int screen_h = img_siz_y;
	int image_w = img_siz_x;
	int image_h = img_siz_y;

	screen = SDL_CreateWindow("Player", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
							  screen_w, screen_h,
							  SDL_WINDOW_OPENGL);

	sdl_renderer = SDL_CreateRenderer(screen, -1, 0);
	sdl_texture = SDL_CreateTexture(sdl_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, img_siz_x, img_siz_y);
	sdl_rect.x = 0;
	sdl_rect.y = 0;
	sdl_rect.w = screen_w;
	sdl_rect.h = screen_h;

	Timer timer;
	int frame_count = 0;

	//Main loop
	bool flag_running = true;

	while (flag_running)
	{

		for (int i = 0; i < img_siz_x; i++)
		{
			for (int j = 0; j < img_siz_y; j++)
			{
				image.buffer[i][j] = image.buffer[i][j] * count;
			}
		}

		count++;
		render(img_siz_x, img_siz_y, spp, cam_pos, cam_dir, cam_top, focal, near_clip, image, scene);

		for (int i = 0; i < img_siz_x; i++)
		{
			for (int j = 0; j < img_siz_y; j++)
			{
				image.buffer[i][j] = image.buffer[i][j] * 1.0 / count;
			}
		}

		image.Clamp();
		image.FilpV();

		uint8_t *pixels = new uint8_t[image_h * image_w * 4];
		for (int y = 0; y < image_h; y++)
		{
			for (int x = 0; x < image_w; x++)
			{
				vec3 vec = image.Get(x, y);

				uint8_t r = vec.x * 255;
				uint8_t g = vec.y * 255;
				uint8_t b = vec.z * 255;

				pixels[y * image_w * 4 + x * 4 + 0] = 0;
				pixels[y * image_w * 4 + x * 4 + 1] = b;
				pixels[y * image_w * 4 + x * 4 + 2] = g;
				pixels[y * image_w * 4 + x * 4 + 3] = r;
			}
		}

		SDL_UpdateTexture(sdl_texture, &sdl_rect, pixels, image_w * 4);

		delete[] pixels;

		SDL_RenderClear(sdl_renderer);
		SDL_RenderCopy(sdl_renderer, sdl_texture, NULL, &sdl_rect);
		SDL_RenderPresent(sdl_renderer);

		image.FilpV();

		frame_count++;
		cout << "Frame " << frame_count << "  AvgFPS " << fixed << setprecision(2) << 1.0 * frame_count / timer.Current() << endl;

		SDL_Event sdl_event;
		while (SDL_PollEvent(&sdl_event))
		{
			if (sdl_event.type == SDL_QUIT)
			{
				flag_running = false;
			}
			if (sdl_event.type == SDL_KEYUP)
			{
				if (sdl_event.key.keysym.sym == SDLK_ESCAPE)
				{
					flag_running = false;
				}
			}
		}
	}

	SDL_Quit();

	image.WriteToTGA("output.tga");
}