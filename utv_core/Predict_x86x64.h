/* �����R�[�h�͂r�i�h�r ���s�R�[�h�͂b�q�k�e */
/* $Id$ */

#pragma once

template<int F> void tuned_PredictCylindricalLeftAndCount8(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t*pSrcEnd, uint32_t *pCountTable);
template<int F> void tuned_RestoreCylindricalLeft8(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd);
template<int F> void tuned_PredictCylindricalLeftAndCount10(uint16_t *pDst, const uint16_t *pSrcBegin, const uint16_t*pSrcEnd, uint32_t *pCountTable);
template<int F> void tuned_RestoreCylindricalLeft10(uint16_t *pDst, const uint16_t *pSrcBegin, const uint16_t *pSrcEnd);
template<int F> void tuned_PredictCylindricalWrongMedianAndCount8(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t dwStride, uint32_t *pCountTable);
template<int F> void tuned_RestoreCylindricalWrongMedian8(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t dwStride);