/*
  CryptoGarage - Crypto

  Class where all the crypto magic happens.
  CryptoGarage uses a challenge-response system based on AES-GCM and SHA256 for key generation.
  Challenge-response has some major benefits over a rolling code bases exchange, for example
  immunity to replay-attacks and easy managment of multiple remote devices, in our case smartphones.

  Based on two highly awesome libraries:

  # Arduino Cryptographic Library by Southern Storm Software, Pty Ltd.
  https://rweather.github.io/arduinolibs/crypto.html

  # ESP8266TrueRandom by Peter Knight and Marvin Roger
  https://github.com/marvinroger/ESP8266TrueRandom

*/

#pragma once

#include <GCM.h>
#include <AES.h>
#include <SHA512.h>

#ifdef ARDUINO_ARCH_ESP8266
#include <ESP8266TrueRandom.h>
#endif

#include "Config.h"
#include <PrintDebug.h>
#include "cBase64.h"

const int AES256_KEY_LEN = 32;
const int AES_GCM_TAG_LEN = 16; //Recommended value is 16. Don't change unless you understand how AES-GCM works.
const int AES_GCM_IV_LEN = 12; //Recommended value is 12.

const char KEY_CRYPTO_PROBE[] = "probe";
const char KEY_CRYPTO_KEY[] = "key";

class Crypto {

  private:
    uint8_t aesKey[AES256_KEY_LEN];
    void keyDerivationFunction(String &devicepass, const char * vaultname);

  public:

    //on initialize the devicepass gets hashed with SHA256 and is used as AES key
	//optional argument vaultname for specifying the pmem vault where the key will be cached.
    void init(String &devicepass);
	void init(String &devicepass, const char * vaultname);

    //fill the passed IV with random bytes
    static void getRandomIV(uint8_t * iv);

    //fill the passed challenge with random bytes
    static void getRandom(uint8_t * arr, int arr_len);

    //encrypt a String, using an IV and writes the tag into tag and the encrypted byte array into out
    void encryptData(uint8_t * out, const String &data, uint8_t * iv, uint8_t * tag);

    //encrypt a byte array, using an IV writes the tag into tag and the encrypted byte array into out
    void encryptData(uint8_t * out, uint8_t * data, int dataLen, uint8_t * iv, uint8_t * tag);

    //decrypt a byte array using an IV and a tag, writes output into out
    void decryptData(uint8_t * out, uint8_t * data, int dataLen, uint8_t * iv, uint8_t * tag);

    //decrypt a byte array using an IV and a tag, returns a String.
    String decryptData(uint8_t * data, int dataLen, uint8_t * iv, uint8_t * tag);

    static String bytesToBase64(uint8_t * bytes, int len);
    static void base64ToBytes(const String &in, uint8_t * out);
    static String base64ToBytes(const String &in);
    static uint16_t base64DecodedLength(const String &b64);

    //get SHA256(devicepass) as byte array
    const uint8_t * getAES256Key();

    //get base64(SHA256(devicepass)) as String
    String getAES256Key_b64();

    ~Crypto() {}
};
