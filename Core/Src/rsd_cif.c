/*
 * analog.c
 *
 *  Created on: Nov 24, 2025
 *      Author: gtimpani
 */

#include <rsd_cif.h>

//#define DEVICE_NOMINAL
#ifndef DEVICE_NOMINAL
#define DEVICE_REDUNDANT
#endif

/*********************************************************
* PRIVATE STRUCTURED TYPES
**********************************************************/
/*********************************************************
* Input
**********************************************************/
/*!
  @brief SPI_if struct {SPI_id, CS_PIN}
*/
static const SPI_if SPI_N       = {0U, SPI_N_CS_PIN};
static const SPI_if SPI_R       = {1U, SPI_R_CS_PIN};
static const SPI_if SPI_NONE    = {2U, 0x0};

/*!
  @brief ADC_n struct {ADC_id, ADC_active_ch_mask, ADC_SPI_if}
*/
static const ADC_n ADC_A        = {0U,   0x1F, SPI_N};
static const ADC_n ADC_B        = {1U,   0x1F, SPI_R};
static const ADC_n ADC_NONE     = {2U,   0x00, SPI_NONE};

/*!
  @brief MUX_x struct {MUX_id, MUX_type, ADC, ADC_channel, MUX_port, MUX_addressMask, MUX_active_ch_mask}
*/
#define MULTIPLEXERS_LIST \
    X(MUX_NONE, 0xFF,   ADC_NONE,       0U,      0x0,        0x0,                     0x0) \
    X(MUX_1,    32U,    ADC_A,          0U,      MUX_PORT_N, MUX_PORT_N_ADDRESS_MASK, 0xFFFFFFFF) \
    X(MUX_2,    32U,    ADC_B,          0U,      MUX_PORT_R, MUX_PORT_R_ADDRESS_MASK, 0xFFFFFFFF) \
    X(MUX_3,    32U,    ADC_A,          1U,      MUX_PORT_N, MUX_PORT_N_ADDRESS_MASK, 0xFFFFFFFF) \
    X(MUX_4,    32U,    ADC_B,          1U,      MUX_PORT_R, MUX_PORT_R_ADDRESS_MASK, 0xFFFFFFFF) \
    X(MUX_5,    32U,    ADC_A,          2U,      MUX_PORT_N, MUX_PORT_N_ADDRESS_MASK, 0xFFFFFFFF) \
    X(MUX_6,    32U,    ADC_B,          2U,      MUX_PORT_R, MUX_PORT_R_ADDRESS_MASK, 0xFFFFFFFF) \
    X(MUX_7,    32U,    ADC_A,          3U,      MUX_PORT_N, MUX_PORT_N_ADDRESS_MASK, 0x7FFFFFFF) \
    X(MUX_8,    32U,    ADC_B,          3U,      MUX_PORT_R, MUX_PORT_R_ADDRESS_MASK, 0x7FFFFFFF) \
    X(MUX_9,    32U,    ADC_A,          4U,      MUX_PORT_N, MUX_PORT_N_ADDRESS_MASK, 0xFFFF3FFF) \
    X(MUX_10,   32U,    ADC_B,          4U,      MUX_PORT_R, MUX_PORT_R_ADDRESS_MASK, 0xFFFF0FFF)

static const MUX_x multiplexers[] = {
#define X(MUX_id, MUX_type, ADC, ADC_channel, MUX_port, MUX_addressMask, MUX_active_ch_mask) \
    [MUX_id] = {MUX_id, MUX_type, ADC, ADC_channel, MUX_port, MUX_addressMask, MUX_active_ch_mask},
    MULTIPLEXERS_LIST
#undef X
};

/*!
  @brief RSD_analog_parameter struct {param, MUX_n, MUX_n_analogInputChannel, MUX_r, MUX_r_analogInputChannel}
*/
#define ANALOG_SIGNAL_LIST \
    X(PAR_INITIAL_MASS_OF_WATER,    	MUX_NONE,   0U,     		MUX_NONE,   0U)  	\
	X(PAR_INITIAL_VOL_OF_WATER,     	MUX_NONE,   0U,     		MUX_NONE,   0U)  	\
	X(PAR_BLADDER_TANK_TOT_VOL,     	MUX_NONE,   0U,     		MUX_NONE,   0U)  	\
	X(PAR_PRESSURANT_TANK_VOL,      	MUX_NONE,   0U,     		MUX_NONE,   0U)  	\
	X(PAR_P_N2_BoR,                 	MUX_NONE,   0U,     		MUX_NONE,   0U)  	\
	X(PAR_T_N2_BoR,                 	MUX_NONE,   0U,     		MUX_NONE,   0U)  	\
	X(PAR_P_H2O_BoR_PS2,            	MUX_NONE,   0U,     		MUX_NONE,   0U)  	\
	X(PAR_P_H2O_BoR_PS3,            	MUX_NONE,   0U,     		MUX_NONE,   0U)  	\
	X(PAR_T_H2O_BoR_T09,            	MUX_NONE,   0U,     		MUX_NONE,   0U)  	\
	X(PAR_T_H2O_BoR_T10,            	MUX_NONE,   0U,     		MUX_NONE,   0U)  	\
	X(PAR_T_H2O_BoR_T11,            	MUX_NONE,   0U,     		MUX_NONE,   0U)  	\
	X(PAR_T_H2O_BoR_T12,            	MUX_NONE,   0U,     		MUX_NONE,   0U)  	\
	X(PAR_REMAINING_WATER,          	MUX_NONE,   0U,     		MUX_NONE,   0U)  	\
	X(PAR_REMAINING_PRESS,          	MUX_NONE,   0U,     		MUX_NONE,   0U)  	\
	X(PAR_TRANSFERR_WATER,          	MUX_NONE,   0U,     		MUX_NONE,   0U)  	\
	X(PAR_MV_A_CURR,                	MUX_7,      29U,    		MUX_NONE,   0U)  	\
	X(PAR_MV_A_VOLT,                	MUX_7,      28U,    		MUX_NONE,   0U)  	\
	X(PAR_MV_B_CURR,                	MUX_8,      29U,    		MUX_NONE,   0U)  	\
	X(PAR_MV_B_VOLT,                	MUX_8,      28U,    		MUX_NONE,   0U)  	\
	X(PAR_IV_A_CURR,                	MUX_7,      31U,    		MUX_NONE,   0U)  	\
	X(PAR_IV_A_VOLT,                	MUX_7,      30U,    		MUX_NONE,   0U)  	\
	X(PAR_IV_B_CURR,                	MUX_8,      31U,    		MUX_NONE,   0U)  	\
	X(PAR_IV_B_VOLT,                	MUX_8,      30U,    		MUX_NONE,   0U)  	\
	X(PAR_VV1_A_CURR,               	MUX_9,      7U,     		MUX_NONE,   0U)  	\
	X(PAR_VV1_A_VOLT,               	MUX_9,      8U,     		MUX_NONE,   0U)  	\
	X(PAR_VV1_B_CURR,               	MUX_10,     7U,     		MUX_NONE,   0U)  	\
	X(PAR_VV1_B_VOLT,               	MUX_10,     8U,     		MUX_NONE,   0U)  	\
	X(PAR_VV2_A_CURR,               	MUX_9,      9U,     		MUX_NONE,   0U)  	\
	X(PAR_VV2_A_VOLT,               	MUX_9,      10U,    		MUX_NONE,   0U)  	\
	X(PAR_VV2_B_CURR,               	MUX_10,     9U,     		MUX_NONE,   0U)  	\
	X(PAR_VV2_B_VOLT,               	MUX_10,     10U,    		MUX_NONE,   0U)  	\
	X(PAR_VV3_A_CURR,               	MUX_9,      11U,    		MUX_NONE,   0U)  	\
	X(PAR_VV3_A_VOLT,               	MUX_9,      12U,    		MUX_NONE,   0U)  	\
	X(PAR_VV3_B_CURR,               	MUX_10,     11U,    		MUX_NONE,   0U)  	\
	X(PAR_VV3_B_VOLT,               	MUX_10,     12U,    		MUX_NONE,   0U)  	\
	X(PAR_VV4_CURR,                 	MUX_9,      13U,    		MUX_NONE,   0U)  	\
	X(PAR_VV4_VOLT,                 	MUX_9,      14U,    		MUX_NONE,   0U)  	\
	X(PAR_EPR_1A_CURR,              	MUX_7,      1U,     		MUX_NONE,   0U)  	\
	X(PAR_EPR_1A_VOLT,              	MUX_7,      2U,     		MUX_NONE,   0U)  	\
	X(PAR_EPR_1B_CURR,              	MUX_8,      1U,     		MUX_NONE,   0U)  	\
	X(PAR_EPR_1B_VOLT,              	MUX_8,      2U,     		MUX_NONE,   0U)  	\
	X(PAR_EPR_2A_CURR,              	MUX_7,      3U,     		MUX_NONE,   0U)  	\
	X(PAR_EPR_2A_VOLT,              	MUX_7,      4U,     		MUX_NONE,   0U)  	\
	X(PAR_EPR_2B_CURR,              	MUX_8,      3U,     		MUX_NONE,   0U)  	\
	X(PAR_EPR_2B_VOLT,              	MUX_8,      4U,     		MUX_NONE,   0U)  	\
	X(PAR_EPR_3A_CURR,              	MUX_7,      5U,     		MUX_NONE,   0U)  	\
	X(PAR_EPR_3A_VOLT,              	MUX_7,      6U,     		MUX_NONE,   0U)  	\
	X(PAR_EPR_3B_CURR,              	MUX_8,      5U,     		MUX_NONE,   0U)  	\
	X(PAR_EPR_3B_VOLT,              	MUX_8,      6U,     		MUX_NONE,   0U)  	\
	X(PAR_EPR_A_PS1,                	MUX_9,      3U,     		MUX_NONE,   0U)  	\
	X(PAR_EPR_B_PS1,                	MUX_10,     3U,     		MUX_NONE,   0U)  	\
	X(PAR_EPR_A_PS2,                	MUX_9,      6U,     		MUX_NONE,   0U)  	\
	X(PAR_EPR_B_PS2,                	MUX_10,     6U,     		MUX_NONE,   0U)  	\
	X(PAR_EPR_A_PS1_CURR,           	MUX_9,      1U,     		MUX_NONE,   0U)  	\
	X(PAR_EPR_A_PS1_VOLT,           	MUX_9,      2U,     		MUX_NONE,   0U)  	\
	X(PAR_EPR_B_PS1_CURR,           	MUX_10,     1U,     		MUX_NONE,   0U)  	\
	X(PAR_EPR_B_PS1_VOLT,           	MUX_10,     2U,     		MUX_NONE,   0U)  	\
	X(PAR_EPR_A_PS2_CURR,           	MUX_9,      4U,     		MUX_NONE,   0U)  	\
	X(PAR_EPR_A_PS2_VOLT,           	MUX_9,      5U,     		MUX_NONE,   0U)  	\
	X(PAR_EPR_B_PS2_CURR,           	MUX_10,     4U,     		MUX_NONE,   0U)  	\
	X(PAR_EPR_B_PS2_VOLT,           	MUX_10,     5U,     		MUX_NONE,   0U)  	\
	X(PAR_RSD_PS1,                  	MUX_7,      9U,     		MUX_8,      9U)  	\
	X(PAR_RSD_PS2,                  	MUX_7,      12U,    		MUX_8,      12U) 	\
	X(PAR_RSD_PS3,                  	MUX_7,      15U,    		MUX_8,      15U) 	\
	X(PAR_RSD_PS4_A,                	MUX_7,      18U,    		MUX_NONE,   0U)  	\
	X(PAR_RSD_PS4_B,                	MUX_NONE,   0U,     		MUX_8,      18U) 	\
	X(PAR_RSD_PS5,                  	MUX_7,      21U,    		MUX_8,      21U) 	\
	X(PAR_RSD_PS6,                  	MUX_7,      24U,    		MUX_8,      24U) 	\
	X(PAR_RSD_PS7,                  	MUX_7,      27U,    		MUX_8,      27U) 	\
	X(PAR_TS_TEMP_01,               	MUX_1,      1U,     		MUX_2,      1U)  	\
	X(PAR_TS_TEMP_02,               	MUX_1,      3U,     		MUX_2,      3U)  	\
	X(PAR_TS_TEMP_03,               	MUX_1,      5U,     		MUX_2,      5U)  	\
	X(PAR_TS_TEMP_04,               	MUX_1,      7U,     		MUX_2,      7U)  	\
	X(PAR_TS_TEMP_05,               	MUX_1,      9U,     		MUX_2,      9U)  	\
	X(PAR_TS_TEMP_06,               	MUX_1,      11U,    		MUX_2,      11U) 	\
	X(PAR_TS_TEMP_07,               	MUX_1,      13U,    		MUX_2,      13U) 	\
	X(PAR_TS_TEMP_08,               	MUX_1,      15U,    		MUX_2,      15U) 	\
	X(PAR_TS_TEMP_09,               	MUX_1,      17U,    		MUX_2,      17U) 	\
	X(PAR_TS_TEMP_10,               	MUX_1,      19U,    		MUX_2,      19U) 	\
	X(PAR_TS_TEMP_11,               	MUX_1,      21U,    		MUX_2,      21U) 	\
	X(PAR_TS_TEMP_12,               	MUX_1,      23U,    		MUX_2,      23U) 	\
	X(PAR_TS_TEMP_13,               	MUX_1,      25U,    		MUX_2,      25U) 	\
	X(PAR_TS_TEMP_14,               	MUX_1,      27U,    		MUX_2,      27U) 	\
	X(PAR_TS_TEMP_15,               	MUX_1,      29U,    		MUX_2,      29U) 	\
	X(PAR_TS_TEMP_16,               	MUX_1,      31U,    		MUX_2,      31U) 	\
	X(PAR_TS_TEMP_17,               	MUX_3,      1U,     		MUX_4,      1U)  	\
	X(PAR_TS_TEMP_18,               	MUX_3,      3U,     		MUX_4,      3U)  	\
	X(PAR_TS_TEMP_19,               	MUX_3,      5U,     		MUX_4,      5U)  	\
	X(PAR_TS_TEMP_20,               	MUX_3,      7U,     		MUX_4,      7U)  	\
	X(PAR_TS_TEMP_21,               	MUX_3,      9U,     		MUX_4,      9U)  	\
	X(PAR_TS_TEMP_22,               	MUX_3,      11U,    		MUX_4,      11U) 	\
	X(PAR_TS_TEMP_23,               	MUX_3,      13U,    		MUX_4,      13U) 	\
	X(PAR_TS_TEMP_24,               	MUX_3,      15U,    		MUX_4,      15U) 	\
	X(PAR_TS_TEMP_25,               	MUX_3,      17U,    		MUX_4,      17U) 	\
	X(PAR_TS_TEMP_26,               	MUX_3,      19U,    		MUX_4,      19U) 	\
	X(PAR_TS_TEMP_27,               	MUX_3,      21U,    		MUX_4,      21U) 	\
	X(PAR_TS_TEMP_28,               	MUX_3,      23U,    		MUX_4,      23U) 	\
	X(PAR_TS_TEMP_29,               	MUX_3,      25U,    		MUX_4,      25U) 	\
	X(PAR_TS_TEMP_30,               	MUX_3,      27U,    		MUX_4,      27U) 	\
	X(PAR_TS_TEMP_31,               	MUX_3,      29U,    		MUX_4,      29U) 	\
	X(PAR_TS_TEMP_32,               	MUX_3,      31U,    		MUX_4,      31U) 	\
	X(PAR_RSD_PS1_CURR,             	MUX_7,      7U,     		MUX_8,      7U)  	\
	X(PAR_RSD_PS1_VOLT,             	MUX_7,      8U,     		MUX_8,      8U)  	\
	X(PAR_RSD_PS2_CURR,             	MUX_7,      10U,    		MUX_8,      10U) 	\
	X(PAR_RSD_PS2_VOLT,             	MUX_7,      11U,    		MUX_8,      11U) 	\
	X(PAR_RSD_PS3_CURR,             	MUX_7,      13U,    		MUX_8,      13U) 	\
	X(PAR_RSD_PS3_VOLT,             	MUX_7,      14U,    		MUX_8,      14U) 	\
	X(PAR_RSD_PS4_A_CURR,           	MUX_7,      16U,    		MUX_NONE,   0U)  	\
	X(PAR_RSD_PS4_A_VOLT,           	MUX_7,      17U,    		MUX_NONE,   0U)  	\
	X(PAR_RSD_PS4_B_CURR,           	MUX_NONE,   0U,     		MUX_8,      16U) 	\
	X(PAR_RSD_PS4_B_VOLT,           	MUX_NONE,   0U,     		MUX_8,      17U) 	\
	X(PAR_RSD_PS5_CURR,             	MUX_7,      19U,    		MUX_8,      19U) 	\
	X(PAR_RSD_PS5_VOLT,             	MUX_7,      20U,    		MUX_8,      20U) 	\
	X(PAR_RSD_PS6_CURR,             	MUX_7,      22U,    		MUX_8,      22U) 	\
	X(PAR_RSD_PS6_VOLT,             	MUX_7,      23U,    		MUX_8,      23U) 	\
	X(PAR_RSD_PS7_CURR,             	MUX_7,      25U,    		MUX_8,      25U) 	\
	X(PAR_RSD_PS7_VOLT,             	MUX_7,      26U,    		MUX_8,      26U) 	\
	X(PAR_HT_CURR_01,               	MUX_5,      1U,     		MUX_6,      1U)  	\
	X(PAR_HT_CURR_02,               	MUX_5,      2U,     		MUX_6, 		2U)  	\
	X(PAR_HT_CURR_03, 					MUX_5,      3U,     		MUX_6, 		3U)  	\
	X(PAR_HT_CURR_04, 					MUX_5,      4U,     		MUX_6, 		4U)  	\
	X(PAR_HT_CURR_05, 					MUX_5,      5U,     		MUX_6, 		5U)  	\
	X(PAR_HT_CURR_06, 					MUX_5,      6U,     		MUX_6, 		6U)  	\
	X(PAR_HT_CURR_07, 					MUX_5,      7U,     		MUX_6, 		7U)  	\
	X(PAR_HT_CURR_08, 					MUX_5,      8U,     		MUX_6, 		8U)  	\
	X(PAR_HT_CURR_09, 					MUX_5,      9U,     		MUX_6, 		9U)  	\
	X(PAR_HT_CURR_10, 					MUX_5,      10U,    		MUX_6,		10U) 	\
	X(PAR_HT_CURR_11, 					MUX_5,      11U,    		MUX_6,		11U) 	\
	X(PAR_HT_CURR_12, 					MUX_5, 		12U, 			MUX_6,		12U) 	\
	X(PAR_HT_CURR_13, 					MUX_5, 		13U, 			MUX_6,		13U) 	\
	X(PAR_HT_CURR_14, 					MUX_5, 		14U, 			MUX_6,		14U) 	\
	X(PAR_HT_CURR_15, 					MUX_5, 		15U, 			MUX_6,		15U) 	\
	X(PAR_HT_CURR_16, 					MUX_5, 		16U, 			MUX_6,		16U) 	\
	X(PAR_HT_CURR_17, 					MUX_5, 		17U, 			MUX_6,		17U) 	\
	X(PAR_HT_CURR_18, 					MUX_5, 		18U, 			MUX_6,		18U) 	\
	X(PAR_HT_CURR_19, 					MUX_5, 		19U, 			MUX_6,		19U) 	\
	X(PAR_HT_CURR_20, 					MUX_5, 		20U, 			MUX_6,		20U) 	\
	X(PAR_HT_CURR_21, 					MUX_5, 		21U, 			MUX_6,		21U) 	\
	X(PAR_HT_CURR_22, 					MUX_5, 		22U, 			MUX_6,		22U) 	\
	X(PAR_HT_CURR_23, 					MUX_5, 		23U, 			MUX_6,		23U) 	\
	X(PAR_HT_CURR_24, 					MUX_5, 		24U, 			MUX_6,		24U) 	\
	X(PAR_HT_CURR_25, 					MUX_5, 		25U, 			MUX_6,		25U) 	\
	X(PAR_HT_CURR_26, 					MUX_5, 		26U, 			MUX_6,		26U) 	\
	X(PAR_HT_CURR_27, 					MUX_5, 		27U, 			MUX_6,		27U) 	\
	X(PAR_HT_CURR_28, 					MUX_5, 		28U, 			MUX_6,		28U) 	\
	X(PAR_HT_CURR_29, 					MUX_5, 		29U, 			MUX_6,		29U) 	\
	X(PAR_HT_CURR_30, 					MUX_5, 		30U, 			MUX_6,		30U) 	\
	X(PAR_HT_CURR_31, 					MUX_5, 		31U, 			MUX_6,		31U) 	\
	X(PAR_HT_CURR_32, 					MUX_5, 		32U, 			MUX_6,		32U) 	\
	X(PAR_RESERVED_149,               	MUX_NONE,   0U,     		MUX_NONE,   0U)  	\
	X(PAR_RESERVED_150,               	MUX_NONE,   0U,     		MUX_NONE,   0U)  	\
	X(PAR_TS_CURR_01,               	MUX_1,      2U,     		MUX_2,      2U)  	\
	X(PAR_TS_VOLT_01,               	MUX_NONE,   0U,     		MUX_NONE,   0U)  	\
	X(PAR_TS_CURR_02,               	MUX_1,      4U,     		MUX_2,      4U)  	\
	X(PAR_TS_VOLT_02,               	MUX_NONE,   0U,     		MUX_NONE,   0U)  	\
	X(PAR_TS_CURR_03,               	MUX_1,      6U,     		MUX_2,      6U)  	\
	X(PAR_TS_VOLT_03,               	MUX_NONE,   0U,     		MUX_NONE,   0U)  	\
	X(PAR_TS_CURR_04,               	MUX_1,      8U,     		MUX_2,      8U)  	\
	X(PAR_TS_VOLT_04,               	MUX_NONE,   0U,     		MUX_NONE,   0U)  	\
	X(PAR_TS_CURR_05,               	MUX_1,      10U,    		MUX_2,      10U) 	\
	X(PAR_TS_VOLT_05,               	MUX_NONE,   0U,     		MUX_NONE,   0U)  	\
	X(PAR_TS_CURR_06,               	MUX_1,      12U,    		MUX_2,      12U) 	\
	X(PAR_TS_VOLT_06,               	MUX_NONE,   0U,     		MUX_NONE,   0U)  	\
	X(PAR_TS_CURR_07,               	MUX_1,      14U,    		MUX_2,      14U) 	\
	X(PAR_TS_VOLT_07,               	MUX_NONE,   0U,     		MUX_NONE,   0U)  	\
	X(PAR_TS_CURR_08,               	MUX_1,      16U,    		MUX_2,      16U) 	\
	X(PAR_TS_VOLT_08,               	MUX_NONE,   0U,     		MUX_NONE,   0U)  	\
	X(PAR_TS_CURR_09,               	MUX_1,      18U,    		MUX_2,      18U) 	\
	X(PAR_TS_VOLT_09,               	MUX_NONE,   0U,     		MUX_NONE,   0U)  	\
	X(PAR_TS_CURR_10,               	MUX_1,      20U,    		MUX_2,      20U) 	\
	X(PAR_TS_VOLT_10,               	MUX_NONE,   0U,     		MUX_NONE,   0U)  	\
	X(PAR_TS_CURR_11,               	MUX_1,      22U,    		MUX_2,      22U) 	\
	X(PAR_TS_VOLT_11,               	MUX_NONE,   0U,     		MUX_NONE,   0U)  	\
	X(PAR_TS_CURR_12,               	MUX_1,      24U,    		MUX_2,      24U) 	\
	X(PAR_TS_VOLT_12,               	MUX_NONE,   0U,     		MUX_NONE,   0U)  	\
	X(PAR_TS_CURR_13,               	MUX_1,      26U,    		MUX_2,      26U) 	\
	X(PAR_TS_VOLT_13,               	MUX_NONE,   0U,     		MUX_NONE,   0U)  	\
	X(PAR_TS_CURR_14,               	MUX_1,      28U,    		MUX_2,      28U) 	\
	X(PAR_TS_VOLT_14,               	MUX_NONE,   0U,     		MUX_NONE,   0U)  	\
	X(PAR_TS_CURR_15,               	MUX_1,      30U,    		MUX_2,      30U) 	\
	X(PAR_TS_VOLT_15,               	MUX_NONE,   0U,     		MUX_NONE,   0U)  	\
	X(PAR_TS_CURR_16,               	MUX_1,      32U,    		MUX_2,      32U) 	\
	X(PAR_TS_VOLT_16,               	MUX_NONE,   0U,     		MUX_NONE,   0U)  	\
	X(PAR_TS_CURR_17,               	MUX_3,      2U,     		MUX_4,      2U)  	\
	X(PAR_TS_VOLT_17,               	MUX_NONE,   0U,     		MUX_NONE,   0U)  	\
	X(PAR_TS_CURR_18,               	MUX_3,      4U,     		MUX_4,      4U)  	\
	X(PAR_TS_VOLT_18,               	MUX_NONE,   0U,     		MUX_NONE,   0U)  	\
	X(PAR_TS_CURR_19,               	MUX_3,      6U,     		MUX_4,      6U)  	\
	X(PAR_TS_VOLT_19,               	MUX_NONE,   0U,     		MUX_NONE,   0U)  	\
	X(PAR_TS_CURR_20,               	MUX_3,      8U,     		MUX_4,      8U)  	\
	X(PAR_TS_VOLT_20,               	MUX_NONE,   0U,     		MUX_NONE,   0U)  	\
	X(PAR_TS_CURR_21,               	MUX_3,      10U,    		MUX_4,      10U) 	\
	X(PAR_TS_VOLT_21,               	MUX_NONE,   0U,     		MUX_NONE,   0U)  	\
	X(PAR_TS_CURR_22,               	MUX_3,      12U,    		MUX_4,      12U) 	\
	X(PAR_TS_VOLT_22,               	MUX_NONE,   0U,     		MUX_NONE,   0U)  	\
	X(PAR_TS_CURR_23,               	MUX_3,      14U,    		MUX_4,      14U) 	\
	X(PAR_TS_VOLT_23,               	MUX_NONE,   0U,     		MUX_NONE,   0U)  	\
	X(PAR_TS_CURR_24,               	MUX_3,      16U,    		MUX_4,      16U) 	\
	X(PAR_TS_VOLT_24,               	MUX_NONE,   0U,     		MUX_NONE,   0U)  	\
	X(PAR_TS_CURR_25,               	MUX_3,      18U,    		MUX_4,      18U) 	\
	X(PAR_TS_VOLT_25,               	MUX_NONE,   0U,     		MUX_NONE,   0U)  	\
	X(PAR_TS_CURR_26,               	MUX_3,      20U,    		MUX_4,      20U) 	\
	X(PAR_TS_VOLT_26,               	MUX_NONE,   0U,     		MUX_NONE,   0U)  	\
	X(PAR_TS_CURR_27,               	MUX_3,      22U,    		MUX_4,      22U) 	\
	X(PAR_TS_VOLT_27,               	MUX_NONE,   0U,     		MUX_NONE,   0U)  	\
	X(PAR_TS_CURR_28,               	MUX_3,      24U,    		MUX_4,      24U) 	\
	X(PAR_TS_VOLT_28,               	MUX_NONE,   0U,     		MUX_NONE,   0U)  	\
	X(PAR_TS_CURR_29,               	MUX_3,      26U,    		MUX_4,      26U) 	\
	X(PAR_TS_VOLT_29,               	MUX_NONE,   0U,     		MUX_NONE,   0U)  	\
	X(PAR_TS_CURR_30,               	MUX_3,      28U,    		MUX_4,      28U) 	\
	X(PAR_TS_VOLT_30,               	MUX_NONE,   0U,     		MUX_NONE,   0U)  	\
	X(PAR_TS_CURR_31,               	MUX_3,      30U,    		MUX_4,      30U) 	\
	X(PAR_TS_VOLT_31,               	MUX_NONE,   0U,     		MUX_NONE,   0U)  	\
	X(PAR_TS_CURR_32,               	MUX_3,      32U,    		MUX_4,      32U) 	\
	X(PAR_TS_VOLT_32,               	MUX_NONE,   0U,     		MUX_NONE,   0U)  	\
	X(PAR_RCE_AN_PWR_CURR,          	MUX_9,      17U,    		MUX_10,     17U) 	\
	X(PAR_RCE_10VAN_PWR_CURR,       	MUX_9,      18U,    		MUX_10, 	18U) 	\
	X(PAR_RCE_3V3DC_CURR,           	MUX_9,      19U, 			MUX_10, 	19U) 	\
	X(PAR_RCE_1V8DC_CURR,           	MUX_9,      20U, 			MUX_10, 	20U) 	\
	X(PAR_RCE_HVDC_CURR,            	MUX_9,      21U, 			MUX_10, 	21U) 	\
	X(PAR_RCE_MHVDC_CURR,           	MUX_9,      22U, 			MUX_10, 	22U) 	\
	X(PAR_RCE_MVDC_CURR,            	MUX_9,      23U, 			MUX_10, 	23U) 	\
	X(PAR_RCE_LMVDC_CURR,           	MUX_9,      24U, 			MUX_10, 	24U) 	\
	X(PAR_RCE_LVDC_CURR,            	MUX_9,      25U, 			MUX_10, 	25U) 	\
	X(PAR_RCE_AN_ADC,               	MUX_9,      26U, 			MUX_10, 	26U) 	\
	X(PAR_RCE_10VAN_ADC,            	MUX_9,      27U, 			MUX_10, 	27U) 	\
	X(PAR_RCE_LVDC_ADC,             	MUX_9,      28U, 			MUX_10, 	28U) 	\
	X(PAR_RCE_LMVDC_ADC,            	MUX_9,      29U, 			MUX_10, 	29U) 	\
	X(PAR_RCE_MVDC_ADC,             	MUX_9,      30U, 			MUX_10, 	30U) 	\
	X(PAR_RCE_MHVDC_ADC,            	MUX_9,      31U, 			MUX_10, 	31U) 	\
	X(PAR_RCE_HVDC_ADC,             	MUX_9,      32U, 			MUX_10, 	32U) 	\
    X(PAR_EPR_PS2_LOW_EMERGENCY_THR,    MUX_NONE, 	0U, 			MUX_NONE, 	0U) 	\
	X(PAR_EPR_V2_MAX_OPENTIME_TIMEOUT,  MUX_NONE, 	0U, 			MUX_NONE, 	0U) 	\
	X(PAR_EPR_PS2_OUT_SETPOINT,         MUX_NONE, 	0U, 			MUX_NONE, 	0U) 	\
	X(PAR_EPR_PS2_OUT_LOW_THR,          MUX_NONE, 	0U, 			MUX_NONE, 	0U) 	\
	X(PAR_EPR_PS2_OUT_HIG_THR,          MUX_NONE, 	0U, 			MUX_NONE, 	0U) 	\
	X(PAR_FDIR_CFG_BITMAP,              MUX_NONE, 	0U, 			MUX_NONE, 	0U) 	\
	X(PAR_FDIR_LEAK_THR,                MUX_NONE, 	0U, 			MUX_NONE, 	0U) 	\
	X(PAR_FDIR_LEAK_TIMEOUT,            MUX_NONE, 	0U, 			MUX_NONE, 	0U) 	\
	X(PAR_FDIR_GATE_PS,                 MUX_NONE, 	0U, 			MUX_NONE, 	0U) 	\
	X(PAR_FDIR_GATE_TIMEOUT,            MUX_NONE, 	0U, 			MUX_NONE, 	0U) 	\
	X(PAR_FDIR_ICING_PS_THR,            MUX_NONE, 	0U, 			MUX_NONE, 	0U) 	\
	X(PAR_FDIR_ICING_PS_TIMEOUT,        MUX_NONE, 	0U, 			MUX_NONE, 	0U) 	\
	X(PAR_FDIR_ICING_PS_DELTA,          MUX_NONE, 	0U, 			MUX_NONE, 	0U) 	\
	X(PAR_FDIR_OVP_PS_LOW_THR,          MUX_NONE, 	0U, 			MUX_NONE, 	0U) 	\
	X(PAR_FDIR_OVP_PS_HIG_THR,          MUX_NONE, 	0U, 			MUX_NONE, 	0U) 	\
	X(PAR_FDIR_EPR_PS_DELTA_THR,        MUX_NONE, 	0U, 			MUX_NONE, 	0U) 	\
	X(PAR_FDIR_EPR_EPR_PS_2_SETPOINT,   MUX_NONE, 	0U, 			MUX_NONE, 	0U) 	\
	X(PAR_FDIR_EPR_PS_2_SETPOINT,       MUX_NONE, 	0U, 			MUX_NONE, 	0U) 	\
	X(PAR_FDIR_EPR_PS_3_SETPOINT,       MUX_NONE, 	0U, 			MUX_NONE, 	0U) 	\
	X(PAR_FDIR_VENTING_PS_THR,          MUX_NONE, 	0U, 			MUX_NONE, 	0U) 	\
	X(PAR_FDIR_REFCOMPLETED_PS_THR,     MUX_NONE, 	0U, 			MUX_NONE, 	0U)

static const RSD_analog_parameter analogParameters[] = {
#define X(sig, mux_n, analog_input_ch, mux_r, analog_r_input_ch) \
    [sig] = {sig, multiplexers[mux_n], analog_input_ch, multiplexers[mux_r], analog_r_input_ch},
    ANALOG_SIGNAL_LIST
#undef X
};

typedef struct
{
	int key;
	uint8_t value;
} hashmap;

hashmap analog_table[PAR_DEF_NUMBER] = {0,0};

/* PUBLIC FUNCTIONS */
void initAnalogTable(void)
{
	RSD_analog_parameter RSD_p;

	for(int i = 0; i < PAR_DEF_NUMBER; i++)
	{
		RSD_p = analogParameters[i];

		if(RSD_p.MUX_n.MUX_id != MUX_NONE)
		{
#ifdef DEVICE_NOMINAL
			if(RSD_p.MUX_n.ADC.ADC_id == 0U) /* ADC_A */
#else
			if(RSD_p.MUX_n.ADC.ADC_id == 1U) /* ADC_B */
#endif
			{
				analog_table[i].key = RSD_p.MUX_n.ADC_channel << 5U;
				analog_table[i].key |= ( RSD_p.MUX_n_analogInputChannel - 1U );

			} else if(RSD_p.MUX_r.MUX_id != MUX_NONE)
			{
				analog_table[i].key = RSD_p.MUX_r.ADC_channel << 5U;
				analog_table[i].key |= ( RSD_p.MUX_r_analogInputChannel - 1U );

			} else
			{
				analog_table[i].key = -1;
			}

		} else if(RSD_p.MUX_r.MUX_id != MUX_NONE)

		{
#ifdef DEVICE_NOMINAL
			if(RSD_p.MUX_r.ADC.ADC_id == 0U) /* ADC_A */
#else
			if(RSD_p.MUX_r.ADC.ADC_id == 1U) /* ADC_B */
#endif

			{
				analog_table[i].key = RSD_p.MUX_r.ADC_channel << 5U;
				analog_table[i].key |= ( RSD_p.MUX_r_analogInputChannel - 1U );

			} else
			{
				analog_table[i].key = -1;
			}

		} else

		{
			analog_table[i].key = -1;
		}

		analog_table[i].value = i;
	}

	bubbleSortTable();
}

void bubbleSortTable(void)
{
	for(int i = 0; i < ( PAR_DEF_NUMBER - 1); i++)
	{
		for(int j = 0; j < PAR_DEF_NUMBER; j++)
		{
			if(analog_table[j].key > analog_table[j+1].key)
			{
				hashmap temp = analog_table[j];
				analog_table[j] = analog_table[j+1];
				analog_table[j+1] = temp;
			}
		}
	}
}

/**
 * @retval map index for found key
 */

int binarySearch(uint8_t size, int key)
{
	int low = 0;
	int high = size - 1;

	while(low <= high)
	{
		int mid = low + (high - low) / 2;

		if(analog_table[mid].key == key)
		{
			return analog_table[mid].value;
		}

		if(analog_table[mid].key < key)
		{
			low = mid + 1;
		}

		if(analog_table[mid].key > key)
		{
			high = mid - 1;
		}
	}

	return -1;
}

hashmap* getMap(void)
{
	return analog_table;
}

