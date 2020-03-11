/* �����R�[�h�͂r�i�h�r ���s�R�[�h�͂b�q�k�e */
/* $Id$ */

#pragma once
#include "Codec.h"
#include "UQ00Codec.h"
#include "FrameBuffer.h"
#include "Thread.h"
#include "HuffmanCode.h"

class CUQY0Codec :
	public CUQ00Codec
{
public:
	static const utvf_t m_utvfCodec = UTVF_UQY0;

private:
	static const utvf_t m_utvfEncoderInput[];
	static const utvf_t m_utvfDecoderOutput[];
	static const utvf_t m_utvfCompressed[];

protected:

public:
	CUQY0Codec(const char *pszInterfaceName);
	virtual ~CUQY0Codec(void) {}
	static CCodec *CreateInstance(const char *pszInterfaceName) { return new CUQY0Codec(pszInterfaceName); }

public:
	virtual const utvf_t *GetEncoderInputFormat(void) { return m_utvfEncoderInput; }
	virtual const utvf_t *GetDecoderOutputFormat(void) { return m_utvfDecoderOutput; }
	virtual const utvf_t *GetCompressedFormat(void) { return m_utvfCompressed; }

protected:
	virtual const char *GetColorFormatName(void) { return "YUV420"; }
	virtual int GetRealBitCount(void) { return 15; }
	virtual int GetNumPlanes(void) { return 3; }
	virtual int GetMacroPixelWidth(void) { return 2; }
	virtual int GetMacroPixelHeight(void) { return 2; }

	virtual void CalcPlaneSizes(unsigned int width, unsigned int height);
	virtual void ConvertToPlanar(uint32_t nBandIndex);
	virtual void ConvertFromPlanar(uint32_t nBandIndex);
	virtual bool PredictDirect(uint32_t nBandIndex);
	virtual void GenerateDecodeTable(uint32_t nPlaneIndex);
	virtual bool DecodeDirect(uint32_t nBandIndex);
	virtual void RestoreCustom(uint32_t nBandIndex, int nPlaneIndex, uint8_t* const* pDstBegin);
};
