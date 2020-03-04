/**
 *
 * @copyright &copy; 2010 - 2020, Fraunhofer-Gesellschaft zur Foerderung der
 *  angewandten Forschung e.V. All rights reserved.
 *
 * BSD 3-Clause License
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1.  Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of the copyright holder nor the names of its
 *     contributors may be used to endorse or promote products derived from
 *     this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * We kindly request you to use one or more of the following phrases to refer
 * to foxBMS in your hardware, software, documentation or advertising
 * materials:
 *
 * &Prime;This product uses parts of foxBMS&reg;&Prime;
 *
 * &Prime;This product includes parts of foxBMS&reg;&Prime;
 *
 * &Prime;This product is derived from foxBMS&reg;&Prime;
 *
 */

/**
 * @file    foxmath.h
 * @author  foxBMS Team
 * @date    18.01.2018 (date of creation)
 * @ingroup DRIVERS
 * @prefix  MATH
 *
 * @brief   math library for often used math functions
 *
 * @details This header file contains the different math function declarations.
 *          Currently the following functions are supported:
 *          - Slope
 *          - Linear interpolation
 *
 */

#ifndef FOXMATH_H_
#define FOXMATH_H_

/*================== Includes ===============================================*/
#include "general.h"
#include <math.h>
#include <stdlib.h>
#include <float.h>

/*================== Macros and Definitions =================================*/
#define MINDIFF     2.25e-308

#if defined(__STRICT_ANSI__)
#define M_E         2.7182818284590452354
#define M_LOG2E     1.4426950408889634074
#define M_LOG10E    0.43429448190325182765
#define M_LN2       _M_LN2
#define M_LN10      2.30258509299404568402
#define M_PI        3.14159265358979323846
#define M_PI_2      1.57079632679489661923
#define M_PI_4      0.78539816339744830962
#define M_1_PI      0.31830988618379067154
#define M_2_PI      0.63661977236758134308
#define M_2_SQRTPI  1.12837916709551257390
#define M_SQRT2     1.41421356237309504880
#define M_SQRT1_2   0.70710678118654752440

#define M_TWOPI         (M_PI * 2.0)
#define M_3PI_4         2.3561944901923448370E0
#define M_SQRTPI        1.77245385090551602792981
#define M_LN2LO         1.9082149292705877000E-10
#define M_LN2HI         6.9314718036912381649E-1
#define M_SQRT3         1.73205080756887719000
#define M_IVLN10        0.43429448190325182765 /* 1 / log(10) */
#define M_LOG2_E        _M_LN2
#define M_INVLN2        1.4426950408889633870E0  /* 1 / log(2) */
#endif /* __STRICT_ANSI__ */

/*================== Extern Constant and Variable Declarations ==============*/

/*================== Extern Function Prototypes =============================*/

/**
* @brief   Linear inter-/extrapolates a third point according to two given points
*
* @param   x1:               x-value of point 1
* @param   y1:               y-value of point 1
* @param   x2:               x-value of point 2
* @param   y2:               y-value of point 2
* @param   x_interpolate:    x value of interpolation point
*
* @return  interpolated value (float)
*/
extern float MATH_linearInterpolation(float x1, float y1, float x2, float y2, float x_interpolate);

/**
 * @brief Swap bytes of uint16_t value
 *
 * @param   val:    value to swap bytes: 0x1234 -> 0x3412
 */
uint16_t MATH_swapBytes_uint16_t(uint16_t val);

/**
 * @brief Swap bytes of uint32_t value
 *
 * @param   val:    value to swap bytes: 0x12345678 -> 0x78563412
 */
uint32_t MATH_swapBytes_uint32_t(uint32_t val);

/**
 * @brief Swap bytes  of uint64_t value
 *
 * @param   val:    value to swap bytes: 0x1122334455667788 -> 0x8877665544332211
 */
uint64_t MATH_swapBytes_uint64_t(uint64_t val);

#endif /* FOXMATH_H_ */
