// options.c - Copyright (c) 2020-25 Andre M. Maree/KSS Technologies (Pty) Ltd.

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
const char ioSXmes[] = iosetSX_MESSAGE;

// ###################################### Private variables ########################################

// ####################################### Public variables ########################################

int (* pfAppHandler)(int, int);

// ####################################### Private functions #######################################

static int xOptionSetupRange(int ON, u64_t ** ppU64, int * piBase, int * piWidth) {
	if (ON < ioB2_0) {
		*ppU64 = &sNVSvars.ioBX.ioB1;
		*piBase = ioB1_0;
		*piWidth = 1;
	} else if (ON < ioB3_0) {
		*ppU64 = &sNVSvars.ioBX.ioB2;
		*piBase = ioB2_0;
		*piWidth = 2;
	} else if (ON < ioB4_0) {
		*ppU64 = &sNVSvars.ioBX.ioB3;
		*piBase = ioB3_0;
		*piWidth = 3;
	} else if (ON < ioB8_0) {
		*ppU64 = &sNVSvars.ioBX.ioB4;
		*piBase = ioB4_0;
		*piWidth = 4;
	} else if (ON < ioBXlast) {
		*ppU64 = &sNVSvars.ioBX.ioB8;
		*piBase = ioB8_0;
		*piWidth = 8;
	} else {
		return erFAILURE;			/* invalid ON */
	}
	return erSUCCESS;
}

static void xOptionsSetDefaults(void) { memcpy(&sNVSvars.ioBX, &ioDefaults, sizeof(ioset_t)); }

// ########################################## SET support ##########################################

void vOptionSet(int ON, int OV) {
	int Base, Width;
	u64_t * pU64;
	int iRV = xOptionSetupRange(ON, &pU64, &Base, &Width);
	if (iRV < erSUCCESS)
		return;						/* invalid ON */
	// range parameters configured, calculate & make changes
	int Shift = (ON - Base) * Width;
	u64_t Mask = BIT_MASK64(Shift, Shift + Width - 1);
	IF_RP(debugSETTERS, "ON=%d  OV=%d  B=%d  W=%d  S=%d ", ON, OV, Base, Width, Shift);
	IF_RP(debugSETTERS, " M=x%lX%lX  Val=x%lX%lX", MSWofU64(Mask), LSWofU64(Mask), MSWofU64(*pU64), LSWofU64(*pU64));
	*pU64 &= ~Mask;										// remove old bit value
	IF_RP(debugSETTERS, " -> x%lX%lX", MSWofU64(*pU64), LSWofU64(*pU64));
	*pU64 |= ((u64_t)OV << Shift) & Mask;				// insert new bit value
	IF_RP(debugSETTERS, " -> x%lX%lX" strNL, MSWofU64(*pU64), LSWofU64(*pU64));
}

int xOptionSet(int ON, int OV) {
	int iRV = 1;
	const char * pcMess;
	if (ON < ioBXlast) {			// determine the max option value, based on size, and check validity
		int EVL = (ON < ioB2_0) ? 1 : (ON < ioB3_0) ? 3 : (ON < ioB4_0) ? 7 : (ON < ioB8_0) ? 15 : 255;
		#if (appAEP > 0)
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
		#ifdef xAppOptionsHandler
		if (iRV == 1 && pfAppHandler)
			iRV = xAppOptionsHandler(ON,OV);
		#endif
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

int xOptionGet(int ON) {
	int Base, Width;
	u64_t * pU64;
	int iRV = xOptionSetupRange(ON, &pU64, &Base, &Width);
	if (iRV < erSUCCESS)
		return iRV;
	int Shift = (ON - Base) * Width;
	u64_t Mask = BIT_MASK64(0, Width - 1);
	u64_t Val = *pU64;
	IF_RP(debugGETTERS, "ON=%d  B=%d  W=%d  S=%d", ON, Base, Width, Shift);
	IF_RP(debugGETTERS, " M=x%08lX%08lX  Val=x%08lX%08lX", MSWofU64(Mask), LSWofU64(Mask), MSWofU64(Val), LSWofU64(Val));
	Val >>= Shift;
	IF_RP(debugGETTERS, " -> x%08lX%08lX", MSWofU64(Val), LSWofU64(Val));
	Val &= Mask;
	IF_RP(debugGETTERS, " -> x%08lX%08lX (%d)" strNL, MSWofU64(Val), LSWofU64(Val), (int) Val);
	return (int) Val;
}

int xOptionGetDefault(int ON) {
	return (ON >= ioBXlast) ? erFAILURE :
			(ON >= ioB8_0) ? maskGET8B(ioDefaults.ioB8, (ON - ioB8_0), u64_t) :
			(ON >= ioB4_0) ? maskGET4B(ioDefaults.ioB4, (ON - ioB4_0), u64_t) :
			(ON >= ioB3_0) ? maskGET3B(ioDefaults.ioB3, (ON - ioB3_0), u64_t) :
			(ON >= ioB2_0) ? maskGET2B(ioDefaults.ioB2, (ON - ioB2_0), u64_t) :
							maskGET1B(ioDefaults.ioB1, (ON - ioB1_0), u64_t);
}

// #################################### Option status reporting ####################################

void vOptionsShow(report_t * psR) {
	const char * pcMess = ioBXmes;
	int Cur, Def, Col, Len, Idx = 0;
	const char * pcForm;
	u64_t u64Val;
	for (int Num = ioB1_0; Num < ioBXlast; ++Num) {
		if (Num == ioB1_0) {
			pcForm = "1";
			u64Val = sNVSvars.ioBX.ioB1;
			Idx = 0;
		} else if (Num == ioB2_0) {
			pcForm = "2";
			u64Val = sNVSvars.ioBX.ioB2;
			Idx = 0;
		} else if (Num == ioB3_0) {
			pcForm = "3";
			u64Val = sNVSvars.ioBX.ioB3;
			Idx = 0;
		} else if (Num == ioB4_0) {
			pcForm = strNL "4";
			u64Val = sNVSvars.ioBX.ioB4;
			Idx = 0;
		} else if (Num == ioB8_0) {
			pcForm = "8";
			u64Val = sNVSvars.ioBX.ioB8;
			Idx = 0;
		} else {
			pcForm = NULL;
		}
		Cur = xOptionGet(Num);
		Def = xOptionGetDefault(Num);
		Col = (Cur == Def) ? xpfCOL(attrRESET,0) : xpfCOL(colourFG_RED,0);
		if (pcForm)
			xReport(psR, "%C%s-Bit options:%C x%0llX" strNL, xpfCOL(colourFG_CYAN,0), pcForm, xpfCOL(attrRESET,0), u64Val);
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
	}		// end of Num = ioB8_7
	xReport(psR, "%CNon-bit options:%C" strNL "%s", xpfSGR(0,0,colourFG_CYAN,0), xpfSGR(0,0,attrRESET,0), ioSXmes);
}

#endif
