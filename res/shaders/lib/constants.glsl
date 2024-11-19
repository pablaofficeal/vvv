#ifndef CONSTANTS_GLSL_
#define CONSTANTS_GLSL_

#define PI 3.1415926535897932384626433832795
#define PI2 (PI*2)

// geometry
#define CURVATURE_FACTOR 0.002

// lighting
#define SKY_LIGHT_MUL 2.9
#define SKY_LIGHT_TINT vec3(0.9, 0.8, 1.0)
#define MAX_SKY_LIGHT vec3(0.1, 0.11, 0.14)

// fog
#define FOG_POS_SCALE vec3(1.0, 0.2, 1.0)

#endif // CONSTANTS_GLSL_
