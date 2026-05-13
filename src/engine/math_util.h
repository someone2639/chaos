#ifndef MATH_UTIL_H
#define MATH_UTIL_H

#include <PR/ultratypes.h>

#include "types.h"

/*
 * The sine and cosine tables overlap, but "#define gCosineTable (gSineTable +
 * 0x400)" doesn't give expected codegen; gSineTable and gCosineTable need to
 * be different symbols for code to match. Most likely the tables were placed
 * adjacent to each other, and gSineTable cut short, such that reads overflow
 * into gCosineTable.
 *
 * These kinds of out of bounds reads are undefined behavior, and break on
 * e.g. GCC (which doesn't place the tables next to each other, and probably
 * exploits array sizes for range analysis-based optimizations as well).
 * Thus, for non-IDO compilers we use the standard-compliant version.
 */
extern f32 gSineTable[];
#ifdef AVOID_UB
#define gCosineTable (gSineTable + 0x400)
#else
extern f32 gCosineTable[];
#endif

#define sins(x) gSineTable[(u16) (x) >> 4]
#define coss(x) gCosineTable[(u16) (x) >> 4]

#define min(a, b) ((a) <= (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

#define sqr(x) ((x) * (x))

#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))

#define NEAR_ZERO   __FLT_EPSILON__
#define NEAR_ONE    (1.0f - __FLT_EPSILON__)

#define FLOAT_ONE   0x3F800000

// from limits.h
#define S8_MAX __SCHAR_MAX__
#define S8_MIN (-S8_MAX - 1)
#define U8_MAX (S8_MAX * 2 + 1)
#define S16_MAX __SHRT_MAX__
#define S16_MIN (-S16_MAX - 1)
#define U16_MAX (S16_MAX * 2 + 1)
#define S32_MAX __INT_MAX__
#define S32_MIN (-S32_MAX - 1)
#define U32_MAX (S32_MAX * 2U + 1U)
#define S64_MAX __LONG_LONG_MAX__
#define S64_MIN (-S64_MAX - 1LL)
#define U64_MAX (S64_MAX * 2ULL + 1ULL)
#define F32_MAX __FLT_MAX__
#define F32_MIN __FLT_MIN__
#define F64_MAX __DBL_MAX__
#define F64_MIN __DBL_MIN__

#define CLAMP_U8( x)        CLAMP((x),     0x0,  U8_MAX)
#define CLAMP_S8( x)        CLAMP((x),  S8_MIN,  S8_MAX)
#define CLAMP_U16(x)        CLAMP((x),     0x0, U16_MAX)
#define CLAMP_S16(x)        CLAMP((x), S16_MIN, S16_MAX)
#define CLAMP_U32(x)        CLAMP((x),     0x0, U32_MAX)
#define CLAMP_S32(x)        CLAMP((x), S32_MIN, S32_MAX)
#define CLAMP_U64(x)        CLAMP((x),     0x0, U64_MAX)
#define CLAMP_S64(x)        CLAMP((x), S64_MIN, S64_MAX)
#define CLAMP_F32(x)        CLAMP((x), F32_MIN, F32_MAX)
#define CLAMP_F64(x)        CLAMP((x), F64_MIN, F64_MAX)

// Get the sign of a number
#define signum_positive(x) (((x) < 0) ? -1 : 1)


// Vector operations

// Set all elements of a vector to the same constant
#define vec2_same(v, s)     (((v)[0]) = ((v)[1])                       = (s))
#define vec3_same(v, s)     (((v)[0]) = ((v)[1]) = ((v)[2])            = (s))
#define vec4_same(v, s)     (((v)[0]) = ((v)[1]) = ((v)[2]) = ((v)[3]) = (s))

// Set all elements of a vector to zero
#define vec2_zero(v)        (vec2_same((v), 0))
#define vec3_zero(v)        (vec3_same((v), 0))
#define vec4_zero(v)        (vec4_same((v), 0))

// Sum of the squares of all elements of a vector
#define vec2_sumsq(v)       (sqr((v)[0]) + sqr((v)[1]))
#define vec3_sumsq(v)       (sqr((v)[0]) + sqr((v)[1]) + sqr((v)[2]))
#define vec4_sumsq(v)       (sqr((v)[0]) + sqr((v)[1]) + sqr((v)[2]) + sqr((v)[3]))

// Calculate the magnitude of a vector
#define vec2_mag(v)         (sqrtf(vec2_sumsq(v)))
#define vec3_mag(v)         (sqrtf(vec3_sumsq(v)))
#define vec4_mag(v)         (sqrtf(vec4_sumsq(v)))

/**
 * Set 'dest' to the normal vector of a triangle with vertices a, b and c.
 * Equivalent to cross((c-b), (c-a)).
 */
#define find_vector_perpendicular_to_plane(dest, a, b, c) {                                     \
    (dest)[0] = ((b)[1] - (a)[1]) * ((c)[2] - (b)[2]) - ((c)[1] - (b)[1]) * ((b)[2] - (a)[2]);  \
    (dest)[1] = ((b)[2] - (a)[2]) * ((c)[0] - (b)[0]) - ((c)[2] - (b)[2]) * ((b)[0] - (a)[0]);  \
    (dest)[2] = ((b)[0] - (a)[0]) * ((c)[1] - (b)[1]) - ((c)[0] - (b)[0]) * ((b)[1] - (a)[1]);  \
}

// Set the elements of vector 'dst' to the given values
#define vec2_set(dst, x, y) {           \
    (dst)[0] = (x);                     \
    (dst)[1] = (y);                     \
}
#define vec3_set(dst, x, y, z) {        \
    (dst)[0] = (x);                     \
    (dst)[1] = (y);                     \
    (dst)[2] = (z);                     \
}
#define vec4_set(dst, x, y, z, w) {     \
    (dst)[0] = (x);                     \
    (dst)[1] = (y);                     \
    (dst)[2] = (z);                     \
    (dst)[3] = (w);                     \
}

#define vec3f_set vec3_set
#define vec3i_set vec3_set
#define vec3s_set vec3_set

// Copy vector 'src' to vector 'dst'
#define vec2_copy(dst, src) {           \
    __auto_type _x = (src)[0];          \
    __auto_type _y = (src)[1];          \
    (dst)[0] = _x;                      \
    (dst)[1] = _y;                      \
}
#define vec3_copy(dst, src) {           \
    __auto_type _x = (src)[0];          \
    __auto_type _y = (src)[1];          \
    __auto_type _z = (src)[2];          \
    (dst)[0] = _x;                      \
    (dst)[1] = _y;                      \
    (dst)[2] = _z;                      \
}
#define vec4_copy(dst, src) {           \
    __auto_type _x = (src)[0];          \
    __auto_type _y = (src)[1];          \
    __auto_type _z = (src)[2];          \
    __auto_type _w = (src)[3];          \
    (dst)[0] = _x;                      \
    (dst)[1] = _y;                      \
    (dst)[2] = _z;                      \
    (dst)[3] = _w;                      \
}

#define vec3f_copy vec3_copy
#define vec3i_copy vec3_copy
#define vec3s_copy vec3_copy
#define vec3s_to_vec3i vec3_copy
#define vec3s_to_vec3f vec3_copy
#define vec3i_to_vec3s vec3_copy
#define vec3i_to_vec3f vec3_copy
#define vec3f_to_vec3s vec3_copy
#define vec3f_to_vec3i vec3_copy

#define surface_normal_to_vec3f(dst, surf) vec3f_copy((dst), &((surf)->normal.x))

// Copy vector 'src' to vector 'dst' and add a scalar to the y component
#define vec3_copy_y_off(dst, src, y) {  \
    __auto_type _x = (src)[0];          \
    __auto_type _y = (src)[1] + (y);    \
    __auto_type _z = (src)[2];          \
    (dst)[0] = _x;                      \
    (dst)[1] = _y;                      \
    (dst)[2] = _z;                      \
}

#define vec3f_copy_y_off vec3_copy_y_off

// Set vector 'dst' to the sum of vectors 'src1' and 'src2'
#define vec2_sum(dst, src1, src2) {         \
    __auto_type _x = (src1)[0] + (src2)[0]; \
    __auto_type _y = (src1)[1] + (src2)[1]; \
    (dst)[0] = _x;                          \
    (dst)[1] = _y;                          \
}
#define vec3_sum(dst, src1, src2) {         \
    __auto_type _x = (src1)[0] + (src2)[0]; \
    __auto_type _y = (src1)[1] + (src2)[1]; \
    __auto_type _z = (src1)[2] + (src2)[2]; \
    (dst)[0] = _x;                          \
    (dst)[1] = _y;                          \
    (dst)[2] = _z;                          \
}
#define vec4_sum(dst, src1, src2) {         \
    __auto_type _x = (src1)[0] + (src2)[0]; \
    __auto_type _y = (src1)[1] + (src2)[1]; \
    __auto_type _z = (src1)[2] + (src2)[2]; \
    __auto_type _w = (src1)[3] + (src2)[3]; \
    (dst)[0] = _x;                          \
    (dst)[1] = _y;                          \
    (dst)[2] = _z;                          \
    (dst)[3] = _w;                          \
}

#define vec3f_sum vec3_sum
#define vec3i_sum vec3_sum
#define vec3s_sum vec3_sum

// Add the vector 'src' to vector 'dst'
#define vec2_add(dst, src) vec2_sum((dst), (dst), (src))
#define vec3_add(dst, src) vec3_sum((dst), (dst), (src))
#define vec4_add(dst, src) vec4_sum((dst), (dst), (src))

#define vec3f_add vec3_add
#define vec3i_add vec3_add
#define vec3s_add vec3_add

// Set vector 'dst' to the difference of vectors 'src1' and 'src2'
#define vec2_diff(dst, src1, src2) {        \
    __auto_type _x = (src1)[0] - (src2)[0]; \
    __auto_type _y = (src1)[1] - (src2)[1]; \
    (dst)[0] = _x;                          \
    (dst)[1] = _y;                          \
}
#define vec3_diff(dst, src1, src2) {        \
    __auto_type _x = (src1)[0] - (src2)[0]; \
    __auto_type _y = (src1)[1] - (src2)[1]; \
    __auto_type _z = (src1)[2] - (src2)[2]; \
    (dst)[0] = _x;                          \
    (dst)[1] = _y;                          \
    (dst)[2] = _z;                          \
}
#define vec4_diff(dst, src1, src2) {        \
    __auto_type _x = (src1)[0] - (src2)[0]; \
    __auto_type _y = (src1)[1] - (src2)[1]; \
    __auto_type _z = (src1)[2] - (src2)[2]; \
    __auto_type _w = (src1)[3] - (src2)[3]; \
    (dst)[0] = _x;                          \
    (dst)[1] = _y;                          \
    (dst)[2] = _z;                          \
    (dst)[3] = _w;                          \
}

#define vec3f_diff vec3_diff
#define vec3i_diff vec3_diff
#define vec3s_diff vec3_diff

// Subtract the vector 'src' from vector 'dst'
#define vec2_sub(dst, src) vec2_diff((dst), (dst), (src))
#define vec3_sub(dst, src) vec3_diff((dst), (dst), (src))
#define vec4_sub(dst, src) vec4_diff((dst), (dst), (src))

#define vec3f_sub vec3_sub
#define vec3i_sub vec3_sub
#define vec3s_sub vec3_sub

// Set vector 'dst' to vector 'src' scaled by the scalar 'x'
#define vec2_scale_dest(dst, src, x) {  \
    __auto_type _x = (src)[0] * (x);    \
    __auto_type _y = (src)[1] * (x);    \
    (dst)[0] = _x;                      \
    (dst)[1] = _y;                      \
}
#define vec3_scale_dest(dst, src, x) {  \
    __auto_type _x = (src)[0] * (x);    \
    __auto_type _y = (src)[1] * (x);    \
    __auto_type _z = (src)[2] * (x);    \
    (dst)[0] = _x;                      \
    (dst)[1] = _y;                      \
    (dst)[2] = _z;                      \
}
#define vec4_scale_dest(dst, src, x) {  \
    __auto_type _x = (src)[0] * (x);    \
    __auto_type _y = (src)[1] * (x);    \
    __auto_type _z = (src)[2] * (x);    \
    __auto_type _w = (src)[3] * (x);    \
    (dst)[0] = _x;                      \
    (dst)[1] = _y;                      \
    (dst)[2] = _z;                      \
    (dst)[3] = _w;                      \
}

// Scale vector 'dst' by the scalar 'x'
#define vec2_scale(dst, x) vec2_scale_dest(dst, dst, x)
#define vec3_scale(dst, x) vec3_scale_dest(dst, dst, x)
#define vec4_scale(dst, x) vec4_scale_dest(dst, dst, x)

// Set vector 'dst' to vector 'src1' divided by vector 'src2'
#define vec2_quot(dst, src1, src2) {        \
    __auto_type _x = (src1)[0] / (src2)[0]; \
    __auto_type _y = (src1)[1] / (src2)[1]; \
    (dst)[0] = _x;                          \
    (dst)[1] = _y;                          \
}
#define vec3_quot(dst, src1, src2) {        \
    __auto_type _x = (src1)[0] / (src2)[0]; \
    __auto_type _y = (src1)[1] / (src2)[1]; \
    __auto_type _z = (src1)[2] / (src2)[2]; \
    (dst)[0] = _x;                          \
    (dst)[1] = _y;                          \
    (dst)[2] = _z;                          \
}
#define vec4_quot(dst, src1, src2) {        \
    __auto_type _x = (src1)[0] / (src2)[0]; \
    __auto_type _y = (src1)[1] / (src2)[1]; \
    __auto_type _z = (src1)[2] / (src2)[2]; \
    __auto_type _w = (src1)[3] / (src2)[3]; \
    (dst)[0] = _x;                          \
    (dst)[1] = _y;                          \
    (dst)[2] = _z;                          \
    (dst)[3] = _w;                          \
}

#define vec3f_quot vec3_quot
#define vec3i_quot vec3_quot
#define vec3s_quot vec3_quot

// Divide vector 'dst' by vector 'src'
#define vec2_div(dst, src) vec2_quot((dst), (dst), (src))
#define vec3_div(dst, src) vec3_quot((dst), (dst), (src))
#define vec4_div(dst, src) vec4_quot((dst), (dst), (src))

#define vec3f_div vec3_div
#define vec3i_div vec3_div
#define vec3s_div vec3_div

// Calculate the dot product of two vectors
#define vec2_dot(a, b)       (((a)[0] * (b)[0]) + ((a)[1] * (b)[1]))
#define vec3_dot(a, b)      (vec2_dot((a), (b)) + ((a)[2] * (b)[2]))
#define vec4_dot(a, b)      (vec3_dot((a), (b)) + ((a)[3] * (b)[3]))

#define vec3f_dot vec3_dot

// Make vector 'dest' the cross product of vectors a and b.
#define vec3_cross(dst, a, b) {                             \
    __auto_type _x = ((a)[1] * (b)[2]) - ((a)[2] * (b)[1]); \
    __auto_type _y = ((a)[2] * (b)[0]) - ((a)[0] * (b)[2]); \
    __auto_type _z = ((a)[0] * (b)[1]) - ((a)[1] * (b)[0]); \
    (dst)[0] = _x;                                          \
    (dst)[1] = _y;                                          \
    (dst)[2] = _z;                                          \
}

#define vec3f_cross vec3_cross

// Scale vector 'v' so it has length 1
#define vec3_normalize(v) {                       \
    f32 _v_invmag = vec3_mag((v));                 \
    _v_invmag = (1.0f / MAX(_v_invmag, NEAR_ZERO)); \
    vec3_scale((v), _v_invmag);                    \
}

#define vec3f_normalize vec3_normalize

// If the magnitude of vector 'v' is greater than 'max', scale it down to 'max'
#define vec3_set_max_dist(v, max) { \
    f32 _v_mag = vec3_mag(v);        \
    f32 _max = max;                 \
    _v_mag = MAX(_v_mag, NEAR_ZERO);  \
    if (_v_mag > _max) {             \
        _v_mag = (_max / _v_mag);     \
        vec3_scale(v, _v_mag);       \
    }                               \
}

// Transform the vector 'srcV' by the matrix 'mtx' and store the result in 'dstV'. Ignores translation.
#define linear_mtxf_mul_vec3(mtx, dstV, srcV) {                                                          \
    __auto_type _x = ((mtx)[0][0] * (srcV)[0]) + ((mtx)[1][0] * (srcV)[1]) + ((mtx)[2][0] * (srcV)[2]); \
    __auto_type _y = ((mtx)[0][1] * (srcV)[0]) + ((mtx)[1][1] * (srcV)[1]) + ((mtx)[2][1] * (srcV)[2]); \
    __auto_type _z = ((mtx)[0][2] * (srcV)[0]) + ((mtx)[1][2] * (srcV)[1]) + ((mtx)[2][2] * (srcV)[2]); \
    (dstV)[0] = _x;                                                                                     \
    (dstV)[1] = _y;                                                                                     \
    (dstV)[2] = _z;                                                                                     \
}

// Transform the vector 'srcV' by the matrix 'mtx' including translation, and store the result in 'dstV'
#define linear_mtxf_mul_vec3_and_translate(mtx, dstV, srcV) { \
    linear_mtxf_mul_vec3((mtx), (dstV), (srcV));              \
    vec3_add((dstV), (mtx)[3]);                               \
}

// Transform the vector 'srcV' by the transpose of the matrix 'mtx'
// and store the result in 'dstV'. Ignores translation.
// For most transformation matrices, this will apply the inverse of the transformation.
#define linear_mtxf_transpose_mul_vec3(mtx, dstV, srcV) { \
    __auto_type _x = vec3_dot((mtx)[0], (srcV));          \
    __auto_type _y = vec3_dot((mtx)[1], (srcV));          \
    __auto_type _z = vec3_dot((mtx)[2], (srcV));          \
    (dstV)[0] = _x;                                       \
    (dstV)[1] = _y;                                       \
    (dstV)[2] = _z;                                       \
}

#define linear_mtxf_mul_vec3f linear_mtxf_mul_vec3
#define linear_mtxf_mul_vec3f_and_translate linear_mtxf_mul_vec3_and_translate
#define linear_mtxf_transpose_mul_vec3f linear_mtxf_transpose_mul_vec3


// Angles and distances between vectors

/// Finds the distance between two vectors
#define vec3_get_dist(from, to, dist) { \
    Vec3f _d;                           \
    vec3_diff(_d, (to), (from));        \
    *(dist) = vec3_mag((_d));           \
}

#define vec3f_get_dist vec3_get_dist
#define vec3s_get_dist vec3_get_dist

/// Finds the horizontal distance between two vectors
#define vec3_get_lateral_dist(from, to, lateralDist) { \
    Vec3f _d;                                          \
    vec3_diff(_d, (to), (from));                       \
    *(lateralDist) = sqrtf(sqr(_d[0]) + sqr(_d[2]));   \
}

#define vec3f_get_lateral_dist vec3_get_lateral_dist
#define vec3s_get_lateral_dist vec3_get_lateral_dist

/// Finds the pitch between two vectors
#define vec3_get_pitch(from, to, pitch) {                     \
    Vec3f _d;                                                 \
    vec3_diff(_d, (to), (from));                              \
    *(pitch) = atan2s(sqrtf(sqr(_d[0]) + sqr(_d[2])), _d[1]); \
}

#define vec3f_get_pitch vec3_get_pitch
#define vec3s_get_pitch vec3_get_pitch

/// Finds the yaw between two vectors
#define vec3_get_yaw(from, to, yaw) { \
    f32 _dx = ((to)[0] - (from)[0]);  \
    f32 _dz = ((to)[2] - (from)[2]);  \
    *(yaw) = atan2s(_dz, _dx);        \
}

#define vec3f_get_yaw vec3_get_yaw
#define vec3s_get_yaw vec3_get_yaw

// Finds the distance, pitch, and yaw between two vectors
#define vec3_get_dist_and_angle(from, to, dist, pitch, yaw) { \
    Vec3f _d;                                                 \
    vec3f_diff(_d, (to), (from));                             \
    f32 _xz = (sqr(_d[0]) + sqr(_d[2]));                      \
    *(dist)  = sqrtf(_xz + sqr(_d[1]));                         \
    *(pitch) = atan2s(sqrtf(_xz), _d[1]);                       \
    *(yaw)   = atan2s(_d[2], _d[0]);                            \
}

#define vec3f_get_dist_and_angle vec3_get_dist_and_angle
#define vec3s_get_dist_and_angle vec3_get_dist_and_angle

// Constructs the 'to' point which is distance 'dist' away from the 'from' position,
// and has the angles pitch and yaw.
#define vec3_set_dist_and_angle(from, to, dist, pitch, yaw) { \
    f32 _dcos = ((dist) * coss(pitch));                         \
    __auto_type _x = ((from)[0] + (_dcos  * sins(yaw)));       \
    __auto_type _y = ((from)[1] + ((dist) * sins(pitch)));     \
    __auto_type _z = ((from)[2] + (_dcos  * coss(yaw)));       \
    (to)[0] = _x;                                             \
    (to)[1] = _y;                                             \
    (to)[2] = _z;                                             \
}

#define vec3f_set_dist_and_angle vec3_set_dist_and_angle
#define vec3s_set_dist_and_angle vec3_set_dist_and_angle

#define DIVIDE_AND_CEIL(numerator, divisor) ( ( (u32)(numerator) + ((u32)(divisor)-1) ) / ((u32)(divisor)) )


void mtxf_copy(Mat4 dest, Mat4 src);
void mtxf_identity(Mat4 mtx);
void mtxf_translate(Mat4 dest, Vec3f b);
void mtxf_lookat(Mat4 mtx, Vec3f from, Vec3f to, s16 roll);
void mtxf_rotate_zxy_and_translate(Mat4 dest, Vec3f translate, Vec3s rotate);
void mtxf_rotate_xyz_and_translate(Mat4 dest, Vec3f b, Vec3s c);
void mtxf_billboard(Mat4 dest, Mat4 mtx, Vec3f position, s16 angle);
void mtxf_align_terrain_normal(Mat4 dest, Vec3f upDir, Vec3f pos, s16 yaw);
void mtxf_align_terrain_triangle(Mat4 mtx, Vec3f pos, s16 yaw, f32 radius);
void mtxf_mul(Mat4 dest, Mat4 a, Mat4 b);
void mtxf_scale_vec3f(Mat4 dest, Mat4 mtx, Vec3f s);
void mtxf_mul_vec3s(Mat4 mtx, Vec3s b);
void mtxf_to_mtx(Mtx *dest, Mat4 src);
void mtxf_rotate_xy(Mtx *mtx, s16 angle);
void get_pos_from_transform_mtx(Vec3f dest, Mat4 objMtx, Mat4 camMtx);
s32 approach_s32(s32 current, s32 target, s32 inc, s32 dec);
f32 approach_f32(f32 current, f32 target, f32 inc, f32 dec);
s16 atan2s(f32 y, f32 x);
f32 atan2f(f32 a, f32 b);
void spline_get_weights(Vec4f result, f32 t, UNUSED s32 c);
void anim_spline_init(Vec4s *keyFrames);
s32 anim_spline_poll(Vec3f result);

ALWAYS_INLINE s32 roundf(f32 in) {
    f32 tmp;
    s32 out;
    __asm__("round.w.s %0,%1" : "=f" (tmp) : "f" (in ));
    __asm__("mfc1      %0,%1" : "=r" (out) : "f" (tmp));
    return out;
}

ALWAYS_INLINE f32 remap(f32 x, f32 fromA, f32 toA, f32 fromB, f32 toB) {
    return (x - fromA) / (toA - fromA) * (toB - fromB) + fromB;
}

ALWAYS_INLINE f32 lerpf(f32 from, f32 to, f32 amount) {
    return (from + (to - from) * amount);
}

ALWAYS_INLINE f32 to_smoothstop(f32 x) {
    f32 sq = sqr(1.0f - x);
    return 1.0f - sq;
}

// Commonly known as ease-in
ALWAYS_INLINE f32 smoothstart(f32 from, f32 to, f32 amount) {
    return lerpf(from, to, sqr(amount));
}

// Commonly known as ease-out
ALWAYS_INLINE f32 smoothstop(f32 from, f32 to, f32 amount) {
    return lerpf(from, to, to_smoothstop(amount));
}

// Commonly known as ease-in-out
ALWAYS_INLINE f32 smoothstep(f32 from, f32 to, f32 amount) {
    amount = sqr(amount) * (3.0f - 2.0f * amount);

    return lerpf(from, to, amount);
}

#endif // MATH_UTIL_H
