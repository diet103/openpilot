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
void car_err_fun(double *nom_x, double *delta_x, double *out_5610978937466055828);
void car_inv_err_fun(double *nom_x, double *true_x, double *out_5480576221247925480);
void car_H_mod_fun(double *state, double *out_4661936272521892545);
void car_f_fun(double *state, double dt, double *out_5772486210202038372);
void car_F_fun(double *state, double dt, double *out_7489111653634861418);
void car_h_25(double *state, double *unused, double *out_5254441042497304880);
void car_H_25(double *state, double *unused, double *out_1138887633876527397);
void car_h_24(double *state, double *unused, double *out_6721044703978417638);
void car_H_24(double *state, double *unused, double *out_4696813232678850533);
void car_h_30(double *state, double *unused, double *out_2306715052141819919);
void car_H_30(double *state, double *unused, double *out_8055577975368144152);
void car_h_26(double *state, double *unused, double *out_8336149587772056062);
void car_H_26(double *state, double *unused, double *out_4443413603637327998);
void car_h_27(double *state, double *unused, double *out_6197950895398967315);
void car_H_27(double *state, double *unused, double *out_5880814663567719241);
void car_h_29(double *state, double *unused, double *out_3250224905043482354);
void car_H_29(double *state, double *unused, double *out_8565809319682536336);
void car_h_28(double *state, double *unused, double *out_9212007126866251616);
void car_H_28(double *state, double *unused, double *out_3483410302613005762);
void car_h_31(double *state, double *unused, double *out_4428788936709462024);
void car_H_31(double *state, double *unused, double *out_8215562884388344650);
void car_predict(double *in_x, double *in_P, double *in_Q, double dt);
void car_set_rotational_inertia(double x);
void car_set_center_to_front(double x);
void car_set_center_to_rear(double x);
void car_set_stiffness_front(double x);
void car_set_stiffness_rear(double x);
}