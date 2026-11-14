// options_size.h - Copyright (c) 2026 Andre M. Maree / KSS Technologies (Pty) Ltd.
//
// Compile-time sizing of the 1/2/3/4/8-bit option sets (z-comp options, ioset_t in struct_union.h, the NVS vars blob
// in hal_esp32 hal_config.h). A project sets any count with add_compile_definitions() in its top-level CMakeLists.txt,
// so every component sees the same value, e.g.
//		add_compile_definitions( optB1_NUM=8 optB2_NUM=0 optB3_NUM=0 optB4_NUM=2 optB8_NUM=2 )
// Undefined counts default to the full set (64/32/21/16/8 = 40 bytes): irmacs and every project without
// definitions build byte-identical to before. Steps are one storage byte per width.

#pragma once

// Bytes per width as plain literals (they are token-pasted by options_val.h)
#ifndef	optB1_NUM
	#define	optB1_NUM			64						// 0, 8, 16 .. 64
#endif
#if	(optB1_NUM == 0)
	#define	optB1_BYTES			0
#elif	(optB1_NUM == 8)
	#define	optB1_BYTES			1
#elif	(optB1_NUM == 16)
	#define	optB1_BYTES			2
#elif	(optB1_NUM == 24)
	#define	optB1_BYTES			3
#elif	(optB1_NUM == 32)
	#define	optB1_BYTES			4
#elif	(optB1_NUM == 40)
	#define	optB1_BYTES			5
#elif	(optB1_NUM == 48)
	#define	optB1_BYTES			6
#elif	(optB1_NUM == 56)
	#define	optB1_BYTES			7
#elif	(optB1_NUM == 64)
	#define	optB1_BYTES			8
#else
	#error "optB1_NUM must be 0, 8, 16 .. 64"
#endif

#ifndef	optB2_NUM
	#define	optB2_NUM			32						// 0, 4, 8 .. 32
#endif
#if		(optB2_NUM == 0)
	#define	optB2_BYTES			0
#elif	(optB2_NUM == 4)
	#define	optB2_BYTES			1
#elif	(optB2_NUM == 8)
	#define	optB2_BYTES			2
#elif	(optB2_NUM == 12)
	#define	optB2_BYTES			3
#elif	(optB2_NUM == 16)
	#define	optB2_BYTES			4
#elif	(optB2_NUM == 20)
	#define	optB2_BYTES			5
#elif	(optB2_NUM == 24)
	#define	optB2_BYTES			6
#elif	(optB2_NUM == 28)
	#define	optB2_BYTES			7
#elif	(optB2_NUM == 32)
	#define	optB2_BYTES			8
#else
	#error "optB2_NUM must be 0, 4, 8 .. 32"
#endif

#ifndef	optB3_NUM
	#define	optB3_NUM			21						// 0, 2, 5, 8, 10, 13, 16, 18, 21
#endif
#if		(optB3_NUM == 0)
	#define	optB3_BYTES			0
#elif	(optB3_NUM == 2)
	#define	optB3_BYTES			1
#elif	(optB3_NUM == 5)
	#define	optB3_BYTES			2
#elif	(optB3_NUM == 8)
	#define	optB3_BYTES			3
#elif	(optB3_NUM == 10)
	#define	optB3_BYTES			4
#elif	(optB3_NUM == 13)
	#define	optB3_BYTES			5
#elif	(optB3_NUM == 16)
	#define	optB3_BYTES			6
#elif	(optB3_NUM == 18)
	#define	optB3_BYTES			7
#elif	(optB3_NUM == 21)
	#define	optB3_BYTES			8
#else
	#error "optB3_NUM must be 0, 2, 5, 8, 10, 13, 16, 18 or 21"
#endif

#ifndef	optB4_NUM
	#define	optB4_NUM			16						// 0, 2, 4 .. 16
#endif
#if		(optB4_NUM == 0)
	#define	optB4_BYTES			0
#elif	(optB4_NUM == 2)
	#define	optB4_BYTES			1
#elif	(optB4_NUM == 4)
	#define	optB4_BYTES			2
#elif	(optB4_NUM == 6)
	#define	optB4_BYTES			3
#elif	(optB4_NUM == 8)
	#define	optB4_BYTES			4
#elif	(optB4_NUM == 10)
	#define	optB4_BYTES			5
#elif	(optB4_NUM == 12)
	#define	optB4_BYTES			6
#elif	(optB4_NUM == 14)
	#define	optB4_BYTES			7
#elif	(optB4_NUM == 16)
	#define	optB4_BYTES			8
#else
	#error "optB4_NUM must be 0, 2, 4 .. 16"
#endif

#ifndef	optB8_NUM
	#define	optB8_NUM			8						// 0, 1, 2 .. 8
#endif
#if		(optB8_NUM >= 0 && optB8_NUM <= 8)
	#define	optB8_BYTES			optB8_NUM
#else
	#error "optB8_NUM must be 0 .. 8"
#endif

#define	optBYTES				(optB1_BYTES + optB2_BYTES + optB3_BYTES + optB4_BYTES + optB8_BYTES)
#if (optBYTES == 0)
	#error "at least one option width must have a non-zero count (ioset_t cannot be empty: C size 0, C++ size 1)"
#endif

/* Sizing signature for the NVS vars blob (hal_config.h nvs_vars_t.OptSig): one nibble of byte count per width,
 * XOR'd with the full set so the full set is 0 - the value the existing irmacs blobs already hold in that spare
 * space. A blob written with another sizing then fails the check and is reset to defaults instead of misread. */
#define	optSIG_OF(b1, b2, b3, b4, b8)	((u32_t)(b1) | ((u32_t)(b2) << 4) | ((u32_t)(b3) << 8) | ((u32_t)(b4) << 12) | ((u32_t)(b8) << 16))
#define	optSIG					(optSIG_OF(optB1_BYTES, optB2_BYTES, optB3_BYTES, optB4_BYTES, optB8_BYTES) ^ optSIG_OF(8, 8, 8, 8, 8))

#define	optFULL					(optB1_NUM == 64 && optB2_NUM == 32 && optB3_NUM == 21 && optB4_NUM == 16 && optB8_NUM == 8)

#include "definitions.h"								// u8_t, DUMB_STATIC_ASSERT (common)

/* 1/2/3/4/8-bit option variables, sized at compile time (above). Each width is a little-endian byte array:
 * option n of a width occupies bits [n * width, n * width + width - 1] counted from bit 0 of byte 0 - the same bit
 * positions the former u64_t unions gave (byte 0 = least significant byte), so the full set is byte-identical to
 * the old layout and existing NVS blobs read unchanged. Access only through options.c (xOptionGet/xOptionSet). */
typedef struct __attribute__((packed)) ioset_t {
#if (optB1_BYTES > 0)
	u8_t ioB1[optB1_BYTES];							// 1-bit options
#endif
#if (optB2_BYTES > 0)
	u8_t ioB2[optB2_BYTES];							// 2-bit options
#endif
#if (optB3_BYTES > 0)
	u8_t ioB3[optB3_BYTES];							// 3-bit options (top bit of the last byte unused at 21)
#endif
#if (optB4_BYTES > 0)
	u8_t ioB4[optB4_BYTES];							// 4-bit options
#endif
#if (optB8_BYTES > 0)
	u8_t ioB8[optB8_BYTES];							// 8-bit options
#endif
} ioset_t;
DUMB_STATIC_ASSERT(sizeof(ioset_t) == optBYTES);
