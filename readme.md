# Night-Up Ray: Physical-based Path Tracer

## Dependency

C++17, Qt (with OpenGL for GUI and raster-based quick preview), OpenMP (optional)

## Feature Set

- Simple custom scene description format to define geometry, lighting and material together with .obj and .mtl files

- Monte Carlo samplers

- Bounding volume hierarchy (BVH): Surface Area Heuristic

- Area (mesh) lights

- Direct lighting integrator (sampling light)

- Path tracing integrator with Russian roulette

- Importance sampling for Lambert and Blinn-Phong (viewing as Microfacet-based)

- Microfacet-based material (GGX with Smith-G), importance sampling based on NDF function

- Texturing with Bilinear interpolation

- CPU parallelization via multithreading (OpenMP)

- GUI with Qt, Raster-based preview via OpenGL and interactive camera control


### WIP

- Environment Mapping (IBL)

- Transmission Materials

- Bump mapping

- Denoising


## Gallary

![](https://github.com/mollnn/nuRay/blob/main/docs/imgs/sponza_512x512x256.jpg?raw=true)

![](https://github.com/mollnn/nuRay/blob/main/docs/imgs/mitsuba_gold_512x512x512.jpg?raw=true)