/* �����R�[�h�͂r�i�h�r ���s�R�[�h�͂b�q�k�e */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "FrameBuffer.h"

CFrameBuffer::CFrameBuffer(void)
{
	for (int i = 0; i < MAX_PLANE; i++)
	{
		m_pAllocatedAddr[i] = NULL;
		m_pBufferAddr[i] = NULL;
	}
	m_nPlanes = 0;
}

CFrameBuffer::~CFrameBuffer(void)
{
	for (int i = 0; i < MAX_PLANE; i++)
	{
		if (m_pAllocatedAddr[i] != NULL)
		{
#ifdef _WIN32
			VirtualFree(m_pAllocatedAddr[i], 0, MEM_RELEASE);
#endif
#if defined(__APPLE__) || defined(__unix__)
			munmap(m_pAllocatedAddr[i], m_cbAllocated[i]);
#endif
		}
	}
}

void CFrameBuffer::AddPlane(size_t cbBuffer, size_t cbMargin)
{
#ifdef _WIN32
	SYSTEM_INFO si;
#endif
	size_t cbAllocateUnit;
	size_t cbAllocated;
	uint8_t *pAllocatedAddr;
	/*
	 * �v���[�����ƂɊJ�n�A�h���X�����������炵�A�L���b�V���̃X���b�V���O���������B
	 * �I�t�Z�b�g�́A
	 *   - �L���b�V�����C���T�C�Y�i�ŋ߂̑����̃v���Z�b�T�ł� 64 �o�C�g�j�̐����{�łȂ���΂Ȃ�Ȃ�
	 *   - L1 �L���b�V���� 1way �̃T�C�Y���v���[�����Ŋ������l���]�܂���
	 * �ŋ߂� Intel �̃v���Z�b�T�ł� 1way �̃T�C�Y�� 4KiB �ŁA�v���[�����͍ő�� 4 �Ȃ̂ŁA
	 * �I�t�Z�b�g�� 1KiB ���K�؂ƂȂ�B
	 * 
	 * ���̍ŋ߂̃v���Z�b�T�ł� 1way �̃T�C�Y�� 4KiB ��菬�������Ƃ͂Ȃ��̂ŁA���̒l�Ŗ��Ȃ��B
	 * Arm �Ƃ����l���n�߂�Ƃ܂��Ⴄ��������Ȃ����A 1way �̃T�C�Y�� 4KiB ��菬�����悤��
	 * ���K�͂ȃv���Z�b�T�� UtVideo ���g�����Ƃ͂Ȃ����낤�B
	 */
	size_t cbOffset = m_nPlanes * 1024;

#ifdef _WIN32
	GetSystemInfo(&si);
	cbAllocateUnit = si.dwPageSize;
#endif
#if defined(__APPLE__) || defined(__unix__)
	cbAllocateUnit = getpagesize();
#endif
	cbMargin = ROUNDUP(cbMargin, cbAllocateUnit);
	cbAllocated = cbMargin + ROUNDUP(cbBuffer + cbMargin + cbOffset, cbAllocateUnit);

#ifdef _WIN32
	pAllocatedAddr = (uint8_t *)VirtualAlloc(NULL, cbAllocated, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (pAllocatedAddr == NULL)
		return;
#endif
#if defined(__APPLE__) || defined(__unix__)
	pAllocatedAddr = (uint8_t *)mmap(NULL, cbAllocated, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
	if (pAllocatedAddr == MAP_FAILED)
		return;
#endif

	m_pAllocatedAddr[m_nPlanes] = pAllocatedAddr;
	m_cbAllocated[m_nPlanes] = cbAllocated;
	m_pBufferAddr[m_nPlanes] = pAllocatedAddr + cbMargin + cbOffset;

	m_nPlanes++;

	return;
}
