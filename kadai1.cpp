// 共通の関数
#include "common.h"

// 標準ライブラリ
#include <cmath>

// 点群の横の数と縦の数
constexpr int slices{ 200 }, stacks{ 200 };

// 点のデータ
static float point[stacks][slices][3];

// 色データのアスペクト比
static float aspect;

// 円周率
constexpr float pi{ 3.14159265f };

// 周期
constexpr float cycle{ 5.0f };

//
// 設定（最初に一度だけ実行されます）
//
bool setup()
{
  // 点データを登録する
  createPoint(slices, stacks);

  // 画像の読み込み
  const cv::Mat image{ cv::imread("image.jpg") };

  // 色データを登録する
  createColor(image);

  // 色データのアスペクト比を求める
  aspect = float(image.cols) / float(image.rows);

  // セットアップに成功した
  return true;
}

//
// 更新（毎回実行されます）
//
bool update()
{
  // cycle ごとに 0→1 に変化する値
  const float t{ fmod(getTime(), cycle) / cycle };

  // cycle ごとに 0→2π に変化する位相
  const float phase{ 2.0f * pi * t };

  for (int j = 0; j < stacks; ++j)
  {
    for (int i = 0; i < slices; ++i)
    {
      // x 方向のパラメータ (0≦u≦1)
      const float u{ float(i) / float(slices - 1) };

      // y 方向のパラメータ (0≦v≦1)
      const float v{ float(j) / float(stacks - 1) };

      // 頂点の位置
      point[j][i][0] = (4.0f * u - 2.0f) * aspect;
      point[j][i][1] = 4.0f * v - 2.0f;

      // 高さを中心からの距離で決定する
      const float r{ hypot(point[j][i][0], point[j][i][1]) };
      point[j][i][2] = sin(2.0f * pi * r + phase) * 0.2f;
    }
  }

  // 点データを更新する
  submitPoint(slices, stacks, point);

  // プログラムを終了しない
  return true;
}
