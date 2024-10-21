/*
* CryptoGarage - Message
* 
* Static class which parses and encrypts/decrypts custom CryptoGarage TCP messages.
*/

#pragma once

#include "Config.h"
#include <PrintDebug.h>
#include "Crypto.h"
#include "ChallengeManager.h"
#include "Command.h"

//  Message structure:
//  [MESSAGE_BEGIN] CODE : b64(IV) : b64(TAG) : b64(enc(challenge_response_b64 : challenge_request_b64 : DATA, IV)) [MESSAGE_END]


//Our TCP messages are either plain ACK without data,
// ERR with error message string or encrypted DATA
enum MessageType {HELLO, ACK, NACK, ERR, DATA, NOPE};

const char HEADER_HELLO = 'H';
const char HEADER_ACK = 'A';
const char HEADER_NACK = 'N';
const char HEADER_DATA = 'D';
const char HEADER_ERR = 'F';


const char ENCRYPTED_CIOTv2_MESSAGE[] = "CIOTv2:::";
const char UNENCRYPTED_ERROR[] = "ERROR:::";

const char MESSAGE_BEGIN[] = "[BEGIN]";
const char MESSAGE_CONTINUE[] = "[CONTINUE]";
const char MESSAGE_END[] = "[END]";

const char FLAG_NONE[] = "";
const char FLAG_KEEP_ALIVE[] = "F";
const char FLAG_BINARY[] = "B";

const int FLAGS_LEN = 5;

struct Msg {
  MessageType type;
  char flags[FLAGS_LEN + 1];
  String data;
  uint8_t challenge[CHALLENGE_LEN];
};

struct ProcessMessageStruct{
  MessageType responseCode;
  String responseData;
  String flags;
  Validation validationResult;
};

class Message {
  public:
    static char typeToChar(MessageType t);
    static MessageType charToType(char type);
    static String encrypt(Crypto &crypto, MessageType type, const uint8_t * data, const int data_len, uint8_t * challenge_response, uint8_t * challenge_request, const char flags[FLAGS_LEN+1]);
    static String encrypt(Crypto &crypto, MessageType type, const String &data, uint8_t * challenge_response, uint8_t * challenge_request, const String &flags);
    static void decrypt(Msg &msg, Crypto &crypto, String &s, ChallengeManager &cm);
    static String wrap(String &message);
    static String unwrap(String &message);
	static String unwrap(const char * message);
    static String getParam(String &message, int index, bool remaining=false);
};
