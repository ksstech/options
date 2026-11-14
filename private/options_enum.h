// options_enum.h - option numbers, sized at compile time (common/include/options_size.h)
#pragma once

#include "options_size.h"

#ifdef __cplusplus
extern "C" {
#endif
// ######################################### Enumerations ##########################################
/* Each width starts where the previous one ends (ioBx_BASE). Names exist only for options inside the configured
 * count, so an application that names an option outside its sizing fails to compile. With the full set (default)
 * every number is unchanged: ioB1_0 = 0 .. ioB8_7 = 140, ioBXlast = 141, ioS_NWMO 141 .. ioS_IOdef 146. */
enum {
	ioB1_BASE = 0,
	ioB2_BASE = ioB1_BASE + optB1_NUM,
	ioB3_BASE = ioB2_BASE + optB2_NUM,
	ioB4_BASE = ioB3_BASE + optB3_NUM,
	ioB8_BASE = ioB4_BASE + optB4_NUM,
	ioBXlast  = ioB8_BASE + optB8_NUM,				// Non NVS options #######################################
	ioS_NWMO  = ioBXlast,							// {0->3} 				(network mode)
	ioS_WIFI,										// {ssid} {pswd}
	ioS_MQTT,										// {w.x.y.z[:???]}		(mqtt broker/proxy)
	ioS_PEEK,										// {addr} {size}
	ioS_POKE,										// {addr} u8 [u8 ... u8]
	ioS_IOdef,										// 					(reset to defaults)
	ioS_LAST = ioS_IOdef,
};

enum {								// 1-bit options, ioB1_BASE + n
#if (optB1_NUM == 0)
	ioB1_0 = ioB1_BASE,						// start of all options (range/loop start in rules, testbed): kept when 1-bit is empty
#endif
#if (optB1_NUM >= 8)
	ioB1_0 = ioB1_BASE + 0,
	ioB1_1,
	ioB1_2,
	ioB1_3,
	ioB1_4,
	ioB1_5,
	ioB1_6,
	ioB1_7,
#endif
#if (optB1_NUM >= 16)
	ioB1_8 = ioB1_BASE + 8,
	ioB1_9,
	ioB1_10,
	ioB1_11,
	ioB1_12,
	ioB1_13,
	ioB1_14,
	ioB1_15,
#endif
#if (optB1_NUM >= 24)
	ioB1_16 = ioB1_BASE + 16,
	ioB1_17,
	ioB1_18,
	ioB1_19,
	ioB1_20,
	ioB1_21,
	ioB1_22,
	ioB1_23,
#endif
#if (optB1_NUM >= 32)
	ioB1_24 = ioB1_BASE + 24,
	ioB1_25,
	ioB1_26,
	ioB1_27,
	ioB1_28,
	ioB1_29,
	ioB1_30,
	ioB1_31,
#endif
#if (optB1_NUM >= 40)
	ioB1_32 = ioB1_BASE + 32,
	ioB1_33,
	ioB1_34,
	ioB1_35,
	ioB1_36,
	ioB1_37,
	ioB1_38,
	ioB1_39,
#endif
#if (optB1_NUM >= 48)
	ioB1_40 = ioB1_BASE + 40,
	ioB1_41,
	ioB1_42,
	ioB1_43,
	ioB1_44,
	ioB1_45,
	ioB1_46,
	ioB1_47,
#endif
#if (optB1_NUM >= 56)
	ioB1_48 = ioB1_BASE + 48,
	ioB1_49,
	ioB1_50,
	ioB1_51,
	ioB1_52,
	ioB1_53,
	ioB1_54,
	ioB1_55,
#endif
#if (optB1_NUM >= 64)
	ioB1_56 = ioB1_BASE + 56,
	ioB1_57,
	ioB1_58,
	ioB1_59,
	ioB1_60,
	ioB1_61,
	ioB1_62,
	ioB1_63,
#endif
	ioB1_END_ = 0							// keeps the enum non-empty
};

enum {								// 2-bit options, ioB2_BASE + n
#if (optB2_NUM >= 4)
	ioB2_0 = ioB2_BASE + 0,
	ioB2_1,
	ioB2_2,
	ioB2_3,
#endif
#if (optB2_NUM >= 8)
	ioB2_4 = ioB2_BASE + 4,
	ioB2_5,
	ioB2_6,
	ioB2_7,
#endif
#if (optB2_NUM >= 12)
	ioB2_8 = ioB2_BASE + 8,
	ioB2_9,
	ioB2_10,
	ioB2_11,
#endif
#if (optB2_NUM >= 16)
	ioB2_12 = ioB2_BASE + 12,
	ioB2_13,
	ioB2_14,
	ioB2_15,
#endif
#if (optB2_NUM >= 20)
	ioB2_16 = ioB2_BASE + 16,
	ioB2_17,
	ioB2_18,
	ioB2_19,
#endif
#if (optB2_NUM >= 24)
	ioB2_20 = ioB2_BASE + 20,
	ioB2_21,
	ioB2_22,
	ioB2_23,
#endif
#if (optB2_NUM >= 28)
	ioB2_24 = ioB2_BASE + 24,
	ioB2_25,
	ioB2_26,
	ioB2_27,
#endif
#if (optB2_NUM >= 32)
	ioB2_28 = ioB2_BASE + 28,
	ioB2_29,
	ioB2_30,
	ioB2_31,
#endif
	ioB2_END_ = 0							// keeps the enum non-empty
};

enum {								// 3-bit options, ioB3_BASE + n
#if (optB3_NUM >= 2)
	ioB3_0 = ioB3_BASE + 0,
	ioB3_1,
#endif
#if (optB3_NUM >= 5)
	ioB3_2 = ioB3_BASE + 2,
	ioB3_3,
	ioB3_4,
#endif
#if (optB3_NUM >= 8)
	ioB3_5 = ioB3_BASE + 5,
	ioB3_6,
	ioB3_7,
#endif
#if (optB3_NUM >= 10)
	ioB3_8 = ioB3_BASE + 8,
	ioB3_9,
#endif
#if (optB3_NUM >= 13)
	ioB3_10 = ioB3_BASE + 10,
	ioB3_11,
	ioB3_12,
#endif
#if (optB3_NUM >= 16)
	ioB3_13 = ioB3_BASE + 13,
	ioB3_14,
	ioB3_15,
#endif
#if (optB3_NUM >= 18)
	ioB3_16 = ioB3_BASE + 16,
	ioB3_17,
#endif
#if (optB3_NUM >= 21)
	ioB3_18 = ioB3_BASE + 18,
	ioB3_19,
	ioB3_20,
#endif
	ioB3_END_ = 0							// keeps the enum non-empty
};

enum {								// 4-bit options, ioB4_BASE + n
#if (optB4_NUM >= 2)
	ioB4_0 = ioB4_BASE + 0,
	ioB4_1,
#endif
#if (optB4_NUM >= 4)
	ioB4_2 = ioB4_BASE + 2,
	ioB4_3,
#endif
#if (optB4_NUM >= 6)
	ioB4_4 = ioB4_BASE + 4,
	ioB4_5,
#endif
#if (optB4_NUM >= 8)
	ioB4_6 = ioB4_BASE + 6,
	ioB4_7,
#endif
#if (optB4_NUM >= 10)
	ioB4_8 = ioB4_BASE + 8,
	ioB4_9,
#endif
#if (optB4_NUM >= 12)
	ioB4_10 = ioB4_BASE + 10,
	ioB4_11,
#endif
#if (optB4_NUM >= 14)
	ioB4_12 = ioB4_BASE + 12,
	ioB4_13,
#endif
#if (optB4_NUM >= 16)
	ioB4_14 = ioB4_BASE + 14,
	ioB4_15,
#endif
	ioB4_END_ = 0							// keeps the enum non-empty
};

enum {								// 8-bit options, ioB8_BASE + n
#if (optB8_NUM >= 1)
	ioB8_0 = ioB8_BASE + 0,
#endif
#if (optB8_NUM >= 2)
	ioB8_1 = ioB8_BASE + 1,
#endif
#if (optB8_NUM >= 3)
	ioB8_2 = ioB8_BASE + 2,
#endif
#if (optB8_NUM >= 4)
	ioB8_3 = ioB8_BASE + 3,
#endif
#if (optB8_NUM >= 5)
	ioB8_4 = ioB8_BASE + 4,
#endif
#if (optB8_NUM >= 6)
	ioB8_5 = ioB8_BASE + 5,
#endif
#if (optB8_NUM >= 7)
	ioB8_6 = ioB8_BASE + 6,
#endif
#if (optB8_NUM >= 8)
	ioB8_7 = ioB8_BASE + 7,
#endif
	ioB8_END_ = 0							// keeps the enum non-empty
};

#ifdef __cplusplus
}
#endif
