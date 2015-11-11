#pragma once

#include <cstdint>
#include <string>

using namespace std;

class Blowfish
{
public:
	Blowfish(const string& Key);
	
	// Paly2's note : I add this constructor and this function for my own use of this file :p Moreover, I need the "key" string is not const.
	Blowfish() {}
	
	void init_with_key(const std::string& Key);

	string encrypt(string plainText);

	string decrypt(string encryptedText);

private:
	string key;

	uint32_t p[16 + 2];
	uint32_t s[4][256];

	void blowfish_encrypt(uint32_t *l, uint32_t *r);

	void blowfish_decrypt(uint32_t *l, uint32_t *r);

	uint32_t f(const uint32_t x);

	static const uint32_t P_BLOCKS[];
	static const uint32_t S_BLOCKS[4][256];
};
