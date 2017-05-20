#ifdef _WIN32
#  include <Windows.h>
#  define NOTIFY(msg) MessageBox(NULL, TEXT(msg), TEXT("���f�B�A�f�U�C���Z�~�i�[�P"), MB_ICONERROR | MB_OK)
#else
#  define NOTIFY(msg) std::cerr << msg << '\n'
#endif
#include <cassert>

// �E�B���h�E�֘A�̏���
#include "Window.h"

// ���b�V��
#include "Mesh.h"

// ���ʂ̊֐�
#include "common.h"

// OpenCV
#include <opencv2/opencv.hpp>

namespace
{
  // ����
  const GgSimpleShader::Light light =
  {
    { 0.2f, 0.2f, 0.2f, 1.0f },                           // ��������
    { 1.0f, 1.0f, 1.0f, 0.0f },                           // �g�U���ˌ�����
    { 1.0f, 1.0f, 1.0f, 0.0f },                           // ���ʌ�����
    { 0.0f, 0.0f, 1.0f, 0.0f }                            // �ʒu
  };

  // �ގ�
  const GgSimpleShader::Material material =
  {
    { 0.8f, 0.8f, 0.8f, 1.0f },                           // �����̔��ˌW��
    { 0.8f, 0.8f, 0.8f, 1.0f },                           // �g�U���ˌW��
    { 0.2f, 0.2f, 0.2f, 1.0f },                           // ���ʔ��ˌW��
    50.0f                                                 // �P���W��
  };

  // �f�[�^
  struct Data
  {
    // �_�f�[�^�̃e�N�X�`��
    GLuint point;

    // �F�f�[�^�̃e�N�X�`��
    GLuint color;

    // �f�[�^�̃C���X�^���X�̃|�C���^
    static Data *pData;

    // �R���X�g���N�^
    Data()
    {
      glGenTextures(1, &point);
      glGenTextures(1, &color);
      if (!pData) pData = this;
    }

    // �f�X�g���N�^
    virtual ~Data()
    {
      glDeleteTextures(1, &point);
      glDeleteTextures(1, &color);
    }

    // �_�f�[�^�̍쐬
    void createPoint(GLsizei width, GLsizei height, const GLvoid *data = nullptr) const
    {
      // �_�f�[�^�̃e�N�X�`�����쐬����
      glBindTexture(GL_TEXTURE_2D, point);
      glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, data);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    // �_�f�[�^�̓]��
    void submitPoint(GLsizei width, GLsizei height, const GLvoid *data) const
    {
      // �_�f�[�^�̃e�N�X�`���Ƀf�[�^��]������
      glBindTexture(GL_TEXTURE_2D, point);
      glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
      glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_FLOAT, data);
    }

    // �F�f�[�^�̍쐬
    void createColor(GLsizei width, GLsizei height, GLenum format, const GLvoid *data = nullptr) const
    {
      // �F�f�[�^�̃e�N�X�`�����쐬����
      glBindTexture(GL_TEXTURE_2D, color);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    // �F�f�[�^�̓]��
    void submitColor(GLsizei width, GLsizei height, GLenum format, const GLvoid *data) const
    {
      // �F�f�[�^�̃e�N�X�`���Ƀf�[�^��]������
      glBindTexture(GL_TEXTURE_2D, color);
      glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, data);
    }
  };

  // �f�[�^�̃|�C���^
  Data *Data::pData(nullptr);

  // ���b�V���̕��ƍ���
  int pointSize[2];

  // cv::Mat �� type() �� OpenGL �� format �ɕϊ�����
  GLenum toFormat(int type)
  {
    switch (type)
    {
    case CV_8UC1:
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
      return GL_RED;
    case CV_8UC2:
      glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
      return GL_RG;
    case CV_8UC4:
      glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
      return GL_BGRA;
    default:
      assert(false);
    case CV_8UC3:
      break;
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    return GL_BGR;
  }
}

// �_�f�[�^�̍쐬
void createPoint(int width, int height, const void *point)
{
  pointSize[0] = width;
  pointSize[1] = height;

  Data::pData->createPoint(width, height, point);
}

// �_�f�[�^�̍쐬
void createPoint(const cv::Mat &point)
{
  assert(point.type() == CV_32FC3);
  createPoint(point.cols, point.rows, point.data);
}

// �_�f�[�^�̓]��
void submitPoint(int width, int height, const void *point)
{
  Data::pData->submitPoint(width, height, point);
}

// �_�f�[�^�̓]��
void submitPoint(const cv::Mat &point)
{
  assert(point.type() == CV_32FC3);
  submitPoint(point.cols, point.rows, point.data);
}

// �F�f�[�^�̍쐬
void createColor(int width, int height, const void *color)
{
  Data::pData->createColor(width, height, GL_BGR, color);
}

// �F�f�[�^�̍쐬
void createColor(const cv::Mat &color)
{
  Data::pData->createColor(color.cols, color.rows, toFormat(color.type()), color.data);
}

// �F�f�[�^�̓]��
void submitColor(int width, int height, const void *color)
{
  Data::pData->submitColor(width, height, GL_BGR, color);
}

// �F�f�[�^�̓]��
void submitColor(const cv::Mat &color)
{
  Data::pData->submitColor(color.cols, color.rows, toFormat(color.type()), color.data);
}

// �����̎擾
float getTime()
{
  return static_cast<float>(glfwGetTime());
}

//
// ���C���v���O����
//
int main()
{
  // �E�B���h�E���J��
  Window window;

  // �E�B���h�E���J�������ǂ����e�X�g
  if (!window.get())
  {
    NOTIFY("�E�B���h�E���J���܂���ł���");
    return EXIT_FAILURE;
  }

  // �f�[�^���쐬����
  Data data;

  // �ݒ�
  if (!setup())
  {
    NOTIFY("�������Ɏ��s���܂���");
    return EXIT_FAILURE;
  }

  // ���b�V�����쐬����
  Mesh mesh(pointSize);

  // �r���[�ϊ��s������߂�
  const GgMatrix mv(ggLookat(0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f));

  // �A�ʏ������s��
  glEnable(GL_DEPTH_TEST);

  // �w�ʃ|���S����`�悷��
  glDisable(GL_CULL_FACE);

  // �w�i�F��ݒ肷��
  glClearColor(0.3f, 0.4f, 0.5f, 0.0f);

  // �E�B���h�E�����܂ŌJ��Ԃ�
  while (!window.shouldClose())
  {
    // �X�V
    if (!update()) break;

    // ��ʂ��N���A����
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // �V�F�[�_��L���ɂ���
    mesh.shader.use();

    // �����ƍގ���ݒ肷��
    mesh.shader.loadLight(light);
    mesh.shader.loadMaterial(material);

    // �_�e�N�X�`���̃T���v�����O�Ɏg�p����e�N�X�`�����j�b�g��ݒ肷��
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, data.point);

    // �F�e�N�X�`���̃T���v�����O�Ɏg�p����e�N�X�`�����j�b�g��ݒ肷��
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, data.color);

    // �ϊ��s���ݒ肷��
    mesh.shader.loadMatrix(ggPerspective(1.0f, window.getAspect(), 1.0f, 10.0f), mv * window.getLeftTrackball());

    // ���b�V����`�悷��
    mesh.draw();

    // �_�u���o�b�t�@�����O
    window.swapBuffers();
  }
}
