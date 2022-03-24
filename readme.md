# Night-Up Ray: Physically-based Offline Renderer

## Feature Set

### Basic

- Simple custom scene description format to define geometry, lighting and material together with .obj and .mtl files

- Monte Carlo samplers

- Bounding Volume Hierarchy with Surface Area Heuristic

- Area (mesh) lights

- Path tracing integrator with Russian Roulette (RR) and Next Event Estimator (NEE, Direct lighting integrator)

- Importance sampling of BSDF/NDF

- Microfacet-based material (GGX with Smith-G)

- Texturing with Bilinear interpolation

- Environment Mapping (IBL)

- CPU parallelization via multithreading

- (Disabled now) Denoising (JBF)

- Translucent Materials

- GUI with Qt, Raster-based preview via OpenGL and interactive camera control

- Primary Sample Space Metropolis Light Transport (PSSMLT)

### Extension

The features below may not work well with environment lighting or some advanced materials like translucent GGX. 

- Bidirectional Path Tracing (BDPT)
 
- Photon Mapping with kd-tree

- Neural Radiance Caching (need fix)
 
### WIP

These features is coming.

- SVGF Denoiser

- RAE Denoiser

- Bump mapping

- Tessellation Displacement Mapping

- Quasi Monte Carlo sampler

### Future

Pies in the sky.

- SSS Materials with BSSRDF Approximation

- Assimp-based Scene Loader

- Scene hierarchy

- Particle system support


## Dependency

C++17 (gcc recommended), Qt (with OpenGL), OpenMP (optional)


## Usage

Now we only provide Command or GUI interaction mode. API is coming soon.


### CLI

1. Configure and build project `projects/nuRay.pro`. 

2. Run `nuRay` in a terminal with parameters that describe the scene, output file and render settings. For example:

``` plain
./nuRay  ../scenes/cornell/CornellBox-Mirror.obj -s 100  --output result.bmp  --params renderer=ptnee imgw=320 imgh=240 spp=16 campos=0,120,200 cameuler=0,0,0 camfov=64 camasp=1.333
```

Parameters can be divided into three parts. 

- The first part is scene description. You can put one or more Wavefront OBJ file path followed by optional parameters describing transform (`-p` for translating, `-s` for scaling). 

- The second part is about where to write result of rendering: `--output` followed by exactly one file path. 

- The last part is rendering parameters, indicating which renderer to use, image size, number of samples per pixel, camera position, camera direction, camera vertical FOV and film aspect of camera. Extra parameters depends on renderer, such as number of photons for photon mapping, or learning rate for neural radiance cache. Refer to the section **Renderer and Parameters** for more information.


### GUI

1. Configure and build project `projects/nuRay.pro`. 
   
2. Run program `nuRay` with no command parameters. 
   
3. Enter scene description in the text box. Each line for one Wavefront OBJ file path followed by optional parameters describing transform (`-p` for translating, `-s` for scaling). For example:

``` plain
../scenes/cornell/CornellBox-Mirror.obj -p 0 0 0 -s 100
```

4. Click `Load` button. Models and materials will be loaded and accelerating structures will be built. When it's done, preview with low-resolution can be seen.
   
5. Draw with left or right mouse button pressed in the right-top widget of the window to set camera position and direction. You can also control the camera by entering parameters in the correspounding edit box. 

6. Select renderer in the combo box. Edit parameters in the correspounding edit box.

7. Click render button to get result. Most renderers provide interactive feedback of process. Click cancel button if you want to terminate current rendering.

8. Render results (by clicking render button, not draft preview) will be automatically saved in BMP format named by current time in format `HH-MM-SS.bmp` in the working directory. Check or throw them manually if needed.



## Gallary

![mitsuba-envmap-512x512x512](https://github.com/mollnn/nuRay/blob/main/docs/imgs/mitsuba-envmap-512x512x512.jpg?raw=true)

![sponza_512x512x256](https://github.com/mollnn/nuRay/blob/main/docs/imgs/sponza_512x512x256.jpg?raw=true)

![mitsuba_gold_512x512x512](https://github.com/mollnn/nuRay/blob/main/docs/imgs/mitsuba_gold_512x512x512.jpg?raw=true)