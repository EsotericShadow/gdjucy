# GDJucy
 This is a personal project that aim is to link Godot with the JUCE audio engine. I am currently working on.

GodotにJUCEのオーディオエンジンを組み合わせて使いたいと思って試作中の個人的なプロジェクトです。完成品のライブラリではありません。

現在はGodot4.3上でGDExtensionの形でJUCE8.0.2の機能を利用することに成功しています。

# 現状の課題
しかし現状では、単純なサイン波を鳴らすだけでも、数秒から一分前後の間隔でクリックノイズ（ポップノイズ・プチノイズ？）が鳴るという問題があります。今のままでは実用性は低いです。

この個人プロジェクトの公開目的は情報共有と問題解決です。この問題の解決策や改善につながるアドバイスを頂けると非常に助かります。

#

# 今までに分かっていること
* デバイスタイプやバッファサイズを変えてもノイズには特に効果がない
* 処理の高速化（コンパイル時の最適化等）はノイズの頻度の低減に効果があるっぽいが、ノイズをなくせるわけでは今のところない。
* スレッドの優先度を変えるのは効果が特になさそう
* Godot側のオーディオ出力デバイスは「Dummy」に設定している（実質無効化らしいが効果は感じられない）


# 開発環境（別途インストールが必要なもの）
* Windows 10/11 [^1]
* Godot4.3 - https://godotengine.org/download/windows/ [^2]
* CMake3.30.3 - https://cmake.org/download/ [^3]
* Visual Studio Community 2022 https://visualstudio.microsoft.com/ja/vs/community/ [^4]

[^1]:クリーンなWindows10の場合は「Visual C++再頒布可能パッケージ」をインストールする必要があります（もしかしたら追加で.net Frameworkも必要かも）
[^2]:Godotエディタを使います。バージョンは完全一致させる必要があります
[^3]:このバージョンは適当に新しければよく、一致させなくていいです
[^4]:「C++によるデスクトップ開発」ワークロードを有効化する必要あり

# ライセンス
* AGPLv3となります。（JUCEの非商用ライセンスの制約から）
* プルリクエスト等で提供されたコード部分については、当プロジェクトへその著作権の寄贈に同意した事になりますので、その点をご注意、ご了承の上お願いします。

