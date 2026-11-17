#include "car.h"

namespace {
#define DIM 9
#define EDIM 9
#define MEDIM 9
typedef void (*Hfun)(double *, double *, double *);

double rotational_inertia;

void set_rotational_inertia(double x){ rotational_inertia = x;}

double center_to_front;

void set_center_to_front(double x){ center_to_front = x;}

double center_to_rear;

void set_center_to_rear(double x){ center_to_rear = x;}

double stiffness_front;

void set_stiffness_front(double x){ stiffness_front = x;}

double stiffness_rear;

void set_stiffness_rear(double x){ stiffness_rear = x;}
const static double MAHA_THRESH_25 = 3.8414588206941227;
const static double MAHA_THRESH_24 = 5.991464547107981;
const static double MAHA_THRESH_30 = 3.8414588206941227;
const static double MAHA_THRESH_26 = 3.8414588206941227;
const static double MAHA_THRESH_27 = 3.8414588206941227;
const static double MAHA_THRESH_29 = 3.8414588206941227;
const static double MAHA_THRESH_28 = 3.8414588206941227;
const static double MAHA_THRESH_31 = 3.8414588206941227;

/******************************************************************************
 *                      Code generated with SymPy 1.14.0                      *
 *                                                                            *
 *              See http://www.sympy.org/ for more information.               *
 *                                                                            *
 *                         This file is part of 'ekf'                         *
 ******************************************************************************/
void err_fun(double *nom_x, double *delta_x, double *out_93630131099483078) {
   out_93630131099483078[0] = delta_x[0] + nom_x[0];
   out_93630131099483078[1] = delta_x[1] + nom_x[1];
   out_93630131099483078[2] = delta_x[2] + nom_x[2];
   out_93630131099483078[3] = delta_x[3] + nom_x[3];
   out_93630131099483078[4] = delta_x[4] + nom_x[4];
   out_93630131099483078[5] = delta_x[5] + nom_x[5];
   out_93630131099483078[6] = delta_x[6] + nom_x[6];
   out_93630131099483078[7] = delta_x[7] + nom_x[7];
   out_93630131099483078[8] = delta_x[8] + nom_x[8];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_8677716757174088306) {
   out_8677716757174088306[0] = -nom_x[0] + true_x[0];
   out_8677716757174088306[1] = -nom_x[1] + true_x[1];
   out_8677716757174088306[2] = -nom_x[2] + true_x[2];
   out_8677716757174088306[3] = -nom_x[3] + true_x[3];
   out_8677716757174088306[4] = -nom_x[4] + true_x[4];
   out_8677716757174088306[5] = -nom_x[5] + true_x[5];
   out_8677716757174088306[6] = -nom_x[6] + true_x[6];
   out_8677716757174088306[7] = -nom_x[7] + true_x[7];
   out_8677716757174088306[8] = -nom_x[8] + true_x[8];
}
void H_mod_fun(double *state, double *out_8542951934772758986) {
   out_8542951934772758986[0] = 1.0;
   out_8542951934772758986[1] = 0.0;
   out_8542951934772758986[2] = 0.0;
   out_8542951934772758986[3] = 0.0;
   out_8542951934772758986[4] = 0.0;
   out_8542951934772758986[5] = 0.0;
   out_8542951934772758986[6] = 0.0;
   out_8542951934772758986[7] = 0.0;
   out_8542951934772758986[8] = 0.0;
   out_8542951934772758986[9] = 0.0;
   out_8542951934772758986[10] = 1.0;
   out_8542951934772758986[11] = 0.0;
   out_8542951934772758986[12] = 0.0;
   out_8542951934772758986[13] = 0.0;
   out_8542951934772758986[14] = 0.0;
   out_8542951934772758986[15] = 0.0;
   out_8542951934772758986[16] = 0.0;
   out_8542951934772758986[17] = 0.0;
   out_8542951934772758986[18] = 0.0;
   out_8542951934772758986[19] = 0.0;
   out_8542951934772758986[20] = 1.0;
   out_8542951934772758986[21] = 0.0;
   out_8542951934772758986[22] = 0.0;
   out_8542951934772758986[23] = 0.0;
   out_8542951934772758986[24] = 0.0;
   out_8542951934772758986[25] = 0.0;
   out_8542951934772758986[26] = 0.0;
   out_8542951934772758986[27] = 0.0;
   out_8542951934772758986[28] = 0.0;
   out_8542951934772758986[29] = 0.0;
   out_8542951934772758986[30] = 1.0;
   out_8542951934772758986[31] = 0.0;
   out_8542951934772758986[32] = 0.0;
   out_8542951934772758986[33] = 0.0;
   out_8542951934772758986[34] = 0.0;
   out_8542951934772758986[35] = 0.0;
   out_8542951934772758986[36] = 0.0;
   out_8542951934772758986[37] = 0.0;
   out_8542951934772758986[38] = 0.0;
   out_8542951934772758986[39] = 0.0;
   out_8542951934772758986[40] = 1.0;
   out_8542951934772758986[41] = 0.0;
   out_8542951934772758986[42] = 0.0;
   out_8542951934772758986[43] = 0.0;
   out_8542951934772758986[44] = 0.0;
   out_8542951934772758986[45] = 0.0;
   out_8542951934772758986[46] = 0.0;
   out_8542951934772758986[47] = 0.0;
   out_8542951934772758986[48] = 0.0;
   out_8542951934772758986[49] = 0.0;
   out_8542951934772758986[50] = 1.0;
   out_8542951934772758986[51] = 0.0;
   out_8542951934772758986[52] = 0.0;
   out_8542951934772758986[53] = 0.0;
   out_8542951934772758986[54] = 0.0;
   out_8542951934772758986[55] = 0.0;
   out_8542951934772758986[56] = 0.0;
   out_8542951934772758986[57] = 0.0;
   out_8542951934772758986[58] = 0.0;
   out_8542951934772758986[59] = 0.0;
   out_8542951934772758986[60] = 1.0;
   out_8542951934772758986[61] = 0.0;
   out_8542951934772758986[62] = 0.0;
   out_8542951934772758986[63] = 0.0;
   out_8542951934772758986[64] = 0.0;
   out_8542951934772758986[65] = 0.0;
   out_8542951934772758986[66] = 0.0;
   out_8542951934772758986[67] = 0.0;
   out_8542951934772758986[68] = 0.0;
   out_8542951934772758986[69] = 0.0;
   out_8542951934772758986[70] = 1.0;
   out_8542951934772758986[71] = 0.0;
   out_8542951934772758986[72] = 0.0;
   out_8542951934772758986[73] = 0.0;
   out_8542951934772758986[74] = 0.0;
   out_8542951934772758986[75] = 0.0;
   out_8542951934772758986[76] = 0.0;
   out_8542951934772758986[77] = 0.0;
   out_8542951934772758986[78] = 0.0;
   out_8542951934772758986[79] = 0.0;
   out_8542951934772758986[80] = 1.0;
}
void f_fun(double *state, double dt, double *out_4805745385691618402) {
   out_4805745385691618402[0] = state[0];
   out_4805745385691618402[1] = state[1];
   out_4805745385691618402[2] = state[2];
   out_4805745385691618402[3] = state[3];
   out_4805745385691618402[4] = state[4];
   out_4805745385691618402[5] = dt*(stiffness_front*(-state[2] - state[3] + state[7])*state[0]/state[1] + (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/state[4] + ((-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/state[4] - state[4])*state[6] - 9.8100000000000005*state[8]) + state[5];
   out_4805745385691618402[6] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*state[4])) + state[6];
   out_4805745385691618402[7] = state[7];
   out_4805745385691618402[8] = state[8];
}
void F_fun(double *state, double dt, double *out_2344257086088637941) {
   out_2344257086088637941[0] = 1;
   out_2344257086088637941[1] = 0;
   out_2344257086088637941[2] = 0;
   out_2344257086088637941[3] = 0;
   out_2344257086088637941[4] = 0;
   out_2344257086088637941[5] = 0;
   out_2344257086088637941[6] = 0;
   out_2344257086088637941[7] = 0;
   out_2344257086088637941[8] = 0;
   out_2344257086088637941[9] = 0;
   out_2344257086088637941[10] = 1;
   out_2344257086088637941[11] = 0;
   out_2344257086088637941[12] = 0;
   out_2344257086088637941[13] = 0;
   out_2344257086088637941[14] = 0;
   out_2344257086088637941[15] = 0;
   out_2344257086088637941[16] = 0;
   out_2344257086088637941[17] = 0;
   out_2344257086088637941[18] = 0;
   out_2344257086088637941[19] = 0;
   out_2344257086088637941[20] = 1;
   out_2344257086088637941[21] = 0;
   out_2344257086088637941[22] = 0;
   out_2344257086088637941[23] = 0;
   out_2344257086088637941[24] = 0;
   out_2344257086088637941[25] = 0;
   out_2344257086088637941[26] = 0;
   out_2344257086088637941[27] = 0;
   out_2344257086088637941[28] = 0;
   out_2344257086088637941[29] = 0;
   out_2344257086088637941[30] = 1;
   out_2344257086088637941[31] = 0;
   out_2344257086088637941[32] = 0;
   out_2344257086088637941[33] = 0;
   out_2344257086088637941[34] = 0;
   out_2344257086088637941[35] = 0;
   out_2344257086088637941[36] = 0;
   out_2344257086088637941[37] = 0;
   out_2344257086088637941[38] = 0;
   out_2344257086088637941[39] = 0;
   out_2344257086088637941[40] = 1;
   out_2344257086088637941[41] = 0;
   out_2344257086088637941[42] = 0;
   out_2344257086088637941[43] = 0;
   out_2344257086088637941[44] = 0;
   out_2344257086088637941[45] = dt*(stiffness_front*(-state[2] - state[3] + state[7])/state[1] + (-stiffness_front - stiffness_rear)*state[5]/state[4] + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[6]/state[4]);
   out_2344257086088637941[46] = -dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/pow(state[1], 2);
   out_2344257086088637941[47] = -dt*stiffness_front*state[0]/state[1];
   out_2344257086088637941[48] = -dt*stiffness_front*state[0]/state[1];
   out_2344257086088637941[49] = dt*(-(-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/pow(state[4], 2) + (-(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/pow(state[4], 2) - 1)*state[6]);
   out_2344257086088637941[50] = dt*(-stiffness_front*state[0] - stiffness_rear*state[0])/state[4] + 1;
   out_2344257086088637941[51] = dt*((-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/state[4] - state[4]);
   out_2344257086088637941[52] = dt*stiffness_front*state[0]/state[1];
   out_2344257086088637941[53] = -9.8100000000000005*dt;
   out_2344257086088637941[54] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front - pow(center_to_rear, 2)*stiffness_rear)*state[6]/(rotational_inertia*state[4]));
   out_2344257086088637941[55] = -center_to_front*dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*pow(state[1], 2));
   out_2344257086088637941[56] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_2344257086088637941[57] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_2344257086088637941[58] = dt*(-(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*pow(state[4], 2)) - (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*pow(state[4], 2)));
   out_2344257086088637941[59] = dt*(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(rotational_inertia*state[4]);
   out_2344257086088637941[60] = dt*(-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])/(rotational_inertia*state[4]) + 1;
   out_2344257086088637941[61] = center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_2344257086088637941[62] = 0;
   out_2344257086088637941[63] = 0;
   out_2344257086088637941[64] = 0;
   out_2344257086088637941[65] = 0;
   out_2344257086088637941[66] = 0;
   out_2344257086088637941[67] = 0;
   out_2344257086088637941[68] = 0;
   out_2344257086088637941[69] = 0;
   out_2344257086088637941[70] = 1;
   out_2344257086088637941[71] = 0;
   out_2344257086088637941[72] = 0;
   out_2344257086088637941[73] = 0;
   out_2344257086088637941[74] = 0;
   out_2344257086088637941[75] = 0;
   out_2344257086088637941[76] = 0;
   out_2344257086088637941[77] = 0;
   out_2344257086088637941[78] = 0;
   out_2344257086088637941[79] = 0;
   out_2344257086088637941[80] = 1;
}
void h_25(double *state, double *unused, double *out_4462526449509899381) {
   out_4462526449509899381[0] = state[6];
}
void H_25(double *state, double *unused, double *out_3406769179046762679) {
   out_3406769179046762679[0] = 0;
   out_3406769179046762679[1] = 0;
   out_3406769179046762679[2] = 0;
   out_3406769179046762679[3] = 0;
   out_3406769179046762679[4] = 0;
   out_3406769179046762679[5] = 0;
   out_3406769179046762679[6] = 1;
   out_3406769179046762679[7] = 0;
   out_3406769179046762679[8] = 0;
}
void h_24(double *state, double *unused, double *out_9208820493134158213) {
   out_9208820493134158213[0] = state[4];
   out_9208820493134158213[1] = state[5];
}
void H_24(double *state, double *unused, double *out_1462045685980944173) {
   out_1462045685980944173[0] = 0;
   out_1462045685980944173[1] = 0;
   out_1462045685980944173[2] = 0;
   out_1462045685980944173[3] = 0;
   out_1462045685980944173[4] = 1;
   out_1462045685980944173[5] = 0;
   out_1462045685980944173[6] = 0;
   out_1462045685980944173[7] = 0;
   out_1462045685980944173[8] = 0;
   out_1462045685980944173[9] = 0;
   out_1462045685980944173[10] = 0;
   out_1462045685980944173[11] = 0;
   out_1462045685980944173[12] = 0;
   out_1462045685980944173[13] = 0;
   out_1462045685980944173[14] = 1;
   out_1462045685980944173[15] = 0;
   out_1462045685980944173[16] = 0;
   out_1462045685980944173[17] = 0;
}
void h_30(double *state, double *unused, double *out_6220952367378979794) {
   out_6220952367378979794[0] = state[4];
}
void H_30(double *state, double *unused, double *out_1120927151080845519) {
   out_1120927151080845519[0] = 0;
   out_1120927151080845519[1] = 0;
   out_1120927151080845519[2] = 0;
   out_1120927151080845519[3] = 0;
   out_1120927151080845519[4] = 1;
   out_1120927151080845519[5] = 0;
   out_1120927151080845519[6] = 0;
   out_1120927151080845519[7] = 0;
   out_1120927151080845519[8] = 0;
}
void h_26(double *state, double *unused, double *out_9189005654886239137) {
   out_9189005654886239137[0] = state[7];
}
void H_26(double *state, double *unused, double *out_334734139827293545) {
   out_334734139827293545[0] = 0;
   out_334734139827293545[1] = 0;
   out_334734139827293545[2] = 0;
   out_334734139827293545[3] = 0;
   out_334734139827293545[4] = 0;
   out_334734139827293545[5] = 0;
   out_334734139827293545[6] = 0;
   out_334734139827293545[7] = 1;
   out_334734139827293545[8] = 0;
}
void h_27(double *state, double *unused, double *out_5406036302411561816) {
   out_5406036302411561816[0] = state[3];
}
void H_27(double *state, double *unused, double *out_3295690462881270430) {
   out_3295690462881270430[0] = 0;
   out_3295690462881270430[1] = 0;
   out_3295690462881270430[2] = 0;
   out_3295690462881270430[3] = 1;
   out_3295690462881270430[4] = 0;
   out_3295690462881270430[5] = 0;
   out_3295690462881270430[6] = 0;
   out_3295690462881270430[7] = 0;
   out_3295690462881270430[8] = 0;
}
void h_29(double *state, double *unused, double *out_7224118438669726177) {
   out_7224118438669726177[0] = state[1];
}
void H_29(double *state, double *unused, double *out_610695806766453335) {
   out_610695806766453335[0] = 0;
   out_610695806766453335[1] = 1;
   out_610695806766453335[2] = 0;
   out_610695806766453335[3] = 0;
   out_610695806766453335[4] = 0;
   out_610695806766453335[5] = 0;
   out_610695806766453335[6] = 0;
   out_610695806766453335[7] = 0;
   out_610695806766453335[8] = 0;
}
void h_28(double *state, double *unused, double *out_8442822353855894501) {
   out_8442822353855894501[0] = state[0];
}
void H_28(double *state, double *unused, double *out_5693094823835983909) {
   out_5693094823835983909[0] = 1;
   out_5693094823835983909[1] = 0;
   out_5693094823835983909[2] = 0;
   out_5693094823835983909[3] = 0;
   out_5693094823835983909[4] = 0;
   out_5693094823835983909[5] = 0;
   out_5693094823835983909[6] = 0;
   out_5693094823835983909[7] = 0;
   out_5693094823835983909[8] = 0;
}
void h_31(double *state, double *unused, double *out_2724426454948158948) {
   out_2724426454948158948[0] = state[8];
}
void H_31(double *state, double *unused, double *out_960942242060645021) {
   out_960942242060645021[0] = 0;
   out_960942242060645021[1] = 0;
   out_960942242060645021[2] = 0;
   out_960942242060645021[3] = 0;
   out_960942242060645021[4] = 0;
   out_960942242060645021[5] = 0;
   out_960942242060645021[6] = 0;
   out_960942242060645021[7] = 0;
   out_960942242060645021[8] = 1;
}
#include <eigen3/Eigen/Dense>
#include <iostream>

typedef Eigen::Matrix<double, DIM, DIM, Eigen::RowMajor> DDM;
typedef Eigen::Matrix<double, EDIM, EDIM, Eigen::RowMajor> EEM;
typedef Eigen::Matrix<double, DIM, EDIM, Eigen::RowMajor> DEM;

void predict(double *in_x, double *in_P, double *in_Q, double dt) {
  typedef Eigen::Matrix<double, MEDIM, MEDIM, Eigen::RowMajor> RRM;

  double nx[DIM] = {0};
  double in_F[EDIM*EDIM] = {0};

  // functions from sympy
  f_fun(in_x, dt, nx);
  F_fun(in_x, dt, in_F);


  EEM F(in_F);
  EEM P(in_P);
  EEM Q(in_Q);

  RRM F_main = F.topLeftCorner(MEDIM, MEDIM);
  P.topLeftCorner(MEDIM, MEDIM) = (F_main * P.topLeftCorner(MEDIM, MEDIM)) * F_main.transpose();
  P.topRightCorner(MEDIM, EDIM - MEDIM) = F_main * P.topRightCorner(MEDIM, EDIM - MEDIM);
  P.bottomLeftCorner(EDIM - MEDIM, MEDIM) = P.bottomLeftCorner(EDIM - MEDIM, MEDIM) * F_main.transpose();

  P = P + dt*Q;

  // copy out state
  memcpy(in_x, nx, DIM * sizeof(double));
  memcpy(in_P, P.data(), EDIM * EDIM * sizeof(double));
}

// note: extra_args dim only correct when null space projecting
// otherwise 1
template <int ZDIM, int EADIM, bool MAHA_TEST>
void update(double *in_x, double *in_P, Hfun h_fun, Hfun H_fun, Hfun Hea_fun, double *in_z, double *in_R, double *in_ea, double MAHA_THRESHOLD) {
  typedef Eigen::Matrix<double, ZDIM, ZDIM, Eigen::RowMajor> ZZM;
  typedef Eigen::Matrix<double, ZDIM, DIM, Eigen::RowMajor> ZDM;
  typedef Eigen::Matrix<double, Eigen::Dynamic, EDIM, Eigen::RowMajor> XEM;
  //typedef Eigen::Matrix<double, EDIM, ZDIM, Eigen::RowMajor> EZM;
  typedef Eigen::Matrix<double, Eigen::Dynamic, 1> X1M;
  typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> XXM;

  double in_hx[ZDIM] = {0};
  double in_H[ZDIM * DIM] = {0};
  double in_H_mod[EDIM * DIM] = {0};
  double delta_x[EDIM] = {0};
  double x_new[DIM] = {0};


  // state x, P
  Eigen::Matrix<double, ZDIM, 1> z(in_z);
  EEM P(in_P);
  ZZM pre_R(in_R);

  // functions from sympy
  h_fun(in_x, in_ea, in_hx);
  H_fun(in_x, in_ea, in_H);
  ZDM pre_H(in_H);

  // get y (y = z - hx)
  Eigen::Matrix<double, ZDIM, 1> pre_y(in_hx); pre_y = z - pre_y;
  X1M y; XXM H; XXM R;
  if (Hea_fun){
    typedef Eigen::Matrix<double, ZDIM, EADIM, Eigen::RowMajor> ZAM;
    double in_Hea[ZDIM * EADIM] = {0};
    Hea_fun(in_x, in_ea, in_Hea);
    ZAM Hea(in_Hea);
    XXM A = Hea.transpose().fullPivLu().kernel();


    y = A.transpose() * pre_y;
    H = A.transpose() * pre_H;
    R = A.transpose() * pre_R * A;
  } else {
    y = pre_y;
    H = pre_H;
    R = pre_R;
  }
  // get modified H
  H_mod_fun(in_x, in_H_mod);
  DEM H_mod(in_H_mod);
  XEM H_err = H * H_mod;

  // Do mahalobis distance test
  if (MAHA_TEST){
    XXM a = (H_err * P * H_err.transpose() + R).inverse();
    double maha_dist = y.transpose() * a * y;
    if (maha_dist > MAHA_THRESHOLD){
      R = 1.0e16 * R;
    }
  }

  // Outlier resilient weighting
  double weight = 1;//(1.5)/(1 + y.squaredNorm()/R.sum());

  // kalman gains and I_KH
  XXM S = ((H_err * P) * H_err.transpose()) + R/weight;
  XEM KT = S.fullPivLu().solve(H_err * P.transpose());
  //EZM K = KT.transpose(); TODO: WHY DOES THIS NOT COMPILE?
  //EZM K = S.fullPivLu().solve(H_err * P.transpose()).transpose();
  //std::cout << "Here is the matrix rot:\n" << K << std::endl;
  EEM I_KH = Eigen::Matrix<double, EDIM, EDIM>::Identity() - (KT.transpose() * H_err);

  // update state by injecting dx
  Eigen::Matrix<double, EDIM, 1> dx(delta_x);
  dx  = (KT.transpose() * y);
  memcpy(delta_x, dx.data(), EDIM * sizeof(double));
  err_fun(in_x, delta_x, x_new);
  Eigen::Matrix<double, DIM, 1> x(x_new);

  // update cov
  P = ((I_KH * P) * I_KH.transpose()) + ((KT.transpose() * R) * KT);

  // copy out state
  memcpy(in_x, x.data(), DIM * sizeof(double));
  memcpy(in_P, P.data(), EDIM * EDIM * sizeof(double));
  memcpy(in_z, y.data(), y.rows() * sizeof(double));
}




}
extern "C" {

void car_update_25(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_25, H_25, NULL, in_z, in_R, in_ea, MAHA_THRESH_25);
}
void car_update_24(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<2, 3, 0>(in_x, in_P, h_24, H_24, NULL, in_z, in_R, in_ea, MAHA_THRESH_24);
}
void car_update_30(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_30, H_30, NULL, in_z, in_R, in_ea, MAHA_THRESH_30);
}
void car_update_26(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_26, H_26, NULL, in_z, in_R, in_ea, MAHA_THRESH_26);
}
void car_update_27(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_27, H_27, NULL, in_z, in_R, in_ea, MAHA_THRESH_27);
}
void car_update_29(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_29, H_29, NULL, in_z, in_R, in_ea, MAHA_THRESH_29);
}
void car_update_28(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_28, H_28, NULL, in_z, in_R, in_ea, MAHA_THRESH_28);
}
void car_update_31(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_31, H_31, NULL, in_z, in_R, in_ea, MAHA_THRESH_31);
}
void car_err_fun(double *nom_x, double *delta_x, double *out_93630131099483078) {
  err_fun(nom_x, delta_x, out_93630131099483078);
}
void car_inv_err_fun(double *nom_x, double *true_x, double *out_8677716757174088306) {
  inv_err_fun(nom_x, true_x, out_8677716757174088306);
}
void car_H_mod_fun(double *state, double *out_8542951934772758986) {
  H_mod_fun(state, out_8542951934772758986);
}
void car_f_fun(double *state, double dt, double *out_4805745385691618402) {
  f_fun(state,  dt, out_4805745385691618402);
}
void car_F_fun(double *state, double dt, double *out_2344257086088637941) {
  F_fun(state,  dt, out_2344257086088637941);
}
void car_h_25(double *state, double *unused, double *out_4462526449509899381) {
  h_25(state, unused, out_4462526449509899381);
}
void car_H_25(double *state, double *unused, double *out_3406769179046762679) {
  H_25(state, unused, out_3406769179046762679);
}
void car_h_24(double *state, double *unused, double *out_9208820493134158213) {
  h_24(state, unused, out_9208820493134158213);
}
void car_H_24(double *state, double *unused, double *out_1462045685980944173) {
  H_24(state, unused, out_1462045685980944173);
}
void car_h_30(double *state, double *unused, double *out_6220952367378979794) {
  h_30(state, unused, out_6220952367378979794);
}
void car_H_30(double *state, double *unused, double *out_1120927151080845519) {
  H_30(state, unused, out_1120927151080845519);
}
void car_h_26(double *state, double *unused, double *out_9189005654886239137) {
  h_26(state, unused, out_9189005654886239137);
}
void car_H_26(double *state, double *unused, double *out_334734139827293545) {
  H_26(state, unused, out_334734139827293545);
}
void car_h_27(double *state, double *unused, double *out_5406036302411561816) {
  h_27(state, unused, out_5406036302411561816);
}
void car_H_27(double *state, double *unused, double *out_3295690462881270430) {
  H_27(state, unused, out_3295690462881270430);
}
void car_h_29(double *state, double *unused, double *out_7224118438669726177) {
  h_29(state, unused, out_7224118438669726177);
}
void car_H_29(double *state, double *unused, double *out_610695806766453335) {
  H_29(state, unused, out_610695806766453335);
}
void car_h_28(double *state, double *unused, double *out_8442822353855894501) {
  h_28(state, unused, out_8442822353855894501);
}
void car_H_28(double *state, double *unused, double *out_5693094823835983909) {
  H_28(state, unused, out_5693094823835983909);
}
void car_h_31(double *state, double *unused, double *out_2724426454948158948) {
  h_31(state, unused, out_2724426454948158948);
}
void car_H_31(double *state, double *unused, double *out_960942242060645021) {
  H_31(state, unused, out_960942242060645021);
}
void car_predict(double *in_x, double *in_P, double *in_Q, double dt) {
  predict(in_x, in_P, in_Q, dt);
}
void car_set_rotational_inertia(double x) {
  set_rotational_inertia(x);
}
void car_set_center_to_front(double x) {
  set_center_to_front(x);
}
void car_set_center_to_rear(double x) {
  set_center_to_rear(x);
}
void car_set_stiffness_front(double x) {
  set_stiffness_front(x);
}
void car_set_stiffness_rear(double x) {
  set_stiffness_rear(x);
}
}

const EKF car = {
  .name = "car",
  .kinds = { 25, 24, 30, 26, 27, 29, 28, 31 },
  .feature_kinds = {  },
  .f_fun = car_f_fun,
  .F_fun = car_F_fun,
  .err_fun = car_err_fun,
  .inv_err_fun = car_inv_err_fun,
  .H_mod_fun = car_H_mod_fun,
  .predict = car_predict,
  .hs = {
    { 25, car_h_25 },
    { 24, car_h_24 },
    { 30, car_h_30 },
    { 26, car_h_26 },
    { 27, car_h_27 },
    { 29, car_h_29 },
    { 28, car_h_28 },
    { 31, car_h_31 },
  },
  .Hs = {
    { 25, car_H_25 },
    { 24, car_H_24 },
    { 30, car_H_30 },
    { 26, car_H_26 },
    { 27, car_H_27 },
    { 29, car_H_29 },
    { 28, car_H_28 },
    { 31, car_H_31 },
  },
  .updates = {
    { 25, car_update_25 },
    { 24, car_update_24 },
    { 30, car_update_30 },
    { 26, car_update_26 },
    { 27, car_update_27 },
    { 29, car_update_29 },
    { 28, car_update_28 },
    { 31, car_update_31 },
  },
  .Hes = {
  },
  .sets = {
    { "rotational_inertia", car_set_rotational_inertia },
    { "center_to_front", car_set_center_to_front },
    { "center_to_rear", car_set_center_to_rear },
    { "stiffness_front", car_set_stiffness_front },
    { "stiffness_rear", car_set_stiffness_rear },
  },
  .extra_routines = {
  },
};

ekf_lib_init(car)
