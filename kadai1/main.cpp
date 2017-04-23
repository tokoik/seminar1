#ifdef _WIN32
#  include <Windows.h>
#  define NOTIFY(msg) MessageBox(NULL, TEXT(msg), TEXT("メディアデザインセミナー１"), MB_ICONERROR | MB_OK)
#else
#  define NOTIFY(msg) std::cerr << msg << '\n'
#endif
#include <cassert>

// ウィンドウ関連の処理
#include "Window.h"

// メッシュ
#include "Mesh.h"

// 共通の関数
#include "common.h"

// OpenCV
#include <opencv2/opencv.hpp>

namespace
{
  // 光源
  const GgSimpleShader::Light light =
  {
    { 0.2f, 0.2f, 0.2f, 1.0f },                           // 環境光成分
    { 1.0f, 1.0f, 1.0f, 0.0f },                           // 拡散反射光成分
    { 1.0f, 1.0f, 1.0f, 0.0f },                           // 鏡面光成分
    { 0.0f, 0.0f, 1.0f, 0.0f }                            // 位置
  };

  // 材質
  const GgSimpleShader::Material material =
  {
    { 0.8f, 0.8f, 0.8f, 1.0f },                           // 環境光の反射係数
    { 0.8f, 0.8f, 0.8f, 1.0f },                           // 拡散反射係数
    { 0.2f, 0.2f, 0.2f, 1.0f },                           // 鏡面反射係数
    50.0f                                                 // 輝き係数
  };

  // データ
  struct Data
  {
    // 点データのテクスチャ
    GLuint point;

    // 色データのテクスチャ
    GLuint color;

    // データのインスタンスのポインタ
    static Data *pData;

    // コンストラクタ
    Data()
    {
      glGenTextures(1, &point);
      glGenTextures(1, &color);
      if (!pData) pData = this;
    }

    // デストラクタ
    virtual ~Data()
    {
      glDeleteTextures(1, &point);
      glDeleteTextures(1, &color);
    }

    // 点データの作成
    void createPoint(GLsizei width, GLsizei height, const GLvoid *data = nullptr) const
    {
      // 点データのテクスチャを作成する
      glBindTexture(GL_TEXTURE_2D, point);
      glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, data);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    // 点データの転送
    void submitPoint(GLsizei width, GLsizei height, const GLvoid *data) const
    {
      // 点データのテクスチャにデータを転送する
      glBindTexture(GL_TEXTURE_2D, point);
      glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
      glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_FLOAT, data);
    }

    // 色データの作成
    void createColor(GLsizei width, GLsizei height, GLenum format, const GLvoid *data = nullptr) const
    {
      // 色データのテクスチャを作成する
      glBindTexture(GL_TEXTURE_2D, color);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    // 色データの転送
    void submitColor(GLsizei width, GLsizei height, GLenum format, const GLvoid *data) const
    {
      // 色データのテクスチャにデータを転送する
      glBindTexture(GL_TEXTURE_2D, color);
      glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, data);
    }
  };

  // データのポインタ
  Data *Data::pData(nullptr);

  // メッシュの幅と高さ
  int pointSize[2];

  // cv::Mat の type() を OpenGL の format に変換する
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

// 点データの作成
void createPoint(int width, int height, const void *point)
{
  pointSize[0] = width;
  pointSize[1] = height;

  Data::pData->createPoint(width, height, point);
}

// 点データの作成
void createPoint(const cv::Mat &point)
{
  assert(point.type() == CV_32FC3);
  createPoint(point.cols, point.rows, point.data);
}

// 点データの転送
void submitPoint(int width, int height, const void *point)
{
  Data::pData->submitPoint(width, height, point);
}

// 点データの転送
void submitPoint(const cv::Mat &point)
{
  assert(point.type() == CV_32FC3);
  submitPoint(point.cols, point.rows, point.data);
}

// 色データの作成
void createColor(int width, int height, const void *color)
{
  Data::pData->createColor(width, height, GL_BGR, color);
}

// 色データの作成
void createColor(const cv::Mat &color)
{
  Data::pData->createColor(color.cols, color.rows, toFormat(color.type()), color.data);
}

// 色データの転送
void submitColor(int width, int height, const void *color)
{
  Data::pData->submitColor(width, height, GL_BGR, color);
}

// 色データの転送
void submitColor(const cv::Mat &color)
{
  Data::pData->submitColor(color.cols, color.rows, toFormat(color.type()), color.data);
}

// 時刻の取得
float getTime()
{
  return static_cast<float>(glfwGetTime());
}

//
// メインプログラム
//
int main()
{
  // ウィンドウを開く
  Window window;

  // ウィンドウが開けたかどうかテスト
  if (!window.get())
  {
    NOTIFY("ウィンドウが開けませんでした");
    return EXIT_FAILURE;
  }

  // データを作成する
  Data data;

  // 設定
  if (!setup())
  {
    NOTIFY("初期化に失敗しました");
    return EXIT_FAILURE;
  }

  // メッシュを作成する
  Mesh mesh(pointSize);

  // ビュー変換行列を求める
  const GgMatrix mv(ggLookat(0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f));

  // 陰面消去を行う
  glEnable(GL_DEPTH_TEST);

  // 背面ポリゴンを描画する
  glDisable(GL_CULL_FACE);

  // 背景色を設定する
  glClearColor(0.3f, 0.4f, 0.5f, 0.0f);

  // ウィンドウを閉じるまで繰り返す
  while (!window.shouldClose())
  {
    // 更新
    if (!update()) break;

    // 画面をクリアする
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // シェーダを有効にする
    mesh.shader.use();

    // 光源と材質を設定する
    mesh.shader.loadLight(light);
    mesh.shader.loadMaterial(material);

    // 点テクスチャのサンプリングに使用するテクスチャユニットを設定する
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, data.point);

    // 色テクスチャのサンプリングに使用するテクスチャユニットを設定する
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, data.color);

    // 変換行列を設定する
    mesh.shader.loadMatrix(ggPerspective(1.0f, window.getAspect(), 1.0f, 10.0f), mv * window.getLeftTrackball());

    // メッシュを描画する
    mesh.draw();

    // ダブルバッファリング
    window.swapBuffers();
  }
}
