#pragma once

//
// ���ʂ̊֐�
//

// �_�f�[�^�̍쐬
extern void createPoint(int width, int height, const void *point = nullptr);

// �_�f�[�^�̓]��
extern void submitPoint(int width, int height, const void *point);

// �F�f�[�^�̍쐬
extern void createColor(int width, int height, const void *color = nullptr);

// �F�f�[�^�̓]��
extern void submitColor(int width, int height, const void *color);

// ���ݎ����̎擾
extern float getTime();

// �ݒ�
extern bool setup();

// �X�V
extern bool update();
