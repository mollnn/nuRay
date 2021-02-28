# minipt 多线程支持 (std::thread)

用 std::thread 实现了 minipt 的多线程

这里其实比较简单，因为并没有什么需要上锁的东西，对输出图像的修改本来就是互斥的嘛

主要是要解决一下随机数的问题，如果照抄原来代码，发现每帧跑出来都是一样的

这里我们用了 std::random_device 和 std::uniform_int_distribution 来解决问题

重新封装后的随机函数

``` cpp
random_device global_random_device;
uniform_int_distribution<int> global_uniform_int_distribution(0, RAND_MAX);

double randf() { return 1.0f * global_uniform_int_distribution(global_random_device) / RAND_MAX; }
```

渲染控制主线程

``` cpp
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
```

渲染子线程

``` cpp
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
```

测试结果单线程 3.2 fps, 双线程 5.3 fps