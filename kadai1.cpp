// 共通の関数
#include "common.h"

// 標準ライブラリ
#include <cmath>

// 点群の横の数と縦の数
constexpr int slices{ 200 }, stacks{ 200 };

// 点のデータ
static float point[stacks][slices][3];

// 色のデータ
static unsigned char color[stacks][slices][3];

// 円周率
constexpr float pi{ 3.14159265f };

//
// 設定（最初に一度だけ実行されます）
//
bool setup()
{
  for (int j = 0; j < stacks; ++j)
  {
    for (int i = 0; i < slices; ++i)
    {
      // x 方向のパラメータ (0≦u≦1)
      const float u{ float(i) / float(slices - 1) };

      // y 方向のパラメータ (0≦v≦1)
      const float v{ float(j) / float(stacks - 1) };

      // 頂点の位置
      point[j][i][0] = u * 2.0f - 1.0f;
      point[j][i][1] = v * 2.0f - 1.0f;
      point[j][i][2] = 0.0f;

      // 頂点の色
      color[j][i][0] = 100;
      color[j][i][1] = 0;
      color[j][i][2] = 0;
    }
  }

  // 点データを登録する
  createPoint(slices, stacks, point);

  // 色データを登録する
  createColor(slices, stacks, color);

  // セットアップに成功した
  return true;
}

//
// 更新（毎回実行されます）
//
bool update()
{
  // プログラムを終了しない
  return true;
}
