#version 150 core

//
// メッシュの描画
//

// 点データのテクスチャ
uniform sampler2D point;

// 変換行列
uniform mat4 mp;                                    // 投影変換行列
uniform mat4 mv;                                    // モデルビュー変換行列
uniform mat4 mn;                                    // 法線変換行列

// 光源
uniform vec4 lamb;                                  // 環境光成分
uniform vec4 ldiff;                                 // 拡散反射光成分
uniform vec4 lspec;                                 // 鏡面反射光成分
uniform vec4 lpos;                                  // 位置

// 材質
uniform vec4 kamb;                                  // 環境光の反射係数
uniform vec4 kdiff;                                 // 拡散反射係数
uniform vec4 kspec;                                 // 鏡面反射係数
uniform float kshi;                                 // 輝き係数

// ラスタライザに送る頂点属性
out vec2 texcoord;                                  // テクスチャ座標
out vec4 idiff;                                     // 拡散反射光強度
out vec4 ispec;                                     // 鏡面反射光強度

void main(void)
{
  // テクスチャ座標
  int x = gl_VertexID >> 1;
  int y = 1 - (gl_VertexID & 1) + gl_InstanceID;
  texcoord = (vec2(x, y) + 0.5) / vec2(textureSize(point, 0));

  // 位置，視線，光線，中間
  vec4 p = mv * texture(point, texcoord);
  vec3 v = normalize(p.xyz / p.w);
  vec3 l = normalize((lpos * p.w - p * lpos.w).xyz);
  vec3 h = normalize(l - v);

  // 接線，従接線，法線
  vec3 t = (textureOffset(point, texcoord, ivec2(1, 0)) - textureOffset(point, texcoord, ivec2(-1, 0))).xyz;
  vec3 b = (textureOffset(point, texcoord, ivec2(0, 1)) - textureOffset(point, texcoord, ivec2(0, -1))).xyz;
  vec3 n = normalize(mat3(mn) * cross(t, b));

  // 陰影
  idiff = max(dot(n, l), 0.0) * kdiff * ldiff + kamb * lamb;
  ispec = pow(max(dot(n, h), 0.0), kshi) * kspec * lspec;

  // 頂点位置
  gl_Position = mp * p;
}
