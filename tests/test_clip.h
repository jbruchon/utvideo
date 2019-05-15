/* �����R�[�h�͂r�i�h�r ���s�R�[�h�͂b�q�k�e */
/* $Id$ */

#pragma once

#include "print_log_value.h"

/*
 * �f�R�[�h�e�X�g
 * 
 * ���̓N���b�v�i���̓t�H�[�}�b�g���R�[�f�b�N��FourCC�����܂�j
 * �o�̓N���b�v�i�o�̓t�H�[�}�b�g�����܂�j
 * ���e�덷
 */
extern const vector<tuple<string, string, unsigned int>> vecDecodeClips;

/*
 * �G���R�[�h�e�X�g
 *
 * ���̓N���b�v�i���̓t�H�[�}�b�g�����܂�j
 * �o�̓N���b�v�i�o�̓t�H�[�}�b�g���R�[�f�b�N�� FourCC �� extradata �����܂�j
 * �G���R�[�_�̃R���t�B�M�����[�V�����f�[�^
 * �o�̓N���b�v�̃X�g���[���w�b�_�� extradata �̔�r�}�X�N
 */
extern const vector<tuple<string, string, vector<uint8_t>, vector<uint8_t>>> vecEncodeClips;

/*
 * �G���R�[�h�f�R�[�h�e�X�g
 * ���̓N���b�v�i���̓t�H�[�}�b�g�����܂�j
 * �o�̓N���b�v�i�o�̓t�H�[�}�b�g�����܂�j
 * �R�[�f�b�N�� FourCC
 * �G���R�[�_�̃R���t�B�M�����[�V�����f�[�^
 * ���e�덷
 */
extern const vector<tuple<string, string, uint32_t, vector<uint8_t>, unsigned int>> vecEncDecClips;
