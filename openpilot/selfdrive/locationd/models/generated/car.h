#pragma once
#include "rednose/helpers/ekf.h"
extern "C" {
void car_update_25(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_24(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_30(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_26(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_27(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_29(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_28(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_31(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_err_fun(double *nom_x, double *delta_x, double *out_93630131099483078);
void car_inv_err_fun(double *nom_x, double *true_x, double *out_8677716757174088306);
void car_H_mod_fun(double *state, double *out_8542951934772758986);
void car_f_fun(double *state, double dt, double *out_4805745385691618402);
void car_F_fun(double *state, double dt, double *out_2344257086088637941);
void car_h_25(double *state, double *unused, double *out_4462526449509899381);
void car_H_25(double *state, double *unused, double *out_3406769179046762679);
void car_h_24(double *state, double *unused, double *out_9208820493134158213);
void car_H_24(double *state, double *unused, double *out_1462045685980944173);
void car_h_30(double *state, double *unused, double *out_6220952367378979794);
void car_H_30(double *state, double *unused, double *out_1120927151080845519);
void car_h_26(double *state, double *unused, double *out_9189005654886239137);
void car_H_26(double *state, double *unused, double *out_334734139827293545);
void car_h_27(double *state, double *unused, double *out_5406036302411561816);
void car_H_27(double *state, double *unused, double *out_3295690462881270430);
void car_h_29(double *state, double *unused, double *out_7224118438669726177);
void car_H_29(double *state, double *unused, double *out_610695806766453335);
void car_h_28(double *state, double *unused, double *out_8442822353855894501);
void car_H_28(double *state, double *unused, double *out_5693094823835983909);
void car_h_31(double *state, double *unused, double *out_2724426454948158948);
void car_H_31(double *state, double *unused, double *out_960942242060645021);
void car_predict(double *in_x, double *in_P, double *in_Q, double dt);
void car_set_rotational_inertia(double x);
void car_set_center_to_front(double x);
void car_set_center_to_rear(double x);
void car_set_stiffness_front(double x);
void car_set_stiffness_rear(double x);
}