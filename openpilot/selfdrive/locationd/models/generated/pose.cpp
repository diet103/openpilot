#include "pose.h"

namespace {
#define DIM 18
#define EDIM 18
#define MEDIM 18
typedef void (*Hfun)(double *, double *, double *);
const static double MAHA_THRESH_4 = 7.814727903251177;
const static double MAHA_THRESH_10 = 7.814727903251177;
const static double MAHA_THRESH_13 = 7.814727903251177;
const static double MAHA_THRESH_14 = 7.814727903251177;

/******************************************************************************
 *                      Code generated with SymPy 1.14.0                      *
 *                                                                            *
 *              See http://www.sympy.org/ for more information.               *
 *                                                                            *
 *                         This file is part of 'ekf'                         *
 ******************************************************************************/
void err_fun(double *nom_x, double *delta_x, double *out_7442492993183452915) {
   out_7442492993183452915[0] = delta_x[0] + nom_x[0];
   out_7442492993183452915[1] = delta_x[1] + nom_x[1];
   out_7442492993183452915[2] = delta_x[2] + nom_x[2];
   out_7442492993183452915[3] = delta_x[3] + nom_x[3];
   out_7442492993183452915[4] = delta_x[4] + nom_x[4];
   out_7442492993183452915[5] = delta_x[5] + nom_x[5];
   out_7442492993183452915[6] = delta_x[6] + nom_x[6];
   out_7442492993183452915[7] = delta_x[7] + nom_x[7];
   out_7442492993183452915[8] = delta_x[8] + nom_x[8];
   out_7442492993183452915[9] = delta_x[9] + nom_x[9];
   out_7442492993183452915[10] = delta_x[10] + nom_x[10];
   out_7442492993183452915[11] = delta_x[11] + nom_x[11];
   out_7442492993183452915[12] = delta_x[12] + nom_x[12];
   out_7442492993183452915[13] = delta_x[13] + nom_x[13];
   out_7442492993183452915[14] = delta_x[14] + nom_x[14];
   out_7442492993183452915[15] = delta_x[15] + nom_x[15];
   out_7442492993183452915[16] = delta_x[16] + nom_x[16];
   out_7442492993183452915[17] = delta_x[17] + nom_x[17];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_6140138630151309189) {
   out_6140138630151309189[0] = -nom_x[0] + true_x[0];
   out_6140138630151309189[1] = -nom_x[1] + true_x[1];
   out_6140138630151309189[2] = -nom_x[2] + true_x[2];
   out_6140138630151309189[3] = -nom_x[3] + true_x[3];
   out_6140138630151309189[4] = -nom_x[4] + true_x[4];
   out_6140138630151309189[5] = -nom_x[5] + true_x[5];
   out_6140138630151309189[6] = -nom_x[6] + true_x[6];
   out_6140138630151309189[7] = -nom_x[7] + true_x[7];
   out_6140138630151309189[8] = -nom_x[8] + true_x[8];
   out_6140138630151309189[9] = -nom_x[9] + true_x[9];
   out_6140138630151309189[10] = -nom_x[10] + true_x[10];
   out_6140138630151309189[11] = -nom_x[11] + true_x[11];
   out_6140138630151309189[12] = -nom_x[12] + true_x[12];
   out_6140138630151309189[13] = -nom_x[13] + true_x[13];
   out_6140138630151309189[14] = -nom_x[14] + true_x[14];
   out_6140138630151309189[15] = -nom_x[15] + true_x[15];
   out_6140138630151309189[16] = -nom_x[16] + true_x[16];
   out_6140138630151309189[17] = -nom_x[17] + true_x[17];
}
void H_mod_fun(double *state, double *out_5396528562455239188) {
   out_5396528562455239188[0] = 1.0;
   out_5396528562455239188[1] = 0.0;
   out_5396528562455239188[2] = 0.0;
   out_5396528562455239188[3] = 0.0;
   out_5396528562455239188[4] = 0.0;
   out_5396528562455239188[5] = 0.0;
   out_5396528562455239188[6] = 0.0;
   out_5396528562455239188[7] = 0.0;
   out_5396528562455239188[8] = 0.0;
   out_5396528562455239188[9] = 0.0;
   out_5396528562455239188[10] = 0.0;
   out_5396528562455239188[11] = 0.0;
   out_5396528562455239188[12] = 0.0;
   out_5396528562455239188[13] = 0.0;
   out_5396528562455239188[14] = 0.0;
   out_5396528562455239188[15] = 0.0;
   out_5396528562455239188[16] = 0.0;
   out_5396528562455239188[17] = 0.0;
   out_5396528562455239188[18] = 0.0;
   out_5396528562455239188[19] = 1.0;
   out_5396528562455239188[20] = 0.0;
   out_5396528562455239188[21] = 0.0;
   out_5396528562455239188[22] = 0.0;
   out_5396528562455239188[23] = 0.0;
   out_5396528562455239188[24] = 0.0;
   out_5396528562455239188[25] = 0.0;
   out_5396528562455239188[26] = 0.0;
   out_5396528562455239188[27] = 0.0;
   out_5396528562455239188[28] = 0.0;
   out_5396528562455239188[29] = 0.0;
   out_5396528562455239188[30] = 0.0;
   out_5396528562455239188[31] = 0.0;
   out_5396528562455239188[32] = 0.0;
   out_5396528562455239188[33] = 0.0;
   out_5396528562455239188[34] = 0.0;
   out_5396528562455239188[35] = 0.0;
   out_5396528562455239188[36] = 0.0;
   out_5396528562455239188[37] = 0.0;
   out_5396528562455239188[38] = 1.0;
   out_5396528562455239188[39] = 0.0;
   out_5396528562455239188[40] = 0.0;
   out_5396528562455239188[41] = 0.0;
   out_5396528562455239188[42] = 0.0;
   out_5396528562455239188[43] = 0.0;
   out_5396528562455239188[44] = 0.0;
   out_5396528562455239188[45] = 0.0;
   out_5396528562455239188[46] = 0.0;
   out_5396528562455239188[47] = 0.0;
   out_5396528562455239188[48] = 0.0;
   out_5396528562455239188[49] = 0.0;
   out_5396528562455239188[50] = 0.0;
   out_5396528562455239188[51] = 0.0;
   out_5396528562455239188[52] = 0.0;
   out_5396528562455239188[53] = 0.0;
   out_5396528562455239188[54] = 0.0;
   out_5396528562455239188[55] = 0.0;
   out_5396528562455239188[56] = 0.0;
   out_5396528562455239188[57] = 1.0;
   out_5396528562455239188[58] = 0.0;
   out_5396528562455239188[59] = 0.0;
   out_5396528562455239188[60] = 0.0;
   out_5396528562455239188[61] = 0.0;
   out_5396528562455239188[62] = 0.0;
   out_5396528562455239188[63] = 0.0;
   out_5396528562455239188[64] = 0.0;
   out_5396528562455239188[65] = 0.0;
   out_5396528562455239188[66] = 0.0;
   out_5396528562455239188[67] = 0.0;
   out_5396528562455239188[68] = 0.0;
   out_5396528562455239188[69] = 0.0;
   out_5396528562455239188[70] = 0.0;
   out_5396528562455239188[71] = 0.0;
   out_5396528562455239188[72] = 0.0;
   out_5396528562455239188[73] = 0.0;
   out_5396528562455239188[74] = 0.0;
   out_5396528562455239188[75] = 0.0;
   out_5396528562455239188[76] = 1.0;
   out_5396528562455239188[77] = 0.0;
   out_5396528562455239188[78] = 0.0;
   out_5396528562455239188[79] = 0.0;
   out_5396528562455239188[80] = 0.0;
   out_5396528562455239188[81] = 0.0;
   out_5396528562455239188[82] = 0.0;
   out_5396528562455239188[83] = 0.0;
   out_5396528562455239188[84] = 0.0;
   out_5396528562455239188[85] = 0.0;
   out_5396528562455239188[86] = 0.0;
   out_5396528562455239188[87] = 0.0;
   out_5396528562455239188[88] = 0.0;
   out_5396528562455239188[89] = 0.0;
   out_5396528562455239188[90] = 0.0;
   out_5396528562455239188[91] = 0.0;
   out_5396528562455239188[92] = 0.0;
   out_5396528562455239188[93] = 0.0;
   out_5396528562455239188[94] = 0.0;
   out_5396528562455239188[95] = 1.0;
   out_5396528562455239188[96] = 0.0;
   out_5396528562455239188[97] = 0.0;
   out_5396528562455239188[98] = 0.0;
   out_5396528562455239188[99] = 0.0;
   out_5396528562455239188[100] = 0.0;
   out_5396528562455239188[101] = 0.0;
   out_5396528562455239188[102] = 0.0;
   out_5396528562455239188[103] = 0.0;
   out_5396528562455239188[104] = 0.0;
   out_5396528562455239188[105] = 0.0;
   out_5396528562455239188[106] = 0.0;
   out_5396528562455239188[107] = 0.0;
   out_5396528562455239188[108] = 0.0;
   out_5396528562455239188[109] = 0.0;
   out_5396528562455239188[110] = 0.0;
   out_5396528562455239188[111] = 0.0;
   out_5396528562455239188[112] = 0.0;
   out_5396528562455239188[113] = 0.0;
   out_5396528562455239188[114] = 1.0;
   out_5396528562455239188[115] = 0.0;
   out_5396528562455239188[116] = 0.0;
   out_5396528562455239188[117] = 0.0;
   out_5396528562455239188[118] = 0.0;
   out_5396528562455239188[119] = 0.0;
   out_5396528562455239188[120] = 0.0;
   out_5396528562455239188[121] = 0.0;
   out_5396528562455239188[122] = 0.0;
   out_5396528562455239188[123] = 0.0;
   out_5396528562455239188[124] = 0.0;
   out_5396528562455239188[125] = 0.0;
   out_5396528562455239188[126] = 0.0;
   out_5396528562455239188[127] = 0.0;
   out_5396528562455239188[128] = 0.0;
   out_5396528562455239188[129] = 0.0;
   out_5396528562455239188[130] = 0.0;
   out_5396528562455239188[131] = 0.0;
   out_5396528562455239188[132] = 0.0;
   out_5396528562455239188[133] = 1.0;
   out_5396528562455239188[134] = 0.0;
   out_5396528562455239188[135] = 0.0;
   out_5396528562455239188[136] = 0.0;
   out_5396528562455239188[137] = 0.0;
   out_5396528562455239188[138] = 0.0;
   out_5396528562455239188[139] = 0.0;
   out_5396528562455239188[140] = 0.0;
   out_5396528562455239188[141] = 0.0;
   out_5396528562455239188[142] = 0.0;
   out_5396528562455239188[143] = 0.0;
   out_5396528562455239188[144] = 0.0;
   out_5396528562455239188[145] = 0.0;
   out_5396528562455239188[146] = 0.0;
   out_5396528562455239188[147] = 0.0;
   out_5396528562455239188[148] = 0.0;
   out_5396528562455239188[149] = 0.0;
   out_5396528562455239188[150] = 0.0;
   out_5396528562455239188[151] = 0.0;
   out_5396528562455239188[152] = 1.0;
   out_5396528562455239188[153] = 0.0;
   out_5396528562455239188[154] = 0.0;
   out_5396528562455239188[155] = 0.0;
   out_5396528562455239188[156] = 0.0;
   out_5396528562455239188[157] = 0.0;
   out_5396528562455239188[158] = 0.0;
   out_5396528562455239188[159] = 0.0;
   out_5396528562455239188[160] = 0.0;
   out_5396528562455239188[161] = 0.0;
   out_5396528562455239188[162] = 0.0;
   out_5396528562455239188[163] = 0.0;
   out_5396528562455239188[164] = 0.0;
   out_5396528562455239188[165] = 0.0;
   out_5396528562455239188[166] = 0.0;
   out_5396528562455239188[167] = 0.0;
   out_5396528562455239188[168] = 0.0;
   out_5396528562455239188[169] = 0.0;
   out_5396528562455239188[170] = 0.0;
   out_5396528562455239188[171] = 1.0;
   out_5396528562455239188[172] = 0.0;
   out_5396528562455239188[173] = 0.0;
   out_5396528562455239188[174] = 0.0;
   out_5396528562455239188[175] = 0.0;
   out_5396528562455239188[176] = 0.0;
   out_5396528562455239188[177] = 0.0;
   out_5396528562455239188[178] = 0.0;
   out_5396528562455239188[179] = 0.0;
   out_5396528562455239188[180] = 0.0;
   out_5396528562455239188[181] = 0.0;
   out_5396528562455239188[182] = 0.0;
   out_5396528562455239188[183] = 0.0;
   out_5396528562455239188[184] = 0.0;
   out_5396528562455239188[185] = 0.0;
   out_5396528562455239188[186] = 0.0;
   out_5396528562455239188[187] = 0.0;
   out_5396528562455239188[188] = 0.0;
   out_5396528562455239188[189] = 0.0;
   out_5396528562455239188[190] = 1.0;
   out_5396528562455239188[191] = 0.0;
   out_5396528562455239188[192] = 0.0;
   out_5396528562455239188[193] = 0.0;
   out_5396528562455239188[194] = 0.0;
   out_5396528562455239188[195] = 0.0;
   out_5396528562455239188[196] = 0.0;
   out_5396528562455239188[197] = 0.0;
   out_5396528562455239188[198] = 0.0;
   out_5396528562455239188[199] = 0.0;
   out_5396528562455239188[200] = 0.0;
   out_5396528562455239188[201] = 0.0;
   out_5396528562455239188[202] = 0.0;
   out_5396528562455239188[203] = 0.0;
   out_5396528562455239188[204] = 0.0;
   out_5396528562455239188[205] = 0.0;
   out_5396528562455239188[206] = 0.0;
   out_5396528562455239188[207] = 0.0;
   out_5396528562455239188[208] = 0.0;
   out_5396528562455239188[209] = 1.0;
   out_5396528562455239188[210] = 0.0;
   out_5396528562455239188[211] = 0.0;
   out_5396528562455239188[212] = 0.0;
   out_5396528562455239188[213] = 0.0;
   out_5396528562455239188[214] = 0.0;
   out_5396528562455239188[215] = 0.0;
   out_5396528562455239188[216] = 0.0;
   out_5396528562455239188[217] = 0.0;
   out_5396528562455239188[218] = 0.0;
   out_5396528562455239188[219] = 0.0;
   out_5396528562455239188[220] = 0.0;
   out_5396528562455239188[221] = 0.0;
   out_5396528562455239188[222] = 0.0;
   out_5396528562455239188[223] = 0.0;
   out_5396528562455239188[224] = 0.0;
   out_5396528562455239188[225] = 0.0;
   out_5396528562455239188[226] = 0.0;
   out_5396528562455239188[227] = 0.0;
   out_5396528562455239188[228] = 1.0;
   out_5396528562455239188[229] = 0.0;
   out_5396528562455239188[230] = 0.0;
   out_5396528562455239188[231] = 0.0;
   out_5396528562455239188[232] = 0.0;
   out_5396528562455239188[233] = 0.0;
   out_5396528562455239188[234] = 0.0;
   out_5396528562455239188[235] = 0.0;
   out_5396528562455239188[236] = 0.0;
   out_5396528562455239188[237] = 0.0;
   out_5396528562455239188[238] = 0.0;
   out_5396528562455239188[239] = 0.0;
   out_5396528562455239188[240] = 0.0;
   out_5396528562455239188[241] = 0.0;
   out_5396528562455239188[242] = 0.0;
   out_5396528562455239188[243] = 0.0;
   out_5396528562455239188[244] = 0.0;
   out_5396528562455239188[245] = 0.0;
   out_5396528562455239188[246] = 0.0;
   out_5396528562455239188[247] = 1.0;
   out_5396528562455239188[248] = 0.0;
   out_5396528562455239188[249] = 0.0;
   out_5396528562455239188[250] = 0.0;
   out_5396528562455239188[251] = 0.0;
   out_5396528562455239188[252] = 0.0;
   out_5396528562455239188[253] = 0.0;
   out_5396528562455239188[254] = 0.0;
   out_5396528562455239188[255] = 0.0;
   out_5396528562455239188[256] = 0.0;
   out_5396528562455239188[257] = 0.0;
   out_5396528562455239188[258] = 0.0;
   out_5396528562455239188[259] = 0.0;
   out_5396528562455239188[260] = 0.0;
   out_5396528562455239188[261] = 0.0;
   out_5396528562455239188[262] = 0.0;
   out_5396528562455239188[263] = 0.0;
   out_5396528562455239188[264] = 0.0;
   out_5396528562455239188[265] = 0.0;
   out_5396528562455239188[266] = 1.0;
   out_5396528562455239188[267] = 0.0;
   out_5396528562455239188[268] = 0.0;
   out_5396528562455239188[269] = 0.0;
   out_5396528562455239188[270] = 0.0;
   out_5396528562455239188[271] = 0.0;
   out_5396528562455239188[272] = 0.0;
   out_5396528562455239188[273] = 0.0;
   out_5396528562455239188[274] = 0.0;
   out_5396528562455239188[275] = 0.0;
   out_5396528562455239188[276] = 0.0;
   out_5396528562455239188[277] = 0.0;
   out_5396528562455239188[278] = 0.0;
   out_5396528562455239188[279] = 0.0;
   out_5396528562455239188[280] = 0.0;
   out_5396528562455239188[281] = 0.0;
   out_5396528562455239188[282] = 0.0;
   out_5396528562455239188[283] = 0.0;
   out_5396528562455239188[284] = 0.0;
   out_5396528562455239188[285] = 1.0;
   out_5396528562455239188[286] = 0.0;
   out_5396528562455239188[287] = 0.0;
   out_5396528562455239188[288] = 0.0;
   out_5396528562455239188[289] = 0.0;
   out_5396528562455239188[290] = 0.0;
   out_5396528562455239188[291] = 0.0;
   out_5396528562455239188[292] = 0.0;
   out_5396528562455239188[293] = 0.0;
   out_5396528562455239188[294] = 0.0;
   out_5396528562455239188[295] = 0.0;
   out_5396528562455239188[296] = 0.0;
   out_5396528562455239188[297] = 0.0;
   out_5396528562455239188[298] = 0.0;
   out_5396528562455239188[299] = 0.0;
   out_5396528562455239188[300] = 0.0;
   out_5396528562455239188[301] = 0.0;
   out_5396528562455239188[302] = 0.0;
   out_5396528562455239188[303] = 0.0;
   out_5396528562455239188[304] = 1.0;
   out_5396528562455239188[305] = 0.0;
   out_5396528562455239188[306] = 0.0;
   out_5396528562455239188[307] = 0.0;
   out_5396528562455239188[308] = 0.0;
   out_5396528562455239188[309] = 0.0;
   out_5396528562455239188[310] = 0.0;
   out_5396528562455239188[311] = 0.0;
   out_5396528562455239188[312] = 0.0;
   out_5396528562455239188[313] = 0.0;
   out_5396528562455239188[314] = 0.0;
   out_5396528562455239188[315] = 0.0;
   out_5396528562455239188[316] = 0.0;
   out_5396528562455239188[317] = 0.0;
   out_5396528562455239188[318] = 0.0;
   out_5396528562455239188[319] = 0.0;
   out_5396528562455239188[320] = 0.0;
   out_5396528562455239188[321] = 0.0;
   out_5396528562455239188[322] = 0.0;
   out_5396528562455239188[323] = 1.0;
}
void f_fun(double *state, double dt, double *out_3110033524529218965) {
   out_3110033524529218965[0] = atan2((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), -(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]));
   out_3110033524529218965[1] = asin(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]));
   out_3110033524529218965[2] = atan2(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), -(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]));
   out_3110033524529218965[3] = dt*state[12] + state[3];
   out_3110033524529218965[4] = dt*state[13] + state[4];
   out_3110033524529218965[5] = dt*state[14] + state[5];
   out_3110033524529218965[6] = state[6];
   out_3110033524529218965[7] = state[7];
   out_3110033524529218965[8] = state[8];
   out_3110033524529218965[9] = state[9];
   out_3110033524529218965[10] = state[10];
   out_3110033524529218965[11] = state[11];
   out_3110033524529218965[12] = state[12];
   out_3110033524529218965[13] = state[13];
   out_3110033524529218965[14] = state[14];
   out_3110033524529218965[15] = state[15];
   out_3110033524529218965[16] = state[16];
   out_3110033524529218965[17] = state[17];
}
void F_fun(double *state, double dt, double *out_2384259379620246573) {
   out_2384259379620246573[0] = ((-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*cos(state[0])*cos(state[1]) - sin(state[0])*cos(dt*state[6])*cos(dt*state[7])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + ((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*cos(state[0])*cos(state[1]) - sin(dt*state[6])*sin(state[0])*cos(dt*state[7])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_2384259379620246573[1] = ((-sin(dt*state[6])*sin(dt*state[8]) - sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*cos(state[1]) - (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*sin(state[1]) - sin(state[1])*cos(dt*state[6])*cos(dt*state[7])*cos(state[0]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*sin(state[1]) + (-sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) + sin(dt*state[8])*cos(dt*state[6]))*cos(state[1]) - sin(dt*state[6])*sin(state[1])*cos(dt*state[7])*cos(state[0]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_2384259379620246573[2] = 0;
   out_2384259379620246573[3] = 0;
   out_2384259379620246573[4] = 0;
   out_2384259379620246573[5] = 0;
   out_2384259379620246573[6] = (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(dt*cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*sin(dt*state[8]) - dt*sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-dt*sin(dt*state[6])*cos(dt*state[8]) + dt*sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) - dt*cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (dt*sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_2384259379620246573[7] = (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[6])*sin(dt*state[7])*cos(state[0])*cos(state[1]) + dt*sin(dt*state[6])*sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) - dt*sin(dt*state[6])*sin(state[1])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[7])*cos(dt*state[6])*cos(state[0])*cos(state[1]) + dt*sin(dt*state[8])*sin(state[0])*cos(dt*state[6])*cos(dt*state[7])*cos(state[1]) - dt*sin(state[1])*cos(dt*state[6])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_2384259379620246573[8] = ((dt*sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + dt*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (dt*sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + ((dt*sin(dt*state[6])*sin(dt*state[8]) + dt*sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*cos(dt*state[8]) + dt*sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_2384259379620246573[9] = 0;
   out_2384259379620246573[10] = 0;
   out_2384259379620246573[11] = 0;
   out_2384259379620246573[12] = 0;
   out_2384259379620246573[13] = 0;
   out_2384259379620246573[14] = 0;
   out_2384259379620246573[15] = 0;
   out_2384259379620246573[16] = 0;
   out_2384259379620246573[17] = 0;
   out_2384259379620246573[18] = (-sin(dt*state[7])*sin(state[0])*cos(state[1]) - sin(dt*state[8])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_2384259379620246573[19] = (-sin(dt*state[7])*sin(state[1])*cos(state[0]) + sin(dt*state[8])*sin(state[0])*sin(state[1])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_2384259379620246573[20] = 0;
   out_2384259379620246573[21] = 0;
   out_2384259379620246573[22] = 0;
   out_2384259379620246573[23] = 0;
   out_2384259379620246573[24] = 0;
   out_2384259379620246573[25] = (dt*sin(dt*state[7])*sin(dt*state[8])*sin(state[0])*cos(state[1]) - dt*sin(dt*state[7])*sin(state[1])*cos(dt*state[8]) + dt*cos(dt*state[7])*cos(state[0])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_2384259379620246573[26] = (-dt*sin(dt*state[8])*sin(state[1])*cos(dt*state[7]) - dt*sin(state[0])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_2384259379620246573[27] = 0;
   out_2384259379620246573[28] = 0;
   out_2384259379620246573[29] = 0;
   out_2384259379620246573[30] = 0;
   out_2384259379620246573[31] = 0;
   out_2384259379620246573[32] = 0;
   out_2384259379620246573[33] = 0;
   out_2384259379620246573[34] = 0;
   out_2384259379620246573[35] = 0;
   out_2384259379620246573[36] = ((sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[7]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[7]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_2384259379620246573[37] = (-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(-sin(dt*state[7])*sin(state[2])*cos(state[0])*cos(state[1]) + sin(dt*state[8])*sin(state[0])*sin(state[2])*cos(dt*state[7])*cos(state[1]) - sin(state[1])*sin(state[2])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*(-sin(dt*state[7])*cos(state[0])*cos(state[1])*cos(state[2]) + sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1])*cos(state[2]) - sin(state[1])*cos(dt*state[7])*cos(dt*state[8])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_2384259379620246573[38] = ((-sin(state[0])*sin(state[2]) - sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (-sin(state[0])*sin(state[1])*sin(state[2]) - cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_2384259379620246573[39] = 0;
   out_2384259379620246573[40] = 0;
   out_2384259379620246573[41] = 0;
   out_2384259379620246573[42] = 0;
   out_2384259379620246573[43] = (-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(dt*(sin(state[0])*cos(state[2]) - sin(state[1])*sin(state[2])*cos(state[0]))*cos(dt*state[7]) - dt*(sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[7])*sin(dt*state[8]) - dt*sin(dt*state[7])*sin(state[2])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*(dt*(-sin(state[0])*sin(state[2]) - sin(state[1])*cos(state[0])*cos(state[2]))*cos(dt*state[7]) - dt*(sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[7])*sin(dt*state[8]) - dt*sin(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_2384259379620246573[44] = (dt*(sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*cos(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*sin(state[2])*cos(dt*state[7])*cos(state[1]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + (dt*(sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*cos(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[7])*cos(state[1])*cos(state[2]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_2384259379620246573[45] = 0;
   out_2384259379620246573[46] = 0;
   out_2384259379620246573[47] = 0;
   out_2384259379620246573[48] = 0;
   out_2384259379620246573[49] = 0;
   out_2384259379620246573[50] = 0;
   out_2384259379620246573[51] = 0;
   out_2384259379620246573[52] = 0;
   out_2384259379620246573[53] = 0;
   out_2384259379620246573[54] = 0;
   out_2384259379620246573[55] = 0;
   out_2384259379620246573[56] = 0;
   out_2384259379620246573[57] = 1;
   out_2384259379620246573[58] = 0;
   out_2384259379620246573[59] = 0;
   out_2384259379620246573[60] = 0;
   out_2384259379620246573[61] = 0;
   out_2384259379620246573[62] = 0;
   out_2384259379620246573[63] = 0;
   out_2384259379620246573[64] = 0;
   out_2384259379620246573[65] = 0;
   out_2384259379620246573[66] = dt;
   out_2384259379620246573[67] = 0;
   out_2384259379620246573[68] = 0;
   out_2384259379620246573[69] = 0;
   out_2384259379620246573[70] = 0;
   out_2384259379620246573[71] = 0;
   out_2384259379620246573[72] = 0;
   out_2384259379620246573[73] = 0;
   out_2384259379620246573[74] = 0;
   out_2384259379620246573[75] = 0;
   out_2384259379620246573[76] = 1;
   out_2384259379620246573[77] = 0;
   out_2384259379620246573[78] = 0;
   out_2384259379620246573[79] = 0;
   out_2384259379620246573[80] = 0;
   out_2384259379620246573[81] = 0;
   out_2384259379620246573[82] = 0;
   out_2384259379620246573[83] = 0;
   out_2384259379620246573[84] = 0;
   out_2384259379620246573[85] = dt;
   out_2384259379620246573[86] = 0;
   out_2384259379620246573[87] = 0;
   out_2384259379620246573[88] = 0;
   out_2384259379620246573[89] = 0;
   out_2384259379620246573[90] = 0;
   out_2384259379620246573[91] = 0;
   out_2384259379620246573[92] = 0;
   out_2384259379620246573[93] = 0;
   out_2384259379620246573[94] = 0;
   out_2384259379620246573[95] = 1;
   out_2384259379620246573[96] = 0;
   out_2384259379620246573[97] = 0;
   out_2384259379620246573[98] = 0;
   out_2384259379620246573[99] = 0;
   out_2384259379620246573[100] = 0;
   out_2384259379620246573[101] = 0;
   out_2384259379620246573[102] = 0;
   out_2384259379620246573[103] = 0;
   out_2384259379620246573[104] = dt;
   out_2384259379620246573[105] = 0;
   out_2384259379620246573[106] = 0;
   out_2384259379620246573[107] = 0;
   out_2384259379620246573[108] = 0;
   out_2384259379620246573[109] = 0;
   out_2384259379620246573[110] = 0;
   out_2384259379620246573[111] = 0;
   out_2384259379620246573[112] = 0;
   out_2384259379620246573[113] = 0;
   out_2384259379620246573[114] = 1;
   out_2384259379620246573[115] = 0;
   out_2384259379620246573[116] = 0;
   out_2384259379620246573[117] = 0;
   out_2384259379620246573[118] = 0;
   out_2384259379620246573[119] = 0;
   out_2384259379620246573[120] = 0;
   out_2384259379620246573[121] = 0;
   out_2384259379620246573[122] = 0;
   out_2384259379620246573[123] = 0;
   out_2384259379620246573[124] = 0;
   out_2384259379620246573[125] = 0;
   out_2384259379620246573[126] = 0;
   out_2384259379620246573[127] = 0;
   out_2384259379620246573[128] = 0;
   out_2384259379620246573[129] = 0;
   out_2384259379620246573[130] = 0;
   out_2384259379620246573[131] = 0;
   out_2384259379620246573[132] = 0;
   out_2384259379620246573[133] = 1;
   out_2384259379620246573[134] = 0;
   out_2384259379620246573[135] = 0;
   out_2384259379620246573[136] = 0;
   out_2384259379620246573[137] = 0;
   out_2384259379620246573[138] = 0;
   out_2384259379620246573[139] = 0;
   out_2384259379620246573[140] = 0;
   out_2384259379620246573[141] = 0;
   out_2384259379620246573[142] = 0;
   out_2384259379620246573[143] = 0;
   out_2384259379620246573[144] = 0;
   out_2384259379620246573[145] = 0;
   out_2384259379620246573[146] = 0;
   out_2384259379620246573[147] = 0;
   out_2384259379620246573[148] = 0;
   out_2384259379620246573[149] = 0;
   out_2384259379620246573[150] = 0;
   out_2384259379620246573[151] = 0;
   out_2384259379620246573[152] = 1;
   out_2384259379620246573[153] = 0;
   out_2384259379620246573[154] = 0;
   out_2384259379620246573[155] = 0;
   out_2384259379620246573[156] = 0;
   out_2384259379620246573[157] = 0;
   out_2384259379620246573[158] = 0;
   out_2384259379620246573[159] = 0;
   out_2384259379620246573[160] = 0;
   out_2384259379620246573[161] = 0;
   out_2384259379620246573[162] = 0;
   out_2384259379620246573[163] = 0;
   out_2384259379620246573[164] = 0;
   out_2384259379620246573[165] = 0;
   out_2384259379620246573[166] = 0;
   out_2384259379620246573[167] = 0;
   out_2384259379620246573[168] = 0;
   out_2384259379620246573[169] = 0;
   out_2384259379620246573[170] = 0;
   out_2384259379620246573[171] = 1;
   out_2384259379620246573[172] = 0;
   out_2384259379620246573[173] = 0;
   out_2384259379620246573[174] = 0;
   out_2384259379620246573[175] = 0;
   out_2384259379620246573[176] = 0;
   out_2384259379620246573[177] = 0;
   out_2384259379620246573[178] = 0;
   out_2384259379620246573[179] = 0;
   out_2384259379620246573[180] = 0;
   out_2384259379620246573[181] = 0;
   out_2384259379620246573[182] = 0;
   out_2384259379620246573[183] = 0;
   out_2384259379620246573[184] = 0;
   out_2384259379620246573[185] = 0;
   out_2384259379620246573[186] = 0;
   out_2384259379620246573[187] = 0;
   out_2384259379620246573[188] = 0;
   out_2384259379620246573[189] = 0;
   out_2384259379620246573[190] = 1;
   out_2384259379620246573[191] = 0;
   out_2384259379620246573[192] = 0;
   out_2384259379620246573[193] = 0;
   out_2384259379620246573[194] = 0;
   out_2384259379620246573[195] = 0;
   out_2384259379620246573[196] = 0;
   out_2384259379620246573[197] = 0;
   out_2384259379620246573[198] = 0;
   out_2384259379620246573[199] = 0;
   out_2384259379620246573[200] = 0;
   out_2384259379620246573[201] = 0;
   out_2384259379620246573[202] = 0;
   out_2384259379620246573[203] = 0;
   out_2384259379620246573[204] = 0;
   out_2384259379620246573[205] = 0;
   out_2384259379620246573[206] = 0;
   out_2384259379620246573[207] = 0;
   out_2384259379620246573[208] = 0;
   out_2384259379620246573[209] = 1;
   out_2384259379620246573[210] = 0;
   out_2384259379620246573[211] = 0;
   out_2384259379620246573[212] = 0;
   out_2384259379620246573[213] = 0;
   out_2384259379620246573[214] = 0;
   out_2384259379620246573[215] = 0;
   out_2384259379620246573[216] = 0;
   out_2384259379620246573[217] = 0;
   out_2384259379620246573[218] = 0;
   out_2384259379620246573[219] = 0;
   out_2384259379620246573[220] = 0;
   out_2384259379620246573[221] = 0;
   out_2384259379620246573[222] = 0;
   out_2384259379620246573[223] = 0;
   out_2384259379620246573[224] = 0;
   out_2384259379620246573[225] = 0;
   out_2384259379620246573[226] = 0;
   out_2384259379620246573[227] = 0;
   out_2384259379620246573[228] = 1;
   out_2384259379620246573[229] = 0;
   out_2384259379620246573[230] = 0;
   out_2384259379620246573[231] = 0;
   out_2384259379620246573[232] = 0;
   out_2384259379620246573[233] = 0;
   out_2384259379620246573[234] = 0;
   out_2384259379620246573[235] = 0;
   out_2384259379620246573[236] = 0;
   out_2384259379620246573[237] = 0;
   out_2384259379620246573[238] = 0;
   out_2384259379620246573[239] = 0;
   out_2384259379620246573[240] = 0;
   out_2384259379620246573[241] = 0;
   out_2384259379620246573[242] = 0;
   out_2384259379620246573[243] = 0;
   out_2384259379620246573[244] = 0;
   out_2384259379620246573[245] = 0;
   out_2384259379620246573[246] = 0;
   out_2384259379620246573[247] = 1;
   out_2384259379620246573[248] = 0;
   out_2384259379620246573[249] = 0;
   out_2384259379620246573[250] = 0;
   out_2384259379620246573[251] = 0;
   out_2384259379620246573[252] = 0;
   out_2384259379620246573[253] = 0;
   out_2384259379620246573[254] = 0;
   out_2384259379620246573[255] = 0;
   out_2384259379620246573[256] = 0;
   out_2384259379620246573[257] = 0;
   out_2384259379620246573[258] = 0;
   out_2384259379620246573[259] = 0;
   out_2384259379620246573[260] = 0;
   out_2384259379620246573[261] = 0;
   out_2384259379620246573[262] = 0;
   out_2384259379620246573[263] = 0;
   out_2384259379620246573[264] = 0;
   out_2384259379620246573[265] = 0;
   out_2384259379620246573[266] = 1;
   out_2384259379620246573[267] = 0;
   out_2384259379620246573[268] = 0;
   out_2384259379620246573[269] = 0;
   out_2384259379620246573[270] = 0;
   out_2384259379620246573[271] = 0;
   out_2384259379620246573[272] = 0;
   out_2384259379620246573[273] = 0;
   out_2384259379620246573[274] = 0;
   out_2384259379620246573[275] = 0;
   out_2384259379620246573[276] = 0;
   out_2384259379620246573[277] = 0;
   out_2384259379620246573[278] = 0;
   out_2384259379620246573[279] = 0;
   out_2384259379620246573[280] = 0;
   out_2384259379620246573[281] = 0;
   out_2384259379620246573[282] = 0;
   out_2384259379620246573[283] = 0;
   out_2384259379620246573[284] = 0;
   out_2384259379620246573[285] = 1;
   out_2384259379620246573[286] = 0;
   out_2384259379620246573[287] = 0;
   out_2384259379620246573[288] = 0;
   out_2384259379620246573[289] = 0;
   out_2384259379620246573[290] = 0;
   out_2384259379620246573[291] = 0;
   out_2384259379620246573[292] = 0;
   out_2384259379620246573[293] = 0;
   out_2384259379620246573[294] = 0;
   out_2384259379620246573[295] = 0;
   out_2384259379620246573[296] = 0;
   out_2384259379620246573[297] = 0;
   out_2384259379620246573[298] = 0;
   out_2384259379620246573[299] = 0;
   out_2384259379620246573[300] = 0;
   out_2384259379620246573[301] = 0;
   out_2384259379620246573[302] = 0;
   out_2384259379620246573[303] = 0;
   out_2384259379620246573[304] = 1;
   out_2384259379620246573[305] = 0;
   out_2384259379620246573[306] = 0;
   out_2384259379620246573[307] = 0;
   out_2384259379620246573[308] = 0;
   out_2384259379620246573[309] = 0;
   out_2384259379620246573[310] = 0;
   out_2384259379620246573[311] = 0;
   out_2384259379620246573[312] = 0;
   out_2384259379620246573[313] = 0;
   out_2384259379620246573[314] = 0;
   out_2384259379620246573[315] = 0;
   out_2384259379620246573[316] = 0;
   out_2384259379620246573[317] = 0;
   out_2384259379620246573[318] = 0;
   out_2384259379620246573[319] = 0;
   out_2384259379620246573[320] = 0;
   out_2384259379620246573[321] = 0;
   out_2384259379620246573[322] = 0;
   out_2384259379620246573[323] = 1;
}
void h_4(double *state, double *unused, double *out_8005419480238059638) {
   out_8005419480238059638[0] = state[6] + state[9];
   out_8005419480238059638[1] = state[7] + state[10];
   out_8005419480238059638[2] = state[8] + state[11];
}
void H_4(double *state, double *unused, double *out_4198581080572341084) {
   out_4198581080572341084[0] = 0;
   out_4198581080572341084[1] = 0;
   out_4198581080572341084[2] = 0;
   out_4198581080572341084[3] = 0;
   out_4198581080572341084[4] = 0;
   out_4198581080572341084[5] = 0;
   out_4198581080572341084[6] = 1;
   out_4198581080572341084[7] = 0;
   out_4198581080572341084[8] = 0;
   out_4198581080572341084[9] = 1;
   out_4198581080572341084[10] = 0;
   out_4198581080572341084[11] = 0;
   out_4198581080572341084[12] = 0;
   out_4198581080572341084[13] = 0;
   out_4198581080572341084[14] = 0;
   out_4198581080572341084[15] = 0;
   out_4198581080572341084[16] = 0;
   out_4198581080572341084[17] = 0;
   out_4198581080572341084[18] = 0;
   out_4198581080572341084[19] = 0;
   out_4198581080572341084[20] = 0;
   out_4198581080572341084[21] = 0;
   out_4198581080572341084[22] = 0;
   out_4198581080572341084[23] = 0;
   out_4198581080572341084[24] = 0;
   out_4198581080572341084[25] = 1;
   out_4198581080572341084[26] = 0;
   out_4198581080572341084[27] = 0;
   out_4198581080572341084[28] = 1;
   out_4198581080572341084[29] = 0;
   out_4198581080572341084[30] = 0;
   out_4198581080572341084[31] = 0;
   out_4198581080572341084[32] = 0;
   out_4198581080572341084[33] = 0;
   out_4198581080572341084[34] = 0;
   out_4198581080572341084[35] = 0;
   out_4198581080572341084[36] = 0;
   out_4198581080572341084[37] = 0;
   out_4198581080572341084[38] = 0;
   out_4198581080572341084[39] = 0;
   out_4198581080572341084[40] = 0;
   out_4198581080572341084[41] = 0;
   out_4198581080572341084[42] = 0;
   out_4198581080572341084[43] = 0;
   out_4198581080572341084[44] = 1;
   out_4198581080572341084[45] = 0;
   out_4198581080572341084[46] = 0;
   out_4198581080572341084[47] = 1;
   out_4198581080572341084[48] = 0;
   out_4198581080572341084[49] = 0;
   out_4198581080572341084[50] = 0;
   out_4198581080572341084[51] = 0;
   out_4198581080572341084[52] = 0;
   out_4198581080572341084[53] = 0;
}
void h_10(double *state, double *unused, double *out_3547829476479846202) {
   out_3547829476479846202[0] = 9.8100000000000005*sin(state[1]) - state[4]*state[8] + state[5]*state[7] + state[12] + state[15];
   out_3547829476479846202[1] = -9.8100000000000005*sin(state[0])*cos(state[1]) + state[3]*state[8] - state[5]*state[6] + state[13] + state[16];
   out_3547829476479846202[2] = -9.8100000000000005*cos(state[0])*cos(state[1]) - state[3]*state[7] + state[4]*state[6] + state[14] + state[17];
}
void H_10(double *state, double *unused, double *out_7942659657018811961) {
   out_7942659657018811961[0] = 0;
   out_7942659657018811961[1] = 9.8100000000000005*cos(state[1]);
   out_7942659657018811961[2] = 0;
   out_7942659657018811961[3] = 0;
   out_7942659657018811961[4] = -state[8];
   out_7942659657018811961[5] = state[7];
   out_7942659657018811961[6] = 0;
   out_7942659657018811961[7] = state[5];
   out_7942659657018811961[8] = -state[4];
   out_7942659657018811961[9] = 0;
   out_7942659657018811961[10] = 0;
   out_7942659657018811961[11] = 0;
   out_7942659657018811961[12] = 1;
   out_7942659657018811961[13] = 0;
   out_7942659657018811961[14] = 0;
   out_7942659657018811961[15] = 1;
   out_7942659657018811961[16] = 0;
   out_7942659657018811961[17] = 0;
   out_7942659657018811961[18] = -9.8100000000000005*cos(state[0])*cos(state[1]);
   out_7942659657018811961[19] = 9.8100000000000005*sin(state[0])*sin(state[1]);
   out_7942659657018811961[20] = 0;
   out_7942659657018811961[21] = state[8];
   out_7942659657018811961[22] = 0;
   out_7942659657018811961[23] = -state[6];
   out_7942659657018811961[24] = -state[5];
   out_7942659657018811961[25] = 0;
   out_7942659657018811961[26] = state[3];
   out_7942659657018811961[27] = 0;
   out_7942659657018811961[28] = 0;
   out_7942659657018811961[29] = 0;
   out_7942659657018811961[30] = 0;
   out_7942659657018811961[31] = 1;
   out_7942659657018811961[32] = 0;
   out_7942659657018811961[33] = 0;
   out_7942659657018811961[34] = 1;
   out_7942659657018811961[35] = 0;
   out_7942659657018811961[36] = 9.8100000000000005*sin(state[0])*cos(state[1]);
   out_7942659657018811961[37] = 9.8100000000000005*sin(state[1])*cos(state[0]);
   out_7942659657018811961[38] = 0;
   out_7942659657018811961[39] = -state[7];
   out_7942659657018811961[40] = state[6];
   out_7942659657018811961[41] = 0;
   out_7942659657018811961[42] = state[4];
   out_7942659657018811961[43] = -state[3];
   out_7942659657018811961[44] = 0;
   out_7942659657018811961[45] = 0;
   out_7942659657018811961[46] = 0;
   out_7942659657018811961[47] = 0;
   out_7942659657018811961[48] = 0;
   out_7942659657018811961[49] = 0;
   out_7942659657018811961[50] = 1;
   out_7942659657018811961[51] = 0;
   out_7942659657018811961[52] = 0;
   out_7942659657018811961[53] = 1;
}
void h_13(double *state, double *unused, double *out_4912143115167447329) {
   out_4912143115167447329[0] = state[3];
   out_4912143115167447329[1] = state[4];
   out_4912143115167447329[2] = state[5];
}
void H_13(double *state, double *unused, double *out_986307255240008283) {
   out_986307255240008283[0] = 0;
   out_986307255240008283[1] = 0;
   out_986307255240008283[2] = 0;
   out_986307255240008283[3] = 1;
   out_986307255240008283[4] = 0;
   out_986307255240008283[5] = 0;
   out_986307255240008283[6] = 0;
   out_986307255240008283[7] = 0;
   out_986307255240008283[8] = 0;
   out_986307255240008283[9] = 0;
   out_986307255240008283[10] = 0;
   out_986307255240008283[11] = 0;
   out_986307255240008283[12] = 0;
   out_986307255240008283[13] = 0;
   out_986307255240008283[14] = 0;
   out_986307255240008283[15] = 0;
   out_986307255240008283[16] = 0;
   out_986307255240008283[17] = 0;
   out_986307255240008283[18] = 0;
   out_986307255240008283[19] = 0;
   out_986307255240008283[20] = 0;
   out_986307255240008283[21] = 0;
   out_986307255240008283[22] = 1;
   out_986307255240008283[23] = 0;
   out_986307255240008283[24] = 0;
   out_986307255240008283[25] = 0;
   out_986307255240008283[26] = 0;
   out_986307255240008283[27] = 0;
   out_986307255240008283[28] = 0;
   out_986307255240008283[29] = 0;
   out_986307255240008283[30] = 0;
   out_986307255240008283[31] = 0;
   out_986307255240008283[32] = 0;
   out_986307255240008283[33] = 0;
   out_986307255240008283[34] = 0;
   out_986307255240008283[35] = 0;
   out_986307255240008283[36] = 0;
   out_986307255240008283[37] = 0;
   out_986307255240008283[38] = 0;
   out_986307255240008283[39] = 0;
   out_986307255240008283[40] = 0;
   out_986307255240008283[41] = 1;
   out_986307255240008283[42] = 0;
   out_986307255240008283[43] = 0;
   out_986307255240008283[44] = 0;
   out_986307255240008283[45] = 0;
   out_986307255240008283[46] = 0;
   out_986307255240008283[47] = 0;
   out_986307255240008283[48] = 0;
   out_986307255240008283[49] = 0;
   out_986307255240008283[50] = 0;
   out_986307255240008283[51] = 0;
   out_986307255240008283[52] = 0;
   out_986307255240008283[53] = 0;
}
void h_14(double *state, double *unused, double *out_3408798223072298079) {
   out_3408798223072298079[0] = state[6];
   out_3408798223072298079[1] = state[7];
   out_3408798223072298079[2] = state[8];
}
void H_14(double *state, double *unused, double *out_7281369512867713380) {
   out_7281369512867713380[0] = 0;
   out_7281369512867713380[1] = 0;
   out_7281369512867713380[2] = 0;
   out_7281369512867713380[3] = 0;
   out_7281369512867713380[4] = 0;
   out_7281369512867713380[5] = 0;
   out_7281369512867713380[6] = 1;
   out_7281369512867713380[7] = 0;
   out_7281369512867713380[8] = 0;
   out_7281369512867713380[9] = 0;
   out_7281369512867713380[10] = 0;
   out_7281369512867713380[11] = 0;
   out_7281369512867713380[12] = 0;
   out_7281369512867713380[13] = 0;
   out_7281369512867713380[14] = 0;
   out_7281369512867713380[15] = 0;
   out_7281369512867713380[16] = 0;
   out_7281369512867713380[17] = 0;
   out_7281369512867713380[18] = 0;
   out_7281369512867713380[19] = 0;
   out_7281369512867713380[20] = 0;
   out_7281369512867713380[21] = 0;
   out_7281369512867713380[22] = 0;
   out_7281369512867713380[23] = 0;
   out_7281369512867713380[24] = 0;
   out_7281369512867713380[25] = 1;
   out_7281369512867713380[26] = 0;
   out_7281369512867713380[27] = 0;
   out_7281369512867713380[28] = 0;
   out_7281369512867713380[29] = 0;
   out_7281369512867713380[30] = 0;
   out_7281369512867713380[31] = 0;
   out_7281369512867713380[32] = 0;
   out_7281369512867713380[33] = 0;
   out_7281369512867713380[34] = 0;
   out_7281369512867713380[35] = 0;
   out_7281369512867713380[36] = 0;
   out_7281369512867713380[37] = 0;
   out_7281369512867713380[38] = 0;
   out_7281369512867713380[39] = 0;
   out_7281369512867713380[40] = 0;
   out_7281369512867713380[41] = 0;
   out_7281369512867713380[42] = 0;
   out_7281369512867713380[43] = 0;
   out_7281369512867713380[44] = 1;
   out_7281369512867713380[45] = 0;
   out_7281369512867713380[46] = 0;
   out_7281369512867713380[47] = 0;
   out_7281369512867713380[48] = 0;
   out_7281369512867713380[49] = 0;
   out_7281369512867713380[50] = 0;
   out_7281369512867713380[51] = 0;
   out_7281369512867713380[52] = 0;
   out_7281369512867713380[53] = 0;
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

void pose_update_4(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<3, 3, 0>(in_x, in_P, h_4, H_4, NULL, in_z, in_R, in_ea, MAHA_THRESH_4);
}
void pose_update_10(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<3, 3, 0>(in_x, in_P, h_10, H_10, NULL, in_z, in_R, in_ea, MAHA_THRESH_10);
}
void pose_update_13(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<3, 3, 0>(in_x, in_P, h_13, H_13, NULL, in_z, in_R, in_ea, MAHA_THRESH_13);
}
void pose_update_14(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<3, 3, 0>(in_x, in_P, h_14, H_14, NULL, in_z, in_R, in_ea, MAHA_THRESH_14);
}
void pose_err_fun(double *nom_x, double *delta_x, double *out_7442492993183452915) {
  err_fun(nom_x, delta_x, out_7442492993183452915);
}
void pose_inv_err_fun(double *nom_x, double *true_x, double *out_6140138630151309189) {
  inv_err_fun(nom_x, true_x, out_6140138630151309189);
}
void pose_H_mod_fun(double *state, double *out_5396528562455239188) {
  H_mod_fun(state, out_5396528562455239188);
}
void pose_f_fun(double *state, double dt, double *out_3110033524529218965) {
  f_fun(state,  dt, out_3110033524529218965);
}
void pose_F_fun(double *state, double dt, double *out_2384259379620246573) {
  F_fun(state,  dt, out_2384259379620246573);
}
void pose_h_4(double *state, double *unused, double *out_8005419480238059638) {
  h_4(state, unused, out_8005419480238059638);
}
void pose_H_4(double *state, double *unused, double *out_4198581080572341084) {
  H_4(state, unused, out_4198581080572341084);
}
void pose_h_10(double *state, double *unused, double *out_3547829476479846202) {
  h_10(state, unused, out_3547829476479846202);
}
void pose_H_10(double *state, double *unused, double *out_7942659657018811961) {
  H_10(state, unused, out_7942659657018811961);
}
void pose_h_13(double *state, double *unused, double *out_4912143115167447329) {
  h_13(state, unused, out_4912143115167447329);
}
void pose_H_13(double *state, double *unused, double *out_986307255240008283) {
  H_13(state, unused, out_986307255240008283);
}
void pose_h_14(double *state, double *unused, double *out_3408798223072298079) {
  h_14(state, unused, out_3408798223072298079);
}
void pose_H_14(double *state, double *unused, double *out_7281369512867713380) {
  H_14(state, unused, out_7281369512867713380);
}
void pose_predict(double *in_x, double *in_P, double *in_Q, double dt) {
  predict(in_x, in_P, in_Q, dt);
}
}

const EKF pose = {
  .name = "pose",
  .kinds = { 4, 10, 13, 14 },
  .feature_kinds = {  },
  .f_fun = pose_f_fun,
  .F_fun = pose_F_fun,
  .err_fun = pose_err_fun,
  .inv_err_fun = pose_inv_err_fun,
  .H_mod_fun = pose_H_mod_fun,
  .predict = pose_predict,
  .hs = {
    { 4, pose_h_4 },
    { 10, pose_h_10 },
    { 13, pose_h_13 },
    { 14, pose_h_14 },
  },
  .Hs = {
    { 4, pose_H_4 },
    { 10, pose_H_10 },
    { 13, pose_H_13 },
    { 14, pose_H_14 },
  },
  .updates = {
    { 4, pose_update_4 },
    { 10, pose_update_10 },
    { 13, pose_update_13 },
    { 14, pose_update_14 },
  },
  .Hes = {
  },
  .sets = {
  },
  .extra_routines = {
  },
};

ekf_lib_init(pose)
