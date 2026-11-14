// options.c - Copyright (c) 2020-26 Andre M. Maree/KSS Technologies (Pty) Ltd.

#include "hal_platform.h"
#include "options.h"

#if (appOPTIONS > 0)									// only continue if app_options.h exists

#include "syslog.h"
#include "errors_events.h"

#include <string.h>

// ############################################ Macros #############################################

#define debugFLAG					0xF000
#define debugSETTERS				(debugFLAG & 0x0001)
#define debugGETTERS				(debugFLAG & 0x0002)
#define	debugTIMING					(debugFLAG_GLOBAL & debugFLAG & 0x1000)
#define	debugTRACK					(debugFLAG_GLOBAL & debugFLAG & 0x2000)
#define	debugPARAM					(debugFLAG_GLOBAL & debugFLAG & 0x4000)
#define	debugRESULT					(debugFLAG_GLOBAL & debugFLAG & 0x8000)

// ######################################### Enumerations ##########################################

// ########################################## Structures ###########################################

// ###################################### Private constants ########################################

ioset_t const ioDefaults = { iosetDEFAULTS };
const char ioBXmes[] = iosetHELP_MESSAGE;
#if (optFULL)
const char ioSXmes[] = iosetSX_MESSAGE;
#endif

// ###################################### Private variables ########################################

// ####################################### Public variables ########################################

int (* pfAppHandler)(int, int);

// ####################################### Private functions #######################################

/* IRAM: xOptionGet() is reached from ESP_INTR_FLAG_IRAM ISR context via OPT_GET() - see
 * hal_gpio.c halGPIO_IntHandler/halGPIO_TickHook. It must therefore execute with the flash cache
 * disabled. It reads only sNVSvars (DRAM) and these static helpers, so IRAM_ATTR on them is
 * sufficient - no strings, no further calls (byte loops, not memcpy). The SETTERS stay in flash:
 * they write NVS, which cannot happen from that context anyway.
 * Storage (struct_union.h ioset_t): one little-endian byte array per width, sized by options_size.h;
 * option n of width w occupies bits n*w .. n*w+w-1 of the width's array, as with the former u64_t. */
typedef struct opt_range_t { u8_t * pU8; int Bytes, Base, Width; } opt_range_t;

static int IRAM_ATTR xOptionSetupRange(const ioset_t * psIO, int ON, opt_range_t * psR) {
	u8_t * pBase = (u8_t *) psIO;						// widths follow each other in ioset_t (packed)
	#if (optB1_BYTES > 0)
	if (ON >= ioB1_BASE && ON < ioB2_BASE) { *psR = (opt_range_t) { pBase, optB1_BYTES, ioB1_BASE, 1 }; return erSUCCESS; }
	#endif
	pBase += optB1_BYTES;
	#if (optB2_BYTES > 0)
	if (ON >= ioB2_BASE && ON < ioB3_BASE) { *psR = (opt_range_t) { pBase, optB2_BYTES, ioB2_BASE, 2 }; return erSUCCESS; }
	#endif
	pBase += optB2_BYTES;
	#if (optB3_BYTES > 0)
	if (ON >= ioB3_BASE && ON < ioB4_BASE) { *psR = (opt_range_t) { pBase, optB3_BYTES, ioB3_BASE, 3 }; return erSUCCESS; }
	#endif
	pBase += optB3_BYTES;
	#if (optB4_BYTES > 0)
	if (ON >= ioB4_BASE && ON < ioB8_BASE) { *psR = (opt_range_t) { pBase, optB4_BYTES, ioB4_BASE, 4 }; return erSUCCESS; }
	#endif
	pBase += optB4_BYTES;
	#if (optB8_BYTES > 0)
	if (ON >= ioB8_BASE && ON < ioBXlast) { *psR = (opt_range_t) { pBase, optB8_BYTES, ioB8_BASE, 8 }; return erSUCCESS; }
	#endif
	return erFAILURE;									/* invalid ON */
}

static u64_t IRAM_ATTR xOptionLoad(const opt_range_t * psR) {	// little-endian bytes -> u64_t
	u64_t Val = 0;
	for (int i = psR->Bytes - 1; i >= 0; --i)
		Val = (Val << 8) | psR->pU8[i];
	return Val;
}

static void xOptionStore(const opt_range_t * psR, u64_t Val) {	// u64_t -> little-endian bytes
	for (int i = 0; i < psR->Bytes; ++i) {
		psR->pU8[i] = (u8_t) Val;
		Val >>= 8;
	}
}

static int IRAM_ATTR xOptionRead(const ioset_t * psIO, int ON) {
	opt_range_t sR;
	if (xOptionSetupRange(psIO, ON, &sR) < erSUCCESS)
		return erFAILURE;
	int Shift = (ON - sR.Base) * sR.Width;
	return (int) ((xOptionLoad(&sR) >> Shift) & BIT_MASK64(0, sR.Width - 1));
}

static void xOptionsSetDefaults(void) { memcpy(&sNVSvars.ioBX, &ioDefaults, sizeof(ioset_t)); }

// ########################################## SET support ##########################################

void vOptionSet(int ON, int OV) {
	opt_range_t sR;
	if (xOptionSetupRange(&sNVSvars.ioBX, ON, &sR) < erSUCCESS)
		return;											/* invalid ON */
	int Shift = (ON - sR.Base) * sR.Width;
	u64_t Mask = BIT_MASK64(Shift, Shift + sR.Width - 1);
	u64_t Val = xOptionLoad(&sR);
	IF_RP(debugSETTERS, "ON=%d  OV=%d  B=%d  W=%d  S=%d  Val=x%lX%lX", ON, OV, sR.Base, sR.Width, Shift, MSWofU64(Val), LSWofU64(Val));
	Val &= ~Mask;										// remove old bit value
	Val |= ((u64_t)OV << Shift) & Mask;					// insert new bit value
	IF_RP(debugSETTERS, " -> x%lX%lX" strNL, MSWofU64(Val), LSWofU64(Val));
	xOptionStore(&sR, Val);
}

int xOptionSet(int ON, int OV) {
	int iRV = 1;
	const char * pcMess;
	if (ON < ioBXlast) {			// determine the max option value, based on size, and check validity
		int EVL = (ON < ioB2_BASE) ? 1 : (ON < ioB3_BASE) ? 3 : (ON < ioB4_BASE) ? 7 : (ON < ioB8_BASE) ? 15 : 255;
		#if (cmakeAEP > 0)
		if (ON == ioMQTT_QoS && OUTSIDE(0, OV, 2)) {	// exception MQTT QoS levels 0->2 allowed
			pcMess = "MQTT QoS 0->2";
			iRV = erINV_VALUE;
		} else
		#endif
		if (OUTSIDE(0, OV, EVL)) {
			pcMess = "Invalid option value";
			iRV = erINV_VALUE;
		}
	} else if (ON != ioS_IOdef) {						// exception from all above ioB8_7/140
		pcMess = "Invalid option number";
		iRV = erINV_OPERATION;
	} 
	if (iRV == 1) {					// to avoid unnecessary flash writes, set iRV=0 if not changed
		if (ON <= ioBXlast) {							// All B1_0 -> B8_7
			if (xOptionGet(ON) != OV) vOptionSet(ON, OV); else iRV = 0;
		} else if (ON == ioS_IOdef) {					// reset to defaults
			xOptionsSetDefaults();						// leave iRV=1 incase PF specified
		} else {
			IF_myASSERT(debugTRACK, 0);
		}
		// if iRV=1 (something changed) trigger special handler if specified
		if (iRV == 1 && pfAppHandler)
			iRV = xAppOptionsHandler(ON,OV);
	} else {
		SL_ERR("ON=%d OV=%d %s (%ld)", ON, OV, pcMess, iRV);
	}
	return iRV;
}

int	xOptionSetPersist(int ON, int OV, int PF) {
	int iRV = xOptionSet(ON, OV);
	if (iRV <= 0)
		PF = 0;			// If error from handler or nothing changed, clear persistence flag
	if ((iRV >= erSUCCESS) && PF == 1)
		halVarsUpdateBlobs(vfNVSBLOB);
	return iRV;
}

// ########################################## GET support ##########################################

int IRAM_ATTR xOptionGet(int ON) {
	int iRV = xOptionRead(&sNVSvars.ioBX, ON);
	IF_RP(debugGETTERS, "ON=%d -> %d" strNL, ON, iRV);
	return iRV;
}

int xOptionGetDefault(int ON) { return xOptionRead(&ioDefaults, ON); }

// #################################### Option status reporting ####################################

void vOptionsShow(report_t * psR) {
	const char * pcMess = ioBXmes;
	int Cur, Def, Col, Len, Idx = 0;
	for (int Num = ioB1_BASE; Num < ioBXlast; ++Num) {
		const char * pcForm = (Num == ioB1_BASE) ? "1" : (Num == ioB2_BASE) ? "2" : (Num == ioB3_BASE) ? "3" :
							(Num == ioB4_BASE) ? strNL "4" : (Num == ioB8_BASE) ? "8" : NULL;
		if (pcForm) {									// first option of a width: header with the raw bytes
			opt_range_t sR;
			xOptionSetupRange(&sNVSvars.ioBX, Num, &sR);
			xReport(psR, "%C%s-Bit options:%C x%0llX" strNL, xpfCOL(colourFG_CYAN,0), pcForm, xpfCOL(attrRESET,0), xOptionLoad(&sR));
			Idx = 0;
		}
		Cur = xOptionGet(Num);
		Def = xOptionGetDefault(Num);
		Col = (Cur == Def) ? xpfCOL(attrRESET,0) : xpfCOL(colourFG_RED,0);
		xReport(psR, "%3d=%C%X %s%C", Num, Col, Cur, pcMess, xpfCOL(attrRESET,0));
		Len = strlen(pcMess);
		if (Idx == 7) {
			xReport(psR, strNL);
			Idx = 0;
		} else {					// calculate # of space to pad with for next option
			xReport(psR, "%.*s", 12 - Len - (Cur > 0x0F ? 2 : 1), "           ");
			++Idx;
		}
		pcMess += Len + 1;			// adjust message pointer for message length and terminating NUL
	}
	#if (optFULL)
	xReport(psR, "%CNon-bit options:%C" strNL "%s", xpfSGR(0,0,colourFG_CYAN,0), xpfSGR(0,0,attrRESET,0), ioSXmes);
	#else							// reduced sizing: the numbers move, ioSXmes (141..146) does not apply
	xReport(psR, "%CNon-bit options:%C" strNL "%d=NWmode\t%d=NWcred\t%d=MQhost\t%d=Peek\t%d=Poke\t%d=IOrset" strNLx2,
		xpfSGR(0,0,colourFG_CYAN,0), xpfSGR(0,0,attrRESET,0), ioS_NWMO, ioS_WIFI, ioS_MQTT, ioS_PEEK, ioS_POKE, ioS_IOdef);
	#endif
}

#endif
