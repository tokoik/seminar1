#version 150 core
#extension GL_ARB_explicit_attrib_location : enable

//
// メッシュの描画
//

// 色データのテクスチャ
uniform sampler2D color;

// ラスタライザから受け取る頂点属性
in vec2 texcoord;                                   // テクスチャ座標
in vec4 idiff;                                      // 拡散反射光強度
in vec4 ispec;                                      // 鏡面反射光強度

// フレームバッファへの出力
layout (location = 0) out vec4 fc;                  // フラグメントの色

void main(void)
{
  // 画素の陰影を求める
  fc = idiff * texture(color, texcoord) + ispec;
}
