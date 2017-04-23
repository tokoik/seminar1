// 共通の関数
#include "common.h"

// 標準ライブラリ
#include <cmath>

// メッシュの幅と高さ
const int width(100), height(100);

// 点データ
float point[height][width][3];

// 色データ
unsigned char color[height][width][3];

//
// 設定（最初に一度だけ実行されます）
//
bool setup()
{
  for (int y = 0; y < height; ++y)
  {
    for (int x = 0; x < width; ++x)
    {
      // x 方向のパラメータ (0≦u≦1)
      const float u(float(x) / float(width - 1));

      // y 方向のパラメータ (0≦v≦1)
      const float v(float(y) / float(height - 1));

      // 頂点の位置
      point[y][x][0] = u * 2.0f - 1.0f;
      point[y][x][1] = v * 2.0f - 1.0f;
      point[y][x][2] = 0.0f;

      // 頂点の色
      color[y][x][0] = 100;
      color[y][x][1] = 0;
      color[y][x][2] = 0;
    }
  }

  // 点データを登録する
  createPoint(width, height, point);

  // 色データを登録する
  createColor(width, height, color);

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
