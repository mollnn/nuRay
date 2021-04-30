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
#include "objloader.hpp"

int main(int argc, char *argv[])
{
	fastmath::presolve();

	Scene scene;
	TestScene test_scene;

	objl::Loader Loader;
	std::string obj_path = "cube.obj";

	scene.triangles.push_back({{0, -100, 100}, {0, 100, 100}, {100, 100, 100}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0.9, 0.9, 1.5}, 1}});
	scene.triangles.push_back({{0, -100, 100}, {100, -100, 100}, {100, 100, 100}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {1.6, 1.2, 0.9}, 1}});

	scene.triangles.push_back({{-100, -100, -3}, {-100, 100, -3}, {100, 100, -3}, {{0.5, 0.5, 0.5}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, 1}});
	scene.triangles.push_back({{-100, -100, -3}, {100, -100, -3}, {100, 100, -3}, {{0.5, 0.5, 0.5}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, 1}});
	// Load .obj File
	bool loadout = Loader.LoadFile(obj_path);
	for (auto mesh : Loader.LoadedMeshes)
	{
		cout << "New face loaded" << endl;
		for (int i = 0; i < mesh.Vertices.size(); i += 3)
		{
			scene.triangles.push_back({{mesh.Vertices[i + 0].Position.X, mesh.Vertices[i + 0].Position.Y, mesh.Vertices[i + 0].Position.Z},
									   {mesh.Vertices[i + 1].Position.X, mesh.Vertices[i + 1].Position.Y, mesh.Vertices[i + 1].Position.Z},
									   {mesh.Vertices[i + 2].Position.X, mesh.Vertices[i + 2].Position.Y, mesh.Vertices[i + 2].Position.Z},
									   {{1, 1, 1}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, 1}});
		}
	}

	for (auto &i : scene.triangles)
		i.auto_normal();

	int img_siz_x = 1024;
	double img_aspect = 2.39;
	int img_siz_y = img_siz_x / img_aspect;
	int spp = 1;
	vec3 cam_dir = (vec3){0, 1, -1}.unit();
	vec3 cam_pos = {0, -5, 5};
	vec3 cam_top = {0, 1, 1};
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