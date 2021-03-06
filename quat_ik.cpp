/*************************************************************************/
/*  ik_quat.cpp                                                          */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2019 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2019 Godot Engine contributors (cf. AUTHORS.md)    */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#include "quat_ik.h"

class DirectionConstraint;

Vector<QuatIK> QuatIK::get_swing_twist(Vector3 p_axis) {
	Vector3 euler = get_euler();
	const real_t d = Vector3(
			euler.x,
			euler.y,
			euler.z).dot(Vector3(p_axis.x, p_axis.y, p_axis.z));
	*this = Quat(p_axis * d);
	normalize();
	if (d < 0) {
		operator*(-1.0f);
	}

	QuatIK swing = QuatIK(-x, -y, -z, w);
	swing = operator*(swing);

	Vector<QuatIK> result;
	result.resize(2);
	result.write[0] = swing;
	result.write[1] = QuatIK(x, y, z, w);
	return result;
}

void QuatIK::clamp_to_quadrance_angle(real_t p_cos_half_angle) {
	real_t new_coeff = 1.0f - (p_cos_half_angle * p_cos_half_angle);
	real_t current_coeff = y * y + z * z + w * w;
	if (new_coeff > current_coeff) {
		return;
	} else {
		x = x < 0.0 ? -p_cos_half_angle : p_cos_half_angle;
		real_t composite_coeff = Math::sqrt(new_coeff / current_coeff);
		y *= composite_coeff;
		z *= composite_coeff;
		w *= composite_coeff;
	}
}

void QuatIK::clamp_to_angle(real_t p_angle) {
	real_t cos_half_angle = Math::cos(0.5f * p_angle);
	clamp_to_quadrance_angle(cos_half_angle);
}

QuatIK::QuatIK() {
}

 QuatIK::QuatIK(Quat p_quat) {
	x = p_quat.x;
	y = p_quat.y;
	z = p_quat.z;
	w = p_quat.w;
}

 QuatIK::QuatIK(float p_x, float p_y, float p_z, float p_w) :
		Quat(p_x,
				p_y,
				p_z,
				p_w) {
}

QuatIK::~QuatIK() {
}
