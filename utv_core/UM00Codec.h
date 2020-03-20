/* �����R�[�h�͂r�i�h�r ���s�R�[�h�͂b�q�k�e */
/* $Id$ */

#pragma once
#include "Codec.h"
#include "CodecBase.h"
#include "BandParallelCodec.h"
#include "FrameBuffer.h"
#include "Thread.h"


class CUM00Codec :
	public CBandParallelCodec
{
protected:
	struct ENCODERCONF
	{
		uint8_t ecFlags;
		uint8_t ecDivideCountMinusOne;
		uint16_t ecKeyFrameIntervalMinusOne;
	};

	static constexpr uint8_t EC_FLAGS_DIVIDE_COUNT_AUTO        = 0x01;
	static constexpr uint8_t EC_FLAGS_USE_TEMPORAL_COMPRESSION = 0x02;
	static constexpr uint8_t EC_FLAGS_RESERVED                 = 0xfc;

	static constexpr uint16_t EC_KEY_FRAME_INTERVAL_DEFAULT = 30;
	static constexpr uint16_t EC_KEY_FRAME_INTERVAL_MAX     = 60000; // max keyframe interval (inclusive)

	struct STREAMINFO
	{
		union
		{
			struct
			{
				uint8_t siEncoderImplementation;
				uint8_t siEncoderImplementationSpecificData[7];
			};
			struct
			{
				uint32_t siEncoderVersionAndImplementation;
				uint32_t siOriginalFormat;
			};
		};
		uint8_t siEncodingMode;
		uint8_t siDivideCountMinusOne;
		uint8_t siFlags;
		uint8_t siReserved[5];
	};

	static constexpr uint8_t SI_ENCODING_MODE_8SYMPACK = 2;

	static constexpr uint8_t SI_FLAGS_USE_CONTROL_COMPRESSION  = 0x01;
	static constexpr uint8_t SI_FLAGS_USE_TEMPORAL_COMPRESSION = 0x02;
	static constexpr uint8_t SI_FLAGS_RESERVED                 = 0xfc;

	struct FRAMEINFO
	{
		uint8_t fiFrameType;
		uint8_t fiFlags;
		uint8_t fiReserved[2];
		uint32_t fiSizeArrayOffset;
	};

	// static constexpr uint8_t FI_FRAME_TYPE_COPY  = 0;
	static constexpr uint8_t FI_FRAME_TYPE_INTRA = 1;
	static constexpr uint8_t FI_FRAME_TYPE_DELTA = 2;

	static constexpr uint8_t FI_FLAGS_CONTROL_COMPRESSED = 0x01;
	static constexpr uint8_t FI_FLAGS_RESERVED           = 0xfe;

protected:
	ENCODERCONF m_ec;

	utvf_t m_utvfRaw;
	unsigned int m_nWidth;
	unsigned int m_nHeight;
	const FRAMEINFO* m_fiDecode;
	STREAMINFO m_siDecode;
	uint32_t m_nKeyFrameInterval;
	uint32_t m_nFrameNumber;
	size_t m_cbPlaneSize[4];
	size_t m_cbPlaneWidth[4];
	size_t m_cbPlanePredictStride[4];
	uint8_t* m_pPackedStream[4][256];
	size_t m_cbPackedStream[4][256];
	uint8_t* m_pControlStream[4][256];
	size_t m_cbControlStream[4][256];
	uint8_t* m_pTmpControlStream[4][256];

	std::unique_ptr<CThreadManager> m_ptm;
	std::unique_ptr<CFrameBuffer> m_pCurFrame;
	std::unique_ptr<CFrameBuffer> m_pPrevFrame;

protected:
	CUM00Codec(const char *pszTinyName, const char *pszInterfaceName);
	virtual ~CUM00Codec(void) {}

public:
	virtual void GetLongFriendlyName(char *pszName, size_t cchName);

	virtual const utvf_t *GetEncoderInputFormat(void) = 0;
	virtual const utvf_t *GetDecoderOutputFormat(void) = 0;
	virtual const utvf_t *GetCompressedFormat(void) = 0;
	virtual bool IsTemporalCompressionSupported(void) { return true; }

#ifdef _WIN32
	virtual INT_PTR Configure(HWND hwnd);
	static INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif

	virtual size_t GetStateSize(void);
	virtual int GetState(void *pState, size_t cb);

	virtual int InternalEncodeBegin(utvf_t infmt, unsigned int width, unsigned int height, size_t* cbGrossWidth);
	virtual size_t EncodeFrame(void *pOutput, bool *pbKeyFrame, const void *pInput);
	virtual int InternalEncodeEnd(void);
	virtual size_t EncodeGetExtraDataSize(void);
	virtual int EncodeGetExtraData(void *pExtraData, size_t cb, utvf_t infmt, unsigned int width, unsigned int height);
	virtual size_t EncodeGetOutputSize(utvf_t infmt, unsigned int width, unsigned int height);
	virtual int InternalEncodeQuery(utvf_t infmt, unsigned int width, unsigned int height);

	virtual int InternalDecodeBegin(utvf_t outfmt, unsigned int width, unsigned int height, size_t* cbGrossWidth, const void *pExtraData, size_t cbExtraData);
	virtual size_t DecodeFrame(void *pOutput, const void *pInput);
	virtual int DecodeGetFrameType(bool *pbKeyFrame, const void *pInput);
	virtual int InternalDecodeEnd(void);
	virtual size_t DecodeGetOutputSize(utvf_t outfmt, unsigned int width, unsigned int height, size_t* cbGrossWidth);
	virtual int InternalDecodeQuery(utvf_t outfmt, unsigned int width, unsigned int height, const void *pExtraData, size_t cbExtraData);

protected:
	virtual int InternalSetState(const void *pState, size_t cb);
	void SetDefaultState();
	int CalcFrameMetric(utvf_t rawfmt, unsigned int width, unsigned int height, size_t* cbGrossWidth);

	virtual const char *GetColorFormatName(void) = 0;
	virtual int GetRealBitCount(void) = 0;
	virtual int GetNumPlanes(void) = 0;
	virtual int GetMacroPixelWidth(void) = 0;
	virtual int GetMacroPixelHeight(void) = 0;

	virtual void CalcPlaneSizes(unsigned int width, unsigned int height) = 0;
	virtual void ConvertToPlanar(uint32_t nBandIndex) = 0;
	virtual void ConvertFromPlanar(uint32_t nBandIndex) = 0;
	virtual bool EncodeDirect(uint32_t nBandIndex);
	virtual bool DecodeDirect(uint32_t nBandIndex);

	void EncodeFromPlanar(uint32_t nBandIndex, const uint8_t* const* pSrcBegin, const uint8_t* const* pPrevBegin = NULL);
	void DecodeToPlanar(uint32_t nBandIndex, uint8_t* const* pDstBegin, const uint8_t* const* pPrevBegin = NULL);

private:
	void EncodeProc(uint32_t nBandIndex);
	void DecodeProc(uint32_t nBandIndex);

	class CThreadJob : public ::CThreadJob
	{
	public:
		typedef void (CUM00Codec::*JobProcType)(uint32_t nBandIndex);

	private:
		CUM00Codec *m_pCodec;
		JobProcType m_pfnJobProc;
		uint32_t m_nBandIndex;

	public:
		CThreadJob(CUM00Codec *pCodec, JobProcType pfnJobProc, uint32_t nBandIndex)
		{
			m_pCodec = pCodec;
			m_pfnJobProc = pfnJobProc;
			m_nBandIndex = nBandIndex;
		}

		void JobProc(CThreadManager *)
		{
			(m_pCodec->*m_pfnJobProc)(m_nBandIndex);
		}
	};
};
