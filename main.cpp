#include <cstdint>
#include <iostream>

using namespace std;

typedef uint64_t state_t[5]; // S[0], S[1], ..., S[4]

// 固定IV（Ascon-128）
#define IV 0x80400c0600000000ULL

// ラウンド定数（pa/pb）
const uint64_t ROUND_CONSTANTS[12] = {
    0xF0, 0xE1, 0xD2, 0xC3, 0xB4, 0xA5,
    0x96, 0x87, 0x78, 0x69, 0x5A, 0x4B
};

void add_round_constant(state_t s, int round) {
    s[2] ^= ROUND_CONSTANTS[round];
}

void substitution_layer(state_t s) {
    uint64_t x0 = s[0], x1 = s[1], x2 = s[2], x3 = s[3], x4 = s[4];
    x0 ^= x4; x4 ^= x3; x2 ^= x1;
    uint64_t t0 = ~x0 & x1, t1 = ~x1 & x2;
    uint64_t t2 = ~x2 & x3, t3 = ~x3 & x4, t4 = ~x4 & x0;
    x0 ^= t1; x1 ^= t2; x2 ^= t3; x3 ^= t4; x4 ^= t0;
    x1 ^= x0; x0 ^= x4; x3 ^= x2; x2 = ~x2;
    s[0] = x0; s[1] = x1; s[2] = x2; s[3] = x3; s[4] = x4;
}

void linear_layer(state_t s) {
    s[0] ^= (s[0] >> 19) ^ (s[0] >> 28);
    s[1] ^= (s[1] >> 61) ^ (s[1] >> 39);
    s[2] ^= (s[2] >> 1)  ^ (s[2] >> 6);
    s[3] ^= (s[3] >> 10) ^ (s[3] >> 17);
    s[4] ^= (s[4] >> 7)  ^ (s[4] >> 41);
}

void permutation(state_t s, int rounds) {
    for (int i = 12 - rounds; i < 12; ++i) {
        add_round_constant(s, i);
        substitution_layer(s);
        linear_layer(s);
    }
}

void ascon_encrypt(const uint64_t key[2], const uint64_t nonce[2],
                   const uint64_t plaintext, uint64_t &ciphertext, uint64_t &tag) {
    state_t S;

    // 状態の初期化
    S[0] = IV; // 固定の定数(Ascon仕様で定義)
    S[1] = key[0]; // 鍵の上位64ビット
    S[2] = key[1]; // 鍵の下位64ビット
    S[3] = nonce[0]; // ノンスの上位64ビット
    S[4] = nonce[1]; // ノンスの下位64ビット

    permutation(S, 12); // 12ラウンドの変換（pa）
    S[3] ^= key[0]; // 鍵をもう一度ミックス
    S[4] ^= key[1];

    // ここまででステートSは、鍵とノンスによってランダム性のある状態になっているらしい

    // Associated Data 処理（省略）
    S[4] ^= 0x01;  // ドメインセパレーション

    // ここから平文の暗号化
    uint64_t pt_masked = plaintext & 0x00FFFFFFFFFFFFFFULL;
    S[0] ^= pt_masked; // ステートとXOR → これが暗号化の本質らしい
    ciphertext = S[0]; // 暗号文としてそのまま出力
    permutation(S, 6); // ステートを6ラウンドで更新（認証タグの準備）
    // 暗号文 = S[0] XOR 平文 = 暗号状態と平文の混合結果

    // ファイナライズ（認証タグ）
    S[1] ^= key[0];
    S[2] ^= key[1];
    permutation(S, 12);
    S[3] ^= key[0];
    S[4] ^= key[1];
    tag = S[3];
}

int main() {
    uint64_t key[2]   = {0x0123456789ABCDEFULL, 0x0011223344556677ULL};
    uint64_t nonce[2] = {0x8899AABBCCDDEEFFULL, 0xDEADBEEFCAFEBABEULL};
    uint64_t plaintext = 0x11223344556677ULL;

    uint64_t ciphertext, tag;
    ascon_encrypt(key, nonce, plaintext, ciphertext, tag);

    cout << "Plaintext : 0x" << hex << plaintext << endl;
    cout << "Ciphertext: 0x" << hex << ciphertext << endl;
    cout << "Tag       : 0x" << hex << tag << endl;

    return 0;
}
