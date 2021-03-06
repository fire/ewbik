/*************************************************************************/
/*  skeleton_ik_constraints.cpp                                          */
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

#include "skeleton_modification_3d_ewbik.h"
#include "bone_effector.h"
#include "direction_constraint.h"
#include "kusudama_constraint.h"
#include "scene/3d/skeleton_3d.h"
#include "segmented_skeleton_3d.h"

void SkeletonModification3DEWBIK::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_effector_count"), &SkeletonModification3DEWBIK::get_effector_count);
	ClassDB::bind_method(D_METHOD("set_effector_count", "count"),
			&SkeletonModification3DEWBIK::set_effector_count);
	ClassDB::bind_method(D_METHOD("add_effector", "name", "target_node", "target_transform", "budget"), &SkeletonModification3DEWBIK::add_effector);
	ClassDB::bind_method(D_METHOD("get_effector", "index"), &SkeletonModification3DEWBIK::get_effector);
	ClassDB::bind_method(D_METHOD("set_effector", "index", "effector"), &SkeletonModification3DEWBIK::set_effector);
	ClassDB::bind_method(D_METHOD("set_skeleton_ik_data", "skeleton_ik_data"), &SkeletonModification3DEWBIK::set_state);
	ClassDB::bind_method(D_METHOD("get_skeleton_ik_data"), &SkeletonModification3DEWBIK::get_state);
}

void SkeletonModification3DEWBIK::_get_property_list(List<PropertyInfo> *p_list) const {
	p_list->push_back(PropertyInfo(Variant::INT, "effector_count", PROPERTY_HINT_RANGE, "0,65535,1"));
	for (int i = 0; i < effector_count; i++) {
		p_list->push_back(PropertyInfo(Variant::STRING, "effectors/" + itos(i) + "/name"));
		p_list->push_back(
				PropertyInfo(Variant::TRANSFORM, "effectors/" + itos(i) + "/target_transform"));
		p_list->push_back(
				PropertyInfo(Variant::NODE_PATH, "effectors/" + itos(i) + "/target_node"));
		p_list->push_back(
				PropertyInfo(Variant::FLOAT, "effectors/" + itos(i) + "/budget", PROPERTY_HINT_RANGE, "0,16,or_greater"));
	}
	p_list->push_back(
			PropertyInfo(Variant::STRING, "root_bone"));
	p_list->push_back(
			PropertyInfo(Variant::OBJECT, "state", PROPERTY_HINT_RESOURCE_TYPE, "EWBIKSkeletonIKState"));
}

bool SkeletonModification3DEWBIK::_get(const StringName &p_name, Variant &r_ret) const {
	String name = p_name;
	if (name == "root_bone") {
		r_ret = get_root_bone();
		return true;
	} else if (name == "state") {
		r_ret = get_state();
		return true;
	} else if (name == "effector_count") {
		r_ret = get_effector_count();
		return true;
	} else if (name.begins_with("effectors/")) {
		int index = name.get_slicec('/', 1).to_int();
		String what = name.get_slicec('/', 2);
		ERR_FAIL_INDEX_V(index, get_effector_count(), false);
		ERR_FAIL_COND_V(get_effector(index).is_null(), false);
		if (what == "name") {
			r_ret = get_effector(index)->get_name();
			return true;
		} else if (what == "target_transform") {
			r_ret = get_effector(index)->get_target_transform();
			return true;
		} else if (what == "target_node") {
			r_ret = get_effector(index)->get_target_node();
			return true;
		} else if (what == "budget") {
			r_ret = get_effector(index)->get_budget_ms();
			return true;
		}
	}
	return false;
}

bool SkeletonModification3DEWBIK::_set(const StringName &p_name, const Variant &p_value) {
	String name = p_name;

	if (name == "root_bone") {
		set_root_bone(p_value);
		return true;
	} else if (name == "state") {
		set_state(p_value);
		return true;
	} else if (name == "effector_count") {
		set_effector_count(p_value);
		return true;
	} else if (name.begins_with("effectors/")) {
		int index = name.get_slicec('/', 1).to_int();
		String what = name.get_slicec('/', 2);
		ERR_FAIL_INDEX_V(index, effector_count, false);
		Ref<EWBIKBoneEffector> effector = get_effector(index);
		if (effector.is_null()) {
			effector.instance();
			set_effector(index, effector);
		}
		if (what == "name") {
			name = p_value;
			ERR_FAIL_COND_V(name.is_empty(), false);
			effector->set_name(name);
			_change_notify();
			return true;
		} else if (what == "target_node") {
			effector->set_target_node(p_value);
			_change_notify();
			return true;
		} else if (what == "target_transform") {
			effector->set_target_transform(p_value);
			_change_notify();
			return true;
		} else if (what == "budget") {
			effector->set_budget_ms(p_value);
			_change_notify();
			return true;
		}
	}
	return false;
}

String SkeletonModification3DEWBIK::get_root_bone() const {
	return root_bone;
}

void SkeletonModification3DEWBIK::set_root_bone(String p_root_bone) {
	root_bone = p_root_bone;
}

SkeletonModification3DEWBIK::SkeletonModification3DEWBIK() {
	enabled = true;
	qcp_convergence_check.instance();
	qcp_convergence_check->set_precision(FLT_EPSILON, FLT_EPSILON);
}

SkeletonModification3DEWBIK::~SkeletonModification3DEWBIK() {
}

void SkeletonModification3DEWBIK::set_effector_count(int32_t p_value) {
	multi_effector.resize(p_value);
	effector_count = p_value;
	_change_notify();
}

int32_t SkeletonModification3DEWBIK::get_effector_count() const {
	return effector_count;
}

Ref<EWBIKBoneEffector> SkeletonModification3DEWBIK::get_effector(int32_t p_index) const {
	ERR_FAIL_INDEX_V(p_index, multi_effector.size(), NULL);
	Ref<EWBIKBoneEffector> effector = multi_effector[p_index];
	return effector;
}

void SkeletonModification3DEWBIK::set_effector(int32_t p_index, Ref<EWBIKBoneEffector> p_effector) {
	ERR_FAIL_COND(p_effector.is_null());
	ERR_FAIL_INDEX(p_index, multi_effector.size());
	multi_effector.write[p_index] = p_effector;
	_change_notify();
}

Vector<Ref<EWBIKBoneEffector>> SkeletonModification3DEWBIK::get_bone_effectors() const {
	return multi_effector;
}

int32_t SkeletonModification3DEWBIK::find_effector(String p_name) {
	for (int32_t effector_i = 0; effector_i < multi_effector.size(); effector_i++) {
		if (multi_effector[effector_i].is_valid() && multi_effector[effector_i]->get_name() == p_name) {
			return effector_i;
		}
	}
	return -1;
}

void SkeletonModification3DEWBIK::remove_effector(int32_t p_index) {
	ERR_FAIL_INDEX(p_index, multi_effector.size());
	multi_effector.remove(p_index);
	effector_count--;
	_change_notify();
}

void SkeletonModification3DEWBIK::execute(float delta) {
	ERR_FAIL_COND_MSG(!stack || !is_setup || stack->skeleton == nullptr,
			"Modification is not setup and therefore cannot execute!");
	if (!enabled) {
		return;
	}
	if (task.is_valid()) {
		solve(task, stack->get_strength());
	}
}

void SkeletonModification3DEWBIK::setup_modification(SkeletonModificationStack3D *p_stack) {
	stack = p_stack;
	if (!stack) {
		return;
	}
	Skeleton3D *skeleton = stack->skeleton;
	if (skeleton) {
		if (!constraint_count) {
			Vector<int32_t> roots;
			for (int32_t bone_i = 0; bone_i < skeleton->get_bone_count(); bone_i++) {
				int32_t parent = skeleton->get_bone_parent(bone_i);
				if (parent == -1) {
					roots.push_back(bone_i);
				}
			}
			if (roots.size()) {
				String root_name = skeleton->get_bone_name(roots[0]);
				root_bone = root_name;
			}
		}
		ERR_FAIL_COND(root_bone.is_empty());
		if (!constraint_count) {
			BoneId _bone = skeleton->find_bone(root_bone);
			Ref<EWBIKSegmentedSkeleton3D> chain_item = memnew(EWBIKSegmentedSkeleton3D(this));
			chain_item->bone = _bone;
			Ref<EWBIKSegmentedSkeleton3D> bone_chain = memnew(EWBIKSegmentedSkeleton3D(this));
			bone_chain->init(skeleton, this, multi_effector, bone_chain, nullptr, chain_item);
			Vector<StringName> effectors = bone_chain->get_default_effectors(skeleton, bone_chain, bone_chain);
			set_effector_count(0);
			for (int32_t effector_i = 0; effector_i < effectors.size(); effector_i++) {
				add_effector(effectors[effector_i]);
			}
			register_constraint(skeleton);
		}
		skeleton_ik_state.instance();
		skeleton_ik_state->init(this);
		task = create_simple_task(skeleton, root_bone, -1.0f, 10.0f, this);
	}
	is_setup = true;
	execution_error_found = false;
}

void SkeletonModification3DEWBIK::iterated_improved_solver(Ref<QCP> p_qcp, int32_t p_root_bone, Ref<EWBIKSegmentedSkeleton3D> p_start_from, float p_dampening, int p_iterations, int p_stabilization_passes) {
	Ref<EWBIKSegmentedSkeleton3D> armature = p_start_from;
	if (armature.is_null()) {
		return;
	}
	Ref<EWBIKSegmentedSkeleton3D> pinned_root_chain = armature;
	if (pinned_root_chain.is_null() && p_root_bone != -1) {
		armature = armature->chain_root->find_child(p_root_bone);
	} else {
		armature = pinned_root_chain;
	}
	if (armature.is_valid() && armature->targets.size() > 0) {
		armature->align_axes_to_bones();
		if (p_iterations == -1) {
			p_iterations = armature->ik_iterations;
		}
		float totalIterations = p_iterations;
		if (p_stabilization_passes == -1) {
			p_stabilization_passes = armature->stabilization_passes;
		}
		for (int i = 0; i < p_iterations; i++) {
			if (!armature->base_bone->is_bone_effector(armature->base_bone) && armature->get_child_chains().size()) {
				update_optimal_rotation_to_target_descendants(armature->skeleton, armature->chain_root, Math_PI, true, armature->localized_target_headings, armature->localized_effector_headings, armature->weights, p_qcp, i, totalIterations);
				armature->set_processed(false);
				Vector<Ref<EWBIKSegmentedSkeleton3D>> segmented_armature = armature->get_child_chains();
				for (int32_t i = 0; i < segmented_armature.size(); i++) {
					grouped_recursive_chain_solver(segmented_armature[i], armature->dampening, armature->stabilization_passes, i, totalIterations);
				}
			} else {
				grouped_recursive_chain_solver(armature, p_dampening, p_stabilization_passes, i, totalIterations);
			}
		}
		armature->recursively_align_bones_to_sim_axes_from(armature->chain_root);
	}
}

void SkeletonModification3DEWBIK::grouped_recursive_chain_solver(Ref<EWBIKSegmentedSkeleton3D> p_start_from, float p_dampening, int p_stabilization_passes, int p_iteration, float p_total_iterations) {
	recursive_chain_solver(p_start_from, p_dampening, p_stabilization_passes, p_iteration, p_total_iterations);
	Vector<Ref<EWBIKSegmentedSkeleton3D>> chains = p_start_from->get_child_chains();
	for (int32_t i = 0; i < chains.size(); i++) {
		grouped_recursive_chain_solver(chains[i], p_dampening, p_stabilization_passes, p_iteration, p_total_iterations);
	}
}

void SkeletonModification3DEWBIK::recursive_chain_solver(Ref<EWBIKSegmentedSkeleton3D> p_armature, float p_dampening, int p_stabilization_passes, int p_iteration, float p_total_iterations) {
	if (!p_armature->get_child_chains().size() && p_armature->is_bone_effector(p_armature)) {
		return;
	} else if (!p_armature->is_bone_effector(p_armature)) {
		Vector<Ref<EWBIKSegmentedSkeleton3D>> chains = p_armature->get_child_chains();
		for (int32_t i = 0; i < chains.size(); i++) {
			recursive_chain_solver(chains[i], p_dampening, p_stabilization_passes, p_iteration, p_total_iterations);
			chains.write[i]->set_processed(true);
		}
	}
	QCPSolver(
			p_armature->skeleton,
			p_armature,
			p_armature->dampening,
			false,
			p_iteration,
			p_stabilization_passes,
			p_total_iterations);
}

void SkeletonModification3DEWBIK::apply_bone_chains(Ref<EWBIKState> p_state, float p_strength, Skeleton3D *p_skeleton, BoneId p_bone, Map<int, Ref<EWBIKSegmentedSkeleton3D>> p_bone_segment_map) {
	ERR_FAIL_INDEX(p_bone, p_skeleton->get_bone_count());
	Ref<EWBIKSegmentedSkeleton3D> chain = p_bone_segment_map[p_bone];
	if (chain.is_valid()) {
		Transform shadow_pose;
		IKBasis simulatedLocalAxes = p_state->get_shadow_pose_global(p_bone).get_global();
		shadow_pose.origin = simulatedLocalAxes.get_origin();
		shadow_pose.basis = simulatedLocalAxes.get_rotation();
		p_skeleton->set_bone_local_pose_override(p_bone, shadow_pose, 1.0, true);
		Vector<int32_t> bone_children = p_skeleton->get_bone_children(p_bone);
		for (int32_t bone_i = 0; bone_i < bone_children.size(); bone_i++) {
			apply_bone_chains(p_state, p_strength, p_skeleton, bone_children[bone_i], p_bone_segment_map);
		}
	}
}

void SkeletonModification3DEWBIK::add_effector(String p_name, NodePath p_node, Transform p_transform, real_t p_budget) {
	Ref<EWBIKBoneEffector> effector;
	effector.instance();
	effector->set_name(p_name);
	effector->set_target_node(p_node);
	effector->set_target_transform(p_transform);
	effector->set_budget_ms(p_budget);
	multi_effector.push_back(effector);
	effector_count++;
	_change_notify();
}

void SkeletonModification3DEWBIK::register_constraint(Skeleton3D *p_skeleton) {
	ERR_FAIL_COND(!p_skeleton);
	for (int32_t bone_i = 0; bone_i < p_skeleton->get_bone_count(); bone_i++) {
		Ref<KusudamaConstraint> constraint;
		constraint.instance();
		String bone_name = p_skeleton->get_bone_name(bone_i);
		constraint->set_name(bone_name);
		skeleton_ik_state->set_constraint(bone_i, constraint);
		constraint_count++;
	}
	_change_notify();
}

void SkeletonModification3DEWBIK::QCPSolver(
		Skeleton3D *p_skeleton,
		Ref<EWBIKSegmentedSkeleton3D> p_chain,
		float p_dampening,
		bool p_inverse_weighting,
		int p_stabilization_passes,
		int p_iteration,
		float p_total_iterations) {
	for (int32_t tip_i = 0; tip_i < p_chain->targets.size(); tip_i++) {
		if (p_chain->targets[tip_i].is_null()) {
			continue;
		}
		Ref<EWBIKSegmentedSkeleton3D> start_from = p_chain->targets[tip_i]->chain_item;
		Ref<EWBIKSegmentedSkeleton3D> stop_after = p_chain->chain_root;

		Ref<EWBIKSegmentedSkeleton3D> current_bone = start_from;
		//if the tip is pinned, it should have already been oriented before this function was called.
		while (current_bone.is_valid()) {
			if (!current_bone->ik_orientation_lock) {
				update_optimal_rotation_to_target_descendants(p_skeleton, p_chain, current_bone, p_dampening, false,
						p_stabilization_passes, p_iteration, p_total_iterations);
			}
			if (current_bone == stop_after) {
				current_bone = Ref<EWBIKSegmentedSkeleton3D>(nullptr);
			} else {
				current_bone = current_bone->parent_item;
			}
		}
	}
}

Ref<EWBIKSegmentedSkeleton3D> EWBIKSegmentedSkeleton3D::find_child(const int p_bone_id) {
	return chain_root->bone_segment_map[p_bone_id];
}

Ref<EWBIKSegmentedSkeleton3D> EWBIKSegmentedSkeleton3D::add_child(const int p_bone_id) {
	const int infant_child_id = children.size();
	children.resize(infant_child_id + 1);
	if (children.write[infant_child_id].is_null()) {
		children.write[infant_child_id].instance();
	}
	children.write[infant_child_id]->bone = p_bone_id;
	children.write[infant_child_id]->parent_item = Ref<EWBIKSegmentedSkeleton3D>(this);
	return children.write[infant_child_id];
}

void EWBIKSegmentedSkeleton3D::set_axes_to_returned(Transform p_global, Transform p_to_set, Transform p_limiting_axes,
		float p_cos_half_angle_dampen, float p_angle_dampen) {
	Ref<KusudamaConstraint> constraint = mod->get_state()->get_constraint(bone);
	if (constraint.is_valid()) {
		constraint->set_axes_to_returnful(p_global, p_to_set, p_limiting_axes, p_cos_half_angle_dampen,
				p_angle_dampen);
	}
}

void EWBIKSegmentedSkeleton3D::set_axes_to_be_snapped(Transform p_to_set, Transform p_limiting_axes,
		float p_cos_half_angle_dampen) {
	Ref<KusudamaConstraint> constraint = mod->get_state()->get_constraint(bone);
	if (constraint.is_valid()) {
		constraint->set_axes_to_snapped(p_to_set, p_limiting_axes, p_cos_half_angle_dampen);
	}
}

bool SkeletonModification3DEWBIK::build_chain(Ref<EWBIKTask> p_task) {
	ERR_FAIL_COND_V(-1 == p_task->root_bone, false);
	Ref<EWBIKSegmentedSkeleton3D> chain = p_task->chain;
	chain->chain_root = chain;
	chain->bone = p_task->root_bone;
	chain->init(p_task->skeleton, p_task->ewbik, p_task->ewbik->multi_effector, chain, nullptr, chain);
	chain->filter_and_merge_child_chains();
	chain->bone = p_task->root_bone;
	chain->pb = p_task->skeleton->get_physical_bone(chain->bone);
	return true;
}

void SkeletonModification3DEWBIK::solve_simple(Ref<EWBIKTask> p_task) {
	iterated_improved_solver(p_task->qcp, p_task->root_bone, p_task->chain, p_task->dampening, p_task->max_iterations, p_task->stabilizing_passes);
}

Ref<EWBIKTask> SkeletonModification3DEWBIK::create_simple_task(Skeleton3D *p_sk, String p_root_bone,
		float p_dampening, int p_stabilizing_passes,
		Ref<SkeletonModification3DEWBIK> p_constraints) {
	Ref<EWBIKTask> task = memnew(EWBIKTask(p_constraints));
	task->qcp = p_constraints->qcp_convergence_check;
	task->skeleton = p_sk;
	BoneId bone = p_sk->find_bone(p_root_bone);
	task->root_bone = bone;
	Ref<EWBIKSegmentedSkeleton3D> bone_item;
	bone_item.instance();
	bone_item->mod = p_constraints;
	bone_item->bone = bone;
	ERR_FAIL_COND_V(task->root_bone == -1, NULL);
	ERR_FAIL_COND_V(p_constraints.is_null(), NULL);
	{
		Ref<KusudamaConstraint> constraint;
		constraint.instance();
		constraint->set_name(p_sk->get_bone_name(task->root_bone));
		p_constraints->skeleton_ik_state->set_constraint(0, constraint);
	}
	task->dampening = p_dampening;
	task->stabilizing_passes = p_stabilizing_passes;
	ERR_FAIL_COND_V(!p_constraints->multi_effector.size(), nullptr);
	if (!build_chain(task)) {
		return NULL;
	}
	task->chain->chain_root->print_bone_chains(task->skeleton, task->chain->chain_root);
	return task;
}

void SkeletonModification3DEWBIK::solve(Ref<EWBIKTask> p_task, float blending_delta) {
	if (blending_delta <= 0.01f) {
		return; // Skip solving
	}

	for (int32_t bone_i = 0; bone_i < p_task->skeleton->get_bone_count(); bone_i++) {
		Skeleton3D *skeleton = p_task->skeleton;
		Transform xform = skeleton->get_bone_global_pose(bone_i);
		BoneId parent = skeleton->get_bone_parent(bone_i);
		if (parent != -1) {
			xform = skeleton->get_bone_global_pose(parent).affine_inverse() * xform;
		}
		Ref<EWBIKState> state = p_task->ewbik->skeleton_ik_state;
		IKBasis basis = IKBasis(xform.origin, xform.basis.get_axis(0), xform.basis.get_axis(1), xform.basis.get_axis(2));
		state->set_shadow_bone_pose_local(bone_i, basis);
		state->set_parent(bone_i, parent);
	}

	// for (int32_t constraint_i = 0; constraint_i < p_task->dmik->get_constraint_count(); constraint_i++) {
	// 	Ref<KusudamaConstraint> constraint = p_task->dmik->get_loconstraint(constraint_i);
	// 	ERR_CONTINUE(constraint.is_null());
	// 	for (int32_t direction_i = 0; direction_i < constraint->get_direction_count(); direction_i++) {
	// 		Ref<DirectionConstraint> direction = constraint->get_direction(direction_i);
	// 		if (direction.is_null()) {
	// 			continue;
	// 		}
	// 		Vector3 cp = direction->get_control_point();
	// 		direction->set_control_point(cp.normalized());
	// 		constraint->set_direction(direction_i, direction);
	// 	}
	// }
	int effector_count = p_task->ewbik->get_effector_count();
	p_task->end_effectors.resize(effector_count);
	for (int32_t effector_i = 0; effector_i < effector_count; effector_i++) {
		p_task->end_effectors.write[effector_i].instance();
	}
	for (int32_t name_i = 0; name_i < p_task->end_effectors.size(); name_i++) {
		Ref<EWBIKBoneEffector> effector = p_task->ewbik->get_effector(name_i);
		if (effector.is_null()) {
			continue;
		}
		Ref<EWBIKBoneEffectorTransform> ee;
		ee.instance();
		// TODO Cache as object id
		Node *target_node = p_task->skeleton->get_node_or_null(effector->get_target_node());
		Transform node_xform;
		int32_t bone = p_task->skeleton->find_bone(effector->get_name());
		if (bone == -1) {
			continue;
		}
		if (target_node) {
			Node3D *current_node = Object::cast_to<Node3D>(target_node);
			node_xform = p_task->skeleton->get_global_transform().affine_inverse() * current_node->get_global_transform();
		}
		node_xform = node_xform * effector->get_target_transform();
		node_xform = p_task->skeleton->get_bone_global_pose(bone).affine_inverse() * node_xform;
		ee->goal_transform = node_xform;
		int32_t constraint_i = p_task->ewbik->get_modification_stack()->get_skeleton()->find_bone(effector->get_name());
		Ref<KusudamaConstraint> constraint = p_task->ewbik->skeleton_ik_state->get_constraint(constraint_i);
		if (constraint.is_null()) {
			continue;
		}
		constraint->set_constraint_axes(ee->goal_transform);
		ee->effector_bone = bone;
		p_task->end_effectors.write[name_i] = ee;
	}

	Vector<Ref<EWBIKBoneChainTarget>> targets;
	targets.resize(p_task->end_effectors.size());
	for (int32_t effector_i = 0; effector_i < p_task->end_effectors.size(); effector_i++) {
		Ref<EWBIKBoneEffectorTransform> ee = p_task->end_effectors[effector_i];
		if (ee.is_null()) {
			ee.instance();
			p_task->end_effectors.write[effector_i] = ee;
		}
		Ref<EWBIKBoneChainTarget> target;
		target.instance();
		target->end_effector = ee;
		Ref<EWBIKSegmentedSkeleton3D> bone_chain_item = p_task->chain->chain_root->find_child(ee->effector_bone);
		if (bone_chain_item.is_null()) {
			continue;
		}
		target->chain_item = bone_chain_item;
		targets.write[effector_i] = target;
	}
	p_task->chain->targets = targets;
	p_task->chain->create_headings_arrays();
	if (!p_task->chain->targets.size()) {
		return;
	}
	solve_simple(p_task);
	// Strength is always full strength
	apply_bone_chains(p_task->ewbik->get_state(), 1.0f, p_task->skeleton, p_task->chain->chain_root->bone, p_task->chain->chain_root->bone_segment_map);
}

void SkeletonModification3DEWBIK::set_default_dampening(Ref<EWBIKSegmentedSkeleton3D> r_chain, float p_damp) {
	r_chain->dampening =
			MIN(Math_PI * 3.0f, MAX(Math::absf(std::numeric_limits<real_t>::epsilon()), Math::absf(p_damp)));
	update_armature_segments(r_chain);
}

void SkeletonModification3DEWBIK::update_armature_segments(Ref<EWBIKSegmentedSkeleton3D> r_chain) {
	r_chain->bone_segment_map.clear();
	recursively_update_bone_segment_map_from(r_chain, r_chain->chain_root);
}

void SkeletonModification3DEWBIK::update_optimal_rotation_to_target_descendants(Skeleton3D *p_skeleton, Ref<EWBIKSegmentedSkeleton3D> p_chain_item,
		float p_dampening, bool p_is_translate,
		Vector<Vector3> p_localized_effector_headings,
		Vector<Vector3> p_localized_target_headings,
		Vector<real_t> p_weights,
		Ref<QCP> p_qcp_orientation_aligner, int p_iteration,
		float p_total_iterations) {
	p_qcp_orientation_aligner->set_max_iterations(10);
	QuatIK qcp_rot = p_qcp_orientation_aligner->weighted_superpose(p_localized_effector_headings, p_localized_target_headings,
			p_weights, p_is_translate);
	// Vector3 translate_by = p_qcp_orientation_aligner->get_translation();
	Ref<EWBIKState> state = p_chain_item->mod->get_state();
	float bone_damp = state->get_cos_half_dampen(p_chain_item->bone);
	if (p_dampening != -1) {
		bone_damp = p_dampening;
		qcp_rot.clamp_to_angle(bone_damp);
	} else {
		qcp_rot.clamp_to_quadrance_angle(bone_damp);
	}
	BoneId bone = p_chain_item->bone;
	state->rotate_by(bone, qcp_rot);

	state->force_update_bone_children_transforms(bone);

	p_chain_item->set_axes_to_be_snapped(state->get_shadow_pose_global(bone).get_global_transform(), state->get_shadow_constraint_axes_global(bone).get_global_transform(), bone_damp);
	state->get_shadow_constraint_axes_global(bone);
	//state->translate_shadow_pose_by_global(bone, translate_by);
	//state->translate_constraint_axes_by_global(bone, translate_by);
}

void SkeletonModification3DEWBIK::recursively_update_bone_segment_map_from(Ref<EWBIKSegmentedSkeleton3D> r_chain,
		Ref<EWBIKSegmentedSkeleton3D> p_start_from) {
	for (int32_t child_i = 0; child_i < p_start_from->children.size(); child_i++) {
		r_chain->bone_segment_map.insert(p_start_from->children[child_i]->bone, p_start_from);
	}
}

void SkeletonModification3DEWBIK::update_optimal_rotation_to_target_descendants(Skeleton3D *p_skeleton,
		Ref<EWBIKSegmentedSkeleton3D> r_chain,
		Ref<EWBIKSegmentedSkeleton3D> p_for_bone,
		float p_dampening, bool p_translate,
		int p_stabilization_passes, int p_iteration,
		int p_total_iterations) {
	if (p_for_bone.is_null()) {
		return;
	}
	Ref<EWBIKState> state = r_chain->mod->get_state();
	p_for_bone->force_update_bone_children_transforms(r_chain);
	BoneId bone = p_for_bone->bone;
	IKNode3D bone_xform = state->get_shadow_pose_global(bone);
	Quat best_orientation = bone_xform.get_global().get_rotation();
	float new_dampening = -1;
	if (p_for_bone->parent_item.is_null() || r_chain->localized_target_headings.size() == 1) {
		p_stabilization_passes = 0;
	}
	if (p_translate == true) {
		new_dampening = Math_PI;
	}

	update_target_headings(r_chain, r_chain->localized_target_headings);
	update_effector_headings(r_chain, r_chain->localized_effector_headings);

	float best_rmsd = 0.0f;
	Ref<QCP> qcp_convergence_check;
	qcp_convergence_check.instance();
	qcp_convergence_check->set_precision(FLT_EPSILON, FLT_EPSILON);
	float new_rmsd = 999999.0f;

	if (p_stabilization_passes > 0) {
		best_rmsd = get_manual_msd(r_chain->localized_effector_headings, r_chain->localized_target_headings,
				r_chain->weights);
	}

	for (int stabilization_i = 0; stabilization_i < p_stabilization_passes + 1; stabilization_i++) {
		update_optimal_rotation_to_target_descendants(
				p_skeleton,
				p_for_bone, new_dampening,
				p_translate,
				r_chain->localized_effector_headings,
				r_chain->localized_target_headings,
				r_chain->weights,
				qcp_convergence_check,
				p_iteration,
				p_total_iterations);

		if (p_stabilization_passes > 0) {
			update_effector_headings(r_chain, r_chain->localized_effector_headings);
			new_rmsd = get_manual_msd(r_chain->localized_effector_headings, r_chain->localized_target_headings,
					r_chain->weights);

			if (best_rmsd >= new_rmsd) {
				if (state->get_springy(bone)) {
					if (p_dampening != -1 || p_total_iterations != r_chain->get_default_iterations()) {
						float returnfullness = state->get_pain(bone);
						float dampened_angle = state->get_stiffness(bone) * p_dampening * returnfullness;
						float total_iterations_sq = p_total_iterations * p_total_iterations;
						float scaled_dampened_angle = dampened_angle *
													  ((total_iterations_sq - (p_iteration * p_iteration)) /
															  total_iterations_sq);
						float cos_half_angle = Math::cos(0.5f * scaled_dampened_angle);						
						p_for_bone->set_axes_to_returned(p_skeleton->get_bone_global_pose(bone), bone_xform.get_global_transform(),
								state->get_shadow_constraint_axes_global(bone).get_global_transform(), cos_half_angle,
								scaled_dampened_angle);
					} else {
						p_for_bone->set_axes_to_returned(p_skeleton->get_bone_global_pose(bone), bone_xform.get_global_transform(),
								state->get_shadow_constraint_axes_global(bone).get_global_transform(),
								state->get_cos_half_returnful_dampened(bone)[p_iteration],
								state->get_half_returnful_dampened(bone)[p_iteration]);
					}
					update_effector_headings(r_chain, r_chain->localized_effector_headings);
					new_rmsd = get_manual_msd(r_chain->localized_effector_headings, r_chain->localized_target_headings,
							r_chain->weights);
				}
				best_orientation = state->get_shadow_pose_global(bone).get_global().get_rotation();
				best_rmsd = new_rmsd;
				break;
			}
		} else {
			break;
		}
	}
	if (p_stabilization_passes > 0) {
		state->set_shadow_bone_pose_local(p_for_bone->bone, bone_xform.get_global());
		state->mark_dirty(p_for_bone->bone);
	}
}

float SkeletonModification3DEWBIK::get_manual_msd(Vector<Vector3> &r_localized_effector_headings,
		Vector<Vector3> &r_localized_target_headings,
		const Vector<real_t> &p_weights) {
	float manual_rmsd = 0.0f;
	float wsum = 0.0f;
	for (int i = 0; i < r_localized_target_headings.size(); i++) {
		float xd = r_localized_target_headings[i].x - r_localized_effector_headings[i].x;
		float yd = r_localized_target_headings[i].y - r_localized_effector_headings[i].y;
		float zd = r_localized_target_headings[i].z - r_localized_effector_headings[i].z;
		float mag_sq = p_weights[i] * (xd * xd + yd * yd + zd * zd);
		manual_rmsd += mag_sq;
		wsum += p_weights[i];
	}
	manual_rmsd /= wsum;
	return manual_rmsd;
}

void SkeletonModification3DEWBIK::update_target_headings(Ref<EWBIKSegmentedSkeleton3D> r_chain,
		Vector<Vector3> &r_localized_target_headings) {
	int hdx = 0;
	Ref<EWBIKState> state = r_chain->mod->get_state();
	for (int target_i = 0; target_i < r_chain->targets.size(); target_i++) {
		Ref<EWBIKBoneChainTarget> bct = r_chain->targets[target_i];
		if (bct.is_null()) {
			continue;
		}
		Ref<EWBIKBoneEffectorTransform> ee = bct->end_effector;
		if (ee.is_null()) {
			continue;
		}
		IKBasis pose = state->get_shadow_pose_global(ee->effector_bone).get_global();
		Vector3 origin = pose.get_origin();
		Transform target_axes = ee->goal_transform;
		r_localized_target_headings.write[hdx] = target_axes.origin - origin;
		uint8_t modeCode = r_chain->targets[target_i]->get_mode_code();
		hdx++;
		if ((modeCode & EWBIKBoneChainTarget::XDir) != 0) {
			Ray x_target = state->get_ray_x(ee->effector_bone);
			r_localized_target_headings.write[hdx] = x_target.p1 - origin;
			r_localized_target_headings.write[hdx + 1] = x_target.set_to_inverted_tip(r_localized_target_headings.write[hdx + 1]);
			r_localized_target_headings.write[hdx + 1] -= origin;
			hdx += 2;
		}
		if ((modeCode & EWBIKBoneChainTarget::YDir) != 0) {
			Ray y_target = state->get_ray_y(ee->effector_bone);
			r_localized_target_headings.write[hdx] = y_target.p1 - origin;
			r_localized_target_headings.write[hdx + 1] = y_target.set_to_inverted_tip(r_localized_target_headings.write[hdx + 1]);
			r_localized_target_headings.write[hdx + 1] -= origin;
			hdx += 2;
		}
		if ((modeCode & EWBIKBoneChainTarget::ZDir) != 0) {
			Ray z_target = state->get_ray_z(ee->effector_bone);
			r_localized_target_headings.write[hdx] = z_target.p1 - origin;
			r_localized_target_headings.write[hdx + 1] = z_target.set_to_inverted_tip(r_localized_target_headings.write[hdx + 1]);
			r_localized_target_headings.write[hdx + 1] -= origin;
			hdx += 2;
		}
	}
}

void SkeletonModification3DEWBIK::update_effector_headings(Ref<EWBIKSegmentedSkeleton3D> r_chain,
		Vector<Vector3> &r_localized_effector_headings) {
	int hdx = 0;
	Ref<EWBIKState> state = r_chain->mod->get_state();
	for (int target_i = 0; target_i < r_chain->targets.size(); target_i++) {
		Ref<EWBIKBoneChainTarget> bct = r_chain->targets[target_i];
		if (bct.is_null()) {
			continue;
		}
		Ref<EWBIKBoneEffectorTransform> ee = bct->end_effector;
		if (ee.is_null()) {
			continue;
		}
		int effector_bone = ee->effector_bone;
		IKBasis pose = state->get_shadow_pose_global(effector_bone).get_global();
		// BoneId bone = r_chain->bone;
		Vector3 origin = pose.get_origin();
		r_localized_effector_headings.write[hdx] = pose.get_origin() - origin;
		uint8_t modeCode = r_chain->targets[target_i]->get_mode_code();
		hdx++;
		if ((modeCode & EWBIKBoneChainTarget::XDir) != 0) {
			Ray x_target = state->get_ray_x(effector_bone);
			r_localized_effector_headings.write[hdx] = x_target.p1 - origin;
			r_localized_effector_headings.write[hdx + 1] = x_target.set_to_inverted_tip(r_localized_effector_headings.write[hdx + 1]);
			r_localized_effector_headings.write[hdx + 1] -= origin;
			hdx += 2;
		}
		if ((modeCode & EWBIKBoneChainTarget::YDir) != 0) {
			Ray y_target = state->get_ray_y(effector_bone);
			r_localized_effector_headings.write[hdx] = y_target.p1 - origin;
			r_localized_effector_headings.write[hdx + 1] = y_target.set_to_inverted_tip(r_localized_effector_headings.write[hdx + 1]);
			r_localized_effector_headings.write[hdx + 1] -= origin;
			hdx += 2;
		}
		if ((modeCode & EWBIKBoneChainTarget::ZDir) != 0) {
			Ray z_target = state->get_ray_z(effector_bone);
			r_localized_effector_headings.write[hdx] = z_target.p1 - origin;
			r_localized_effector_headings.write[hdx + 1] = z_target.set_to_inverted_tip(r_localized_effector_headings.write[hdx + 1]);
			r_localized_effector_headings.write[hdx + 1] -= origin;
			hdx += 2;
		}
	}
}

int EWBIKSegmentedSkeleton3D::get_default_iterations() const {
	return ik_iterations;
}

void EWBIKSegmentedSkeleton3D::rootwardly_update_falloff_cache_from(Ref<EWBIKSegmentedSkeleton3D> p_current) {
	Ref<EWBIKSegmentedSkeleton3D> current = p_current;
	while (current.is_valid()) {
		current->chain_root->create_headings_arrays();
		current = current->parent_item;
	}
}

void EWBIKSegmentedSkeleton3D::recursively_create_penalty_array(Ref<EWBIKSegmentedSkeleton3D> from,
		Vector<Vector<real_t>> &r_weight_array,
		Vector<Ref<EWBIKSegmentedSkeleton3D>>
				pin_sequence,
		float current_falloff) {
	if (current_falloff == 0) {
		return;
	} else {
		for (int32_t target_i = 0; target_i < from->targets.size(); target_i++) {
			Ref<EWBIKBoneChainTarget> target = from->targets[target_i];
			if (target.is_null()) {
				continue;
			}
			Vector<real_t> inner_weight_array;
			uint8_t mode_code = target->get_mode_code();
			inner_weight_array.push_back(target->get_target_weight() * current_falloff);
			float max_target_weight = 0.0f;
			if ((mode_code & EWBIKBoneChainTarget::XDir) != 0)
				max_target_weight = MAX(max_target_weight, target->get_x_priority());
			if ((mode_code & EWBIKBoneChainTarget::YDir) != 0)
				max_target_weight = MAX(max_target_weight, target->get_y_priority());
			if ((mode_code & EWBIKBoneChainTarget::ZDir) != 0)
				max_target_weight = MAX(max_target_weight, target->get_z_priority());

			if (max_target_weight == 0.0f)
				max_target_weight = 1.0f;

			max_target_weight = 1.0f;

			if ((mode_code & EWBIKBoneChainTarget::XDir) != 0) {
				float sub_target_weight = target->get_target_weight() * (target->get_x_priority() / max_target_weight) * current_falloff;
				inner_weight_array.push_back(sub_target_weight);
				inner_weight_array.push_back(sub_target_weight);
			}
			if ((mode_code & EWBIKBoneChainTarget::YDir) != 0) {
				float sub_target_weight = target->get_target_weight() * (target->get_y_priority() / max_target_weight) * current_falloff;
				inner_weight_array.push_back(sub_target_weight);
				inner_weight_array.push_back(sub_target_weight);
			}
			if ((mode_code & EWBIKBoneChainTarget::ZDir) != 0) {
				float sub_target_weight = target->get_target_weight() * (target->get_z_priority() / max_target_weight) * current_falloff;
				inner_weight_array.push_back(sub_target_weight);
				inner_weight_array.push_back(sub_target_weight);
			}
			pin_sequence.push_back(target->for_bone());
			r_weight_array.push_back(inner_weight_array);
			float this_falloff = target.is_null() ? 1.0f : target->get_depth_falloff();
			for (int32_t child_i = 0; child_i < children.size(); child_i++) {
				children.write[child_i]->recursively_create_penalty_array(from, r_weight_array, pin_sequence, current_falloff * this_falloff);
			}
		}
	}
}

void EWBIKSegmentedSkeleton3D::create_headings_arrays() {
	Vector<Vector<real_t>> penalty_array;
	Vector<Ref<EWBIKSegmentedSkeleton3D>> target_sequence;
	chain_root->recursively_create_penalty_array(this, penalty_array, target_sequence, 1.0f);
	Vector<Ref<EWBIKSegmentedSkeleton3D>> target_bones;
	target_bones.resize(target_sequence.size());
	int total_headings = 0;
	for (int32_t penalty_i = 0; penalty_i < penalty_array.size(); penalty_i++) {
		total_headings += penalty_array[penalty_i].size();
	}
	for (int pin_i = 0; pin_i < target_sequence.size(); pin_i++) {
		target_bones.write[pin_i] = target_sequence[pin_i];
	}
	localized_effector_headings.resize(total_headings);
	localized_target_headings.resize(total_headings);
	weights.resize(total_headings);
	int current_heading = 0;
	for (int32_t array_i = 0; array_i < penalty_array.size(); array_i++) {
		for (int32_t penalty_i = 0; penalty_i < penalty_array[array_i].size(); penalty_i++) {
			weights.write[current_heading] = penalty_array[array_i][penalty_i];
			localized_effector_headings.write[current_heading] = Vector3();
			localized_target_headings.write[current_heading] = Vector3();
			current_heading++;
		}
	}
}

void EWBIKSegmentedSkeleton3D::force_update_bone_children_transforms(Ref<EWBIKSegmentedSkeleton3D> p_current_chain) {
	Ref<EWBIKState> state = p_current_chain->mod->get_state();
	BoneId bone = p_current_chain->bone;
	state->set_bone_dirty(bone, true);
	Vector<Ref<EWBIKSegmentedSkeleton3D>> bones = p_current_chain->get_bones();
	ERR_FAIL_COND(!p_current_chain->is_chain_active());
	for (int32_t bone_i = 0; bone_i < bones.size(); bone_i++) {
		state->set_bone_dirty(bone, false);
		if (bones[bone_i]->parent_item.is_valid()) {
			state->set_bone_dirty(bone_i, true);
		}
	}
	Vector<Ref<EWBIKSegmentedSkeleton3D>> bone_chains = p_current_chain->get_child_chains();
	for (int32_t i = 0; i < bone_chains.size(); i++) {
		force_update_bone_children_transforms(bone_chains[i]);
	}
}

void EWBIKBoneChainTarget::set_parent_target(EWBIKBoneChainTarget *parent) {
	if (parent_target != NULL) {
		parent_target->remove_child_target(this);
	}
	//set the parent to the global axes if the user
	//tries to set the pin to be its own parent

	if (parent != NULL) {
		parent->add_child_target(this);
		parent_target = parent;
	}
}

void EWBIKBoneChainTarget::remove_child_target(EWBIKBoneChainTarget *child) {
	int32_t target_i = child_targets.find(child);
	if (target_i != -1) {
		child_targets.remove(target_i);
	}
}

void EWBIKBoneChainTarget::add_child_target(EWBIKBoneChainTarget *newChild) {
	if (newChild->is_ancestor_of(this)) {
		set_parent_target(newChild->get_parent_target());
	}
	if (child_targets.find(newChild) != -1) {
		child_targets.push_back(newChild);
	}
}

EWBIKBoneChainTarget *EWBIKBoneChainTarget::get_parent_target() {
	return parent_target;
}

bool EWBIKBoneChainTarget::is_ancestor_of(EWBIKBoneChainTarget *potentialDescendent) {
	bool result = false;
	EWBIKBoneChainTarget *cursor = potentialDescendent->get_parent_target();
	while (cursor) {
		if (cursor == this) {
			result = true;
			break;
		} else {
			cursor = cursor->parent_target;
		}
	}
	return result;
}

float EWBIKBoneChainTarget::get_target_weight() {
	return target_weight;
}

float EWBIKState::get_stiffness(int32_t p_bone) const {
	ERR_FAIL_INDEX_V(p_bone, bones.size(), -1);
	return bones[p_bone].sim_local_ik_node.get_stiffness();
}

void EWBIKState::set_stiffness(int32_t p_bone, float p_stiffness_scalar) {
	ERR_FAIL_INDEX(p_bone, bones.size());
	bones.write[p_bone].sim_local_ik_node.set_stiffness(p_stiffness_scalar);
}

float EWBIKState::get_height(int32_t p_bone) const {
	ERR_FAIL_INDEX_V(p_bone, bones.size(), -1);
	return bones[p_bone].sim_local_ik_node.get_height();
}

void EWBIKState::set_height(int32_t p_bone, float p_height) {
	ERR_FAIL_INDEX(p_bone, bones.size());
	return bones.write[p_bone].sim_local_ik_node.set_height(p_height);
}

Ref<KusudamaConstraint> EWBIKState::get_constraint(int32_t p_bone) const {
	ERR_FAIL_INDEX_V(p_bone, bones.size(), nullptr);
	return bones[p_bone].get_constraint();
}

void EWBIKState::set_constraint(int32_t p_bone, Ref<KusudamaConstraint> p_constraint) {
	ERR_FAIL_INDEX(p_bone, bones.size());
	bones.write[p_bone].set_constraint(p_constraint);
}

void EWBIKState::init(Ref<SkeletonModification3DEWBIK> p_mod) {
	ERR_FAIL_COND(p_mod.is_null());
	mod = p_mod;
	Ref<SkeletonModificationStack3D> stack = p_mod->get_modification_stack();
	ERR_FAIL_COND(stack.is_null());
	Skeleton3D *skeleton = stack->get_skeleton();
	if (!skeleton) {
		return;
	}
	set_bone_count(skeleton->get_bone_count());
	for (int32_t bone_i = 0; bone_i < skeleton->get_bone_count(); bone_i++) {
		set_stiffness(bone_i, -1);
		set_height(bone_i, -1);
		Ref<KusudamaConstraint> constraint;
		constraint.instance();
		constraint->set_name(skeleton->get_bone_name(bone_i));
		set_constraint(bone_i, constraint);
		Transform xform = skeleton->get_bone_global_pose(bone_i);
		BoneId parent = skeleton->get_bone_parent(bone_i);
		if (parent != -1) {
			xform = skeleton->get_bone_global_pose(parent).affine_inverse() * xform;
		}
		IKBasis basis = IKBasis(xform.origin, xform.basis.get_axis(0), xform.basis.get_axis(1), xform.basis.get_axis(2));
		set_shadow_bone_pose_local(bone_i, basis);
		set_parent(bone_i, skeleton->get_bone_parent(bone_i));
	}
}

void EWBIKState::_get_property_list(List<PropertyInfo> *p_list) const {
	p_list->push_back(PropertyInfo(Variant::INT, "bone_count"));
	for (int bone_i = 0; bone_i < get_bone_count(); bone_i++) {
		p_list->push_back(PropertyInfo(Variant::STRING, "bone/" + itos(bone_i) + "/name"));
		p_list->push_back(PropertyInfo(Variant::FLOAT, "bone/" + itos(bone_i) + "/stiffness"));
		p_list->push_back(PropertyInfo(Variant::FLOAT, "bone/" + itos(bone_i) + "/height"));
		p_list->push_back(PropertyInfo(Variant::FLOAT, "bone/" + itos(bone_i) + "/twist_min_angle"));
		p_list->push_back(PropertyInfo(Variant::FLOAT, "bone/" + itos(bone_i) + "/twist_range"));
		p_list->push_back(PropertyInfo(Variant::INT, "bone/" + itos(bone_i) + "/direction_count", PROPERTY_HINT_RANGE, "0,65535,1"));
		p_list->push_back(PropertyInfo(Variant::TRANSFORM, "bone/" + itos(bone_i) + "/constraint_axes"));
		Ref<KusudamaConstraint> kusudama = get_constraint(bone_i);
		if (kusudama.is_null()) {
			continue;
		}
		for (int j = 0; j < kusudama->get_direction_count(); j++) {
			p_list->push_back(PropertyInfo(Variant::FLOAT, "bone/" + itos(bone_i) + "/direction" + "/" + itos(j) + "/radius", PROPERTY_HINT_RANGE, "0,65535,or_greater"));
			p_list->push_back(PropertyInfo(Variant::VECTOR3, "bone/" + itos(bone_i) + "/direction" + "/" + itos(j) + "/control_point"));
		}
	}
}

bool EWBIKState::_get(const StringName &p_name, Variant &r_ret) const {
	String name = p_name;
	if (name == "bone_count") {
		r_ret = get_bone_count();
		return true;
	} else if (name.begins_with("bone/")) {
		int index = name.get_slicec('/', 1).to_int();
		String what = name.get_slicec('/', 2);
		if (what == "stiffness") {
			r_ret = get_stiffness(index);
			return true;
		} else if (what == "height") {
			r_ret = get_height(index);
			return true;
		}
		Ref<KusudamaConstraint> kusudama = get_constraint(index);
		if (kusudama.is_null()) {
			return false;
		}
		if (what == "name") {
			r_ret = kusudama->get_name();
			return true;
		} else if (what == "twist_min_angle") {
			r_ret = kusudama->get_twist_constraint()->get_min_twist_angle();
			return true;
		} else if (what == "twist_range") {
			r_ret = kusudama->get_twist_constraint()->get_range();
			return true;
		} else if (what == "limiting") {
			r_ret = kusudama->get_twist_constraint()->get_range();
			return true;
		} else if (what == "direction_count") {
			r_ret = kusudama->get_direction_count();
			return true;
		} else if (what == "constraint_axes") {
			r_ret = kusudama->get_constraint_axes();
			return true;
		} else if (what == "direction") {
			int direction_index = name.get_slicec('/', 3).to_int();
			ERR_FAIL_INDEX_V(direction_index, kusudama->get_direction_count(), false);
			Ref<DirectionConstraint> direction = kusudama->get_direction(direction_index);
			if (direction.is_null()) {
				return false;
			}
			String direction_what = name.get_slicec('/', 4);
			if (direction_what == "radius") {
				r_ret = direction->get_radius();
				return true;
			} else if (direction_what == "control_point") {
				r_ret = direction->get_control_point();
				return true;
			}
		}
	}
	return false;
}

bool EWBIKState::_set(const StringName &p_name, const Variant &p_value) {
	String name = p_name;
	if (name == "bone_count") {
		set_bone_count(p_value);
		return true;
	} else if (name.begins_with("bone/")) {
		int index = name.get_slicec('/', 1).to_int();
		String what = name.get_slicec('/', 2);
		ERR_FAIL_INDEX_V(index, bone_count, false);
		if (what == "stiffness") {
			set_stiffness(index, p_value);
			_change_notify();
		} else if (what == "height") {
			set_height(index, p_value);
			_change_notify();
		}
		Ref<KusudamaConstraint> constraint;
		constraint.instance();
		set_constraint(index, constraint);
		if (what == "name") {
			constraint->set_name(p_value);
			_change_notify();
			return true;
		} else if (what == "twist_min_angle") {
			Ref<TwistConstraint> twist = constraint->get_twist_constraint();
			twist->set_min_twist_angle(p_value);
			constraint->set_twist_constraint(twist);
			_change_notify();
			return true;
		} else if (what == "twist_range") {
			Ref<TwistConstraint> twist = constraint->get_twist_constraint();
			twist->set_range(p_value);
			constraint->set_twist_constraint(twist);
			_change_notify();
			return true;
		} else if (what == "constraint_axes") {
			constraint->set_constraint_axes(p_value);
			_change_notify();
			return true;
		} else if (what == "direction_count") {
			constraint->set_direction_count(p_value);
			_change_notify();
			return true;
		} else if (what == "direction") {
			int direction_index = name.get_slicec('/', 3).to_int();
			ERR_FAIL_INDEX_V(direction_index, constraint->get_direction_count(), false);
			Ref<DirectionConstraint> direction = constraint->get_direction(direction_index);
			if (direction.is_null()) {
				direction.instance();
				constraint->set_direction(direction_index, direction);
			}
			String direction_what = name.get_slicec('/', 4);
			if (direction_what == "radius") {
				direction->set_radius(p_value);
				_change_notify();
			} else if (direction_what == "control_point") {
				direction->set_control_point(p_value);
				// TODO
				// constraint->optimize_limiting_axes();
				_change_notify();
			} else {
				return false;
			}
			return true;
		}
		return true;
	}
	return false;
}
void EWBIKState::set_bone_count(int32_t p_bone_count) {
	bone_count = p_bone_count;
	bones.resize(p_bone_count);
	for (int32_t bone_i = 0; bone_i < p_bone_count; bone_i++) {
		bones.write[bone_i] = ShadowBone3D();
	}
}

IKNode3D EWBIKState::get_shadow_pose_global(int p_bone) const {
	ERR_FAIL_INDEX_V(p_bone, bones.size(), IKNode3D());
	return bones[p_bone].sim_local_ik_node;
}
//Transform EWBIKState::get_shadow_pose_local(int p_bone) const {
//	ERR_FAIL_INDEX_V(p_bone, bones.size(), Transform());
//	Transform xform;
//	const IKBasis &basis = bones[p_bone].sim_local_ik_node.get_local();
//	xform.origin = basis.get_origin();
//	xform.basis = basis.get_rotation();
//	return xform;
//}
void EWBIKState::set_shadow_bone_pose_local(int p_bone, const IKBasis &p_value) {
	ERR_FAIL_INDEX(p_bone, bones.size());
	bones.write[p_bone].sim_local_ik_node.set_local(p_value);
	mark_dirty(p_bone);
}
IKNode3D EWBIKState::get_shadow_constraint_axes_global(int p_bone) const {
	ERR_FAIL_INDEX_V(p_bone, bones.size(), IKNode3D());
	return bones[p_bone].sim_constraint_ik_node;
}
void EWBIKState::mark_dirty(int32_t p_bone) {
	ERR_FAIL_INDEX(p_bone, bones.size());
	bones.write[p_bone].sim_local_ik_node.dirty = true;
	bones.write[p_bone].sim_constraint_ik_node.dirty = true;
	update_skeleton();
}
bool EWBIKState::is_dirty(int32_t p_bone) const {
	ERR_FAIL_INDEX_V(p_bone, bones.size(), true);
	return bones[p_bone].sim_local_ik_node.dirty || bones[p_bone].sim_constraint_ik_node.dirty;
}
void EWBIKState::_update_process_order() {
	ShadowBone3D *bonesptr = bones.ptrw();
	int len = bones.size();

	parentless_bones.clear();

	for (int bone_i = 0; bone_i < len; bone_i++) {
		if (bonesptr[bone_i].sim_local_ik_node.parent >= len) {
			//validate this just in case
			ERR_PRINT("Bone " + itos(bone_i) + " has invalid parent: " + itos(bonesptr[bone_i].sim_local_ik_node.parent));
			bonesptr[bone_i].sim_local_ik_node.parent = -1;
		}
		bonesptr[bone_i].sim_local_ik_node.child_bones.clear();

		if (bonesptr[bone_i].sim_local_ik_node.parent != -1) {
			int parent_bone_idx = bonesptr[bone_i].sim_local_ik_node.parent;

			// Check to see if this node is already added to the parent:
			if (bonesptr[parent_bone_idx].sim_local_ik_node.child_bones.find(bone_i) < 0) {
				// Add the child node
				bonesptr[parent_bone_idx].sim_local_ik_node.child_bones.push_back(bone_i);
			} else {
				ERR_PRINT("IkNode3D parenthood graph is cyclic");
			}
		} else {
			parentless_bones.push_back(bone_i);
		}
	}
}
void EWBIKState::translate_to(int32_t p_bone, Vector3 p_target) {
	ERR_FAIL_INDEX(p_bone, bones.size());
	ShadowBone3D *bonesptr = bones.ptrw();
	mark_dirty(p_bone);
	if (bonesptr[p_bone].sim_local_ik_node.parent != -1) {
		bonesptr[p_bone].sim_local_ik_node.pose_local.translate_to(bonesptr[bonesptr[p_bone].sim_local_ik_node.parent].sim_local_ik_node.get_global().get_local_of(p_target));
		mark_dirty(p_bone);
	} else {
		bonesptr[p_bone].sim_local_ik_node.pose_local.translate_to(p_target);
		mark_dirty(p_bone);
	}
}
Ray EWBIKState::get_ray_x(int32_t p_bone) {
	ERR_FAIL_INDEX_V(p_bone, bones.size(), Ray());
	ShadowBone3D *bonesptr = bones.ptrw();
	return bonesptr[p_bone].sim_local_ik_node.get_global().get_x_ray();
}
Ray EWBIKState::get_ray_y(int32_t p_bone) {
	ERR_FAIL_INDEX_V(p_bone, bones.size(), Ray());
	ShadowBone3D *bonesptr = bones.ptrw();
	return bonesptr[p_bone].sim_local_ik_node.get_global().get_y_ray();
}
Ray EWBIKState::get_ray_z(int32_t p_bone) {
	ERR_FAIL_INDEX_V(p_bone, bones.size(), Ray());
	ShadowBone3D *bonesptr = bones.ptrw();
	return bonesptr[p_bone].sim_local_ik_node.get_global().get_z_ray();
}
void EWBIKState::rotate_about_x(int32_t p_bone, float angle) {
	ERR_FAIL_INDEX(p_bone, bones.size());
	ShadowBone3D *bonesptr = bones.ptrw();
	mark_dirty(p_bone);
	Quat xRot = Quat(bonesptr[p_bone].sim_local_ik_node.get_global().get_x_heading(), angle);
	rotate_by(p_bone, xRot);
	mark_dirty(p_bone);
}
void EWBIKState::rotate_about_y(int32_t p_bone, float angle) {
	ERR_FAIL_INDEX(p_bone, bones.size());
	ShadowBone3D *bonesptr = bones.ptrw();
	// bonesptr[p_bone].sim_local_ik_node.update_global();
	mark_dirty(p_bone);
	Quat yRot = Quat(bonesptr[p_bone].sim_local_ik_node.get_global().get_y_heading(), angle);
	rotate_by(p_bone, yRot);
	mark_dirty(p_bone);
}
void EWBIKState::rotate_about_z(int32_t p_bone, float angle) {
	ERR_FAIL_INDEX(p_bone, bones.size());
	ShadowBone3D *bonesptr = bones.ptrw();
	// bonesptr[p_bone].sim_local_ik_node.update_global();
	mark_dirty(p_bone);
	Quat zRot = Quat(bonesptr[p_bone].sim_local_ik_node.get_global().get_z_heading(), angle);
	rotate_by(p_bone, zRot);
	mark_dirty(p_bone);
}
void EWBIKState::force_update_bone_children_transforms(int p_bone_idx) {
	ERR_FAIL_INDEX(p_bone_idx, bones.size());
	ShadowBone3D *bonesptr = bones.ptrw();
	List<int> bones_to_process;
	bones_to_process.push_back(p_bone_idx);

	while (bones_to_process.size() > 0) {
		int current_bone_idx = bones_to_process[0];
		bones_to_process.erase(current_bone_idx);

		IKNode3D &bone = bonesptr[current_bone_idx].sim_local_ik_node;
		IKBasis pose = bone.get_local();
		if (bone.parent >= 0) {
			bone.pose_global = bonesptr[bone.parent].sim_local_ik_node.get_global() * pose;
		} else {
			bone.pose_global = pose;
		}
		// Add the bone's children to the list of bones to be processed
		int child_bone_size = bone.child_bones.size();
		for (int i = 0; i < child_bone_size; i++) {
			bones_to_process.push_back(bone.child_bones[i]);
		}
	}
	bones_to_process.push_back(p_bone_idx);
	while (bones_to_process.size() > 0) {
		int current_bone_idx = bones_to_process[0];
		bones_to_process.erase(current_bone_idx);

		IKNode3D &constraint = bonesptr[current_bone_idx].sim_constraint_ik_node;
		IKBasis constraint_axes = constraint.get_local();
		if (constraint.parent >= 0) {
			constraint.pose_global = bonesptr[constraint.parent].sim_constraint_ik_node.get_global() * constraint_axes;
		} else {
			constraint.pose_global = constraint_axes;
		}
		// Add the bone's children to the list of bones to be processed
		int child_bone_size = constraint.child_bones.size();
		for (int i = 0; i < child_bone_size; i++) {
			bones_to_process.push_back(constraint.child_bones[i]);
		}
	}
}
void EWBIKState::update_skeleton() {
	int32_t len = bones.size();
	for (int bone_i = 0; bone_i < len; bone_i++) {
		force_update_bone_children_transforms(bone_i);
		bones.write[bone_i].sim_local_ik_node.dirty = false;
		bones.write[bone_i].sim_constraint_ik_node.dirty = false;
	}
	for (int bone_i = 0; bone_i < parentless_bones.size(); bone_i++) {
		force_update_bone_children_transforms(parentless_bones[bone_i]);
		bones.write[parentless_bones[bone_i]].sim_local_ik_node.dirty = false;
		bones.write[parentless_bones[bone_i]].sim_constraint_ik_node.dirty = false;
	}
}
void EWBIKState::set_parent(int32_t p_bone, int32_t p_parent) {
	ERR_FAIL_INDEX(p_bone, bones.size());
	ERR_FAIL_COND(p_parent != -1 && (p_parent < 0));

	bones.write[p_bone].sim_local_ik_node.parent = p_parent;
	bones.write[p_bone].sim_constraint_ik_node.parent = p_parent;
	_update_process_order();
	mark_dirty(p_bone);
}
int32_t EWBIKState::get_parent(int32_t p_bone) const {
	ERR_FAIL_INDEX_V(p_bone, bones.size(), -1);
	return bones[p_bone].sim_local_ik_node.parent;
}
void EWBIKState::align_shadow_bone_globals_to(int p_bone, IKNode3D p_target) {
	ERR_FAIL_INDEX(p_bone, bones.size());
	force_update_bone_children_transforms(p_bone);
	if (get_parent(p_bone) != -1) {
		IKNode3D shadow_pose_global = get_shadow_pose_global(get_parent(p_bone));
		shadow_pose_global = global_shadow_pose_to_local_pose(p_bone, shadow_pose_global);
		set_shadow_bone_pose_local(p_bone, p_target.get_global());
	} else {
		IKNode3D shadow_pose_global = get_shadow_pose_global(p_bone);
		set_shadow_bone_pose_local(p_bone, shadow_pose_global.get_global());
	}
	mark_dirty(p_bone);
}
void EWBIKState::align_shadow_constraint_globals_to(int p_bone, Transform p_target) {
	ERR_FAIL_INDEX(p_bone, bones.size());
	force_update_bone_children_transforms(p_bone);
	if (get_parent(p_bone) != -1) {
		IKNode3D shadow_constraint_pose_global = get_shadow_constraint_axes_global(get_parent(p_bone));
		shadow_constraint_pose_global = global_constraint_pose_to_local_pose(p_bone, shadow_constraint_pose_global);
		set_shadow_constraint_axes_local(p_bone, shadow_constraint_pose_global.get_global());
	} else {
		IKNode3D shadow_pose_global = get_shadow_constraint_axes_global(p_bone);
		set_shadow_constraint_axes_local(p_bone, shadow_pose_global.get_global());
	}
	mark_dirty(p_bone);
}
void EWBIKState::set_shadow_constraint_axes_local(int p_bone, const IKBasis &value) {
	ERR_FAIL_INDEX(p_bone, bones.size());
	bones.write[p_bone].sim_constraint_ik_node.set_local(value);
	mark_dirty(p_bone);
}

Transform EWBIKState::get_shadow_constraint_axes_local(int p_bone) const {
	ERR_FAIL_INDEX_V(p_bone, bones.size(), Transform());
	Transform xform;
	const IKBasis &basis = bones[p_bone].sim_constraint_ik_node.get_local();
	xform.origin = basis.get_origin();
	xform.basis = basis.get_rotation();

	return xform;
}

IKNode3D EWBIKState::global_shadow_pose_to_local_pose(int p_bone_idx, IKNode3D p_global_pose) {
	if (bones[p_bone_idx].sim_local_ik_node.parent >= 0) {
		int parent_bone_idx = bones[p_bone_idx].sim_local_ik_node.parent;
		IKNode3D ik_node = bones[parent_bone_idx].sim_local_ik_node;
		IKBasis basis = ik_node.get_global();
		basis.translate_by(p_global_pose.get_global().get_origin());
		basis.rotate_by(p_global_pose.get_global().get_rotation());
		// Suspicious TODO
		ik_node.set_local(basis);
		return ik_node;
	} else {
		return p_global_pose;
	}
}
//
//void EWBIKState::translate_constraint_axes_by_global(int32_t p_bone, Vector3 p_translate_by) {
//	ERR_FAIL_INDEX(p_bone, bones.size());
//	IKBasis basis = bones.write[p_bone].sim_constraint_ik_node.get_local();
//	if (bones.write[p_bone].sim_constraint_ik_node.parent != -1) {
//		force_update_bone_children_transforms(p_bone);
//		basis.translate_to(bones.write[p_bone].sim_constraint_ik_node.get_global().get_origin() + p_translate_by);
//	} else {
//		basis.translate_by(p_translate_by);
//	}
//	bones.write[p_bone].sim_constraint_ik_node.set_local(basis);
//	mark_dirty(p_bone);
//}
float EWBIKState::get_cos_half_dampen(int32_t p_bone) const {
	ERR_FAIL_INDEX_V(p_bone, bones.size(), 0.0f);
	return bones[p_bone].cos_half_dampen;
}
void EWBIKState::set_cos_half_dampen(int32_t p_bone, float p_cos_half_dampen) {
	ERR_FAIL_INDEX(p_bone, bones.size());
	bones.write[p_bone].cos_half_dampen = p_cos_half_dampen;
}
Vector<float> EWBIKState::get_half_returnful_dampened(int32_t p_bone) const {
	ERR_FAIL_INDEX_V(p_bone, bones.size(), Vector<float>());
	return bones[p_bone].half_returnful_dampened;
}
Vector<float> EWBIKState::get_cos_half_returnful_dampened(int32_t p_bone) const {
	ERR_FAIL_INDEX_V(p_bone, bones.size(), Vector<float>());
	return bones[p_bone].cos_half_returnful_dampened;
}
void EWBIKState::set_cos_half_returnfullness_dampened(int32_t p_bone, Vector<float> p_dampened) {
	ERR_FAIL_INDEX(p_bone, bones.size());
	bones.write[p_bone].cos_half_returnful_dampened = p_dampened;
}

IKNode3D EWBIKState::global_constraint_pose_to_local_pose(int p_bone_idx, IKNode3D p_global_pose) {
	if (bones[p_bone_idx].sim_local_ik_node.parent >= 0) {
		int parent_bone_idx = bones[p_bone_idx].sim_constraint_ik_node.parent;
		IKNode3D ik_node = bones[parent_bone_idx].sim_constraint_ik_node;
		IKBasis basis = ik_node.get_global();
		basis.translate_by(p_global_pose.get_global().get_origin());
		basis.rotate_by(p_global_pose.get_global().get_rotation());
		// Suspicious TODO
		ik_node.set_local(basis);
		return ik_node;
	} else {
		return p_global_pose;
	}
}

void EWBIKState::set_half_returnfullness_dampened(int32_t p_bone, Vector<float> p_dampened) {
	ERR_FAIL_INDEX(p_bone, bones.size());
	bones.write[p_bone].half_returnful_dampened = p_dampened;
}
void ShadowBone3D::set_constraint(Ref<KusudamaConstraint> p_constraint) {
	constraint = p_constraint;
}
Ref<KusudamaConstraint> ShadowBone3D::get_constraint() const {
	return constraint;
}
void ShadowBone3D::update_cos_dampening(int p_default_iterations, float p_default_dampening) {
	float predampening = 1.0f - sim_local_ik_node.stiffness;
	float dampening = sim_constraint_ik_node.parent == -1 ? Math_PI : predampening * p_default_dampening;
	cos_half_dampen = Math::cos(dampening / 2.0f);
	if (constraint.is_valid() && constraint->get_pain() != 0.0f) {
		springy = true;
		populate_return_dampening_iteration_array(p_default_iterations, p_default_dampening);
	} else {
		springy = false;
	}
}
void ShadowBone3D::populate_return_dampening_iteration_array(int p_default_iterations, float p_default_dampening) {
	float predampen = 1.0f - sim_constraint_ik_node.stiffness;
	float dampening = sim_constraint_ik_node.parent == -1 ? Math_PI : predampen * p_default_dampening;
	float returnful = constraint->get_pain();
	float falloff = 0.2f;
	half_returnful_dampened.resize(p_default_iterations);
	cos_half_returnful_dampened.resize(p_default_iterations);
	float iterations_pow = Math::pow(p_default_iterations, falloff * p_default_iterations * returnful);
	for (int32_t iter_i = 0; iter_i < p_default_iterations; iter_i++) {
		float iteration_scalar =
				((iterations_pow)-Math::pow(iter_i, falloff * p_default_iterations * returnful)) / (iterations_pow);
		float iteration_return_clamp = iteration_scalar * returnful * dampening;
		float cos_iteration_return_clamp = Math::cos(iteration_return_clamp / 2.0f);
		half_returnful_dampened.write[iter_i] = iteration_return_clamp;
		cos_half_returnful_dampened.write[iter_i] = cos_iteration_return_clamp;
	}
}
void EWBIKState::set_bone_dirty(int p_bone, bool p_dirty) {
	ERR_FAIL_INDEX(p_bone, bones.size());
	bones.write[p_bone].sim_local_ik_node.dirty = p_dirty;
	bones.write[p_bone].sim_constraint_ik_node.dirty = p_dirty;
}
bool EWBIKState::get_bone_dirty(int p_bone) const {
	ERR_FAIL_INDEX_V(p_bone, bones.size(), true);
	return bones[p_bone].sim_local_ik_node.dirty || bones[p_bone].sim_constraint_ik_node.dirty;
}
void EWBIKState::rotate_to(int32_t p_bone, Quat p_rot) {
	ERR_FAIL_INDEX(p_bone, bones.size());
	ShadowBone3D *bonesptr = bones.ptrw();
	bonesptr[p_bone].sim_local_ik_node.pose_local.rotate_to(p_rot);
	mark_dirty(p_bone);
}

void EWBIKState::rotate_by(int32_t p_bone, Quat p_add_rotation) {
	ERR_FAIL_INDEX(p_bone, bones.size());
	ShadowBone3D *bonesptr = bones.ptrw();
	// bonesptr[p_bone].sim_local_ik_node.update_global();
	if (bonesptr[p_bone].sim_local_ik_node.parent != -1) {
		Quat new_rot = bonesptr[bonesptr[p_bone].sim_local_ik_node.parent].sim_local_ik_node.get_global().get_local_of_rotation(p_add_rotation);
		bonesptr[p_bone].sim_local_ik_node.get_local().rotate_by(new_rot);
	} else {
		bonesptr[p_bone].sim_local_ik_node.get_local().rotate_by(p_add_rotation);
	}
	mark_dirty(p_bone);
}
void EWBIKState::set_springy(int32_t p_bone, bool p_springy) {
	ERR_FAIL_INDEX(p_bone, bones.size());
	bones.write[p_bone].springy = p_springy;
}
bool EWBIKState::get_springy(int32_t p_bone) const {
	ERR_FAIL_INDEX_V(p_bone, bones.size(), false);
	return bones[p_bone].springy;
}