#version 150 core
#extension GL_ARB_explicit_attrib_location : enable

//
// ���b�V���̕`��
//

// �F�f�[�^�̃e�N�X�`��
uniform sampler2D color;

// ���X�^���C�U����󂯎�钸�_����
in vec2 texcoord;                                   // �e�N�X�`�����W
in vec4 idiff;                                      // �g�U���ˌ����x
in vec4 ispec;                                      // ���ʔ��ˌ����x

// �t���[���o�b�t�@�ւ̏o��
layout (location = 0) out vec4 fc;                  // �t���O�����g�̐F

void main(void)
{
  // ��f�̉A�e�����߂�
  fc = idiff * texture(color, texcoord) + ispec;
}
