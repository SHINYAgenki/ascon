# Ascon

## 技術スタック
暗号復号ともにC++

## 用語確認
| 用語 | 説明 |
| ---- | ---- |
| Ascon | 認証付き暗号(AEAD)の一種暗号化と同時に改ざん検出(これを認証タグというらしい)もしてくれる、軽量でセキュアな暗号方式 |
| AD(Associated Data) | 暗号化はしないけど、改ざん検出に使うデータ|
| State | 64ビット×5=320ビットの配列、内部データ。これを使って暗号化・認証を行う。|
| `IV` | Initialization Vector Ascon内部で固定されている定数（暗号方式やパラメータを表す）|
| `K` | key 鍵128bビット |
| `N` | ノンス 一度限りの使い捨て値128ビット |

### `IV || K || N`

`IV` のビット列、`K` のビット列、`N` のビット列をつなげたもの。

- 実際には以下のように結合される：

- 合計：**320ビット**（= 64 × 5）

- これを内部状態 `S[0]` ～ `S[4]` に割り当てる



## 暗号化の流れ
基本の処理は以下の4ステップ
1. 初期化：鍵・ノンスを使って状態をセットアップ
2. AD処理：省略可能（追加データがある場合だけ）
3. 平文処理：暗号化して暗号文に変換
4. ファイナライズ：認証タグを生成

## 参考資料
- [Ascon v1.2 Submission to NIST](https://csrc.nist.gov/CSRC/media/Projects/lightweight-cryptography/documents/round-2/spec-doc-rnd2/ascon-spec-round2.pdf)
- [Ascon Lightweight Cryptography](https://ascon.isec.tugraz.at/index.html)

