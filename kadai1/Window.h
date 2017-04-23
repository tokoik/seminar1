#pragma once

/*
** ゲームグラフィックス特論用補助プログラム GLFW3 版
**

Copyright (c) 2011-2017 Kohe Tokoi. All Rights Reserved.

Permission is hereby granted, free of charge,  to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction,  including without limitation the rights
to use, copy,  modify, merge,  publish, distribute,  sublicense,  and/or sell
copies or substantial portions of the Software.

The above  copyright notice  and this permission notice  shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE  IS PROVIDED "AS IS",  WITHOUT WARRANTY OF ANY KIND,  EXPRESS OR
IMPLIED,  INCLUDING  BUT  NOT LIMITED  TO THE WARRANTIES  OF MERCHANTABILITY,
FITNESS  FOR  A PARTICULAR PURPOSE  AND NONINFRINGEMENT.  IN  NO EVENT  SHALL
KOHE TOKOI  BE LIABLE FOR ANY CLAIM,  DAMAGES OR OTHER LIABILITY,  WHETHER IN
AN ACTION  OF CONTRACT,  TORT  OR  OTHERWISE,  ARISING  FROM,  OUT OF  OR  IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

**
*/

// 補助プログラム
#include "gg.h"
using namespace gg;

// 標準ライブラリ
#include <cstdlib>
#include <iostream>

// Oculus Rift DK2 を使用するなら非 0
#define USE_OCULUS_RIFT_DK2 0

// Oculus Rift SDK ライブラリ (LibOVR) の組み込み
#if USE_OCULUS_RIFT_DK2
#  if defined(_WIN32)
#    define GLFW_EXPOSE_NATIVE_WIN32
#    define GLFW_EXPOSE_NATIVE_WGL
#    include "glfw3native.h"
#    define OVR_OS_WIN32
#    undef APIENTRY
#    pragma comment(lib, "LibOVR.lib")
#  endif
#  include <OVR_CAPI_GL.h>
#  include <Extras/OVR_Math.h>
#endif

class Window
{
  // ウィンドウの識別子
  GLFWwindow *window;

  // ウィンドウのサイズ
  GLsizei width, height;

  // ウィンドウのアスペクト比
  GLfloat aspect;

  // シフトキー
  bool shift_key;

  // コントロールキー
  bool control_key;

  // 矢印キー
  int arrow[2];

  // シフトキーを押しながら矢印キー
  int shift_arrow[2];

  // コントロールキーを押しながら矢印キー
  int control_arrow[2];

  // マウスの現在位置
  double mouse_x, mouse_y;

  // マウスホイールの回転量
  double wheel_rotation;

  // シフトを押しながらマウスホイールの回転量
  double shift_wheel_rotation;

  // コントロールを押しながらマウスホイールの回転量
  double control_wheel_rotation;

  // 左ドラッグによるトラックボール
  GgTrackball trackball_left;

  // 右ドラッグによるトラックボール
  GgTrackball trackball_right;

  // コピーコンストラクタを封じる
  Window(const Window &w);

  // 代入を封じる
  Window &operator=(const Window &w);

#if USE_OCULUS_RIFT_DK2
  // Oculus Rift のセッション
  ovrSession session;

  // Oculus Rift の情報
  ovrHmdDesc hmdDesc;

  // Oculus Rift のレンダリング情報
  ovrEyeRenderDesc eyeRenderDesc[ovrEye_Count];

  // Oculus Rift の視点情報
  ovrPosef eyePose[ovrEye_Count];

  // Oculus Rift に転送する描画データ
  ovrLayer_Union layerData;

  // Oculus Rift のスクリーンのサイズ
  GLfloat screen[ovrEye_Count][4];

  // Oculus Rift 表示用の FBO
  GLuint oculusFbo[ovrEye_Count];

  // ミラー表示用のレンダリングターゲットのテクスチャ
  ovrGLTexture *mirrorTexture;

  // ミラー表示用の FBO
  GLuint mirrorFbo;
#endif

public:

  // コンストラクタ
  Window(const char *title = "GLFW Window", int width = 640, int height = 480,
    int fullscreen = 0, GLFWwindow *share = nullptr)
    : window(nullptr)
  {
    // 初期化済なら true
    static bool initialized(false);

    // GLFW が初期化されていなければ
    if (!initialized)
    {
#if USE_OCULUS_RIFT_DK2
      // Oculus Rift (LibOVR) を初期化する
      if (OVR_FAILURE(ovr_Initialize(nullptr))) return;

      // プログラム終了時には LibOVR を終了する
      atexit(ovr_Shutdown);

      // LUID は OpenGL では使っていないらしい
      ovrGraphicsLuid luid;

      // Oculus Rift のセッションを作成する
      if (OVR_FAILURE(ovr_Create(&session, &luid))) return;
#endif

      // GLFW を初期化する
      if (glfwInit() == GL_FALSE) return;

      // プログラム終了時の処理を登録
      atexit(glfwTerminate);

      // OpenGL Version 3.2 Core Profile を選択する
      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
      glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
      glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

      // マウス操作の初期値を設定するする
      arrow[0] = arrow[1] = 0;
      shift_arrow[0] = shift_arrow[1] = 0;
      control_arrow[0] = control_arrow[1] = 0;
      wheel_rotation = shift_wheel_rotation = control_wheel_rotation = 0.0;

      // 初期化済みの印をつける
      initialized = true;
    }

    // ディスプレイの情報
    GLFWmonitor *monitor(nullptr);

    // フルスクリーン表示
    if (fullscreen > 0)
    {
      // 接続されているモニタの数を数える
      int mcount;
      GLFWmonitor **const monitors = glfwGetMonitors(&mcount);

      // セカンダリモニタがあればそれを使う
      if (fullscreen > mcount) fullscreen = mcount;
      monitor = monitors[fullscreen - 1];

      // モニタのモードを調べる
      const GLFWvidmode *mode(glfwGetVideoMode(monitor));

      // ウィンドウのサイズをディスプレイのサイズにする
      width = mode->width;
      height = mode->height;
    }

    // GLFW のウィンドウを作成する
    window = glfwCreateWindow(width, height, title, monitor, share);

    // ウィンドウが作成できなければ戻る
    if (!window) return;

    // 現在のウィンドウを処理対象にする
    glfwMakeContextCurrent(window);

    // ゲームグラフィックス特論の都合による初期化を行う
    ggInit();

    // このインスタンスの this ポインタを記録しておく
    glfwSetWindowUserPointer(window, this);

    // キーボードを操作した時の処理を登録する
    glfwSetKeyCallback(window, keyboard);

    // マウスボタンを操作したときの処理を登録する
    glfwSetMouseButtonCallback(window, mouse);

    // マウスホイール操作時に呼び出す処理を登録する
    glfwSetScrollCallback(window, wheel);

#if USE_OCULUS_RIFT_DK2
    // Oculus Rift の情報を取り出す
    hmdDesc = ovr_GetHmdDesc(session);

#  if defined(_DEBUG)
    // Oculus Rift の情報を表示する
    std::cout
      << "\nProduct name: " << hmdDesc.ProductName
      << "\nResolution:   " << hmdDesc.Resolution.w << " x " << hmdDesc.Resolution.h
      << "\nDefault Fov:  (" << hmdDesc.DefaultEyeFov[ovrEye_Left].LeftTan
      << "," << hmdDesc.DefaultEyeFov[ovrEye_Left].DownTan
      << ") - (" << hmdDesc.DefaultEyeFov[ovrEye_Left].RightTan
      << "," << hmdDesc.DefaultEyeFov[ovrEye_Left].UpTan
      << ")\n              (" << hmdDesc.DefaultEyeFov[ovrEye_Right].LeftTan
      << "," << hmdDesc.DefaultEyeFov[ovrEye_Right].DownTan
      << ") - (" << hmdDesc.DefaultEyeFov[ovrEye_Right].RightTan
      << "," << hmdDesc.DefaultEyeFov[ovrEye_Right].UpTan
      << ")\nMaximum Fov:  (" << hmdDesc.MaxEyeFov[ovrEye_Left].LeftTan
      << "," << hmdDesc.MaxEyeFov[ovrEye_Left].DownTan
      << ") - (" << hmdDesc.MaxEyeFov[ovrEye_Left].RightTan
      << "," << hmdDesc.MaxEyeFov[ovrEye_Left].UpTan
      << ")\n              (" << hmdDesc.MaxEyeFov[ovrEye_Right].LeftTan
      << "," << hmdDesc.MaxEyeFov[ovrEye_Right].DownTan
      << ") - (" << hmdDesc.MaxEyeFov[ovrEye_Right].RightTan
      << "," << hmdDesc.MaxEyeFov[ovrEye_Right].UpTan
      << ")\n" << std::endl;
#  endif

    // Oculus Rift 表示用の FBO を作成する
    for (int eye = 0; eye < ovrEye_Count; ++eye)
    {
      // Oculus Rift 表示用の FBO のサイズ
      const auto renderTargetSize(ovr_GetFovTextureSize(session, ovrEyeType(eye), hmdDesc.DefaultEyeFov[eye], 1.0f));

      // Oculus Rift のレンズ補正等の設定値
      eyeRenderDesc[eye] = ovr_GetRenderDesc(session, ovrEyeType(eye), hmdDesc.DefaultEyeFov[eye]);

      // Oculus Rift 表示用の FBO のカラーバッファとして使うテクスチャセットの作成
      ovrSwapTextureSet *colorTexture;
      ovr_CreateSwapTextureSetGL(session, GL_SRGB8_ALPHA8, renderTargetSize.w, renderTargetSize.h, &colorTexture);

      // Oculus Rift 表示用の FBO のデプスバッファとして使うテクスチャセットの作成
      ovrSwapTextureSet *depthTexture;
      ovr_CreateSwapTextureSetGL(session, GL_DEPTH_COMPONENT32F, renderTargetSize.w, renderTargetSize.h, &depthTexture);

      // Oculus Rift に転送する描画データを作成する
      layerData.Header.Type = ovrLayerType_EyeFovDepth;
      layerData.Header.Flags = ovrLayerFlag_TextureOriginAtBottomLeft;   // OpenGL なので左下が原点
      layerData.EyeFov.ColorTexture[eye] = colorTexture;
      layerData.EyeFovDepth.DepthTexture[eye] = depthTexture;
      layerData.EyeFov.Viewport[eye].Pos = OVR::Vector2i(0, 0);
      layerData.EyeFov.Viewport[eye].Size = renderTargetSize;
      layerData.EyeFov.Fov[eye] = hmdDesc.DefaultEyeFov[eye];

      // Oculus Rift の視野を取得する
      const auto fov(eyeRenderDesc[eye].Fov);

      // ウインドウ (表示領域) を保存する
      screen[eye][0] = -fov.LeftTan;
      screen[eye][1] = fov.RightTan;
      screen[eye][2] = -fov.DownTan;
      screen[eye][3] = fov.UpTan;
    }

    // ミラー表示用の FBO を作成する
    if (OVR_SUCCESS(ovr_CreateMirrorTextureGL(session, GL_SRGB8_ALPHA8, width, height, reinterpret_cast<ovrTexture **>(&mirrorTexture))))
    {
      glGenFramebuffers(1, &mirrorFbo);
      glBindFramebuffer(GL_READ_FRAMEBUFFER, mirrorFbo);
      glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mirrorTexture->OGL.TexId, 0);
      glFramebufferRenderbuffer(GL_READ_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
      glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    }

    // Oculus Rift のレンダリング用の FBO を作成する
    glGenFramebuffers(ovrEye_Count, oculusFbo);

    // Oculus Rift にレンダリングするときは sRGB カラースペースを使う
    glEnable(GL_FRAMEBUFFER_SRGB);
#else
    // ウィンドウのサイズ変更時に呼び出す処理を登録する
    glfwSetFramebufferSizeCallback(window, resize);

    // ビューポートと投影変換行列を初期化する
    resize(window, width, height);

    // 垂直帰線区間を待つ
    glfwSwapInterval(1);
#endif
  }

  // デストラクタ
  virtual ~Window()
  {
    // ウィンドウが作成されていれば
    if (window)
    {
#if USE_OCULUS_RIFT_DK2
      // ミラー表示用の FBO を削除する
      glDeleteFramebuffers(1, &mirrorFbo);

      // ミラー表示に使ったテクスチャを開放する
      glDeleteTextures(1, &mirrorTexture->OGL.TexId);
      ovr_DestroyMirrorTexture(session, reinterpret_cast<ovrTexture *>(mirrorTexture));

      // Oculus Rift 表示用の FBO を削除する
      for (int eye = 0; eye < ovrEye_Count; ++eye)
      {
        // Oculus Rift のレンダリング用の FBO を削除する
        glDeleteFramebuffers(1, &oculusFbo[eye]);

        // レンダリングターゲットに使ったテクスチャを開放する
        auto *const colorTexture(layerData.EyeFov.ColorTexture[eye]);
        for (int i = 0; i < colorTexture->TextureCount; ++i)
        {
          const auto *const ctex(reinterpret_cast<ovrGLTexture *>(&colorTexture->Textures[i]));
          glDeleteTextures(1, &ctex->OGL.TexId);
        }
        ovr_DestroySwapTextureSet(session, colorTexture);

        // デプスバッファとして使ったテクスチャを開放する
        auto *const depthTexture(layerData.EyeFovDepth.DepthTexture[eye]);
        for (int i = 0; i < depthTexture->TextureCount; ++i)
        {
          const auto *const dtex(reinterpret_cast<ovrGLTexture *>(&depthTexture->Textures[i]));
          glDeleteTextures(1, &dtex->OGL.TexId);
        }
        ovr_DestroySwapTextureSet(session, depthTexture);
      }

      // Oculus Rift のセッションを破棄する
      ovr_Destroy(session);
#endif

      // ウィンドウを破棄する
      glfwDestroyWindow(window);
    }
  }

  // ウィンドウの識別子の取得
  GLFWwindow *get() const
  {
    return window;
  }

  // ウィンドウを閉じるべきかを判定する
  bool shouldClose()
  {
    // ウィンドウを閉じるか ESC キーがタイプされていれば真を返す
    return glfwWindowShouldClose(window) || glfwGetKey(window, GLFW_KEY_ESCAPE);
  }

#if USE_OCULUS_RIFT_DK2
  // 立体視による描画開始
  void start()
  {
    // フレームのタイミング計測開始
    const auto ftiming(ovr_GetPredictedDisplayTime(session, 0));

    // sensorSampleTime の取得は可能な限り ovr_GetTrackingState() の近くで行う
    layerData.EyeFov.SensorSampleTime = ovr_GetTimeInSeconds();

    // ヘッドトラッキングの状態を取得する
    const auto hmdState(ovr_GetTrackingState(session, ftiming, ovrTrue));

    // 正しい瞳孔間隔をもとに視点の姿勢を取得する
    const ovrVector3f viewOffset[] = { eyeRenderDesc[0].HmdToEyeViewOffset, eyeRenderDesc[1].HmdToEyeViewOffset };
    ovr_CalcEyePoses(hmdState.HeadPose.ThePose, viewOffset, eyePose);
  }

  // 描画する目の指定
  void select(int eye, GLfloat *position, GgQuaternion *orientation)
  {
    // レンダーターゲットに描画する前にレンダーターゲットのインデックスをインクリメントする
    auto *const colorTexture(layerData.EyeFov.ColorTexture[eye]);
    colorTexture->CurrentIndex = (colorTexture->CurrentIndex + 1) % colorTexture->TextureCount;
    auto *const depthTexture(layerData.EyeFovDepth.DepthTexture[eye]);
    depthTexture->CurrentIndex = (depthTexture->CurrentIndex + 1) % depthTexture->TextureCount;

    // レンダーターゲットを切り替える
    glBindFramebuffer(GL_FRAMEBUFFER, oculusFbo[eye]);
    const auto &ctex(reinterpret_cast<ovrGLTexture *>(&colorTexture->Textures[colorTexture->CurrentIndex]));
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ctex->OGL.TexId, 0);
    const auto &dtex(reinterpret_cast<ovrGLTexture *>(&depthTexture->Textures[depthTexture->CurrentIndex]));
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, dtex->OGL.TexId, 0);

    // ビューポートを設定して画面クリア
    const auto &vp(layerData.EyeFov.Viewport[eye]);
    glViewport(vp.Pos.x, vp.Pos.y, vp.Size.w, vp.Size.h);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Oculus Rift の左の位置と向きを取得する
    const auto &o(eyePose[eye].Orientation);
    const auto &p(eyePose[eye].Position);
    const auto &q(eyeRenderDesc[eye].HmdToEyeViewOffset);

    // Oculus Rift ヘッドトラッキング情報を求める
    position[0] = p.x + q.x;
    position[1] = p.y + q.y;
    position[2] = p.z + q.z;
    *orientation = GgQuaternion(o.x, o.y, o.z, o.w);
  }

  // Time Warp 処理に使う投影変換行列の成分の設定
  void setTimewarpProjectionDesc(const GgMatrix &projection)
  {
    // TimeWarp に使う変換行列の成分
    auto &posTimewarpProjectionDesc(layerData.EyeFovDepth.ProjectionDesc);
    posTimewarpProjectionDesc.Projection22 = (projection.get()[4 * 2 + 2] + projection.get()[4 * 3 + 2]) * 0.5f;
    posTimewarpProjectionDesc.Projection23 = projection.get()[4 * 2 + 3] * 0.5f;
    posTimewarpProjectionDesc.Projection32 = projection.get()[4 * 3 + 2];
  }
#endif

  // カラーバッファを入れ替えてイベントを取り出す
  void swapBuffers()
  {
    // エラーチェック
    ggError(__FILE__, __LINE__);

#if USE_OCULUS_RIFT_DK2
    // Oculus Rift 上の描画位置と拡大率を求める
    ovrViewScaleDesc viewScaleDesc;
    viewScaleDesc.HmdSpaceToWorldScaleInMeters = 1.0f;
    viewScaleDesc.HmdToEyeViewOffset[0] = eyeRenderDesc[0].HmdToEyeViewOffset;
    viewScaleDesc.HmdToEyeViewOffset[1] = eyeRenderDesc[1].HmdToEyeViewOffset;

    // 描画データを更新する
    layerData.EyeFov.RenderPose[0] = eyePose[0];
    layerData.EyeFov.RenderPose[1] = eyePose[1];

    // 描画データを Oculus Rift に転送する
    const auto *const layers(&layerData.Header);
    const auto result(ovr_SubmitFrame(session, 0, &viewScaleDesc, &layers, 1));
    // ここで転送に失敗したら Oculus Rift の設定を最初からやり直す必要があるらしい
    // けどめんどくさいのでしない

    // レンダリング結果をミラー表示用のフレームバッファにも転送する
    glBindFramebuffer(GL_READ_FRAMEBUFFER, mirrorFbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    const auto w(mirrorTexture->OGL.Header.TextureSize.w);
    const auto h(mirrorTexture->OGL.Header.TextureSize.h);
    glBlitFramebuffer(0, h, w, 0, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

    // 残っている OpenGL コマンドを実行する
    glFlush();
#else
    // カラーバッファを入れ替える
    glfwSwapBuffers(window);
#endif

    // シフトキーとコントロールキーの状態をリセットする
    shift_key = control_key = false;

    // イベントを取り出す
    glfwPollEvents();

    // マウスの位置を調べる
    glfwGetCursorPos(window, &mouse_x, &mouse_y);
    const GLfloat x(static_cast<GLfloat>(mouse_x));
    const GLfloat y(static_cast<GLfloat>(mouse_y));

    // 左ボタンドラッグ
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1)) trackball_left.motion(x, y);

    // 右ボタンドラッグ
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2)) trackball_right.motion(x, y);
  }

  // ウィンドウのサイズ変更時の処理
  static void resize(GLFWwindow *window, int width, int height)
  {
    // このインスタンスの this ポインタを得る
    Window *const instance(static_cast<Window *>(glfwGetWindowUserPointer(window)));

    if (instance)
    {
      // ウィンドウのサイズを保存する
      instance->width = width;
      instance->height = height;

      // ウィンドウのアスペクト比を保存する
      instance->aspect = static_cast<GLfloat>(width) / static_cast<GLfloat>(height);

      // トラックボール処理の範囲を設定する
      instance->trackball_left.region(width, height);
      instance->trackball_right.region(width, height);

      // ウィンドウ全体に描画する
      glViewport(0, 0, width, height);
    }
  }

  // キーボードをタイプした時の処理
  static void keyboard(GLFWwindow *window, int key, int scancode, int action, int mods)
  {
    // このインスタンスの this ポインタを得る
    Window *const instance(static_cast<Window *>(glfwGetWindowUserPointer(window)));

    if (instance)
    {
      if (action == GLFW_PRESS)
      {
        switch (key)
        {
        case GLFW_KEY_R:
          // マウスホイールの回転量をリセットする
          instance->wheel_rotation = 0.0;
          instance->shift_wheel_rotation = 0.0;
          instance->control_wheel_rotation = 0.0;

          // 矢印キーの設定値をリセットする
          instance->arrow[0] = instance->arrow[1] = 0;
          instance->shift_arrow[0] = instance->shift_arrow[1] = 0;
          instance->control_arrow[0] = instance->control_arrow[1] = 0;

        case GLFW_KEY_O:
          // トラックボールをリセットする
          instance->trackball_left.reset();
          instance->trackball_right.reset();
          break;

        case GLFW_KEY_SPACE:
          break;

        case GLFW_KEY_BACKSPACE:
        case GLFW_KEY_DELETE:
          break;

        case GLFW_KEY_LEFT_SHIFT:
        case GLFW_KEY_RIGHT_SHIFT:
          instance->shift_key = true;
          break;

        case GLFW_KEY_LEFT_CONTROL:
        case GLFW_KEY_RIGHT_CONTROL:
          instance->control_key = true;
          break;

        case GLFW_KEY_UP:
          if (instance->shift_key)
            instance->shift_arrow[1]++;
          else if (instance->control_key)
            instance->control_arrow[1]++;
          else
            instance->arrow[1]++;
          break;

        case GLFW_KEY_DOWN:
          if (instance->shift_key)
            instance->shift_arrow[1]--;
          else if (instance->control_key)
            instance->control_arrow[1]--;
          else
            instance->arrow[1]--;
          break;

        case GLFW_KEY_RIGHT:
          if (instance->shift_key)
            instance->shift_arrow[0]++;
          else if (instance->control_key)
            instance->control_arrow[0]++;
          else
            instance->arrow[0]++;
          break;

        case GLFW_KEY_LEFT:
          if (instance->shift_key)
            instance->shift_arrow[0]--;
          else if (instance->control_key)
            instance->control_arrow[0]--;
          else
            instance->arrow[0]--;
          break;

        default:
          break;
        }
      }
    }
  }

  // マウスボタンを操作したときの処理
  static void mouse(GLFWwindow *window, int button, int action, int mods)
  {
    // このインスタンスの this ポインタを得る
    Window *const instance(static_cast<Window *>(glfwGetWindowUserPointer(window)));

    if (instance)
    {
      // マウスの現在位置を得る
      const GLfloat x(static_cast<GLfloat>(instance->mouse_x));
      const GLfloat y(static_cast<GLfloat>(instance->mouse_y));

      switch (button)
      {
      case GLFW_MOUSE_BUTTON_1:
        if (action)
        {
          // 左ドラッグ開始
          instance->trackball_left.start(x, y);
        }
        else
        {
          // 左ドラッグ終了
          instance->trackball_left.stop(x, y);
        }
        break;

      case GLFW_MOUSE_BUTTON_2:
        if (action)
        {
          // 右ドラッグ開始
          instance->trackball_right.start(x, y);
        }
        else
        {
          // 右ドラッグ終了
          instance->trackball_right.stop(x, y);
        }
        break;

      case GLFW_MOUSE_BUTTON_3:
        break;

      default:
        break;
      }
    }
  }

  // マウスホイール操作時の処理
  static void wheel(GLFWwindow *window, double x, double y)
  {
    // このインスタンスの this ポインタを得る
    Window *const instance(static_cast<Window *>(glfwGetWindowUserPointer(window)));

    if (instance)
    {
      if (instance->shift_key)
        instance->shift_wheel_rotation += y;
      else if (instance->control_key)
        instance->control_wheel_rotation += y;
      else
      {
        instance->wheel_rotation += y;
        if (instance->wheel_rotation < -100.0)
          instance->wheel_rotation = -100.0;
        else if (instance->wheel_rotation > 49.0)
          instance->wheel_rotation = 49.0;
      }
    }
  }

  // ウィンドウの幅を得る
  GLsizei getWidth() const
  {
    return width;
  }

  // ウィンドウの高さを得る
  GLsizei getHeight() const
  {
    return height;
  }

  // ウィンドウのサイズを得る
  void getSize(GLsizei *size) const
  {
    size[0] = getWidth();
    size[1] = getHeight();
  }

  // ウィンドウのアスペクト比を得る
  GLfloat getAspect() const
  {
    return aspect;
  }

  // 矢印キーの現在の X 値を得る
  GLfloat getArrowX() const
  {
    return static_cast<GLfloat>(arrow[0]);
  }

  // 矢印キーの現在の Y 値を得る
  GLfloat getArrowY() const
  {
    return static_cast<GLfloat>(arrow[1]);
  }

  // 矢印キーの現在の値を得る
  void getArrow(GLfloat *arrow) const
  {
    arrow[0] = getArrowX();
    arrow[1] = getArrowY();
  }

  // シフトキーを押しながら矢印キーの現在の X 値を得る
  GLfloat getShiftArrowX() const
  {
    return static_cast<GLfloat>(shift_arrow[0]);
  }

  // シフトキーを押しながら矢印キーの現在の Y 値を得る
  GLfloat getShiftArrowY() const
  {
    return static_cast<GLfloat>(shift_arrow[1]);
  }

  // シフトキーを押しながら矢印キーの現在の値を得る
  void getShiftArrow(GLfloat *shift_arrow) const
  {
    shift_arrow[0] = getShiftArrowX();
    shift_arrow[1] = getShiftArrowY();
  }

  // コントロールキーを押しながら矢印キーの現在の X 値を得る
  GLfloat getControlArrowX() const
  {
    return static_cast<GLfloat>(control_arrow[0]);
  }

  // コントロールキーを押しながら矢印キーの現在の Y 値を得る
  GLfloat getControlArrowY() const
  {
    return static_cast<GLfloat>(control_arrow[1]);
  }

  // コントロールキーを押しながら矢印キーの現在の値を得る
  void getControlArrow(GLfloat *control_arrow) const
  {
    control_arrow[0] = getControlArrowX();
    control_arrow[1] = getControlArrowY();
  }

  // マウスの X 座標を得る
  GLfloat getMouseX() const
  {
    return static_cast<GLfloat>(mouse_x);
  }

  // マウスの Y 座標を得る
  GLfloat getMouseY() const
  {
    return static_cast<GLfloat>(mouse_y);
  }

  // マウスの現在位置を得る
  void getMouse(GLfloat *position) const
  {
    position[0] = getMouseX();
    position[1] = getMouseY();
  }

  // マウスホイールの現在の回転角を得る
  GLfloat getWheel() const
  {
    return static_cast<GLfloat>(wheel_rotation);
  }

  // シフトを押しながらマウスホイールの現在の回転角を得る
  GLfloat getShiftWheel() const
  {
    return static_cast<GLfloat>(shift_wheel_rotation);
  }

  // コントロールを押しながらマウスホイールの現在の回転角を得る
  GLfloat getControlWheel() const
  {
    return static_cast<GLfloat>(control_wheel_rotation);
  }

  // 左ボタンによるトラックボールの回転変換行列を得る
  const GgMatrix getLeftTrackball() const
  {
    return trackball_left.getMatrix();
  }

  // 右ボタンによるトラックボールの回転変換行列を得る
  const GgMatrix getRightTrackball() const
  {
    return trackball_right.getMatrix();
  }
};
