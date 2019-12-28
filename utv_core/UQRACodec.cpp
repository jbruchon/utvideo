/* �����R�[�h�͂r�i�h�r ���s�R�[�h�͂b�q�k�e */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "UQRACodec.h"
#include "Predict.h"
#include "Convert.h"
#include "TunedFunc.h"
#include "ByteOrder.h"

const utvf_t CUQRACodec::m_utvfEncoderInput[] = {
	UTVF_b64a,
	UTVF_INVALID,
};

const utvf_t CUQRACodec::m_utvfDecoderOutput[] = {
	UTVF_b64a,
	UTVF_INVALID,
};

const utvf_t CUQRACodec::m_utvfCompressed[] = {
	UTVF_UQRA,
	UTVF_INVALID,
};

CUQRACodec::CUQRACodec(const char *pszInterfaceName) : CUQ00Codec("UQRA", pszInterfaceName)
{
}

void CUQRACodec::CalcPlaneSizes(unsigned int width, unsigned int height)
{
	m_cbPlaneSize[0]          = width * height * 2;
	m_cbPlaneSize[1]          = width * height * 2;
	m_cbPlaneSize[2]          = width * height * 2;
	m_cbPlaneSize[3]          = width * height * 2;

	m_cbPlaneWidth[0]         = width * 2;
	m_cbPlaneWidth[1]         = width * 2;
	m_cbPlaneWidth[2]         = width * 2;
	m_cbPlaneWidth[3]         = width * 2;

	m_cbPlaneStripeSize[0]    = width * 2;
	m_cbPlaneStripeSize[1]    = width * 2;
	m_cbPlaneStripeSize[2]    = width * 2;
	m_cbPlaneStripeSize[3]    = width * 2;

	m_cbPlanePredictStride[0] = width * 2;
	m_cbPlanePredictStride[1] = width * 2;
	m_cbPlanePredictStride[2] = width * 2;
	m_cbPlanePredictStride[3] = width * 2;
}

void CUQRACodec::ConvertToPlanar(uint32_t nBandIndex)
{
	uint8_t *g, *b, *r, *a;
	const uint8_t *pSrcBegin, *pSrcEnd;

	pSrcBegin = ((uint8_t *)m_pInput) + m_dwStripeBegin[nBandIndex] * m_cbRawStripeSize;
	pSrcEnd   = ((uint8_t *)m_pInput) + m_dwStripeEnd[nBandIndex]   * m_cbRawStripeSize;
	g = m_pCurFrame->GetPlane(0) + m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[0];
	b = m_pCurFrame->GetPlane(1) + m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[1];
	r = m_pCurFrame->GetPlane(2) + m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[2];
	a = m_pCurFrame->GetPlane(3) + m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[3];

	switch (m_utvfRaw)
	{
	case UTVF_b64a:
		ConvertB64aToUQRA(g, b, r, a, pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth);
		break;
	}
}

void CUQRACodec::ConvertFromPlanar(uint32_t nBandIndex)
{
	const uint8_t *g, *b, *r, *a;
	uint8_t *pDstBegin, *pDstEnd;

	pDstBegin = ((uint8_t *)m_pOutput) + m_dwStripeBegin[nBandIndex] * m_cbRawStripeSize;
	pDstEnd   = ((uint8_t *)m_pOutput) + m_dwStripeEnd[nBandIndex]   * m_cbRawStripeSize;
	g = m_pCurFrame->GetPlane(0) + m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[0];
	b = m_pCurFrame->GetPlane(1) + m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[1];
	r = m_pCurFrame->GetPlane(2) + m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[2];
	a = m_pCurFrame->GetPlane(3) + m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[3];

	switch (m_utvfRaw)
	{
	case UTVF_b64a:
		ConvertUQRAToB64a(pDstBegin, pDstEnd, g, b, r, a, m_cbRawNetWidth, m_cbRawGrossWidth);
		break;
	}
}

bool CUQRACodec::PredictDirect(uint32_t nBandIndex)
{
	uint8_t *g, *b, *r, *a;
	const uint8_t *pSrcBegin, *pSrcEnd;

	pSrcBegin = ((uint8_t *)m_pInput) + m_dwStripeBegin[nBandIndex] * m_cbRawStripeSize;
	pSrcEnd = ((uint8_t *)m_pInput) + m_dwStripeEnd[nBandIndex] * m_cbRawStripeSize;
	g = m_pPredicted->GetPlane(0) + m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[0];
	b = m_pPredicted->GetPlane(1) + m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[1];
	r = m_pPredicted->GetPlane(2) + m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[2];
	a = m_pPredicted->GetPlane(3) + m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[3];

	switch (m_utvfRaw)
	{
	case UTVF_b64a:
		ConvertB64aToUQRA_PredictCylindricalLeftAndCount(g, b, r, a, pSrcBegin, pSrcEnd, m_cbRawNetWidth, m_cbRawGrossWidth, m_counts[nBandIndex].dwCount[0], m_counts[nBandIndex].dwCount[1], m_counts[nBandIndex].dwCount[2], m_counts[nBandIndex].dwCount[3]);
		return true;
	}

	return false;
}

bool CUQRACodec::RestoreDirect(uint32_t nBandIndex)
{
	const uint8_t *g, *b, *r, *a;
	uint8_t *pDstBegin, *pDstEnd;

	pDstBegin = ((uint8_t *)m_pOutput) + m_dwStripeBegin[nBandIndex] * m_cbRawStripeSize;
	pDstEnd = ((uint8_t *)m_pOutput) + m_dwStripeEnd[nBandIndex] * m_cbRawStripeSize;
	g = m_pPredicted->GetPlane(0) + m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[0];
	b = m_pPredicted->GetPlane(1) + m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[1];
	r = m_pPredicted->GetPlane(2) + m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[2];
	a = m_pPredicted->GetPlane(3) + m_dwStripeBegin[nBandIndex] * m_cbPlaneStripeSize[3];

	switch (m_utvfRaw)
	{
	case UTVF_b64a:
		ConvertUQRAToB64a_RestoreCylindricalLeft(pDstBegin, pDstEnd, g, b, r, a, m_cbRawNetWidth, m_cbRawGrossWidth);
		return true;
	}

	return false;
}

bool CUQRACodec::IsDirectRestorable()
{
	switch (m_utvfRaw)
	{
	case UTVF_b64a:
		return true;
	}

	return false;
}
