/* �����R�[�h�͂r�i�h�r ���s�R�[�h�͂b�q�k�e */
/* $Id$ */

#include "stdafx.h"
#include "test_win_fmt.h"

BOOST_DATA_TEST_CASE(vcm_ICGetInfo, data::make(vecCodecFcc) ^ data::make(vecCodecShortName) ^ data::make(vecCodecLongName) ^ data::make(vecTemporalCompressionSupported), fcc, wstrShortName, wstrLongName, temporal)
{
	HIC hic;
	LRESULT lr;
	ICINFO info;

	hic = ICOpen(ICTYPE_VIDEO, fcc, ICMODE_QUERY);
	BOOST_REQUIRE(hic != (HIC)NULL);

	lr = ICGetInfo(hic, &info, sizeof(info));
	BOOST_TEST_REQUIRE(lr > (LRESULT)offsetof(ICINFO, szDriver));

	BOOST_CHECK_EQUAL(info.fccHandler, fcc);
	BOOST_CHECK_EQUAL(wstring(info.szName), wstrShortName);
	BOOST_CHECK_EQUAL(wstring(info.szDescription), wstrLongName);
	if (temporal)
		BOOST_TEST_CHECK(info.dwFlags == (VIDCF_TEMPORAL | VIDCF_FASTTEMPORALC | VIDCF_FASTTEMPORALD));
	else
		BOOST_TEST_CHECK(info.dwFlags == 0);

	lr = ICClose(hic);
	BOOST_CHECK(lr == ICERR_OK);
}
