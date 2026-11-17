#pragma once
#include "rednose/helpers/ekf.h"
extern "C" {
void pose_update_4(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_10(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_13(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_14(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_err_fun(double *nom_x, double *delta_x, double *out_7243860342894984546);
void pose_inv_err_fun(double *nom_x, double *true_x, double *out_4584043442594633794);
void pose_H_mod_fun(double *state, double *out_1266553482076709236);
void pose_f_fun(double *state, double dt, double *out_5083776387329352420);
void pose_F_fun(double *state, double dt, double *out_3141212682405851061);
void pose_h_4(double *state, double *unused, double *out_3979435972361617785);
void pose_H_4(double *state, double *unused, double *out_8167206171719157888);
void pose_h_10(double *state, double *unused, double *out_3349891990278921599);
void pose_H_10(double *state, double *unused, double *out_4622200334136351809);
void pose_h_13(double *state, double *unused, double *out_5229982678955302054);
void pose_H_13(double *state, double *unused, double *out_4954932346386825087);
void pose_h_14(double *state, double *unused, double *out_5256885464416768991);
void pose_H_14(double *state, double *unused, double *out_4203965315379673359);
void pose_predict(double *in_x, double *in_P, double *in_Q, double dt);
}