# RaySDL

RaySDL is a CPU ray tracer based off of the book "Ray Tracing in One Weekend" by Peter Shirley.
This project uses the SDL2 rendering library with imgui to display a simple scene in real time.

***

## Controls

All spheres within the scene can be controlled using the wasd keys. There's a button in the
GUI to toggle between each sphere. You can add/remove spheres as well as change their colors.

***

## Rendering

When a ray intersects an object in the scene, depending on the object's material, the ray will
bounce at a certain angle, which is influenced by a random value. 

There are three rendering modes available that slightly change how the scene is rendered:

1. **Regular**: The default. When a ray intersects an object, it will at an angle which is a
   combination of the surface normal and a random value.

2. **PCG**: This is mostly the same as the regular mode, but the random value is generated using
   the PCG random number generator, seeded with the ray's original coordinates. This makes the
   ray bounce at the same angle every time it intersects the same object.

3. **Multi-threaded**: This mode uses multiple threads to render the scene. The viewport is
   divided into a grid of tiles, which are then added to a queue. Each thread will take a tile
   and once it's done, it will take another one. This mode is by far the fastest, but only
   when compilier optimizations are disabled. When optimizations are set to -O3, the regular
   and pcg modes are faster.

The actual rendering is done using the same algorithm, and the final color for each pixel on
the screen is packed into a uint32_t, which is then written to the screen buffer. To help speed
up packing the color for the entire scene, SIMD instructions are used to pack 4 colors at a 
time.
