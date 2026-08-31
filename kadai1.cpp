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

// ビデオ入力
cv::VideoCapture video;

// 最初のフレームを取得した時刻
double start;

//
// 設定（最初に一度だけ実行されます）
//
bool setup()
{
  // 点データを登録する
  createPoint(slices, stacks);

  // ビデオの取得開始
  if (!video.open("video.mp4")) return false;

  // ムービーファイルの1フレームの一時保存先
  cv::Mat image;

  // 最初のフレームを取得する
  if (!video.grab()) return false;

  // 最初のフレームを取得した時刻を記録しておく
  start = getTime();

  // 取得したフレームを保存する
  video.retrieve(image);

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
  // 経過時間
  const double elaps(getTime() - start);

  // 経過時間が現在のフレームの時刻に達していたら
  if (elaps >= video.get(cv::CAP_PROP_POS_MSEC) * 0.001)
  {
    // 1フレーム取得する
    if (!video.grab())
    {
      // フレームが取得できなかったらムービーファイルを巻き戻す
      video.set(cv::CAP_PROP_POS_MSEC, 0.0);

      // 経過時間をリセットする
      start = getTime();
    }
    else
    {
      // ムービーファイルの1フレームの一時保存先
      cv::Mat image;

      // 取得したフレームを保存する
      video.retrieve(image);

      // グレースケール画像に変換する
      cv::Mat gray;
      cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);

      // エッジ検出
      cv::Mat sobelX, sobelY;
      cv::Sobel(gray, sobelX, CV_8U, 1, 0);
      cv::Sobel(gray, sobelY, CV_8U, 0, 1);
      gray = (sobelX + sobelY) * 0.5;

      // 取得したフレームを転送する
      submitColor(image);

      // グレースケール画像を点データの大きさに伸縮する
      cv::resize(gray, gray, cv::Size(stacks, slices));

      // グレースケール画像を上下反転する
      cv::flip(gray, gray, 0);

      for (int j = 0; j < stacks; ++j)
      {
        for (int i = 0; i < slices; ++i)
        {
          // x のパラメータ (0≦u≦1)
          const float u{ float(i) / float(slices - 1) };

          // y のパラメータ (0≦v≦1)
          const float v{ float(j) / float(stacks - 1) };

          // 点の位置
          point[j][i][0] = (4.0f * u - 2.0f) * aspect;
          point[j][i][1] = 4.0f * v - 2.0f;

          // 画素のグレースケール値で決定する
          point[j][i][2] = gray.at<unsigned char>(j, i) * 0.001f;
        }
      }

      // 点データを更新する
      submitPoint(stacks, slices, point);
    }
  }

  // プログラムを終了しない
  return true;
}
