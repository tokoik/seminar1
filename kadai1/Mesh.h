#pragma once

//
// ���b�V��
//

// �`��f�[�^
#include "Shape.h"

class Mesh : public Shape
{
  // �X�g���b�v1��������̒��_�̐�
  const GLsizei vertexCount;

  // �X�g���b�v�̐�
  const GLsizei primitiveCount;

  // �_�f�[�^�̃e�N�X�`���̃T���v���� uniform �ϐ��̏ꏊ
  GLint pointLoc;

  // �F�f�[�^�̃e�N�X�`���̃T���v���� uniform �ϐ��̏ꏊ
  GLint colorLoc;

public:

  // �V�F�[�_
  const GgSimpleShader shader;

  // �R���X�g���N�^
  Mesh(int slices = 2, int stacks = 2)
    : vertexCount(slices * 2)
    , primitiveCount(stacks - 1)
    , shader(GgSimpleShader("mesh.vert", "mesh.frag"))
  {
    // �T���v���� uniform �ϐ��̏ꏊ�𓾂�
    pointLoc = glGetUniformLocation(shader.get(), "point");
    colorLoc = glGetUniformLocation(shader.get(), "color");
  }

  // �R���X�g���N�^
  Mesh(const int *size)
    : Mesh(size[0], size[1])
  {
  }

  // �f�X�g���N�^
  virtual ~Mesh()
  {
  }

  // �`��
  virtual void draw() const
  {
    // �_�e�N�X�`���̃T���v�����O�Ɏg�p����e�N�X�`�����j�b�g�ԍ����T���v���ɐݒ肷��
    glUniform1i(pointLoc, 0);

    // �F�e�N�X�`���̃T���v�����O�Ɏg�p����e�N�X�`�����j�b�g�ԍ����T���v���ɐݒ肷��
    glUniform1i(colorLoc, 1);

    // ���b�V����`�悷��
    bind();
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, vertexCount, primitiveCount);
  }
};
