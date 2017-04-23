#pragma once

// OpenCV
#include <opencv2/opencv.hpp>

//
// 共通の関数
//

// 点データの作成
extern void createPoint(int slices, int stacks, const void *point = nullptr);

// 点データの転送
extern void submitPoint(int slices, int stacks, const void *point);

// 色データの作成
extern void createColor(int width, int height, const void *color = nullptr);

// 色データの転送
extern void submitColor(int width, int height, const void *color);

// 色データの作成 (cv::Mat 版)
extern void createColor(const cv::Mat &color);

// 色データの転送 (cv::Mat 版)
extern void submitColor(const cv::Mat &color);

// 現在時刻の取得
extern float getTime();

// 設定
extern bool setup();

// 更新
extern bool update();
