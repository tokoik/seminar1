// MessageBox の準備
#if defined(_MSC_VER)
#  define NOMINMAX
#  include <Windows.h>
#  include <atlstr.h>
#elif defined(__APPLE__)
#  include <CoreFoundation/CoreFoundation.h>
#else
#  include <iostream>
#endif
#define HEADER_STR "Media Design Seminar 1A"

// アプリケーション本体
#include "GgApp.h"

// メッシュ
#include "Mesh.h"

// 共通の関数
#include "common.h"



namespace
{
  // 光源
  const GgSimpleShader::Light light
  {
    { 0.2f, 0.2f, 0.2f, 1.0f },                           // 環境光成分
    { 1.0f, 1.0f, 1.0f, 0.0f },                           // 拡散反射光成分
    { 1.0f, 1.0f, 1.0f, 0.0f },                           // 鏡面光成分
    { 0.0f, 0.0f, 1.0f, 0.0f }                            // 位置
  };

  // 材質
  const GgSimpleShader::Material material
  {
    { 0.8f, 0.8f, 0.8f, 1.0f },                           // 環境光の反射係数
    { 0.8f, 0.8f, 0.8f, 1.0f },                           // 拡散反射係数
    { 0.2f, 0.2f, 0.2f, 1.0f },                           // 鏡面反射係数
    50.0f                                                 // 輝き係数
  };

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
    case CV_8UC3:
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
      return GL_BGR;
    case CV_8UC4:
      glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
      break;
    default:
      assert(false);
    }

    return GL_BGRA;
  }

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

    // 色データの作成 (cv::Mat 版)
    void createColor(const cv::Mat &data) const
    {
      // cv::Mat の画像のフォーマットを OpenGL 用に変換する
      const GLenum format{ toFormat(data.type()) };

      // 色データのテクスチャを作成する
      createColor(data.cols, data.rows, format, nullptr);
      for (int y = 0; y < data.rows; ++y)
      {
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, data.rows - y - 1, data.cols, 1,
          format, GL_UNSIGNED_BYTE, data.data + data.step * y);
      }
    }

    // 色データの転送 (cv::Mat 版)
    void submitColor(const cv::Mat &data) const
    {
      // cv::Mat の画像のフォーマットを OpenGL 用に変換する
      const GLenum format{ toFormat(data.type()) };

      // 色データのテクスチャにデータを転送する
      glBindTexture(GL_TEXTURE_2D, color);
      for (int y = 0; y < data.rows; ++y)
      {
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, data.rows - y - 1, data.cols, 1,
          format, GL_UNSIGNED_BYTE, data.data + data.step * y);
      }
    }
  };

  // データのポインタ
  Data *Data::pData(nullptr);

  // メッシュの幅と高さ
  int pointSize[2];
}

// 点データの作成
void createPoint(int slices, int stacks, const void *point)
{
  pointSize[0] = slices;
  pointSize[1] = stacks;

  Data::pData->createPoint(slices, stacks, point);
}

// 点データの作成
void createPoint(const cv::Mat &point)
{
  assert(point.type() == CV_32FC3);
  createPoint(point.cols, point.rows, point.data);
}

// 点データの転送
void submitPoint(int slices, int stacks, const void *point)
{
  Data::pData->submitPoint(slices, stacks, point);
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

// 色データの転送
void submitColor(int width, int height, const void *color)
{
  Data::pData->submitColor(width, height, GL_BGR, color);
}

// 色データの作成 (cv::Mat 版)
void createColor(const cv::Mat &color)
{
  Data::pData->createColor(color);
}

// 色データの転送 (cv::Mat 版)
void submitColor(const cv::Mat &color)
{
  Data::pData->submitColor(color);
}

// 時刻の取得
float getTime()
{
  return static_cast<float>(glfwGetTime());
}

//
// メインプログラム
//
int main(int argc, const char* const* argv) try
{
  // ウィンドウ関連の初期設定
  Window::initialize(4, 1);

  // アプリケーション本体
	GgApp app;

	// アプリケーションを実行する
	app.main(argc, argv);
}
catch (const std::exception &e)
{
  // エラーメッセージを表示する
#if defined(_MSC_VER)
  MessageBox(NULL, CString(e.what()), TEXT(HEADER_STR), MB_ICONERROR);
#elif defined(__APPLE__)
  // the following code is copied from http://blog.jorgearimany.com/2010/05/messagebox-from-windows-to-mac.html
  // convert the strings from char* to CFStringRef
  CFStringRef msg_ref = CFStringCreateWithCString(NULL, e.what(), kCFStringEncodingUTF8);

  // result code from the message box
  CFOptionFlags result;

  //launch the message box
  CFUserNotificationDisplayAlert(
    0,                                 // no timeout
    kCFUserNotificationNoteAlertLevel, // change it depending message_type flags ( MB_ICONASTERISK.... etc.)
    NULL,                              // icon url, use default, you can change it depending message_type flags
    NULL,                              // not used
    NULL,                              // localization of strings
    CFSTR(HEADER_STR),                 // header text
    msg_ref,                           // message text
    NULL,                              // default "ok" text in button
    NULL,                              // alternate button title
    NULL,                              // other button title, null--> no other button
    &result                            // response flags
  );

  // Clean up the strings
  CFRelease(msg_ref);
#else
  std::cerr << HEADER_STR << ": " << e.what() << '\n';
#endif

  // ブログラムを終了する
  return EXIT_FAILURE;
}

//
// アプリケーション本体
//
int GgApp::main(int argc, const char* const* argv)
{
  // ウィンドウを開く
  Window window;

  // データを作成する
  Data data;

  // 設定
  if (!setup()) throw "Failed to initialize";

  // 光源
  GgSimpleShader::LightBuffer lightBuffer{ light };

  // 材質
  GgSimpleShader::MaterialBuffer materialBuffer{ material };

  // メッシュを作成する
  Mesh mesh{ pointSize };

  // ビュー変換行列を求める
  const GgMatrix mv{ ggLookat(0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f) };

  // 陰面消去を行う
  glEnable(GL_DEPTH_TEST);

  // 背面ポリゴンを描画する
  glDisable(GL_CULL_FACE);

  // 背景色を設定する
  glClearColor(0.3f, 0.4f, 0.5f, 0.0f);

  // ウィンドウを閉じるまで繰り返す
  while (window)
  {
    // 更新
    if (!update()) break;

    // 画面をクリアする
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 投影変換行列
    const GgMatrix mp{ ggPerspective(1.0f, window.getAspect(), 1.0f, 10.0f) };

    // シェーダを有効にする
    mesh.shader.use(mp, mv * window.getRotationMatrix());

    // 光源と材質を設定する
    lightBuffer.select();
    materialBuffer.select();

    // 点テクスチャのサンプリングに使用するテクスチャユニットを設定する
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, data.point);

    // 色テクスチャのサンプリングに使用するテクスチャユニットを設定する
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, data.color);

    // メッシュを描画する
    mesh.draw();

    // ダブルバッファリング
    window.swapBuffers();
  }

  return EXIT_SUCCESS;
}
