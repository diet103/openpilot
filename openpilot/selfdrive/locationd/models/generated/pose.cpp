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
void err_fun(double *nom_x, double *delta_x, double *out_7243860342894984546) {
   out_7243860342894984546[0] = delta_x[0] + nom_x[0];
   out_7243860342894984546[1] = delta_x[1] + nom_x[1];
   out_7243860342894984546[2] = delta_x[2] + nom_x[2];
   out_7243860342894984546[3] = delta_x[3] + nom_x[3];
   out_7243860342894984546[4] = delta_x[4] + nom_x[4];
   out_7243860342894984546[5] = delta_x[5] + nom_x[5];
   out_7243860342894984546[6] = delta_x[6] + nom_x[6];
   out_7243860342894984546[7] = delta_x[7] + nom_x[7];
   out_7243860342894984546[8] = delta_x[8] + nom_x[8];
   out_7243860342894984546[9] = delta_x[9] + nom_x[9];
   out_7243860342894984546[10] = delta_x[10] + nom_x[10];
   out_7243860342894984546[11] = delta_x[11] + nom_x[11];
   out_7243860342894984546[12] = delta_x[12] + nom_x[12];
   out_7243860342894984546[13] = delta_x[13] + nom_x[13];
   out_7243860342894984546[14] = delta_x[14] + nom_x[14];
   out_7243860342894984546[15] = delta_x[15] + nom_x[15];
   out_7243860342894984546[16] = delta_x[16] + nom_x[16];
   out_7243860342894984546[17] = delta_x[17] + nom_x[17];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_4584043442594633794) {
   out_4584043442594633794[0] = -nom_x[0] + true_x[0];
   out_4584043442594633794[1] = -nom_x[1] + true_x[1];
   out_4584043442594633794[2] = -nom_x[2] + true_x[2];
   out_4584043442594633794[3] = -nom_x[3] + true_x[3];
   out_4584043442594633794[4] = -nom_x[4] + true_x[4];
   out_4584043442594633794[5] = -nom_x[5] + true_x[5];
   out_4584043442594633794[6] = -nom_x[6] + true_x[6];
   out_4584043442594633794[7] = -nom_x[7] + true_x[7];
   out_4584043442594633794[8] = -nom_x[8] + true_x[8];
   out_4584043442594633794[9] = -nom_x[9] + true_x[9];
   out_4584043442594633794[10] = -nom_x[10] + true_x[10];
   out_4584043442594633794[11] = -nom_x[11] + true_x[11];
   out_4584043442594633794[12] = -nom_x[12] + true_x[12];
   out_4584043442594633794[13] = -nom_x[13] + true_x[13];
   out_4584043442594633794[14] = -nom_x[14] + true_x[14];
   out_4584043442594633794[15] = -nom_x[15] + true_x[15];
   out_4584043442594633794[16] = -nom_x[16] + true_x[16];
   out_4584043442594633794[17] = -nom_x[17] + true_x[17];
}
void H_mod_fun(double *state, double *out_1266553482076709236) {
   out_1266553482076709236[0] = 1.0;
   out_1266553482076709236[1] = 0.0;
   out_1266553482076709236[2] = 0.0;
   out_1266553482076709236[3] = 0.0;
   out_1266553482076709236[4] = 0.0;
   out_1266553482076709236[5] = 0.0;
   out_1266553482076709236[6] = 0.0;
   out_1266553482076709236[7] = 0.0;
   out_1266553482076709236[8] = 0.0;
   out_1266553482076709236[9] = 0.0;
   out_1266553482076709236[10] = 0.0;
   out_1266553482076709236[11] = 0.0;
   out_1266553482076709236[12] = 0.0;
   out_1266553482076709236[13] = 0.0;
   out_1266553482076709236[14] = 0.0;
   out_1266553482076709236[15] = 0.0;
   out_1266553482076709236[16] = 0.0;
   out_1266553482076709236[17] = 0.0;
   out_1266553482076709236[18] = 0.0;
   out_1266553482076709236[19] = 1.0;
   out_1266553482076709236[20] = 0.0;
   out_1266553482076709236[21] = 0.0;
   out_1266553482076709236[22] = 0.0;
   out_1266553482076709236[23] = 0.0;
   out_1266553482076709236[24] = 0.0;
   out_1266553482076709236[25] = 0.0;
   out_1266553482076709236[26] = 0.0;
   out_1266553482076709236[27] = 0.0;
   out_1266553482076709236[28] = 0.0;
   out_1266553482076709236[29] = 0.0;
   out_1266553482076709236[30] = 0.0;
   out_1266553482076709236[31] = 0.0;
   out_1266553482076709236[32] = 0.0;
   out_1266553482076709236[33] = 0.0;
   out_1266553482076709236[34] = 0.0;
   out_1266553482076709236[35] = 0.0;
   out_1266553482076709236[36] = 0.0;
   out_1266553482076709236[37] = 0.0;
   out_1266553482076709236[38] = 1.0;
   out_1266553482076709236[39] = 0.0;
   out_1266553482076709236[40] = 0.0;
   out_1266553482076709236[41] = 0.0;
   out_1266553482076709236[42] = 0.0;
   out_1266553482076709236[43] = 0.0;
   out_1266553482076709236[44] = 0.0;
   out_1266553482076709236[45] = 0.0;
   out_1266553482076709236[46] = 0.0;
   out_1266553482076709236[47] = 0.0;
   out_1266553482076709236[48] = 0.0;
   out_1266553482076709236[49] = 0.0;
   out_1266553482076709236[50] = 0.0;
   out_1266553482076709236[51] = 0.0;
   out_1266553482076709236[52] = 0.0;
   out_1266553482076709236[53] = 0.0;
   out_1266553482076709236[54] = 0.0;
   out_1266553482076709236[55] = 0.0;
   out_1266553482076709236[56] = 0.0;
   out_1266553482076709236[57] = 1.0;
   out_1266553482076709236[58] = 0.0;
   out_1266553482076709236[59] = 0.0;
   out_1266553482076709236[60] = 0.0;
   out_1266553482076709236[61] = 0.0;
   out_1266553482076709236[62] = 0.0;
   out_1266553482076709236[63] = 0.0;
   out_1266553482076709236[64] = 0.0;
   out_1266553482076709236[65] = 0.0;
   out_1266553482076709236[66] = 0.0;
   out_1266553482076709236[67] = 0.0;
   out_1266553482076709236[68] = 0.0;
   out_1266553482076709236[69] = 0.0;
   out_1266553482076709236[70] = 0.0;
   out_1266553482076709236[71] = 0.0;
   out_1266553482076709236[72] = 0.0;
   out_1266553482076709236[73] = 0.0;
   out_1266553482076709236[74] = 0.0;
   out_1266553482076709236[75] = 0.0;
   out_1266553482076709236[76] = 1.0;
   out_1266553482076709236[77] = 0.0;
   out_1266553482076709236[78] = 0.0;
   out_1266553482076709236[79] = 0.0;
   out_1266553482076709236[80] = 0.0;
   out_1266553482076709236[81] = 0.0;
   out_1266553482076709236[82] = 0.0;
   out_1266553482076709236[83] = 0.0;
   out_1266553482076709236[84] = 0.0;
   out_1266553482076709236[85] = 0.0;
   out_1266553482076709236[86] = 0.0;
   out_1266553482076709236[87] = 0.0;
   out_1266553482076709236[88] = 0.0;
   out_1266553482076709236[89] = 0.0;
   out_1266553482076709236[90] = 0.0;
   out_1266553482076709236[91] = 0.0;
   out_1266553482076709236[92] = 0.0;
   out_1266553482076709236[93] = 0.0;
   out_1266553482076709236[94] = 0.0;
   out_1266553482076709236[95] = 1.0;
   out_1266553482076709236[96] = 0.0;
   out_1266553482076709236[97] = 0.0;
   out_1266553482076709236[98] = 0.0;
   out_1266553482076709236[99] = 0.0;
   out_1266553482076709236[100] = 0.0;
   out_1266553482076709236[101] = 0.0;
   out_1266553482076709236[102] = 0.0;
   out_1266553482076709236[103] = 0.0;
   out_1266553482076709236[104] = 0.0;
   out_1266553482076709236[105] = 0.0;
   out_1266553482076709236[106] = 0.0;
   out_1266553482076709236[107] = 0.0;
   out_1266553482076709236[108] = 0.0;
   out_1266553482076709236[109] = 0.0;
   out_1266553482076709236[110] = 0.0;
   out_1266553482076709236[111] = 0.0;
   out_1266553482076709236[112] = 0.0;
   out_1266553482076709236[113] = 0.0;
   out_1266553482076709236[114] = 1.0;
   out_1266553482076709236[115] = 0.0;
   out_1266553482076709236[116] = 0.0;
   out_1266553482076709236[117] = 0.0;
   out_1266553482076709236[118] = 0.0;
   out_1266553482076709236[119] = 0.0;
   out_1266553482076709236[120] = 0.0;
   out_1266553482076709236[121] = 0.0;
   out_1266553482076709236[122] = 0.0;
   out_1266553482076709236[123] = 0.0;
   out_1266553482076709236[124] = 0.0;
   out_1266553482076709236[125] = 0.0;
   out_1266553482076709236[126] = 0.0;
   out_1266553482076709236[127] = 0.0;
   out_1266553482076709236[128] = 0.0;
   out_1266553482076709236[129] = 0.0;
   out_1266553482076709236[130] = 0.0;
   out_1266553482076709236[131] = 0.0;
   out_1266553482076709236[132] = 0.0;
   out_1266553482076709236[133] = 1.0;
   out_1266553482076709236[134] = 0.0;
   out_1266553482076709236[135] = 0.0;
   out_1266553482076709236[136] = 0.0;
   out_1266553482076709236[137] = 0.0;
   out_1266553482076709236[138] = 0.0;
   out_1266553482076709236[139] = 0.0;
   out_1266553482076709236[140] = 0.0;
   out_1266553482076709236[141] = 0.0;
   out_1266553482076709236[142] = 0.0;
   out_1266553482076709236[143] = 0.0;
   out_1266553482076709236[144] = 0.0;
   out_1266553482076709236[145] = 0.0;
   out_1266553482076709236[146] = 0.0;
   out_1266553482076709236[147] = 0.0;
   out_1266553482076709236[148] = 0.0;
   out_1266553482076709236[149] = 0.0;
   out_1266553482076709236[150] = 0.0;
   out_1266553482076709236[151] = 0.0;
   out_1266553482076709236[152] = 1.0;
   out_1266553482076709236[153] = 0.0;
   out_1266553482076709236[154] = 0.0;
   out_1266553482076709236[155] = 0.0;
   out_1266553482076709236[156] = 0.0;
   out_1266553482076709236[157] = 0.0;
   out_1266553482076709236[158] = 0.0;
   out_1266553482076709236[159] = 0.0;
   out_1266553482076709236[160] = 0.0;
   out_1266553482076709236[161] = 0.0;
   out_1266553482076709236[162] = 0.0;
   out_1266553482076709236[163] = 0.0;
   out_1266553482076709236[164] = 0.0;
   out_1266553482076709236[165] = 0.0;
   out_1266553482076709236[166] = 0.0;
   out_1266553482076709236[167] = 0.0;
   out_1266553482076709236[168] = 0.0;
   out_1266553482076709236[169] = 0.0;
   out_1266553482076709236[170] = 0.0;
   out_1266553482076709236[171] = 1.0;
   out_1266553482076709236[172] = 0.0;
   out_1266553482076709236[173] = 0.0;
   out_1266553482076709236[174] = 0.0;
   out_1266553482076709236[175] = 0.0;
   out_1266553482076709236[176] = 0.0;
   out_1266553482076709236[177] = 0.0;
   out_1266553482076709236[178] = 0.0;
   out_1266553482076709236[179] = 0.0;
   out_1266553482076709236[180] = 0.0;
   out_1266553482076709236[181] = 0.0;
   out_1266553482076709236[182] = 0.0;
   out_1266553482076709236[183] = 0.0;
   out_1266553482076709236[184] = 0.0;
   out_1266553482076709236[185] = 0.0;
   out_1266553482076709236[186] = 0.0;
   out_1266553482076709236[187] = 0.0;
   out_1266553482076709236[188] = 0.0;
   out_1266553482076709236[189] = 0.0;
   out_1266553482076709236[190] = 1.0;
   out_1266553482076709236[191] = 0.0;
   out_1266553482076709236[192] = 0.0;
   out_1266553482076709236[193] = 0.0;
   out_1266553482076709236[194] = 0.0;
   out_1266553482076709236[195] = 0.0;
   out_1266553482076709236[196] = 0.0;
   out_1266553482076709236[197] = 0.0;
   out_1266553482076709236[198] = 0.0;
   out_1266553482076709236[199] = 0.0;
   out_1266553482076709236[200] = 0.0;
   out_1266553482076709236[201] = 0.0;
   out_1266553482076709236[202] = 0.0;
   out_1266553482076709236[203] = 0.0;
   out_1266553482076709236[204] = 0.0;
   out_1266553482076709236[205] = 0.0;
   out_1266553482076709236[206] = 0.0;
   out_1266553482076709236[207] = 0.0;
   out_1266553482076709236[208] = 0.0;
   out_1266553482076709236[209] = 1.0;
   out_1266553482076709236[210] = 0.0;
   out_1266553482076709236[211] = 0.0;
   out_1266553482076709236[212] = 0.0;
   out_1266553482076709236[213] = 0.0;
   out_1266553482076709236[214] = 0.0;
   out_1266553482076709236[215] = 0.0;
   out_1266553482076709236[216] = 0.0;
   out_1266553482076709236[217] = 0.0;
   out_1266553482076709236[218] = 0.0;
   out_1266553482076709236[219] = 0.0;
   out_1266553482076709236[220] = 0.0;
   out_1266553482076709236[221] = 0.0;
   out_1266553482076709236[222] = 0.0;
   out_1266553482076709236[223] = 0.0;
   out_1266553482076709236[224] = 0.0;
   out_1266553482076709236[225] = 0.0;
   out_1266553482076709236[226] = 0.0;
   out_1266553482076709236[227] = 0.0;
   out_1266553482076709236[228] = 1.0;
   out_1266553482076709236[229] = 0.0;
   out_1266553482076709236[230] = 0.0;
   out_1266553482076709236[231] = 0.0;
   out_1266553482076709236[232] = 0.0;
   out_1266553482076709236[233] = 0.0;
   out_1266553482076709236[234] = 0.0;
   out_1266553482076709236[235] = 0.0;
   out_1266553482076709236[236] = 0.0;
   out_1266553482076709236[237] = 0.0;
   out_1266553482076709236[238] = 0.0;
   out_1266553482076709236[239] = 0.0;
   out_1266553482076709236[240] = 0.0;
   out_1266553482076709236[241] = 0.0;
   out_1266553482076709236[242] = 0.0;
   out_1266553482076709236[243] = 0.0;
   out_1266553482076709236[244] = 0.0;
   out_1266553482076709236[245] = 0.0;
   out_1266553482076709236[246] = 0.0;
   out_1266553482076709236[247] = 1.0;
   out_1266553482076709236[248] = 0.0;
   out_1266553482076709236[249] = 0.0;
   out_1266553482076709236[250] = 0.0;
   out_1266553482076709236[251] = 0.0;
   out_1266553482076709236[252] = 0.0;
   out_1266553482076709236[253] = 0.0;
   out_1266553482076709236[254] = 0.0;
   out_1266553482076709236[255] = 0.0;
   out_1266553482076709236[256] = 0.0;
   out_1266553482076709236[257] = 0.0;
   out_1266553482076709236[258] = 0.0;
   out_1266553482076709236[259] = 0.0;
   out_1266553482076709236[260] = 0.0;
   out_1266553482076709236[261] = 0.0;
   out_1266553482076709236[262] = 0.0;
   out_1266553482076709236[263] = 0.0;
   out_1266553482076709236[264] = 0.0;
   out_1266553482076709236[265] = 0.0;
   out_1266553482076709236[266] = 1.0;
   out_1266553482076709236[267] = 0.0;
   out_1266553482076709236[268] = 0.0;
   out_1266553482076709236[269] = 0.0;
   out_1266553482076709236[270] = 0.0;
   out_1266553482076709236[271] = 0.0;
   out_1266553482076709236[272] = 0.0;
   out_1266553482076709236[273] = 0.0;
   out_1266553482076709236[274] = 0.0;
   out_1266553482076709236[275] = 0.0;
   out_1266553482076709236[276] = 0.0;
   out_1266553482076709236[277] = 0.0;
   out_1266553482076709236[278] = 0.0;
   out_1266553482076709236[279] = 0.0;
   out_1266553482076709236[280] = 0.0;
   out_1266553482076709236[281] = 0.0;
   out_1266553482076709236[282] = 0.0;
   out_1266553482076709236[283] = 0.0;
   out_1266553482076709236[284] = 0.0;
   out_1266553482076709236[285] = 1.0;
   out_1266553482076709236[286] = 0.0;
   out_1266553482076709236[287] = 0.0;
   out_1266553482076709236[288] = 0.0;
   out_1266553482076709236[289] = 0.0;
   out_1266553482076709236[290] = 0.0;
   out_1266553482076709236[291] = 0.0;
   out_1266553482076709236[292] = 0.0;
   out_1266553482076709236[293] = 0.0;
   out_1266553482076709236[294] = 0.0;
   out_1266553482076709236[295] = 0.0;
   out_1266553482076709236[296] = 0.0;
   out_1266553482076709236[297] = 0.0;
   out_1266553482076709236[298] = 0.0;
   out_1266553482076709236[299] = 0.0;
   out_1266553482076709236[300] = 0.0;
   out_1266553482076709236[301] = 0.0;
   out_1266553482076709236[302] = 0.0;
   out_1266553482076709236[303] = 0.0;
   out_1266553482076709236[304] = 1.0;
   out_1266553482076709236[305] = 0.0;
   out_1266553482076709236[306] = 0.0;
   out_1266553482076709236[307] = 0.0;
   out_1266553482076709236[308] = 0.0;
   out_1266553482076709236[309] = 0.0;
   out_1266553482076709236[310] = 0.0;
   out_1266553482076709236[311] = 0.0;
   out_1266553482076709236[312] = 0.0;
   out_1266553482076709236[313] = 0.0;
   out_1266553482076709236[314] = 0.0;
   out_1266553482076709236[315] = 0.0;
   out_1266553482076709236[316] = 0.0;
   out_1266553482076709236[317] = 0.0;
   out_1266553482076709236[318] = 0.0;
   out_1266553482076709236[319] = 0.0;
   out_1266553482076709236[320] = 0.0;
   out_1266553482076709236[321] = 0.0;
   out_1266553482076709236[322] = 0.0;
   out_1266553482076709236[323] = 1.0;
}
void f_fun(double *state, double dt, double *out_5083776387329352420) {
   out_5083776387329352420[0] = atan2((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), -(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]));
   out_5083776387329352420[1] = asin(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]));
   out_5083776387329352420[2] = atan2(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), -(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]));
   out_5083776387329352420[3] = dt*state[12] + state[3];
   out_5083776387329352420[4] = dt*state[13] + state[4];
   out_5083776387329352420[5] = dt*state[14] + state[5];
   out_5083776387329352420[6] = state[6];
   out_5083776387329352420[7] = state[7];
   out_5083776387329352420[8] = state[8];
   out_5083776387329352420[9] = state[9];
   out_5083776387329352420[10] = state[10];
   out_5083776387329352420[11] = state[11];
   out_5083776387329352420[12] = state[12];
   out_5083776387329352420[13] = state[13];
   out_5083776387329352420[14] = state[14];
   out_5083776387329352420[15] = state[15];
   out_5083776387329352420[16] = state[16];
   out_5083776387329352420[17] = state[17];
}
void F_fun(double *state, double dt, double *out_3141212682405851061) {
   out_3141212682405851061[0] = ((-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*cos(state[0])*cos(state[1]) - sin(state[0])*cos(dt*state[6])*cos(dt*state[7])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + ((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*cos(state[0])*cos(state[1]) - sin(dt*state[6])*sin(state[0])*cos(dt*state[7])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_3141212682405851061[1] = ((-sin(dt*state[6])*sin(dt*state[8]) - sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*cos(state[1]) - (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*sin(state[1]) - sin(state[1])*cos(dt*state[6])*cos(dt*state[7])*cos(state[0]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*sin(state[1]) + (-sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) + sin(dt*state[8])*cos(dt*state[6]))*cos(state[1]) - sin(dt*state[6])*sin(state[1])*cos(dt*state[7])*cos(state[0]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_3141212682405851061[2] = 0;
   out_3141212682405851061[3] = 0;
   out_3141212682405851061[4] = 0;
   out_3141212682405851061[5] = 0;
   out_3141212682405851061[6] = (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(dt*cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*sin(dt*state[8]) - dt*sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-dt*sin(dt*state[6])*cos(dt*state[8]) + dt*sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) - dt*cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (dt*sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_3141212682405851061[7] = (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[6])*sin(dt*state[7])*cos(state[0])*cos(state[1]) + dt*sin(dt*state[6])*sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) - dt*sin(dt*state[6])*sin(state[1])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[7])*cos(dt*state[6])*cos(state[0])*cos(state[1]) + dt*sin(dt*state[8])*sin(state[0])*cos(dt*state[6])*cos(dt*state[7])*cos(state[1]) - dt*sin(state[1])*cos(dt*state[6])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_3141212682405851061[8] = ((dt*sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + dt*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (dt*sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + ((dt*sin(dt*state[6])*sin(dt*state[8]) + dt*sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*cos(dt*state[8]) + dt*sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_3141212682405851061[9] = 0;
   out_3141212682405851061[10] = 0;
   out_3141212682405851061[11] = 0;
   out_3141212682405851061[12] = 0;
   out_3141212682405851061[13] = 0;
   out_3141212682405851061[14] = 0;
   out_3141212682405851061[15] = 0;
   out_3141212682405851061[16] = 0;
   out_3141212682405851061[17] = 0;
   out_3141212682405851061[18] = (-sin(dt*state[7])*sin(state[0])*cos(state[1]) - sin(dt*state[8])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_3141212682405851061[19] = (-sin(dt*state[7])*sin(state[1])*cos(state[0]) + sin(dt*state[8])*sin(state[0])*sin(state[1])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_3141212682405851061[20] = 0;
   out_3141212682405851061[21] = 0;
   out_3141212682405851061[22] = 0;
   out_3141212682405851061[23] = 0;
   out_3141212682405851061[24] = 0;
   out_3141212682405851061[25] = (dt*sin(dt*state[7])*sin(dt*state[8])*sin(state[0])*cos(state[1]) - dt*sin(dt*state[7])*sin(state[1])*cos(dt*state[8]) + dt*cos(dt*state[7])*cos(state[0])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_3141212682405851061[26] = (-dt*sin(dt*state[8])*sin(state[1])*cos(dt*state[7]) - dt*sin(state[0])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_3141212682405851061[27] = 0;
   out_3141212682405851061[28] = 0;
   out_3141212682405851061[29] = 0;
   out_3141212682405851061[30] = 0;
   out_3141212682405851061[31] = 0;
   out_3141212682405851061[32] = 0;
   out_3141212682405851061[33] = 0;
   out_3141212682405851061[34] = 0;
   out_3141212682405851061[35] = 0;
   out_3141212682405851061[36] = ((sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[7]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[7]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_3141212682405851061[37] = (-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(-sin(dt*state[7])*sin(state[2])*cos(state[0])*cos(state[1]) + sin(dt*state[8])*sin(state[0])*sin(state[2])*cos(dt*state[7])*cos(state[1]) - sin(state[1])*sin(state[2])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*(-sin(dt*state[7])*cos(state[0])*cos(state[1])*cos(state[2]) + sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1])*cos(state[2]) - sin(state[1])*cos(dt*state[7])*cos(dt*state[8])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_3141212682405851061[38] = ((-sin(state[0])*sin(state[2]) - sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (-sin(state[0])*sin(state[1])*sin(state[2]) - cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_3141212682405851061[39] = 0;
   out_3141212682405851061[40] = 0;
   out_3141212682405851061[41] = 0;
   out_3141212682405851061[42] = 0;
   out_3141212682405851061[43] = (-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(dt*(sin(state[0])*cos(state[2]) - sin(state[1])*sin(state[2])*cos(state[0]))*cos(dt*state[7]) - dt*(sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[7])*sin(dt*state[8]) - dt*sin(dt*state[7])*sin(state[2])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*(dt*(-sin(state[0])*sin(state[2]) - sin(state[1])*cos(state[0])*cos(state[2]))*cos(dt*state[7]) - dt*(sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[7])*sin(dt*state[8]) - dt*sin(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_3141212682405851061[44] = (dt*(sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*cos(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*sin(state[2])*cos(dt*state[7])*cos(state[1]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + (dt*(sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*cos(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[7])*cos(state[1])*cos(state[2]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_3141212682405851061[45] = 0;
   out_3141212682405851061[46] = 0;
   out_3141212682405851061[47] = 0;
   out_3141212682405851061[48] = 0;
   out_3141212682405851061[49] = 0;
   out_3141212682405851061[50] = 0;
   out_3141212682405851061[51] = 0;
   out_3141212682405851061[52] = 0;
   out_3141212682405851061[53] = 0;
   out_3141212682405851061[54] = 0;
   out_3141212682405851061[55] = 0;
   out_3141212682405851061[56] = 0;
   out_3141212682405851061[57] = 1;
   out_3141212682405851061[58] = 0;
   out_3141212682405851061[59] = 0;
   out_3141212682405851061[60] = 0;
   out_3141212682405851061[61] = 0;
   out_3141212682405851061[62] = 0;
   out_3141212682405851061[63] = 0;
   out_3141212682405851061[64] = 0;
   out_3141212682405851061[65] = 0;
   out_3141212682405851061[66] = dt;
   out_3141212682405851061[67] = 0;
   out_3141212682405851061[68] = 0;
   out_3141212682405851061[69] = 0;
   out_3141212682405851061[70] = 0;
   out_3141212682405851061[71] = 0;
   out_3141212682405851061[72] = 0;
   out_3141212682405851061[73] = 0;
   out_3141212682405851061[74] = 0;
   out_3141212682405851061[75] = 0;
   out_3141212682405851061[76] = 1;
   out_3141212682405851061[77] = 0;
   out_3141212682405851061[78] = 0;
   out_3141212682405851061[79] = 0;
   out_3141212682405851061[80] = 0;
   out_3141212682405851061[81] = 0;
   out_3141212682405851061[82] = 0;
   out_3141212682405851061[83] = 0;
   out_3141212682405851061[84] = 0;
   out_3141212682405851061[85] = dt;
   out_3141212682405851061[86] = 0;
   out_3141212682405851061[87] = 0;
   out_3141212682405851061[88] = 0;
   out_3141212682405851061[89] = 0;
   out_3141212682405851061[90] = 0;
   out_3141212682405851061[91] = 0;
   out_3141212682405851061[92] = 0;
   out_3141212682405851061[93] = 0;
   out_3141212682405851061[94] = 0;
   out_3141212682405851061[95] = 1;
   out_3141212682405851061[96] = 0;
   out_3141212682405851061[97] = 0;
   out_3141212682405851061[98] = 0;
   out_3141212682405851061[99] = 0;
   out_3141212682405851061[100] = 0;
   out_3141212682405851061[101] = 0;
   out_3141212682405851061[102] = 0;
   out_3141212682405851061[103] = 0;
   out_3141212682405851061[104] = dt;
   out_3141212682405851061[105] = 0;
   out_3141212682405851061[106] = 0;
   out_3141212682405851061[107] = 0;
   out_3141212682405851061[108] = 0;
   out_3141212682405851061[109] = 0;
   out_3141212682405851061[110] = 0;
   out_3141212682405851061[111] = 0;
   out_3141212682405851061[112] = 0;
   out_3141212682405851061[113] = 0;
   out_3141212682405851061[114] = 1;
   out_3141212682405851061[115] = 0;
   out_3141212682405851061[116] = 0;
   out_3141212682405851061[117] = 0;
   out_3141212682405851061[118] = 0;
   out_3141212682405851061[119] = 0;
   out_3141212682405851061[120] = 0;
   out_3141212682405851061[121] = 0;
   out_3141212682405851061[122] = 0;
   out_3141212682405851061[123] = 0;
   out_3141212682405851061[124] = 0;
   out_3141212682405851061[125] = 0;
   out_3141212682405851061[126] = 0;
   out_3141212682405851061[127] = 0;
   out_3141212682405851061[128] = 0;
   out_3141212682405851061[129] = 0;
   out_3141212682405851061[130] = 0;
   out_3141212682405851061[131] = 0;
   out_3141212682405851061[132] = 0;
   out_3141212682405851061[133] = 1;
   out_3141212682405851061[134] = 0;
   out_3141212682405851061[135] = 0;
   out_3141212682405851061[136] = 0;
   out_3141212682405851061[137] = 0;
   out_3141212682405851061[138] = 0;
   out_3141212682405851061[139] = 0;
   out_3141212682405851061[140] = 0;
   out_3141212682405851061[141] = 0;
   out_3141212682405851061[142] = 0;
   out_3141212682405851061[143] = 0;
   out_3141212682405851061[144] = 0;
   out_3141212682405851061[145] = 0;
   out_3141212682405851061[146] = 0;
   out_3141212682405851061[147] = 0;
   out_3141212682405851061[148] = 0;
   out_3141212682405851061[149] = 0;
   out_3141212682405851061[150] = 0;
   out_3141212682405851061[151] = 0;
   out_3141212682405851061[152] = 1;
   out_3141212682405851061[153] = 0;
   out_3141212682405851061[154] = 0;
   out_3141212682405851061[155] = 0;
   out_3141212682405851061[156] = 0;
   out_3141212682405851061[157] = 0;
   out_3141212682405851061[158] = 0;
   out_3141212682405851061[159] = 0;
   out_3141212682405851061[160] = 0;
   out_3141212682405851061[161] = 0;
   out_3141212682405851061[162] = 0;
   out_3141212682405851061[163] = 0;
   out_3141212682405851061[164] = 0;
   out_3141212682405851061[165] = 0;
   out_3141212682405851061[166] = 0;
   out_3141212682405851061[167] = 0;
   out_3141212682405851061[168] = 0;
   out_3141212682405851061[169] = 0;
   out_3141212682405851061[170] = 0;
   out_3141212682405851061[171] = 1;
   out_3141212682405851061[172] = 0;
   out_3141212682405851061[173] = 0;
   out_3141212682405851061[174] = 0;
   out_3141212682405851061[175] = 0;
   out_3141212682405851061[176] = 0;
   out_3141212682405851061[177] = 0;
   out_3141212682405851061[178] = 0;
   out_3141212682405851061[179] = 0;
   out_3141212682405851061[180] = 0;
   out_3141212682405851061[181] = 0;
   out_3141212682405851061[182] = 0;
   out_3141212682405851061[183] = 0;
   out_3141212682405851061[184] = 0;
   out_3141212682405851061[185] = 0;
   out_3141212682405851061[186] = 0;
   out_3141212682405851061[187] = 0;
   out_3141212682405851061[188] = 0;
   out_3141212682405851061[189] = 0;
   out_3141212682405851061[190] = 1;
   out_3141212682405851061[191] = 0;
   out_3141212682405851061[192] = 0;
   out_3141212682405851061[193] = 0;
   out_3141212682405851061[194] = 0;
   out_3141212682405851061[195] = 0;
   out_3141212682405851061[196] = 0;
   out_3141212682405851061[197] = 0;
   out_3141212682405851061[198] = 0;
   out_3141212682405851061[199] = 0;
   out_3141212682405851061[200] = 0;
   out_3141212682405851061[201] = 0;
   out_3141212682405851061[202] = 0;
   out_3141212682405851061[203] = 0;
   out_3141212682405851061[204] = 0;
   out_3141212682405851061[205] = 0;
   out_3141212682405851061[206] = 0;
   out_3141212682405851061[207] = 0;
   out_3141212682405851061[208] = 0;
   out_3141212682405851061[209] = 1;
   out_3141212682405851061[210] = 0;
   out_3141212682405851061[211] = 0;
   out_3141212682405851061[212] = 0;
   out_3141212682405851061[213] = 0;
   out_3141212682405851061[214] = 0;
   out_3141212682405851061[215] = 0;
   out_3141212682405851061[216] = 0;
   out_3141212682405851061[217] = 0;
   out_3141212682405851061[218] = 0;
   out_3141212682405851061[219] = 0;
   out_3141212682405851061[220] = 0;
   out_3141212682405851061[221] = 0;
   out_3141212682405851061[222] = 0;
   out_3141212682405851061[223] = 0;
   out_3141212682405851061[224] = 0;
   out_3141212682405851061[225] = 0;
   out_3141212682405851061[226] = 0;
   out_3141212682405851061[227] = 0;
   out_3141212682405851061[228] = 1;
   out_3141212682405851061[229] = 0;
   out_3141212682405851061[230] = 0;
   out_3141212682405851061[231] = 0;
   out_3141212682405851061[232] = 0;
   out_3141212682405851061[233] = 0;
   out_3141212682405851061[234] = 0;
   out_3141212682405851061[235] = 0;
   out_3141212682405851061[236] = 0;
   out_3141212682405851061[237] = 0;
   out_3141212682405851061[238] = 0;
   out_3141212682405851061[239] = 0;
   out_3141212682405851061[240] = 0;
   out_3141212682405851061[241] = 0;
   out_3141212682405851061[242] = 0;
   out_3141212682405851061[243] = 0;
   out_3141212682405851061[244] = 0;
   out_3141212682405851061[245] = 0;
   out_3141212682405851061[246] = 0;
   out_3141212682405851061[247] = 1;
   out_3141212682405851061[248] = 0;
   out_3141212682405851061[249] = 0;
   out_3141212682405851061[250] = 0;
   out_3141212682405851061[251] = 0;
   out_3141212682405851061[252] = 0;
   out_3141212682405851061[253] = 0;
   out_3141212682405851061[254] = 0;
   out_3141212682405851061[255] = 0;
   out_3141212682405851061[256] = 0;
   out_3141212682405851061[257] = 0;
   out_3141212682405851061[258] = 0;
   out_3141212682405851061[259] = 0;
   out_3141212682405851061[260] = 0;
   out_3141212682405851061[261] = 0;
   out_3141212682405851061[262] = 0;
   out_3141212682405851061[263] = 0;
   out_3141212682405851061[264] = 0;
   out_3141212682405851061[265] = 0;
   out_3141212682405851061[266] = 1;
   out_3141212682405851061[267] = 0;
   out_3141212682405851061[268] = 0;
   out_3141212682405851061[269] = 0;
   out_3141212682405851061[270] = 0;
   out_3141212682405851061[271] = 0;
   out_3141212682405851061[272] = 0;
   out_3141212682405851061[273] = 0;
   out_3141212682405851061[274] = 0;
   out_3141212682405851061[275] = 0;
   out_3141212682405851061[276] = 0;
   out_3141212682405851061[277] = 0;
   out_3141212682405851061[278] = 0;
   out_3141212682405851061[279] = 0;
   out_3141212682405851061[280] = 0;
   out_3141212682405851061[281] = 0;
   out_3141212682405851061[282] = 0;
   out_3141212682405851061[283] = 0;
   out_3141212682405851061[284] = 0;
   out_3141212682405851061[285] = 1;
   out_3141212682405851061[286] = 0;
   out_3141212682405851061[287] = 0;
   out_3141212682405851061[288] = 0;
   out_3141212682405851061[289] = 0;
   out_3141212682405851061[290] = 0;
   out_3141212682405851061[291] = 0;
   out_3141212682405851061[292] = 0;
   out_3141212682405851061[293] = 0;
   out_3141212682405851061[294] = 0;
   out_3141212682405851061[295] = 0;
   out_3141212682405851061[296] = 0;
   out_3141212682405851061[297] = 0;
   out_3141212682405851061[298] = 0;
   out_3141212682405851061[299] = 0;
   out_3141212682405851061[300] = 0;
   out_3141212682405851061[301] = 0;
   out_3141212682405851061[302] = 0;
   out_3141212682405851061[303] = 0;
   out_3141212682405851061[304] = 1;
   out_3141212682405851061[305] = 0;
   out_3141212682405851061[306] = 0;
   out_3141212682405851061[307] = 0;
   out_3141212682405851061[308] = 0;
   out_3141212682405851061[309] = 0;
   out_3141212682405851061[310] = 0;
   out_3141212682405851061[311] = 0;
   out_3141212682405851061[312] = 0;
   out_3141212682405851061[313] = 0;
   out_3141212682405851061[314] = 0;
   out_3141212682405851061[315] = 0;
   out_3141212682405851061[316] = 0;
   out_3141212682405851061[317] = 0;
   out_3141212682405851061[318] = 0;
   out_3141212682405851061[319] = 0;
   out_3141212682405851061[320] = 0;
   out_3141212682405851061[321] = 0;
   out_3141212682405851061[322] = 0;
   out_3141212682405851061[323] = 1;
}
void h_4(double *state, double *unused, double *out_3979435972361617785) {
   out_3979435972361617785[0] = state[6] + state[9];
   out_3979435972361617785[1] = state[7] + state[10];
   out_3979435972361617785[2] = state[8] + state[11];
}
void H_4(double *state, double *unused, double *out_8167206171719157888) {
   out_8167206171719157888[0] = 0;
   out_8167206171719157888[1] = 0;
   out_8167206171719157888[2] = 0;
   out_8167206171719157888[3] = 0;
   out_8167206171719157888[4] = 0;
   out_8167206171719157888[5] = 0;
   out_8167206171719157888[6] = 1;
   out_8167206171719157888[7] = 0;
   out_8167206171719157888[8] = 0;
   out_8167206171719157888[9] = 1;
   out_8167206171719157888[10] = 0;
   out_8167206171719157888[11] = 0;
   out_8167206171719157888[12] = 0;
   out_8167206171719157888[13] = 0;
   out_8167206171719157888[14] = 0;
   out_8167206171719157888[15] = 0;
   out_8167206171719157888[16] = 0;
   out_8167206171719157888[17] = 0;
   out_8167206171719157888[18] = 0;
   out_8167206171719157888[19] = 0;
   out_8167206171719157888[20] = 0;
   out_8167206171719157888[21] = 0;
   out_8167206171719157888[22] = 0;
   out_8167206171719157888[23] = 0;
   out_8167206171719157888[24] = 0;
   out_8167206171719157888[25] = 1;
   out_8167206171719157888[26] = 0;
   out_8167206171719157888[27] = 0;
   out_8167206171719157888[28] = 1;
   out_8167206171719157888[29] = 0;
   out_8167206171719157888[30] = 0;
   out_8167206171719157888[31] = 0;
   out_8167206171719157888[32] = 0;
   out_8167206171719157888[33] = 0;
   out_8167206171719157888[34] = 0;
   out_8167206171719157888[35] = 0;
   out_8167206171719157888[36] = 0;
   out_8167206171719157888[37] = 0;
   out_8167206171719157888[38] = 0;
   out_8167206171719157888[39] = 0;
   out_8167206171719157888[40] = 0;
   out_8167206171719157888[41] = 0;
   out_8167206171719157888[42] = 0;
   out_8167206171719157888[43] = 0;
   out_8167206171719157888[44] = 1;
   out_8167206171719157888[45] = 0;
   out_8167206171719157888[46] = 0;
   out_8167206171719157888[47] = 1;
   out_8167206171719157888[48] = 0;
   out_8167206171719157888[49] = 0;
   out_8167206171719157888[50] = 0;
   out_8167206171719157888[51] = 0;
   out_8167206171719157888[52] = 0;
   out_8167206171719157888[53] = 0;
}
void h_10(double *state, double *unused, double *out_3349891990278921599) {
   out_3349891990278921599[0] = 9.8100000000000005*sin(state[1]) - state[4]*state[8] + state[5]*state[7] + state[12] + state[15];
   out_3349891990278921599[1] = -9.8100000000000005*sin(state[0])*cos(state[1]) + state[3]*state[8] - state[5]*state[6] + state[13] + state[16];
   out_3349891990278921599[2] = -9.8100000000000005*cos(state[0])*cos(state[1]) - state[3]*state[7] + state[4]*state[6] + state[14] + state[17];
}
void H_10(double *state, double *unused, double *out_4622200334136351809) {
   out_4622200334136351809[0] = 0;
   out_4622200334136351809[1] = 9.8100000000000005*cos(state[1]);
   out_4622200334136351809[2] = 0;
   out_4622200334136351809[3] = 0;
   out_4622200334136351809[4] = -state[8];
   out_4622200334136351809[5] = state[7];
   out_4622200334136351809[6] = 0;
   out_4622200334136351809[7] = state[5];
   out_4622200334136351809[8] = -state[4];
   out_4622200334136351809[9] = 0;
   out_4622200334136351809[10] = 0;
   out_4622200334136351809[11] = 0;
   out_4622200334136351809[12] = 1;
   out_4622200334136351809[13] = 0;
   out_4622200334136351809[14] = 0;
   out_4622200334136351809[15] = 1;
   out_4622200334136351809[16] = 0;
   out_4622200334136351809[17] = 0;
   out_4622200334136351809[18] = -9.8100000000000005*cos(state[0])*cos(state[1]);
   out_4622200334136351809[19] = 9.8100000000000005*sin(state[0])*sin(state[1]);
   out_4622200334136351809[20] = 0;
   out_4622200334136351809[21] = state[8];
   out_4622200334136351809[22] = 0;
   out_4622200334136351809[23] = -state[6];
   out_4622200334136351809[24] = -state[5];
   out_4622200334136351809[25] = 0;
   out_4622200334136351809[26] = state[3];
   out_4622200334136351809[27] = 0;
   out_4622200334136351809[28] = 0;
   out_4622200334136351809[29] = 0;
   out_4622200334136351809[30] = 0;
   out_4622200334136351809[31] = 1;
   out_4622200334136351809[32] = 0;
   out_4622200334136351809[33] = 0;
   out_4622200334136351809[34] = 1;
   out_4622200334136351809[35] = 0;
   out_4622200334136351809[36] = 9.8100000000000005*sin(state[0])*cos(state[1]);
   out_4622200334136351809[37] = 9.8100000000000005*sin(state[1])*cos(state[0]);
   out_4622200334136351809[38] = 0;
   out_4622200334136351809[39] = -state[7];
   out_4622200334136351809[40] = state[6];
   out_4622200334136351809[41] = 0;
   out_4622200334136351809[42] = state[4];
   out_4622200334136351809[43] = -state[3];
   out_4622200334136351809[44] = 0;
   out_4622200334136351809[45] = 0;
   out_4622200334136351809[46] = 0;
   out_4622200334136351809[47] = 0;
   out_4622200334136351809[48] = 0;
   out_4622200334136351809[49] = 0;
   out_4622200334136351809[50] = 1;
   out_4622200334136351809[51] = 0;
   out_4622200334136351809[52] = 0;
   out_4622200334136351809[53] = 1;
}
void h_13(double *state, double *unused, double *out_5229982678955302054) {
   out_5229982678955302054[0] = state[3];
   out_5229982678955302054[1] = state[4];
   out_5229982678955302054[2] = state[5];
}
void H_13(double *state, double *unused, double *out_4954932346386825087) {
   out_4954932346386825087[0] = 0;
   out_4954932346386825087[1] = 0;
   out_4954932346386825087[2] = 0;
   out_4954932346386825087[3] = 1;
   out_4954932346386825087[4] = 0;
   out_4954932346386825087[5] = 0;
   out_4954932346386825087[6] = 0;
   out_4954932346386825087[7] = 0;
   out_4954932346386825087[8] = 0;
   out_4954932346386825087[9] = 0;
   out_4954932346386825087[10] = 0;
   out_4954932346386825087[11] = 0;
   out_4954932346386825087[12] = 0;
   out_4954932346386825087[13] = 0;
   out_4954932346386825087[14] = 0;
   out_4954932346386825087[15] = 0;
   out_4954932346386825087[16] = 0;
   out_4954932346386825087[17] = 0;
   out_4954932346386825087[18] = 0;
   out_4954932346386825087[19] = 0;
   out_4954932346386825087[20] = 0;
   out_4954932346386825087[21] = 0;
   out_4954932346386825087[22] = 1;
   out_4954932346386825087[23] = 0;
   out_4954932346386825087[24] = 0;
   out_4954932346386825087[25] = 0;
   out_4954932346386825087[26] = 0;
   out_4954932346386825087[27] = 0;
   out_4954932346386825087[28] = 0;
   out_4954932346386825087[29] = 0;
   out_4954932346386825087[30] = 0;
   out_4954932346386825087[31] = 0;
   out_4954932346386825087[32] = 0;
   out_4954932346386825087[33] = 0;
   out_4954932346386825087[34] = 0;
   out_4954932346386825087[35] = 0;
   out_4954932346386825087[36] = 0;
   out_4954932346386825087[37] = 0;
   out_4954932346386825087[38] = 0;
   out_4954932346386825087[39] = 0;
   out_4954932346386825087[40] = 0;
   out_4954932346386825087[41] = 1;
   out_4954932346386825087[42] = 0;
   out_4954932346386825087[43] = 0;
   out_4954932346386825087[44] = 0;
   out_4954932346386825087[45] = 0;
   out_4954932346386825087[46] = 0;
   out_4954932346386825087[47] = 0;
   out_4954932346386825087[48] = 0;
   out_4954932346386825087[49] = 0;
   out_4954932346386825087[50] = 0;
   out_4954932346386825087[51] = 0;
   out_4954932346386825087[52] = 0;
   out_4954932346386825087[53] = 0;
}
void h_14(double *state, double *unused, double *out_5256885464416768991) {
   out_5256885464416768991[0] = state[6];
   out_5256885464416768991[1] = state[7];
   out_5256885464416768991[2] = state[8];
}
void H_14(double *state, double *unused, double *out_4203965315379673359) {
   out_4203965315379673359[0] = 0;
   out_4203965315379673359[1] = 0;
   out_4203965315379673359[2] = 0;
   out_4203965315379673359[3] = 0;
   out_4203965315379673359[4] = 0;
   out_4203965315379673359[5] = 0;
   out_4203965315379673359[6] = 1;
   out_4203965315379673359[7] = 0;
   out_4203965315379673359[8] = 0;
   out_4203965315379673359[9] = 0;
   out_4203965315379673359[10] = 0;
   out_4203965315379673359[11] = 0;
   out_4203965315379673359[12] = 0;
   out_4203965315379673359[13] = 0;
   out_4203965315379673359[14] = 0;
   out_4203965315379673359[15] = 0;
   out_4203965315379673359[16] = 0;
   out_4203965315379673359[17] = 0;
   out_4203965315379673359[18] = 0;
   out_4203965315379673359[19] = 0;
   out_4203965315379673359[20] = 0;
   out_4203965315379673359[21] = 0;
   out_4203965315379673359[22] = 0;
   out_4203965315379673359[23] = 0;
   out_4203965315379673359[24] = 0;
   out_4203965315379673359[25] = 1;
   out_4203965315379673359[26] = 0;
   out_4203965315379673359[27] = 0;
   out_4203965315379673359[28] = 0;
   out_4203965315379673359[29] = 0;
   out_4203965315379673359[30] = 0;
   out_4203965315379673359[31] = 0;
   out_4203965315379673359[32] = 0;
   out_4203965315379673359[33] = 0;
   out_4203965315379673359[34] = 0;
   out_4203965315379673359[35] = 0;
   out_4203965315379673359[36] = 0;
   out_4203965315379673359[37] = 0;
   out_4203965315379673359[38] = 0;
   out_4203965315379673359[39] = 0;
   out_4203965315379673359[40] = 0;
   out_4203965315379673359[41] = 0;
   out_4203965315379673359[42] = 0;
   out_4203965315379673359[43] = 0;
   out_4203965315379673359[44] = 1;
   out_4203965315379673359[45] = 0;
   out_4203965315379673359[46] = 0;
   out_4203965315379673359[47] = 0;
   out_4203965315379673359[48] = 0;
   out_4203965315379673359[49] = 0;
   out_4203965315379673359[50] = 0;
   out_4203965315379673359[51] = 0;
   out_4203965315379673359[52] = 0;
   out_4203965315379673359[53] = 0;
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
void pose_err_fun(double *nom_x, double *delta_x, double *out_7243860342894984546) {
  err_fun(nom_x, delta_x, out_7243860342894984546);
}
void pose_inv_err_fun(double *nom_x, double *true_x, double *out_4584043442594633794) {
  inv_err_fun(nom_x, true_x, out_4584043442594633794);
}
void pose_H_mod_fun(double *state, double *out_1266553482076709236) {
  H_mod_fun(state, out_1266553482076709236);
}
void pose_f_fun(double *state, double dt, double *out_5083776387329352420) {
  f_fun(state,  dt, out_5083776387329352420);
}
void pose_F_fun(double *state, double dt, double *out_3141212682405851061) {
  F_fun(state,  dt, out_3141212682405851061);
}
void pose_h_4(double *state, double *unused, double *out_3979435972361617785) {
  h_4(state, unused, out_3979435972361617785);
}
void pose_H_4(double *state, double *unused, double *out_8167206171719157888) {
  H_4(state, unused, out_8167206171719157888);
}
void pose_h_10(double *state, double *unused, double *out_3349891990278921599) {
  h_10(state, unused, out_3349891990278921599);
}
void pose_H_10(double *state, double *unused, double *out_4622200334136351809) {
  H_10(state, unused, out_4622200334136351809);
}
void pose_h_13(double *state, double *unused, double *out_5229982678955302054) {
  h_13(state, unused, out_5229982678955302054);
}
void pose_H_13(double *state, double *unused, double *out_4954932346386825087) {
  H_13(state, unused, out_4954932346386825087);
}
void pose_h_14(double *state, double *unused, double *out_5256885464416768991) {
  h_14(state, unused, out_5256885464416768991);
}
void pose_H_14(double *state, double *unused, double *out_4203965315379673359) {
  H_14(state, unused, out_4203965315379673359);
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
