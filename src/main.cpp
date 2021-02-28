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

vec3 PathTrace(vec3 raypos, vec3 raydir, int depth, const std::vector<Triangle> &triangles)
{
	if (depth > 10)
		return {0, 0, 0};
	auto [hitdis, hitpos, hitobj] = Intersect(triangles, raypos, raydir);
	if (hitdis < 0)
		return {0, 0, 0};

	vec3 normal_outward = hitobj->normal();
	bool is_into = raydir.dot(normal_outward) < 0;				  // 是否在射入内部
	vec3 normal = is_into ? normal_outward : -1 * normal_outward; // 实际光学效应的法线
	double relative_refract_index = is_into ? hitobj->mat.refrect_index : 1.0 / hitobj->mat.refrect_index;

	vec3 in_dir = raydir;								  // 光线入射方向
	double cos_i = abs(in_dir.dot(normal));				  // 入射角 cos
	double sin_i = sqrt(1 - cos_i * cos_i);				  // 入射角 sin
	double sin_j = sin_i / relative_refract_index;		  // 折射角 sin
	double cos_j = sqrt(1 - sin_j * sin_j);				  // 折射角 cos
	vec3 base_dir = (in_dir + cos_i * normal).unit();	  // 入射面中表面的切线方向
	vec3 reflect_dir = (in_dir + 2 * cos_i * normal);	  // 反射光线方向
	vec3 refrect_dir = sin_j * base_dir - cos_j * normal; // 折射光线方向

	double refrect_index_in = is_into ? 1.0 : hitobj->mat.refrect_index;								   // 入射介质折射率
	double refrect_index_out = is_into ? hitobj->mat.refrect_index : 1.0;								   // 折射介质折射率
	double refrect_index_sum = refrect_index_in + refrect_index_out;									   // 折射率之和
	double refrect_index_delta = refrect_index_in - refrect_index_out;									   // 折射率之差
	double fresnel_i0 = refrect_index_delta * refrect_index_delta / refrect_index_sum / refrect_index_sum; // 菲涅尔反射光强系数
	double fresnel_x = 1 - cos_i;																		   // 菲涅尔反射光强因子

	// 特殊处理全反射情况
	if (sin_j > 1 - 1e-6)
		fresnel_x = 1;
	if (isnanf(fresnel_x))
		fresnel_x = 1;

	const Material &material = hitobj->mat; // 命中物体的材质

	double fresnel_reflect_intensity = fresnel_i0 + (1 - fresnel_i0) * pow(fresnel_x, 5); // 菲涅尔反射强度
	double fresnel_refrect_intensity = 1 - fresnel_reflect_intensity;					  // 菲涅尔折射强度

	// 搜索各种光线的概率
	double reflect_probability = material.reflect.avg() + material.refrect.avg() * fresnel_reflect_intensity; // 总镜面反射概率
	double refrect_probability = material.refrect.avg() * fresnel_refrect_intensity;						  // 总折射概率
	double diffuse_probability = material.diffuse.avg();													  // 总漫反射概率

	double sum_probability = diffuse_probability + reflect_probability + refrect_probability + eps;
	diffuse_probability /= sum_probability;
	reflect_probability /= sum_probability;
	refrect_probability /= sum_probability;

	// 光线强度
	vec3 reflect_intensity = material.reflect + material.refrect * fresnel_reflect_intensity; // 总镜面反射强度
	vec3 refrect_intensity = material.refrect * fresnel_refrect_intensity;					  // 总折射强度
	vec3 diffuse_intensity = material.diffuse;												  // 总漫反射强度

	vec3 ans = hitobj->mat.emission;

	double rand_value = randf();
	if (rand_value < diffuse_probability)
	{
		double r2 = randf();
		double phi = randf() * 2 * pi;
		double sqr2 = sqrt(r2);
		double cosphi = fastmath::get_cos_l(phi);
		double sinphi = sqrt(1 - cosphi * cosphi) * (phi < pi ? 1 : -1);
		double du = sqr2 * cosphi;
		double dv = sqr2 * sinphi;
		double dw = sqrt(1 - r2);
		if (raydir.dot(normal) > 0)
			normal = -1 * normal;
		vec3 ew = normal;
		vec3 eu = ((vec3){randf(), randf(), randf()}).unit().cross(normal);
		vec3 ev = eu.cross(ew);
		vec3 difdir = du * eu + dv * ev + dw * ew;
		ans = ans + hitobj->mat.diffuse * PathTrace(hitpos + eps * difdir, difdir, depth + 1, triangles);
	}
	else if (rand_value - diffuse_probability < reflect_probability)
	{
		ans = ans + PathTrace(hitpos + eps * reflect_dir, reflect_dir, depth + 1, triangles) * reflect_intensity / reflect_probability;
	}
	else if (rand_value - diffuse_probability - reflect_probability < refrect_probability)
	{
		ans = ans + PathTrace(hitpos + eps * refrect_dir, refrect_dir, depth + 1, triangles) * refrect_intensity / refrect_probability;
	}
	return ans;
}

void RenderThread(int img_siz_x, int img_siz_y, int spp, vec3 cam_pos, vec3 cam_dir, vec3 cam_top, double focal, double near_clip,
				  Image &image, const vector<Triangle> &scene, double fp_siz_x, double fp_siz_y, vec3 fp_e_x, vec3 fp_e_y,
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

void RenderMain(int img_siz_x, int img_siz_y, int spp, vec3 cam_pos, vec3 cam_dir, vec3 cam_top, double focal, double near_clip,
				Image &image, const vector<Triangle> &scene)
{
	double fov = 2 * atan(36 / 2 / focal);
	double fp_siz_x = 2 * tan(fov / 2);
	double fp_siz_y = fp_siz_x * img_siz_y / img_siz_x;
	vec3 fp_e_y = cam_top;
	vec3 fp_e_x = cam_dir.cross(fp_e_y);
	int img_y_step = (img_siz_y + 7) / 8;

	vector<thread *> thread_list;
	for (int img_y = 0; img_y < img_siz_y; img_y += img_y_step)
	{
		int img_y_min = img_y, img_y_max = min(img_y + img_y_step, img_siz_y) - 1;
		thread *th = new thread(RenderThread, img_siz_x, img_siz_y, spp, cam_pos, cam_dir, cam_top, focal, near_clip,
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

	scene.push_back({{-0.7 - 0.5, 0, 0}, {0.7 - 0.5, 0, 0}, {0 - 0.5, 0, 1.8}, {{0.9, 0.9, 0.8}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, 1}});						 // 演员
	scene.push_back({{-5, -7, 0}, {5, -7, 0}, {0, -7, 5}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {6, 2.5, 0}, 1}});												 // 射灯
	scene.push_back({{-1e2, 1e2, 0}, {1e2, 1e2, 0}, {0, -1e2, 0}, {{0.3, 0.32, 0.4}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, 1}});									 // 地板
	scene.push_back({{-10, -10, 3}, {10, -10, 3}, {0, -5, 5}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {1, 2, 3}, 1}});											 // 背景灯
	scene.push_back({{-1e2, 2, 0}, {1e2, 2, 0}, {0, 2, 1e3}, {{0.5, 0.6, 0.6}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, 1}});										 // 幕布
	scene.push_back({{2 + 0.00 * 3, 2, 0}, {2 + 0.01 * 3, 1.9, 0}, {2 + 0.00 * 3, 2, 1e3}, {{0.3, 0.2, 0.2}, {0.3, 0.2, 0.2}, {0, 0, 0}, {0, 0, 0}, 1}});	 // 镜子边框
	scene.push_back({{2 + 0.49 * 3, -2.9, 0}, {2 + 0.50 * 3, -3, 0}, {2 + 0.50 * 3, -3, 1e3}, {{0.3, 0.2, 0.2}, {0.3, 0.2, 0.2}, {0, 0, 0}, {0, 0, 0}, 1}}); // 镜子边框
	scene.push_back({{2 + 0.49 * 3, -2.9, 0}, {2 + 0.01 * 3, 1.9, 0}, {2 + 0.20 * 3, 0, 1e3}, {{0.1, +.1, 0.1}, {0.6, 0.6, 0.6}, {0, 0, 0}, {0, 0, 0}, 1}}); // 镜子
	scene.push_back({{0, 0 - 2, 0}, {0, 1 - 2, 0}, {1, 0 - 2, 0}, {{0, 0, 0}, {0.1, 0.1, 0.1}, {0.8, 0.8, 0.8}, {0, 0, 0}, 1.5}});							 // 四面体
	scene.push_back({{0, 0 - 2, 0}, {0, 0 - 2, 1}, {0, 1 - 2, 0}, {{0, 0, 0}, {0.1, 0.1, 0.1}, {0.8, 0.8, 0.8}, {0, 0, 0}, 1.5}});							 // 四面体
	scene.push_back({{0, 0 - 2, 0}, {1, 0 - 2, 0}, {0, 0 - 2, 1}, {{0, 0, 0}, {0.1, 0.1, 0.1}, {0.8, 0.8, 0.8}, {0, 0, 0}, 1.5}});							 // 四面体
	scene.push_back({{1, 0 - 2, 0}, {0, 1 - 2, 0}, {0, 0 - 2, 1}, {{0, 0, 0}, {0.1, 0.1, 0.1}, {0.8, 0.8, 0.8}, {0, 0, 0}, 1.5}});							 // 四面体

	for (auto &i : scene)
		i.auto_normal();

	int img_siz_x = 1024;
	double img_aspect = 2.39;
	int img_siz_y = img_siz_x / img_aspect;
	int spp = 1;
	vec3 cam_dir = (vec3){0.8, 1, 0.05}.unit();
	vec3 cam_pos = {-3, -6, 1};
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
		RenderMain(img_siz_x, img_siz_y, spp, cam_pos, cam_dir, cam_top, focal, near_clip, image, scene);

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