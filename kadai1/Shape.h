#pragma once

//
// 形状データの基底クラス
//

// ウィンドウ関連の処理
#include "Window.h"

class Shape
{
  // 頂点配列オブジェクト
  GLuint vao;

public:

  // コンストラクタ
  Shape()
  {
    // 頂点配列オブジェクトを作成する
    glGenVertexArrays(1, &vao);
  }

  // デストラクタ
  virtual ~Shape()
  {
    // 頂点配列オブジェクトを削除する
    glDeleteVertexArrays(1, &vao);
  }

  // 結合
  void bind() const
  {
    // 頂点配列オブジェクトを指定する
    glBindVertexArray(vao);
  }

  // 描画
  virtual void draw() const = 0;
};
