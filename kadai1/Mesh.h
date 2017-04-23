#pragma once

//
// メッシュ
//

// 形状データ
#include "Shape.h"

class Mesh : public Shape
{
  // ストリップ1枚あたりの頂点の数
  const GLsizei vertexCount;

  // ストリップの数
  const GLsizei primitiveCount;

  // 点データのテクスチャのサンプラの uniform 変数の場所
  GLint pointLoc;

  // 色データのテクスチャのサンプラの uniform 変数の場所
  GLint colorLoc;

public:

  // シェーダ
  const GgSimpleShader shader;

  // コンストラクタ
  Mesh(int slices = 2, int stacks = 2)
    : vertexCount(slices * 2)
    , primitiveCount(stacks - 1)
    , shader(GgSimpleShader("mesh.vert", "mesh.frag"))
  {
    // サンプラの uniform 変数の場所を得る
    pointLoc = glGetUniformLocation(shader.get(), "point");
    colorLoc = glGetUniformLocation(shader.get(), "color");
  }

  // コンストラクタ
  Mesh(const int *size)
    : Mesh(size[0], size[1])
  {
  }

  // デストラクタ
  virtual ~Mesh()
  {
  }

  // 描画
  virtual void draw() const
  {
    // 点テクスチャのサンプリングに使用するテクスチャユニット番号をサンプラに設定する
    glUniform1i(pointLoc, 0);

    // 色テクスチャのサンプリングに使用するテクスチャユニット番号をサンプラに設定する
    glUniform1i(colorLoc, 1);

    // メッシュを描画する
    bind();
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, vertexCount, primitiveCount);
  }
};
