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

int main(int argc, char *argv[])
{
	fastmath::presolve();

	Scene scene;
	TestScene test_scene;
	test_scene.Load(scene);

	for (auto &i : scene.triangles)
		i.auto_normal();

	int img_siz_x = 512;
	double img_aspect = 2.39;
	int img_siz_y = (int)(img_siz_x / img_aspect) / 2 * 2;
	int spp = 4;
	vec3 cam_dir = (vec3){0.8, 1, 0}.unit();
	vec3 cam_pos = {-3, -6.5, 1};
	vec3 cam_top = {0, 0, 1};
	double focal = 24;
	double near_clip = 0.1;

	int count = 0;

	int img_width = img_siz_x;
	int img_height = img_siz_y;

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
		Image image(img_siz_x, img_siz_y);

		count++;

		double tx = fmod(timer.Current(), 8);
		cam_pos.y = -7 + (tx < 4 ? tx : 0) + (tx >= 4 ? (8 - tx) : 0);

		RenderMain(img_siz_x, img_siz_y, spp, cam_pos, cam_dir, cam_top, focal, near_clip, image, scene);

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
}