#ifndef __RSD_CIF_H__
#define __RSD_CIF_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/*********************************************************
* SPI
**********************************************************/

/**
 * @def SPI_N_CS_PIN
 * @brief Nominal SPI Chip Select pin
 * @details Defines the microcontroller pin (PA7) used as the Chip Select signal
 * for the nominal SPI interface.
 */
#define SPI_N_CS_PIN                    (7U)

/**
 * @def SPI_R_CS_PIN
 * @brief Redundant SPI Chip Select pin
 * @details Defines the microcontroller pin (PA24) used as the Chip Select signal
 * for the redundant SPI interface.
 */
#define SPI_R_CS_PIN                    (24U)

/*********************************************************
* MUX
**********************************************************/
/**
 * @def MUX_PORT_N
 * @brief Nominal MUX port where address pins reside
 * @details Defines the microcontroller port (Port B) that connects to the
 * multiplexer address pins in the nominal configuration.
 */
#define MUX_PORT_N                        (1U)


/**
 * @def MUX_PORT_N_ADDRESS_MASK
 * @brief Address pin mask for nominal MUX port
 * @details Defines the bit mask (0x1F000000) for pins PB24 to PB28 on the nominal
 * multiplexer port.
 */
#define MUX_PORT_N_ADDRESS_MASK           (0x1F000000U)


/**
 * @def MUX_PORT_N_NWR
 * @brief Not-Write pin for nominal MUX port
 * @details Defines the microcontroller pin (PB29) used as the Not-Write control
 * signal for the nominal multiplexer.
 */
#define MUX_PORT_N_NWR                    (29U)


/**
 * @def MUX_PORT_R
 * @brief Redundant MUX port where address pins reside
 * @details Defines the microcontroller port (Port A) that connects to the
 * multiplexer address pins in the redundant configuration.
 */
#define MUX_PORT_R                        (0U)


/**
 * @def MUX_PORT_R_ADDRESS_MASK
 * @brief Address pin mask for redundant MUX port
 * @details Defines the bit mask (0x3E00) for pins PA9 to PA13 on the redundant
 * multiplexer port.
 */
#define MUX_PORT_R_ADDRESS_MASK           (0x3E00U)

/**
 * @def MUX_PORT_R_NWR
 * @brief Not-Write pin for redundant MUX port
 * @details Defines the microcontroller pin (PA21) used as the Not-Write control
 * signal for the redundant multiplexer.
 */
#define MUX_PORT_R_NWR                    (21U)

/*********************************************************
* PUBLIC STRUCTURED TYPES
**********************************************************/
/**
 * @enum DT_PARAM_LIST
 * @brief Enumeration of all controlled and monitored parameters
 * @details This enum identifies all the controlled and monitored parameters 
 * managed by RCE ASW. Each parameter has a unique identifier that allows the 
 * system to reference specific parameters for monitoring, control, and fault 
 * detection purposes. The parameters are organized by type and include both 
 * nominal and redundant variants.
 */
typedef enum
{
  /* TANK CFG PARAMETERS*/
  PAR_INITIAL_MASS_OF_WATER = 0U,        /*!< Initial mass of water */
  PAR_INITIAL_VOL_OF_WATER = 1U,         /*!< Initial volume of water */
  PAR_BLADDER_TANK_TOT_VOL = 2U,         /*!< Bladder tank total volume */
  PAR_PRESSURANT_TANK_VOL = 3U,          /*!< Pressurant tank volume */
  PAR_P_N2_BoR = 4U,                     /*!< Pressurant tank initial pressure (P_N2_BoR) */
  PAR_T_N2_BoR = 5U,                     /*!< Pressurant tank initial temperature (T_N2_BoR)*/
  PAR_P_H2O_BoR_PS2 = 6U,                /*!< Water tank initial pressure(P_H2O_BoR) retrieved from RSD_PS2*/
  PAR_P_H2O_BoR_PS3 = 7U,                /*!< Water tank initial pressure(P_H2O_BoR) retrieved from RSD_PS3*/
  PAR_T_H2O_BoR_T09 = 8U,                /*!< Water tank initial temperature (T_H2O_BoR)*/
  PAR_T_H2O_BoR_T10 = 9U,                /*!< Water tank initial temperature (T_H2O_BoR)*/
  PAR_T_H2O_BoR_T11 = 10U,               /*!< Water tank initial temperature (T_H2O_BoR)*/
  PAR_T_H2O_BoR_T12 = 11U,               /*!< Water tank initial temperature (T_H2O_BoR)*/
  PAR_REMAINING_WATER = 12U,              /*!< Calculated from pressure and temperature sensors*/
  PAR_REMAINING_PRESS = 13U,              /*!< Calculated from pressure and temperature sensors*/
  PAR_TRANSFERR_WATER = 14U,              /*!< Transferred water calculated*/
  
  /* Solenoid Valves PARAMETERS */
  PAR_MV_A_CURR = 15U,                   /*!< MV Input current to the valve Nominal */  
  PAR_MV_A_VOLT = 16U,                   /*!< MV Input voltage to the valve Nominal */  
  PAR_MV_B_CURR = 17U,                   /*!< MV Input Redun to the valve Nominal */  
  PAR_MV_B_VOLT = 18U,                   /*!< MV Input Redun voltage to the valve Nominal */  
  PAR_IV_A_CURR = 19U,                   /*!< IV Input current to the valve Nominal */  
  PAR_IV_A_VOLT = 20U,                   /*!< IV Input voltage to the valve Nominal */  
  PAR_IV_B_CURR = 21U,                   /*!< IV Input Redun current to the valve Nominal */  
  PAR_IV_B_VOLT = 22U,                   /*!< IV Input Redun voltage to the valve Nominal */  
  PAR_VV1_A_CURR = 23U,                  /*!< VV1 Input current to the valve Nominal */  
  PAR_VV1_A_VOLT = 24U,                  /*!< VV1 Input voltage to the valve Nominal */  
  PAR_VV1_B_CURR = 25U,                  /*!< VV1 Input Redun current to the valve Nominal */  
  PAR_VV1_B_VOLT = 26U,                  /*!< VV1 Input Redun voltage to the valve Nominal */  
  PAR_VV2_A_CURR = 27U,                  /*!< VV2 Input current to the valve Nominal */  
  PAR_VV2_A_VOLT = 28U,                  /*!< VV2 Input voltage to the valve Nominal */  
  PAR_VV2_B_CURR = 29U,                  /*!< VV2 Input Redun current to the valve Nominal */  
  PAR_VV2_B_VOLT = 30U,                  /*!< VV2 Input Redun voltage to the valve Nominal */  
  PAR_VV3_A_CURR = 31U,                  /*!< VV3 Input current to the valve Nominal */  
  PAR_VV3_A_VOLT = 32U,                  /*!< VV3 Input voltage to the valve Nominal */  
  PAR_VV3_B_CURR = 33U,                  /*!< VV3 Input Redun current to the valve Nominal */  
  PAR_VV3_B_VOLT = 34U,                  /*!< VV3 Input Redun voltage to the valve Nominal */  
  PAR_VV4_CURR = 35U,                    /*!< VV4 Input current to the valve Nominal */  
  PAR_VV4_VOLT = 36U,                    /*!< VV4 Input voltage to the valve Nominal */  
  
  /* EPR VALVES */
  PAR_EPR_1A_CURR = 37U,                 /*!< EPR Input Nominal current to the valve Nominal */  
  PAR_EPR_1A_VOLT = 38U,                 /*!< EPR Input Nominal voltage to the valve Nominal */  
  PAR_EPR_1B_CURR = 39U,                 /*!< EPR Input Redun current to the valve Nominal */  
  PAR_EPR_1B_VOLT = 40U,                 /*!< EPR Input Redun voltage to the valve Nominal */  
  PAR_EPR_2A_CURR = 41U,                 /*!< EPR Input Nominal current to the valve Nominal */  
  PAR_EPR_2A_VOLT = 42U,                 /*!< EPR Input Nominal voltage to the valve Nominal */  
  PAR_EPR_2B_CURR = 43U,                 /*!< EPR Input Redun current to the valve Nominal */  
  PAR_EPR_2B_VOLT = 44U,                 /*!< EPR Input Redun voltage to the valve Nominal */  
  PAR_EPR_3A_CURR = 45U,                 /*!< EPR Input Nominal current to the valve Nominal */  
  PAR_EPR_3A_VOLT = 46U,                 /*!< EPR Input Nominal voltage to the valve Nominal */  
  PAR_EPR_3B_CURR = 47U,                 /*!< EPR Input Redun current to the valve Nominal */  
  PAR_EPR_3B_VOLT = 48U,                 /*!< EPR Input Redun voltage to the valve Nominal */  
  
  /* EPR PRESSURE SENSOR */
  PAR_EPR_A_PS1 = 49U,                   /*!< The EPR A Pressure sensor 1 Pressure range [0; 85] bar. The PS_1 is the pressure sensor facing the High-Pressure segment*/
  PAR_EPR_B_PS1 = 50U,                   /*!< The EPR B Pressure sensor 1 Pressure range [0; 85] bar. The PS_1 is the pressure sensor facing the High-Pressure segment*/
  PAR_EPR_A_PS2 = 51U,                   /*!< The EPR A Pressure sensor 2 Pressure range [0; 20] bar. The PS_2 is the pressure sensor facing the Low-Pressure segment*/
  PAR_EPR_B_PS2 = 52U,                   /*!< The EPR B Pressure sensor 2 Pressure range [0; 20] bar. The PS_2 is the pressure sensor facing the Low-Pressure segment*/
  PAR_EPR_A_PS1_CURR = 53U,              /*!< The EPR A Pressure sensor 1 current */
  PAR_EPR_A_PS1_VOLT = 54U,              /*!< The EPR A Pressure sensor 1 voltage */
  PAR_EPR_B_PS1_CURR = 55U,              /*!< The EPR B Pressure sensor 1 current */
  PAR_EPR_B_PS1_VOLT = 56U,              /*!< The EPR B Pressure sensor 1 voltage */
  PAR_EPR_A_PS2_CURR = 57U,              /*!< The EPR A Pressure sensor 2 current */
  PAR_EPR_A_PS2_VOLT = 58U,              /*!< The EPR A Pressure sensor 2 voltage */
  PAR_EPR_B_PS2_CURR = 59U,              /*!< The EPR B Pressure sensor 2 current */
  PAR_EPR_B_PS2_VOLT = 60U,              /*!< The EPR B Pressure sensor 2 voltage */
  
  /* PRESSURE SENSORS */
  PAR_RSD_PS1 = 61U,                     /*!< The RSD_PS_1 Reading. Pressure range [0; 85] bar */
  PAR_RSD_PS2 = 62U,                     /*!< The RSD_PS_2 Reading. Pressure range [0; 20] bar */
  PAR_RSD_PS3 = 63U,                     /*!< The RSD_PS_3 Reading. Pressure range [0; 20] bar */
  PAR_RSD_PS4_A = 64U,                   /*!< The RSD_PS_4 Nominal Reading. Pressure range [0; 20] bar */
  PAR_RSD_PS4_B = 65U,                   /*!< The RSD_PS_4 Redun Reading. Pressure range [0; 20] bar */
  PAR_RSD_PS5 = 66U,                     /*!< The RSD_PS_5 Reading. Pressure range [0; 20] bar */
  PAR_RSD_PS6 = 67U,                     /*!< The RSD_PS_6 Reading. Pressure range [0; 85] bar */
  PAR_RSD_PS7 = 68U,                     /*!< The RSD_PS_7 Reading. Pressure range [0; 20] bar */
  
  /* TEMP SENSORS */
  PAR_TS_TEMP_01 = 69U,                  /*!< Reading from temperature sensor, temperature range [-40; 80] °C */
  PAR_TS_TEMP_02 = 70U,                  /*!< Reading from temperature sensor, temperature range [-40; 80] °C */
  PAR_TS_TEMP_03 = 71U,                  /*!< Reading from temperature sensor, temperature range [-40; 80] °C */
  PAR_TS_TEMP_04 = 72U,                  /*!< Reading from temperature sensor, temperature range [-40; 80] °C */
  PAR_TS_TEMP_05 = 73U,                  /*!< Reading from temperature sensor, temperature range [-40; 80] °C */
  PAR_TS_TEMP_06 = 74U,                  /*!< Reading from temperature sensor, temperature range [-40; 80] °C */
  PAR_TS_TEMP_07 = 75U,                  /*!< Reading from temperature sensor, temperature range [-40; 80] °C */
  PAR_TS_TEMP_08 = 76U,                  /*!< Reading from temperature sensor, temperature range [-40; 80] °C */
  PAR_TS_TEMP_09 = 77U,                  /*!< Reading from temperature sensor, temperature range [-40; 80] °C */
  PAR_TS_TEMP_10 = 78U,                  /*!< Reading from temperature sensor, temperature range [-40; 80] °C */
  PAR_TS_TEMP_11 = 79U,                  /*!< Reading from temperature sensor, temperature range [-40; 80] °C */
  PAR_TS_TEMP_12 = 80U,                  /*!< Reading from temperature sensor, temperature range [-40; 80] °C */
  PAR_TS_TEMP_13 = 81U,                  /*!< Reading from temperature sensor, temperature range [-40; 80] °C */
  PAR_TS_TEMP_14 = 82U,                  /*!< Reading from temperature sensor, temperature range [-40; 80] °C */
  PAR_TS_TEMP_15 = 83U,                  /*!< Reading from temperature sensor, temperature range [-40; 80] °C */
  PAR_TS_TEMP_16 = 84U,                  /*!< Reading from temperature sensor, temperature range [-40; 80] °C */
  PAR_TS_TEMP_17 = 85U,                  /*!< Reading from temperature sensor, temperature range [-40; 80] °C */
  PAR_TS_TEMP_18 = 86U,                  /*!< Reading from temperature sensor, temperature range [-40; 80] °C */
  PAR_TS_TEMP_19 = 87U,                  /*!< Reading from temperature sensor, temperature range [-40; 80] °C */
  PAR_TS_TEMP_20 = 88U,                  /*!< Reading from temperature sensor, temperature range [-40; 80] °C */
  PAR_TS_TEMP_21 = 89U,                  /*!< Reading from temperature sensor, temperature range [-40; 80] °C */
  PAR_TS_TEMP_22 = 90U,                  /*!< Reading from temperature sensor, temperature range [-40; 80] °C */
  PAR_TS_TEMP_23 = 91U,                  /*!< Reading from temperature sensor, temperature range [-40; 80] °C */
  PAR_TS_TEMP_24 = 92U,                  /*!< Reading from temperature sensor, temperature range [-40; 80] °C */
  PAR_TS_TEMP_25 = 93U,                  /*!< Reading from temperature sensor, temperature range [-40; 80] °C */
  PAR_TS_TEMP_26 = 94U,                  /*!< Reading from temperature sensor, temperature range [-40; 80] °C */
  PAR_TS_TEMP_27 = 95U,                  /*!< Reading from temperature sensor, temperature range [-40; 80] °C */
  PAR_TS_TEMP_28 = 96U,                  /*!< Reading from temperature sensor, temperature range [-40; 80] °C */
  PAR_TS_TEMP_29 = 97U,                  /*!< Reading from temperature sensor, temperature range [-40; 80] °C */
  PAR_TS_TEMP_30 = 98U,                  /*!< Reading from temperature sensor, temperature range [-40; 80] °C */
  PAR_TS_TEMP_31 = 99U,                  /*!< Reading from temperature sensor, temperature range [-40; 80] °C */
  PAR_TS_TEMP_32 = 100U,                 /*!< Reading from temperature sensor, temperature range [-40; 80] °C */
  
  /* PRESSURE SENSORS VOLT AND CURRENT */
  PAR_RSD_PS1_CURR = 101U,               /*!< Input current to the pressure sensor */
  PAR_RSD_PS1_VOLT = 102U,               /*!< Input voltage to the pressure sensor */
  PAR_RSD_PS2_CURR = 103U,               /*!< Input current to the pressure sensor */
  PAR_RSD_PS2_VOLT = 104U,               /*!< Input voltage to the pressure sensor */
  PAR_RSD_PS3_CURR = 105U,               /*!< Input current to the pressure sensor */
  PAR_RSD_PS3_VOLT = 106U,               /*!< Input voltage to the pressure sensor */
  PAR_RSD_PS4_A_CURR = 107U,             /*!< Input current to the pressure sensor */
  PAR_RSD_PS4_A_VOLT = 108U,             /*!< Input voltage to the pressure sensor */
  PAR_RSD_PS4_B_CURR = 109U,             /*!< Input current to the pressure sensor */
  PAR_RSD_PS4_B_VOLT = 110U,             /*!< Input voltage to the pressure sensor */
  PAR_RSD_PS5_CURR = 111U,               /*!< Input current to the pressure sensor */
  PAR_RSD_PS5_VOLT = 112U,               /*!< Input voltage to the pressure sensor */
  PAR_RSD_PS6_CURR = 113U,               /*!< Input current to the pressure sensor */
  PAR_RSD_PS6_VOLT = 114U,               /*!< Input voltage to the pressure sensor */
  PAR_RSD_PS7_CURR = 115U,               /*!< Input current to the pressure sensor */
  PAR_RSD_PS7_VOLT = 116U,               /*!< Input voltage to the pressure sensor */
  
  /* HEATERS VOLT AND CURRENT */
  PAR_HT_CURR_01 = 117U,                 /*!< Input current to the heater */
  PAR_HT_CURR_02 = 118U,                 /*!< Input current to the heater */
  PAR_HT_CURR_03 = 119U,                 /*!< Input current to the heater */
  PAR_HT_CURR_04 = 120U,                 /*!< Input current to the heater */
  PAR_HT_CURR_05 = 121U,                 /*!< Input current to the heater */
  PAR_HT_CURR_06 = 122U,                 /*!< Input current to the heater */
  PAR_HT_CURR_07 = 123U,                 /*!< Input current to the heater */
  PAR_HT_CURR_08 = 124U,                 /*!< Input current to the heater */
  PAR_HT_CURR_09 = 125U,                 /*!< Input current to the heater */
  PAR_HT_CURR_10 = 126U,                 /*!< Input current to the heater */
  PAR_HT_CURR_11 = 127U,                 /*!< Input current to the heater */
  PAR_HT_CURR_12 = 128U,                 /*!< Input current to the heater */
  PAR_HT_CURR_13 = 129U,                 /*!< Input current to the heater */
  PAR_HT_CURR_14 = 130U,                 /*!< Input current to the heater */
  PAR_HT_CURR_15 = 131U,                 /*!< Input current to the heater */
  PAR_HT_CURR_16 = 132U,                 /*!< Input current to the heater */
  PAR_HT_CURR_17 = 133U,                 /*!< Input current to the heater */
  PAR_HT_CURR_18 = 134U,                 /*!< Input current to the heater */
  PAR_HT_CURR_19 = 135U,                 /*!< Input current to the heater */
  PAR_HT_CURR_20 = 136U,                 /*!< Input current to the heater */
  PAR_HT_CURR_21 = 137U,                 /*!< Input current to the heater */
  PAR_HT_CURR_22 = 138U,                 /*!< Input current to the heater */
  PAR_HT_CURR_23 = 139U,                 /*!< Input current to the heater */
  PAR_HT_CURR_24 = 140U,                 /*!< Input current to the heater */
  PAR_HT_CURR_25 = 141U,                 /*!< Input current to the heater */
  PAR_HT_CURR_26 = 142U,                 /*!< Input current to the heater */
  PAR_HT_CURR_27 = 143U,                 /*!< Input current to the heater */
  PAR_HT_CURR_28 = 144U,                 /*!< Input current to the heater */
  PAR_HT_CURR_29 = 145U,                 /*!< Input current to the heater */
  PAR_HT_CURR_30 = 146U,                 /*!< Input current to the heater */
  PAR_HT_CURR_31 = 147U,                 /*!< Input current to the heater */
  PAR_HT_CURR_32 = 148U,                 /*!< Input current to the heater */
  
  /*  Filling the gap with reserved values */
  PAR_RESERVED_149 = 149U,               /*!< Reserved */
  PAR_RESERVED_150 = 150U,               /*!< Reserved */
  
    /* Temperature Sensor Current Parameters */
  PAR_TS_CURR_01 = 151U,                 /*!< Input current to the temperature sensor */
  PAR_TS_CURR_02 = 152U,                 /*!< Input current to the temperature sensor */
  PAR_TS_CURR_03 = 153U,                 /*!< Input current to the temperature sensor */
  PAR_TS_CURR_04 = 154U,                 /*!< Input current to the temperature sensor */
  PAR_TS_CURR_05 = 155U,                 /*!< Input current to the temperature sensor */
  PAR_TS_CURR_06 = 156U,                 /*!< Input current to the temperature sensor */
  PAR_TS_CURR_07 = 157U,                 /*!< Input current to the temperature sensor */
  PAR_TS_CURR_08 = 158U,                 /*!< Input current to the temperature sensor */
  PAR_TS_CURR_09 = 159U,                 /*!< Input current to the temperature sensor */
  PAR_TS_CURR_10 = 160U,                 /*!< Input current to the temperature sensor */
  PAR_TS_CURR_11 = 161U,                 /*!< Input current to the temperature sensor */
  PAR_TS_CURR_12 = 162U,                 /*!< Input current to the temperature sensor */
  PAR_TS_CURR_13 = 163U,                 /*!< Input current to the temperature sensor */
  PAR_TS_CURR_14 = 164U,                 /*!< Input current to the temperature sensor */
  PAR_TS_CURR_15 = 165U,                 /*!< Input current to the temperature sensor */
  PAR_TS_CURR_16 = 166U,                 /*!< Input current to the temperature sensor */
  PAR_TS_CURR_17 = 167U,                 /*!< Input current to the temperature sensor */
  PAR_TS_CURR_18 = 168U,                 /*!< Input current to the temperature sensor */
  PAR_TS_CURR_19 = 169U,                 /*!< Input current to the temperature sensor */
  PAR_TS_CURR_20 = 170U,                 /*!< Input current to the temperature sensor */
  PAR_TS_CURR_21 = 171U,                 /*!< Input current to the temperature sensor */
  PAR_TS_CURR_22 = 172U,                 /*!< Input current to the temperature sensor */
  PAR_TS_CURR_23 = 173U,                 /*!< Input current to the temperature sensor */
  PAR_TS_CURR_24 = 174U,                 /*!< Input current to the temperature sensor */
  PAR_TS_CURR_25 = 175U,                 /*!< Input current to the temperature sensor */
  PAR_TS_CURR_26 = 176U,                 /*!< Input current to the temperature sensor */
  PAR_TS_CURR_27 = 177U,                 /*!< Input current to the temperature sensor */
  PAR_TS_CURR_28 = 178U,                 /*!< Input current to the temperature sensor */
  PAR_TS_CURR_29 = 179U,                 /*!< Input current to the temperature sensor */
  PAR_TS_CURR_30 = 180U,                 /*!< Input current to the temperature sensor */
  PAR_TS_CURR_31 = 181U,                 /*!< Input current to the temperature sensor */
  PAR_TS_CURR_32 = 182U,                 /*!< Input current to the temperature sensor */

  /* Temperature Sensor Voltage Parameters */
  PAR_TS_VOLT_01 = 183U,                 /*!< Input voltage to the temperature sensor */
  PAR_TS_VOLT_02 = 184U,                 /*!< Input voltage to the temperature sensor */
  PAR_TS_VOLT_03 = 185U,                 /*!< Input voltage to the temperature sensor */
  PAR_TS_VOLT_04 = 186U,                 /*!< Input voltage to the temperature sensor */
  PAR_TS_VOLT_05 = 187U,                 /*!< Input voltage to the temperature sensor */
  PAR_TS_VOLT_06 = 188U,                 /*!< Input voltage to the temperature sensor */
  PAR_TS_VOLT_07 = 189U,                 /*!< Input voltage to the temperature sensor */
  PAR_TS_VOLT_08 = 190U,                 /*!< Input voltage to the temperature sensor */
  PAR_TS_VOLT_09 = 191U,                 /*!< Input voltage to the temperature sensor */
  PAR_TS_VOLT_10 = 192U,                 /*!< Input voltage to the temperature sensor */
  PAR_TS_VOLT_11 = 193U,                 /*!< Input voltage to the temperature sensor */
  PAR_TS_VOLT_12 = 194U,                 /*!< Input voltage to the temperature sensor */
  PAR_TS_VOLT_13 = 195U,                 /*!< Input voltage to the temperature sensor */
  PAR_TS_VOLT_14 = 196U,                 /*!< Input voltage to the temperature sensor */
  PAR_TS_VOLT_15 = 197U,                 /*!< Input voltage to the temperature sensor */
  PAR_TS_VOLT_16 = 198U,                 /*!< Input voltage to the temperature sensor */
  PAR_TS_VOLT_17 = 199U,                 /*!< Input voltage to the temperature sensor */
  PAR_TS_VOLT_18 = 200U,                 /*!< Input voltage to the temperature sensor */
  PAR_TS_VOLT_19 = 201U,                 /*!< Input voltage to the temperature sensor */
  PAR_TS_VOLT_20 = 202U,                 /*!< Input voltage to the temperature sensor */
  PAR_TS_VOLT_21 = 203U,                 /*!< Input voltage to the temperature sensor */
  PAR_TS_VOLT_22 = 204U,                 /*!< Input voltage to the temperature sensor */
  PAR_TS_VOLT_23 = 205U,                 /*!< Input voltage to the temperature sensor */
  PAR_TS_VOLT_24 = 206U,                 /*!< Input voltage to the temperature sensor */
  PAR_TS_VOLT_25 = 207U,                 /*!< Input voltage to the temperature sensor */
  PAR_TS_VOLT_26 = 208U,                 /*!< Input voltage to the temperature sensor */
  PAR_TS_VOLT_27 = 209U,                 /*!< Input voltage to the temperature sensor */
  PAR_TS_VOLT_28 = 210U,                 /*!< Input voltage to the temperature sensor */
  PAR_TS_VOLT_29 = 211U,                 /*!< Input voltage to the temperature sensor */
  PAR_TS_VOLT_30 = 212U,                 /*!< Input voltage to the temperature sensor */
  PAR_TS_VOLT_31 = 213U,                 /*!< Input voltage to the temperature sensor */
  PAR_TS_VOLT_32 = 214U,                 /*!< Input voltage to the temperature sensor */
  
  /* RCE HK */
  PAR_RCE_AN_PWR_CURR = 215U,            /*!< RCE Board current 01 - AN_PWR_CURR */
  PAR_RCE_10VAN_PWR_CURR = 216U,         /*!< RCE Board voltage 01 */
  PAR_RCE_3V3DC_CURR = 217U,             /*!< RCE Board DCDC current */
  PAR_RCE_1V8DC_CURR = 218U,             /*!< RCE Board DCDC voltage */
  PAR_RCE_HVDC_CURR = 219U,              /*!< RCE Board temperature 01 */
  PAR_RCE_MHVDC_CURR = 220U,             /*!< RCE Board spare parameter 01 */
  PAR_RCE_MVDC_CURR = 221U,              /*!< RCE Board spare parameter 02 */
  PAR_RCE_LMVDC_CURR = 222U,             /*!< RCE Board spare parameter 03 */
  PAR_RCE_LVDC_CURR = 223U,              /*!< RCE Board spare parameter 04 */
  PAR_RCE_AN_ADC = 224U,                 /*!< RCE Board spare parameter 05 */
  PAR_RCE_10VAN_ADC = 225U,              /*!< RCE Board spare parameter 06 */
  PAR_RCE_LVDC_ADC = 226U,               /*!< RCE Board LVDC ADC */
  PAR_RCE_LMVDC_ADC = 227U,              /*!< RCE Board LMVDC ADC */
  PAR_RCE_MVDC_ADC = 228U,               /*!< RCE Board MVDC ADC */
  PAR_RCE_MHVDC_ADC = 229U,              /*!< RCE Board MHVDC ADC */
  PAR_RCE_HVDC_ADC = 230U,               /*!< RCE Board HVDC ADC */
  PAR_RCE_TEMP = 231U,                   /*!< RCE Board Temperature */
          
  /*  EPR and FDIR Configuration Parameters */
  PAR_EPR_PS2_LOW_EMERGENCY_THR = 232U,  /*!< EPR PS2 low emergency threshold */
  PAR_EPR_V2_MAX_OPENTIME_TIMEOUT = 233U,/*!< EPR V2 maximum open time timeout */          
  PAR_EPR_PS2_OUT_SETPOINT = 234U,       /*!< EPR PS2 output setpoint */
  PAR_EPR_PS2_OUT_LOW_THR = 235U,        /*!< EPR PS2 output low threshold */
  PAR_EPR_PS2_OUT_HIG_THR = 236U,        /*!< EPR PS2 output high threshold */
  PAR_FDIR_CFG_BITMAP = 237U,            /*!< Enable/Disable Fault Detection and Isolation Recovery bitmap */  
  PAR_FDIR_LEAK_THR = 238U,              /*!< FDIR procedure - Pressure decreasing rate threshold (default value is 1 bar/s) */  
  PAR_FDIR_LEAK_TIMEOUT = 239U,          /*!< FDIR procedure - Pressure decreasing timeout (default value is 5 seconds) */  
  PAR_FDIR_GATE_PS = 240U,               /*!< FDIR procedure - Increasing pressure threshold (default value is 0.5 bar) */
  PAR_FDIR_GATE_TIMEOUT = 241U,          /*!< FDIR procedure - Increasing pressure timeout (default value is 1 minute) */  
  PAR_FDIR_ICING_PS_THR = 242U,          /*!< FDIR procedure - Icing pressure decreasing threshold (default value is 1 bar) */  
  PAR_FDIR_ICING_PS_TIMEOUT = 243U,      /*!< FDIR procedure - Icing pressure decreasing timeout (default value is 1 minute) */  
  PAR_FDIR_ICING_PS_DELTA = 244U,        /*!< FDIR procedure - Icing pressure delta value (default value is 0.2 bar) */  
  PAR_FDIR_OVP_PS_LOW_THR = 245U,        /*!< FDIR procedure - Overpressure detection Low-Pressure segment threshold (default value is 33 bar) */  
  PAR_FDIR_OVP_PS_HIG_THR = 246U,        /*!< FDIR procedure - Overpressure detection High-Pressure segment threshold (default value is 140 bar) */  
  PAR_FDIR_EPR_PS_DELTA_THR = 247U,      /*!< FDIR procedure - EPR pressure delta pressure threshold (default 2 bar) */
  PAR_FDIR_EPR_EPR_PS_2_SETPOINT = 248U, /*!< FDIR procedure - EPR - EPR_PS_2_A setpoint */
  PAR_FDIR_EPR_PS_2_SETPOINT = 249U,     /*!< FDIR procedure - EPR - PS_2 setpoint */
  PAR_FDIR_EPR_PS_3_SETPOINT = 250U,     /*!< FDIR procedure - EPR - PS_3 setpoint */
  PAR_FDIR_VENTING_PS_THR = 251U,        /*!< FDIR procedure - Venting pressure threshold (default value is 0 bar) */
  PAR_FDIR_REFCOMPLETED_PS_THR = 252U,   /*!< Automatic refuelling completion - Refuelling completed threshold (default value is 20 bar) */
  
  /* END */
  PAR_DEF_NUMBER = 253U                  /*!< Total number of defined parameters */

} DT_PARAM_LIST;
/**
 * @struct SPI_if
 * @brief SPI interface configuration structure
 * @details This structure defines the configuration for an SPI interface, including
 * which SPI controller to use (nominal or redundant), which chip select pin to use,
 * and reserved fields for future expansion. This structure is used to configure
 * communication with SPI peripheral devices like ADCs.
 */
typedef struct
{
    uint8_t  SPI_id;                        /*!< 0 = N, 1 = R */
    uint32_t CS_PIN;                        /*!< Chip Select pin identifier */
    uint16_t spare_1;                       /*!< Reserved for future use */
    uint8_t  spare_2;                       /*!< Reserved for future use */
} SPI_if;

/**
 * @struct ADC_n
 * @brief ADC configuration structure
 * @details This structure defines the configuration for an Analog-to-Digital Converter,
 * including which ADC controller to use, which channels are active, and which SPI
 * interface to use for communication. This structure is used to configure the ADCs
 * for acquiring analog sensor data.
 */
typedef struct
{
    uint8_t ADC_id;                         /*!< identifies ADC 0/1 */
    uint8_t ADC_active_ch_mask;             /*!< bitmask of ADC used channels */
    SPI_if ADC_SPI_if;                      /*!< designated SPI */ 
    uint16_t spare_1;                       /*!< Reserved for future use */
} ADC_n;

/**
 * @enum MULTIPLEXER
 * @brief Enumeration of all multiplexers connected to ADCs
 * @details
 */
typedef enum
{
    MUX_NONE = 0,
    MUX_1		,
	MUX_2		,
	MUX_3		,
	MUX_4		,
	MUX_5		,
	MUX_6		,
	MUX_7		,
	MUX_8		,
	MUX_9		,
	MUX_10		,
	/*  END */
	MUX_COUNT
} MULTIPLEXER;

/**
 * @struct MUX_x
 * @brief Multiplexer configuration structure
 * @details This structure defines the configuration for an analog multiplexer,
 * including which multiplexer to use, its type (16 or 32 channels), which PIO port
 * controls it, address pin mask, and which channels are active. This structure is used
 * to configure multiplexers for routing multiple analog signals to a single ADC input.
 */
typedef struct
{
    uint8_t MUX_id;                         /*!< identifies MUX 1-10 */
    uint8_t MUX_type;                       /*!< 32 = 32-channel MUX, 16 = 16-channel MUX */
    ADC_n ADC;                              /*!< identifies ADC */
    uint8_t ADC_channel;                    /*!< ADC channel to which MUX is input: 0-7 */
    uint32_t MUX_port;                      /*!< MUX PIO Port identifier */
    uint32_t MUX_addressMask;               /*!< MUX address pins mask */
    uint32_t MUX_active_ch_mask;            /*!< bitmask of MUX used channels */
    uint8_t spare_1;                       /*!< Reserved for future use */
    
} MUX_x;

/**
 * @struct RSD_analog_parameter
 * @brief Analog parameter acquisition configuration structure
 * @details This structure defines the complete configuration for acquiring an analog
 * parameter, including which parameter it is, which ADC and channel to use, which
 * multiplexer and channel to use, and how to route the signal through the acquisition
 * chain. This structure provides the complete path from physical sensor to digital value.
 */
typedef struct
{
    DT_PARAM_LIST param;                    /*!< identifies parameter */
    MUX_x MUX_n;                            /*!< identifies nominal MUX */
    uint32_t MUX_n_analogInputChannel;      /*!< identifies nominal MUX input channel: 1-32 */
    MUX_x MUX_r;                            /*!< identifies redundant MUX */
    uint32_t MUX_r_analogInputChannel;      /*!< identifies redundant MUX input channel: 1-32 */
} RSD_analog_parameter;

#endif /* __RSD_CIF_H__ */
