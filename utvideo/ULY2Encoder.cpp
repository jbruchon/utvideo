/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */
/*
 * Ut Video Codec Suite
 * Copyright (C) 2008  UMEZAWA Takeshi
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * 
 * 
 * Ut Video Codec Suite
 * Copyright (C) 2008  梅澤 威志
 * 
 * このプログラムはフリーソフトウェアです。あなたはこれを、フリーソフ
 * トウェア財団によって発行された GNU 一般公衆利用許諾契約書(バージョ
 * ン2か、希望によってはそれ以降のバージョンのうちどれか)の定める条件
 * の下で再頒布または改変することができます。
 * 
 * このプログラムは有用であることを願って頒布されますが、*全くの無保
 * 証* です。商業可能性の保証や特定の目的への適合性は、言外に示された
 * ものも含め全く存在しません。詳しくはGNU 一般公衆利用許諾契約書をご
 * 覧ください。
 * 
 * あなたはこのプログラムと共に、GNU 一般公衆利用許諾契約書の複製物を
 * 一部受け取ったはずです。もし受け取っていなければ、フリーソフトウェ
 * ア財団まで請求してください(宛先は the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA)。
 */

#include "StdAfx.h"
#include "ULY2Encoder.h"
#include "utvideo.h"
#include "HuffmanCode.h"
#include "Predict.h"

CULY2Encoder::CULY2Encoder(void)
{
}

CULY2Encoder::~CULY2Encoder(void)
{
}

DWORD CULY2Encoder::Compress(ICCOMPRESS *icc, DWORD dwSize)
{
	CFrameBuffer *pCurFrame;
	CFrameBuffer *pMedianPredicted;
	FRAMEHEADER *pfh;
	BYTE *p;

	if (icc->lpckid != NULL)
		*icc->lpckid = FCC('dcdc');

	pCurFrame = new CFrameBuffer();
	pCurFrame->AddPlane(m_dwYPlaneSize, m_dwYPlaneStride); // Y
	pCurFrame->AddPlane(m_dwCPlaneSize, m_dwCPlaneStride); // U
	pCurFrame->AddPlane(m_dwCPlaneSize, m_dwCPlaneStride); // V

	pMedianPredicted = new CFrameBuffer();
	pMedianPredicted->AddPlane(m_dwYPlaneSize, m_dwYPlaneStride); // Y
	pMedianPredicted->AddPlane(m_dwCPlaneSize, m_dwCPlaneStride); // U
	pMedianPredicted->AddPlane(m_dwCPlaneSize, m_dwCPlaneStride); // V

	pfh = (FRAMEHEADER *)icc->lpOutput;
	memset(pfh, 0, sizeof(FRAMEHEADER));
	p = (BYTE *)(pfh + 1);

	ConvertFromYUY2ToPlanar(pCurFrame, (BYTE *)icc->lpInput, m_dwFrameSize);

	PredictMedian(pMedianPredicted->GetPlane(0), pCurFrame->GetPlane(0), pCurFrame->GetPlane(0) + m_dwYPlaneSize, m_dwYPlaneStride);
	PredictMedian(pMedianPredicted->GetPlane(1), pCurFrame->GetPlane(1), pCurFrame->GetPlane(1) + m_dwCPlaneSize, m_dwCPlaneStride);
	PredictMedian(pMedianPredicted->GetPlane(2), pCurFrame->GetPlane(2), pCurFrame->GetPlane(2) + m_dwCPlaneSize, m_dwCPlaneStride);

	p += EncodePlane(p, pMedianPredicted->GetPlane(0), pMedianPredicted->GetPlane(0) + m_dwYPlaneSize, m_dwYPlaneStride);
	p += EncodePlane(p, pMedianPredicted->GetPlane(1), pMedianPredicted->GetPlane(1) + m_dwCPlaneSize, m_dwCPlaneStride);
	p += EncodePlane(p, pMedianPredicted->GetPlane(2), pMedianPredicted->GetPlane(2) + m_dwCPlaneSize, m_dwCPlaneStride);
	memset(p, 0, 8);
	p += 8;

	pfh->dwFlags0 = FH_FLAGS0_INTRAFRAME_PREDICT_MEDIAN;
	//pfh->dwFlags1
	//pfh->dwFlags2
	//pfh->dwFlags3

	icc->lpbiOutput->biSizeImage = p - ((BYTE *)icc->lpOutput);
	*icc->lpdwFlags = AVIIF_KEYFRAME;

	delete pCurFrame;
	delete pMedianPredicted;

	return ICERR_OK;
}

DWORD CULY2Encoder::CompressBegin(BITMAPINFOHEADER *pbihIn, BITMAPINFOHEADER *pbihOut)
{
	m_dwFrameStride = ROUNDUP(pbihIn->biWidth, 2) * 2;
	m_dwFrameSize = m_dwFrameStride * pbihIn->biHeight;

	m_dwYPlaneStride = ROUNDUP(pbihIn->biWidth, 2);
	m_dwYPlaneSize = m_dwYPlaneStride * pbihIn->biHeight;

	m_dwCPlaneStride = ROUNDUP(pbihIn->biWidth, 2) / 2;
	m_dwCPlaneSize = m_dwCPlaneStride * pbihIn->biHeight;

	return ICERR_OK;
}

DWORD CULY2Encoder::CompressEnd(void)
{
	return ICERR_OK;
}

DWORD CULY2Encoder::CompressGetFormat(BITMAPINFOHEADER *pbihIn, BITMAPINFOHEADER *pbihOut)
{
	BITMAPINFOEXT *pbieOut = (BITMAPINFOEXT *)pbihOut;

	if (pbihOut == NULL)
		return sizeof(BITMAPINFOEXT);

	memset(pbihOut, 0, sizeof(BITMAPINFOEXT));

	pbieOut->bih.biSize          = sizeof(BITMAPINFOEXT);
	pbieOut->bih.biWidth         = pbihIn->biWidth;
	pbieOut->bih.biHeight        = pbihIn->biHeight;
	pbieOut->bih.biPlanes        = 1;
	pbieOut->bih.biBitCount      = 16;
	pbieOut->bih.biCompression   = FCC('ULY2');
	pbieOut->bih.biSizeImage     = pbihIn->biSizeImage;
	//pbieOut->bih.biXPelsPerMeter
	//pbieOut->bih.biYPelsPerMeter
	//pbieOut->bih.biClrUsed
	//pbieOut->bih.biClrImportant
	pbieOut->dwEncoderVersion  = UTVIDEO_ENCODER_VERSION;
	pbieOut->fccOriginalFormat = pbihIn->biCompression;
	pbieOut->dwFlags0          = BIE_FLAGS0_COMPRESS_HUFFMAN_CODE;
	//pbieOut->dwFlags1
	//pbieOut->dwFlags2
	//pbieOut->dwFlags3
	//pbieOut->dwFlags4
	//pbieOut->dwFlags5
	//pbieOut->dwFlags6
	//pbieOut->dwFlags7

	return ICERR_OK;
}

DWORD CULY2Encoder::CompressGetSize(BITMAPINFOHEADER *pbihIn, BITMAPINFOHEADER *pbihOut)
{
	return pbihIn->biSizeImage;
}

DWORD CULY2Encoder::CompressQuery(BITMAPINFOHEADER *pbihIn, BITMAPINFOHEADER *pbihOut)
{
	if (pbihIn->biCompression == FCC('YUY2'))
		return ICERR_OK;
	else
		return ICERR_BADFORMAT;
}

void CULY2Encoder::ConvertFromYUY2ToPlanar(CFrameBuffer *pBuffer, const BYTE *pSrcBegin, DWORD dwFrameSize)
{
	BYTE *y, *u, *v;
	const BYTE *pSrcEnd, *p;

	pSrcEnd = pSrcBegin + dwFrameSize;
	y = pBuffer->GetPlane(0);
	u = pBuffer->GetPlane(1);
	v = pBuffer->GetPlane(2);

	for (p = pSrcBegin; p < pSrcEnd; p += 4)
	{
		*y++ = *p;
		*u++ = *(p+1);
		*y++ = *(p+2);
		*v++ = *(p+3);
	}
}

DWORD CULY2Encoder::EncodePlane(BYTE *pDst, const BYTE *pSrcBegin, const BYTE *pSrcEnd, DWORD dwFrameStride)
{
	const BYTE *p;
	DWORD count[256];
	BYTE *pCodeLengthTable;
	HUFFMAN_ENCODE_TABLE het;
	DWORD cbEncoded;

	pCodeLengthTable = pDst;

	for (int i = 0; i < 256; i++)
		count[i] = 0;

	for (p = pSrcBegin; p < pSrcEnd; p++)
		count[*p]++;

	GenerateHuffmanCodeLengthTable(pCodeLengthTable, count);
	GenerateHuffmanEncodeTable(&het, pCodeLengthTable);
	cbEncoded = HuffmanEncode(pDst + 256 + 4, pSrcBegin, pSrcEnd, &het);
	*(DWORD *)(pDst + 256) = cbEncoded;
	return cbEncoded + 256 + 4;
}
