#pragma once

//
// �`��f�[�^�̊��N���X
//

// �E�B���h�E�֘A�̏���
#include "Window.h"

class Shape
{
  // ���_�z��I�u�W�F�N�g
  GLuint vao;

public:

  // �R���X�g���N�^
  Shape()
  {
    // ���_�z��I�u�W�F�N�g���쐬����
    glGenVertexArrays(1, &vao);
  }

  // �f�X�g���N�^
  virtual ~Shape()
  {
    // ���_�z��I�u�W�F�N�g���폜����
    glDeleteVertexArrays(1, &vao);
  }

  // ����
  void bind() const
  {
    // ���_�z��I�u�W�F�N�g���w�肷��
    glBindVertexArray(vao);
  }

  // �`��
  virtual void draw() const = 0;
};
