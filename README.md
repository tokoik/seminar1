# seminar1

OpenGL と OpenCV を組み合わせたメッシュ変形およびビデオエフェクトのサンプルプログラムです。

## 概要

本プログラムは、ブログ記事「[メディアデザインセミナー１](https://tokoik.github.io/blog/2019-06-03.html)」の前半の課題として作成された、ビデオ画像や動画ファイル（MP4）を読み込み、OpenGL のテクスチャおよび頂点配列オブジェクト（VAO/VBO）を用いてリアルタイムにメッシュを変形・テクスチャマッピングして表示するサンプルプログラムです。

## 対応環境

- **Windows**: Visual Studio 2019 / 2022 (CMake 3.22 以上)
- **macOS**: Xcode (Command Line Tools 含む, OpenCV 4.x, CMake 3.22 以上)
- **Linux**: Ubuntu 20.04 / 22.04 LTS (GCC, libopencv-dev, libgl1-mesa-dev, CMake 3.22 以上)

## ビルド手順

### Windows (Visual Studio)

OpenCV 4.13.0 および GLFW 3.4 は CMake 実行時に自動的にダウンロードおよび構成されます。

```powershell
cmake -B build -S .
cmake --build build --config Release
```

Visual Studio でソリューションを開く場合は、`build/seminar1.sln` を開いてビルドしてください。

### macOS (Xcode / Makefile)

Homebrew 等で OpenCV をインストールした上でビルドします。

```bash
brew install opencv cmake
cmake -B build -S . -G Xcode
cmake --build build --config Release
```

または Makefile 生成:

```bash
cmake -B build -S .
cmake --build build
```

### Linux (Ubuntu)

必要なパッケージをインストールした上でビルドします。

```bash
sudo apt-get install build-essential cmake libopencv-dev libgl1-mesa-dev
cmake -B build -S .
cmake --build build
```

## 起動方法

ビルド完了後、生成された実行ファイルを実行します。実行ファイルと同じ階層に `image.jpg`, `video.mp4`, シェーダファイル（`mesh.vert`, `mesh.frag`）および OpenCV DLL が自動配置されます。

- **Windows**: `build\Release\seminar1.exe`
- **macOS**: `build/Release/seminar1.app` または `build/seminar1`
- **Linux**: `build/seminar1`

## 操作方法

- **マウス左ドラッグ**: トラックボールによる視点（カメラ）の回転
- **マウス右ドラッグ**: 平行移動（パン）
- **マウスホイール / 中ボタンドラッグ**: ズームイン / ズームアウト
- **Q / ESC キー**: プログラムの終了

## プログラムの解説

- `main.cpp`: アプリケーションのメインループ、OpenGL / OpenCV テクスチャの管理、描画処理を実行します。
- `kadai1.cpp`: 課題処理本体（`setup()` による点群・色データおよびビデオ入力の初期化、`update()` による動画フレームの取得と点群変形・テクスチャ更新）を行います。
- `GgApp.h` / `Window.h`: GLFW3 をラップしたアプリケーションおよびウィンドウ管理クラスです。
- `Mesh.h` / `Shape.h`: 頂点配列オブジェクト（VAO/VBO）のメッシュ描画クラスです。
- `common.h`: `kadai1.cpp` と `main.cpp` 間で共有される関数・データ宣言です。
- `gg.cpp` / `gg.h`: OpenGL 補助ライブラリおよび行列演算・シェーダ読み込み機能を提供します。
- `mesh.vert` / `mesh.frag`: メッシュ描画用のバーテックスシェーダおよびフラグメントシェーダです。
