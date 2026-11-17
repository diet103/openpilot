#pragma once
#include "rednose/helpers/ekf.h"
extern "C" {
void pose_update_4(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_10(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_13(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_14(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_err_fun(double *nom_x, double *delta_x, double *out_7442492993183452915);
void pose_inv_err_fun(double *nom_x, double *true_x, double *out_6140138630151309189);
void pose_H_mod_fun(double *state, double *out_5396528562455239188);
void pose_f_fun(double *state, double dt, double *out_3110033524529218965);
void pose_F_fun(double *state, double dt, double *out_2384259379620246573);
void pose_h_4(double *state, double *unused, double *out_8005419480238059638);
void pose_H_4(double *state, double *unused, double *out_4198581080572341084);
void pose_h_10(double *state, double *unused, double *out_3547829476479846202);
void pose_H_10(double *state, double *unused, double *out_7942659657018811961);
void pose_h_13(double *state, double *unused, double *out_4912143115167447329);
void pose_H_13(double *state, double *unused, double *out_986307255240008283);
void pose_h_14(double *state, double *unused, double *out_3408798223072298079);
void pose_H_14(double *state, double *unused, double *out_7281369512867713380);
void pose_predict(double *in_x, double *in_P, double *in_Q, double dt);
}