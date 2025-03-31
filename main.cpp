#include <cstdint>
#include <iostream>

using namespace std;

// typedefでstate_t型を定義（arrayの代替）
typedef uint64_t state_t[5];

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

    // Initialization: IV || K || N
    S[0] = IV;
    S[1] = key[0];
    S[2] = key[1];
    S[3] = nonce[0];
    S[4] = nonce[1];

    permutation(S, 12);
    S[3] ^= key[0];
    S[4] ^= key[1];

    // Associated Data 処理（省略）
    S[4] ^= 0x01;  // ドメインセパレーション

    // Plaintext (56bit) の Encryption
    uint64_t pt_masked = plaintext & 0x00FFFFFFFFFFFFFFULL;
    S[0] ^= pt_masked;
    ciphertext = S[0];
    permutation(S, 6);

    // Finalization
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
