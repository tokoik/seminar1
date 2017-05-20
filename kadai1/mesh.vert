#version 150 core

//
// ���b�V���̕`��
//

// �_�f�[�^�̃e�N�X�`��
uniform sampler2D point;

// �ϊ��s��
uniform mat4 mp;                                    // ���e�ϊ��s��
uniform mat4 mv;                                    // ���f���r���[�ϊ��s��
uniform mat4 mn;                                    // �@���ϊ��s��

// ����
uniform vec4 lamb;                                  // ��������
uniform vec4 ldiff;                                 // �g�U���ˌ�����
uniform vec4 lspec;                                 // ���ʔ��ˌ�����
uniform vec4 lpos;                                  // �ʒu

// �ގ�
uniform vec4 kamb;                                  // �����̔��ˌW��
uniform vec4 kdiff;                                 // �g�U���ˌW��
uniform vec4 kspec;                                 // ���ʔ��ˌW��
uniform float kshi;                                 // �P���W��

// ���X�^���C�U�ɑ��钸�_����
out vec2 texcoord;                                  // �e�N�X�`�����W
out vec4 idiff;                                     // �g�U���ˌ����x
out vec4 ispec;                                     // ���ʔ��ˌ����x

void main(void)
{
  // �e�N�X�`�����W
  int x = gl_VertexID >> 1;
  int y = 1 - (gl_VertexID & 1) + gl_InstanceID;
  texcoord = (vec2(x, y) + 0.5) / vec2(textureSize(point, 0));

  // �ʒu�C�����C�����C����
  vec4 p = mv * texture(point, texcoord);
  vec3 v = normalize(p.xyz / p.w);
  vec3 l = normalize((lpos * p.w - p * lpos.w).xyz);
  vec3 h = normalize(l - v);

  // �ڐ��C�]�ڐ��C�@��
  vec3 t = (textureOffset(point, texcoord, ivec2(1, 0)) - textureOffset(point, texcoord, ivec2(-1, 0))).xyz;
  vec3 b = (textureOffset(point, texcoord, ivec2(0, 1)) - textureOffset(point, texcoord, ivec2(0, -1))).xyz;
  vec3 n = normalize(mat3(mn) * cross(t, b));

  // �A�e
  idiff = max(dot(n, l), 0.0) * kdiff * ldiff + kamb * lamb;
  ispec = pow(max(dot(n, h), 0.0), kshi) * kspec * lspec;

  // ���_�ʒu
  gl_Position = mp * p;
}
