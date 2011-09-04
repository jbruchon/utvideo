/*  */
/* $Id$ */

#pragma once

#include "Codec.h"

struct CQTDecoder {
	ComponentInstance self;
	ComponentInstance delegateComponent;
	ComponentInstance target;
	OSType componentSubType;
	CCodec *codec;
	OSType **wantedDestinationPixelTypes;
	int beginBandDone;
};
