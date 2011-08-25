/* �����R�[�h�͂r�i�h�r ���s�R�[�h�͂b�q�k�e */
/* $Id$ */

#pragma once
#include "Codec.h"
#include "UL00Codec.h"
#include "FrameBuffer.h"
#include "Thread.h"
#include "HuffmanCode.h"

class CULRGCodec :
	public CUL00Codec
{
private:
	static const utvf_t m_utvfEncoderInput[];
	static const utvf_t m_utvfDecoderOutput[];
	static const utvf_t m_utvfCompressed[];

public:
	CULRGCodec(const char *pszInterfaceName);
	virtual ~CULRGCodec(void);
	static CCodec *CreateInstance(const char *pszInterfaceName);

public:
	virtual const char *GetTinyName(void) { return "ULRG"; }
	virtual const utvf_t *GetEncoderInputFormat(void) { return m_utvfEncoderInput; }
	virtual const utvf_t *GetDecoderOutputFormat(void) { return m_utvfDecoderOutput; }
	virtual const utvf_t *GetCompressedFormat(void) { return m_utvfCompressed; }

protected:
	virtual const char *GetColorFormatName(void) { return "RGB"; }
	virtual WORD GetRealBitCount(void) { return 24; }
	virtual int GetNumPlanes(void) { return 3; }
	virtual void CalcPlaneSizes(unsigned int width, unsigned int height);
	virtual void ConvertToPlanar(DWORD nBandIndex);
	virtual int GetMacroPixelWidth(void) { return 1; }
	virtual int GetMacroPixelHeight(void) { return 1; }

	virtual void ConvertFromPlanar(DWORD nBandIndex);
	virtual bool DecodeDirect(DWORD nBandIndex);
};
