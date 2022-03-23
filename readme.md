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

Now we only provide GUI interaction mode. CLI support is coming soon.

### GUI

1. Configure and build project `projects/nuRay.pro`. Run program `nuRay` with no command parameters. 
2. Enter scene description in the text box. Each line for one Wavefront OBJ file path followed by optional parameters describing transform (`-p` for translating, `-s` for scaling). For example:

``` plain
../scenes/cornell/CornellBox-Mirror.obj -p 0 0 0 -s 100
```

3. Click `Load` button. Models and materials will be loaded and accelerating structures will be built. When it's done, preview with low-resolution can be seen.

4. 


### CLI

WIP...


## Gallary

![mitsuba-envmap-512x512x512](https://github.com/mollnn/nuRay/blob/main/docs/imgs/mitsuba-envmap-512x512x512.jpg?raw=true)

![sponza_512x512x256](https://github.com/mollnn/nuRay/blob/main/docs/imgs/sponza_512x512x256.jpg?raw=true)

![mitsuba_gold_512x512x512](https://github.com/mollnn/nuRay/blob/main/docs/imgs/mitsuba_gold_512x512x512.jpg?raw=true)