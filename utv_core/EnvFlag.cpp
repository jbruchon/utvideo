/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

#include "stdafx.h"
#include "EnvFlag.h"

bool GetEnvFlagBool(const char* pszEnvName)
{
#if defined(_WIN32)
	char p[4];
	auto n = GetEnvironmentVariableA(pszEnvName, p, sizeof(p));
	if (n == 0) // 環境変数が見つからなかった
		return false;
	if (n >= sizeof(p)) // 環境変数が長すぎた
		return true;
#endif
#if defined(__APPLE__) || defined(__unix__)
	char *p;
	p = getenv(pszEnvName);
	if (p == NULL) // 環境変数が見つからなかった
		return false;
#endif
	if (strcmp(p, "") == 0)
		return false;

	return true;
}
