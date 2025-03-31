# Ascon

## 技術スタック
暗号復号ともにC++

## 用語確認
| 用語 | 説明 |
| ---- | ---- |
| Ascon | 認証付き暗号(AEAD)の一種暗号化と同時に改ざん検出(これを認証タグというらしい)もしてくれる、軽量でセキュアな暗号方式 |
| ノンス | 一度だけ使う数 |
| AD(Associated Data) | 暗号化はしないけど、改ざん検出に使うデータ

## 暗号化の流れ
基本の処理は以下の4ステップ
1. 初期化：鍵・ノンスを使って状態をセットアップ
2. AD処理：省略可能（追加データがある場合だけ）
3. 平文処理：暗号化して暗号文に変換
4. ファイナライズ：認証タグを生成

## 参考資料
[Ascon v1.2 Submission to NIST](https://csrc.nist.gov/CSRC/media/Projects/lightweight-cryptography/documents/round-2/spec-doc-rnd2/ascon-spec-round2.pdf)
[Ascon Lightweight Cryptography](https://ascon.isec.tugraz.at/index.html)

