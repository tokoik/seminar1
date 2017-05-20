// ���ʂ̊֐�
#include "common.h"

// �W�����C�u����
#include <cmath>

// ���b�V���̕��ƍ���
const int width(100), height(100);

// �_�f�[�^
float point[height][width][3];

// �F�f�[�^
unsigned char color[height][width][3];

//
// �ݒ�i�ŏ��Ɉ�x�������s����܂��j
//
bool setup()
{
  for (int y = 0; y < height; ++y)
  {
    for (int x = 0; x < width; ++x)
    {
      // x �����̃p�����[�^ (0��u��1)
      const float u(float(x) / float(width - 1));

      // y �����̃p�����[�^ (0��v��1)
      const float v(float(y) / float(height - 1));

      // ���_�̈ʒu
      point[y][x][0] = u * 2.0f - 1.0f;
      point[y][x][1] = v * 2.0f - 1.0f;
      point[y][x][2] = 0.0f;

      // ���_�̐F
      color[y][x][0] = 100;
      color[y][x][1] = 0;
      color[y][x][2] = 0;
    }
  }

  // �_�f�[�^��o�^����
  createPoint(width, height, point);

  // �F�f�[�^��o�^����
  createColor(width, height, color);

  // �Z�b�g�A�b�v�ɐ�������
  return true;
}

//
// �X�V�i������s����܂��j
//
bool update()
{
  // �v���O�������I�����Ȃ�
  return true;
}
