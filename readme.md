# Night-Up Ray: Physically-based Offline Renderer

## Dependency

C++17, Qt (with OpenGL for GUI and raster-based quick preview), OpenMP (optional)

## Feature Set

### Basic

- Simple custom scene description format to define geometry, lighting and material together with .obj and .mtl files

- Monte Carlo samplers

- Bounding volume hierarchy: Surface Area Heuristic

- Area (mesh) lights

- Path tracing integrator with Russian Roulette (RR) and Next Event Estimator (NEE, Direct lighting integrator)

- Importance sampling of BSDF

- Microfacet-based material (GGX with Smith-G), importance sampling based on NDF function

- Texturing with Bilinear interpolation

- Environment Mapping (IBL)

- CPU parallelization via multithreading

- (Disabled now) Denoising (JBF)

- Translucent Materials

- GUI with Qt, Raster-based preview via OpenGL and interactive camera control

- Primary Sample Space Metropolis Light Transport (PSSMLT)

### Extension

The features below may not work well with environment lighting or some advanced materials like translucent GGX. 

- Bidirectional Path Tracing (BDPT) (need fix)
 
- Photon Mapping with kd-tree

- Neural Radiance Caching (need to be replaced with Matrix-based NN implementation, replace terminate strategy, replace loss function)
 
### WIP

These features is coming.

- JSON Config of Renderer (life with GUI is too tough 555)

- Adjustment of Directory Structure 

- Continuous mode 

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


## Gallary

![mitsuba-envmap-512x512x512](https://github.com/mollnn/nuRay/blob/main/docs/imgs/mitsuba-envmap-512x512x512.jpg?raw=true)

![sponza_512x512x256](https://github.com/mollnn/nuRay/blob/main/docs/imgs/sponza_512x512x256.jpg?raw=true)

![mitsuba_gold_512x512x512](https://github.com/mollnn/nuRay/blob/main/docs/imgs/mitsuba_gold_512x512x512.jpg?raw=true)