# Night-Up Ray: Physically-Based Offline Renderer

![sponza_ptnee_640x360x512](https://github.com/mollnn/nuRay/blob/main/docs/imgs/sponza_ptnee_640x360x512.jpg?raw=true)

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

- Primary Sample Space Metropolis Light Transport (PSSMLT) based on naive PT

### Extension

The features below may not work well with environment lighting or some advanced materials like translucent GGX. 

- Bidirectional Path Tracing (BDPT)
 
- Photon Mapping with kd-tree

- Neural Radiance Caching (need fix)
 

### Future Roadmap

Pies in the sky.

- SVGF/RAE Denoiser

- Bump mapping and Tessellation Displacement Mapping

- Quasi Monte Carlo sampler

- SSS Materials with BSSRDF Approximation

- Assimp-based Scene Loader

- Scene hierarchy

- Particle system support


## Dependency

C++17 (gcc recommended), Qt (with OpenGL), OpenMP (optional)


## Usage

This section is about how to use nuRay to synthesize image, providing scene file and some basic parameters. Now we provide Command or GUI interaction mode. API is coming soon.

Example scenes are provided in the directory `scenes`. Due to size issue, we only provided simple ones. You can turn to archives like https://casual-effects.com/data/ or http://www.3drender.com/challenges for more exciting scenes. 

### Command

1. Configure and build project `projects/nuRay.pro`. 

2. Run `nuRay` in a terminal with parameters that describe the scene, output file and render settings. For example:

``` plain
./nuRay  ../scenes/cornell/CornellBox-Original.obj -s 100  --output result.bmp  --params renderer=ptnee imgw=320 imgh=240 spp=16 campos=0,120,200 cameuler=0,0,0 camfov=64 camasp=1.333
```

Parameters can be divided into three parts. 

- The first part is scene description. You can put one or more Wavefront OBJ file path followed by optional parameters describing transform (`-p` for translating, `-s` for scaling). 

- The second part is about where to write result of rendering: `--output` followed by exactly one file path. 

- The last part is rendering parameters, indicating which renderer to use, image size, number of samples per pixel, camera position (x, y, z), camera direction (by Euler angle in degree: yaw, pitch, roll), camera vertical FOV (in degree) and film aspect of camera (film width / height). Extra parameters depends on renderer, such as number of photons for photon mapping, or learning rate for neural radiance cache. Refer to the section **Renderer and Parameters** for more information.


### GUI

![gui](https://github.com/mollnn/nuRay/blob/main/docs/imgs/gui.jpg?raw=true)

1. Configure and build project `projects/nuRay.pro`. 
   
2. Run program `nuRay` with no command parameters. 
   
3. Enter scene description in the text box. Each line for one Wavefront OBJ file path followed by optional parameters describing transform (`-p` for translating, `-s` for scaling). For example:

``` plain
../scenes/cornell/CornellBox-Original.obj -p 0 0 0 -s 100
```

4. Click `Load` button. Models and materials will be loaded and accelerating structures will be built. When it's done, preview with low-resolution can be seen.
   
5. Drag with left or right mouse button pressed, or scroll mouse wheel in the right-top widget of the window to set camera position and direction. You can also control the camera by entering parameters in the correspounding edit box. 

6. Select renderer in the combo box. Edit custom parameters in the correspounding edit box and click `Apply` button. Note that if you write some parameters that already been described in the GUI editor above, such as image width or spp, the custom parameters will simply been overwritten by editor content.

7. Click render button to get result. Most renderers provide interactive feedback of process. Click cancel button if you want to terminate current rendering.

8. Render results (by clicking render button, not draft preview) will be automatically saved in BMP format named by current time in format `HH-MM-SS.bmp` in the working directory. Check or throw them manually if needed.

## Renderer and Parameters

This section provides informations on selecting and customizing renderers. We divide parameters into two categories. Common parameters are shared by most renderers, through which you can take a general control. Specific parameters varies between renderers. Generally, always take care of common parameters, while default value of specific parameters can work well in a lot of cases.

### Common Parameters


| Parameter | Type  | Description                                                                                                                                                                                                     | Default Value |
| --------- | ----- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ------------- |
| renderer  | str   | Renderer you want to use. Candidates including `ptnee`, `pt`, `bdpt`, `pssmlt`, `nrc`.                                                                                                                          | `ptnee`       |
| parallel  | int   | Number of threads if renderer support multithreading.                                                                                                                                                           | 4             |
| blocksize | int   | Size (length of square) of block (tile) if renderer divides task into tile.                                                                                                                                     | 8             |
| imgw      | int   | Image width.                                                                                                                                                                                                    | 1             |
| imgh      | int   | Image height.                                                                                                                                                                                                   | 1             |
| spp       | int   | Number of samples per pixel. For BDPT it is number of light paths or camera paths generated from each pixel. For PSSMLT it is the average number of samples for each pixel. For NRC it is the number of epochs. | 1             |
| campos    | vec3  | Camera position.                                                                                                                                                                                                | (0,0,0)       |
| cameuler  | vec3  | Camera direction (both gaze and up) by Euler angle in degree.                                                                                                                                                   | (0,0,0)       |
| camfov    | float | Vertical field-of-view in degree.                                                                                                                                                                               | 90            |
| camasp    | float | Aspect (width/height) of camera film. In most cases, pixel is square, so camasp just equals to imgw/imgh.                                                                                                       | 1             |
| prr       | float | Probability of Russian Roulette if used. Note that in our implementations we always use fixed live probability.                                                                                                 | 0.8           |

### Path Tracing with Next Event Estimation (default)

No extra parameters.

### Path Tracing (pure)

No extra parameters.

### Bidirectional Path Tracing

![bdpt_cboxo_256.jpg](https://github.com/mollnn/nuRay/blob/main/docs/imgs/bdpt_cboxo_256.jpg?raw=true)

| Parameter | Type | Description                                                                                                                                                                                                                                                                                                  | Default Value |
| --------- | ---- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ | ------------- |
| mis       | bool | Whether to use Multiple Importance Sampling or just assign weight 1/length.                                                                                                                                                                                                                                  | 1             |
| exbuf     | int  | How to extend buffer to show image generated through different sampling techniques. For example, set value to 5 and you will get a matrix of images. Img(0,0) is total output, and Img(i,j) is subimage generated by paths whose light subpath length (number of vertices) is i and eye subpath length is j. | 1             |

### Primary Sample Space Metropolis Light Transport

Note that our PSSMLT is based on pure path tracer without next event estimation.

| Parameter | Type  | Description                                                                  | Default Value |
| --------- | ----- | ---------------------------------------------------------------------------- | ------------- |
| plarge    | float | Probability of large jump.                                                   | 0.3           |
| bsample   | int   | Number of samples to evaluate the overall scaling factor.                    | 10000         |
| s1        | float | Mutation coefficient.                                                        | 1/1024        |
| s2        | float | Mutation coefficient.                                                        | 1/16          |
| s20       | float | Mutation coefficient for first two dims, which are used for coords of pixel. | 1/10          |

### Photon Mapping

![pm_cboxo_1e6_1e3_32.jpg](https://github.com/mollnn/nuRay/blob/main/docs/imgs/pm_cboxo_1e6_1e3_32.jpg?raw=true)

| Parameter | Type | Description        | Default Value |
| --------- | ---- | ------------------ | ------------- |
| n_photons | int  | Number of photons. | 100000        |
| photon_k  | int  | K of KNN query.    | 32            |

### Neural Radiance Caching (experimental)

| Parameter | Type  | Description                                | Default Value |
| --------- | ----- | ------------------------------------------ | ------------- |
| lr        | float | Learning Rate of standard Gradient Descent | 0.001         |

(WIP)

## Hacking scenes

In `scenes\cornell\CornellBox-Mirror-B.obj`, we modify the light source such that most of the scene can only be lighten by reflection of light source in a small corner. Our idea is to wrap the light source so that it can only works as a spot light, and in this way the next event estimator of path tracing will fail, while bidirectional methods working well. 

![hack_cboxb_32spp.jpg](https://github.com/mollnn/nuRay/blob/main/docs/imgs/hack_cboxb_32spp.jpg?raw=true)

## Gallary

Microfacet (Reflect) 

![mitsuba-envmap-512x512x512](https://github.com/mollnn/nuRay/blob/main/docs/imgs/mitsuba-envmap-512x512x512.jpg?raw=true)

Microfacet (Reflect) Gold

![mitsuba_gold_512x512x512](https://github.com/mollnn/nuRay/blob/main/docs/imgs/mitsuba_gold_512x512x512.jpg?raw=true)

Sponza (Area light)

![sponza_512x512x256](https://github.com/mollnn/nuRay/blob/main/docs/imgs/sponza_512x512x256.jpg?raw=true)

Sponza (Pseudo Sun Light)

![sponza_ptnee_640x360x512](https://github.com/mollnn/nuRay/blob/main/docs/imgs/sponza_ptnee_640x360x512.jpg?raw=true)
