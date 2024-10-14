/*
  CryptoGarage - Message

  (implementation)
*/

#include "Message.h"

String Message::encrypt(Crypto &crypto, MessageType type, const uint8_t * data, const int data_len, uint8_t * challenge_response, uint8_t * challenge_request, const char flags[FLAGS_LEN+1]) {
  char header = typeToChar(type);

  switch (type) {

    case ERR:
      return String(UNENCRYPTED_ERROR) + (char*)data;
      break;

    case ACK:
    case NACK:
    case HELLO:
    case DATA:
      { 
        int cleartext_len = 1 + 1 + 5 + 1 + CHALLENGE_LEN + CHALLENGE_LEN + data_len;
        int packet_len = AES_GCM_IV_LEN + AES_GCM_TAG_LEN + cleartext_len;
        uint8_t packet[packet_len];
        
        int pos = 0;
        uint8_t * iv = packet + pos;
        pos += AES_GCM_IV_LEN;
  
        uint8_t * tag = packet + pos;
        pos += AES_GCM_TAG_LEN;
        
        uint8_t * cleartext = packet + pos;
        pos = 0;
        cleartext[pos++] = header;
        cleartext[pos++] = ':';
        strncpy((char*)cleartext+pos, flags, FLAGS_LEN);
        pos += FLAGS_LEN;
        cleartext[pos++] = ':';
        memcpy(cleartext+pos, challenge_response, CHALLENGE_LEN);
        pos += CHALLENGE_LEN;
        memcpy(cleartext+pos, challenge_request, CHALLENGE_LEN);
        pos += CHALLENGE_LEN;
        memcpy(cleartext+pos, data, data_len);
        pos += data_len;
        
        // Performing memory in place decryption, since AES-GCM is a stream cipher and we can save loads of memory that way.
        uint8_t * encryptedData = cleartext;
        crypto.getRandomIV(iv);
        crypto.encryptData(encryptedData, cleartext, cleartext_len, iv, tag);
        
        String message = ENCRYPTED_CIOTv2_MESSAGE;
        message += crypto.bytesToBase64(packet, packet_len);

        return message;
      }
      break;

  }
  return "";
}

String Message::encrypt(Crypto &crypto, MessageType type, const String &data, uint8_t * challenge_response, uint8_t * challenge_request, const String &flags) {
  return encrypt(crypto, type, (uint8_t*)data.c_str(), data.length(), challenge_response, challenge_request, flags.c_str());
}

void Message::decrypt(Msg &msg, Crypto &crypto, String &s, ChallengeManager &cm) {
  if(s.startsWith(ENCRYPTED_CIOTv2_MESSAGE)   ){
    s.replace(ENCRYPTED_CIOTv2_MESSAGE, "");
    int encrypted_len = crypto.base64DecodedLength(s);
    
    if(encrypted_len >= AES_GCM_IV_LEN + AES_GCM_TAG_LEN + 1 + 1 + 5 + 1 + CHALLENGE_LEN + CHALLENGE_LEN){
      uint8_t encrypted[encrypted_len+1];
      crypto.base64ToBytes(s, encrypted);

      uint8_t * iv = encrypted;
      uint8_t * tag = encrypted + AES_GCM_IV_LEN;
      uint8_t * encryptedData = encrypted + AES_GCM_IV_LEN + AES_GCM_TAG_LEN;
      
      int encryptedData_len = encrypted_len - AES_GCM_IV_LEN - AES_GCM_TAG_LEN;
      encryptedData[encryptedData_len] = '\0';
      // Performing memory in place decryption, since AES-GCM is a stream cipher and we can save loads of memory that way.
      uint8_t * decryptedData = encryptedData;
      crypto.decryptData(decryptedData, encryptedData, encryptedData_len, iv, tag); //returns "" if decryption fails
      
      if ((char*)decryptedData == "") {
        msg.type = ERR;
        msg.data = "Decryption error!";
        return;
      }
      
      MessageType type = charToType(decryptedData[0]);
        switch (type) {
          case HELLO: //fall through
          case ACK:
          case NACK:
          case DATA:
            {
              int pos = 2;
              char * flags = (char*)decryptedData + pos;
              pos += 5;
              pos += 1;
              uint8_t * challenge_response = decryptedData + pos;
              pos += CHALLENGE_LEN;
              uint8_t * challenge_request = decryptedData + pos;
              pos += CHALLENGE_LEN;
              uint8_t * data = decryptedData + pos;
              
              if (type != HELLO) {
                //printDebug("Got Challenge Response: " + _challenge_response_b64);
                if (cm.verifyChallenge(challenge_response)) {
                  msg.data = (char*)data;
                } else {
                  msg.type = ERR;
                  msg.data = "Challenge missmatch!";
                  return;
                }
              }
              msg.type = type;
              memcpy(msg.flags, flags, FLAGS_LEN);
              msg.flags[FLAGS_LEN] = '\0';
              memcpy(msg.challenge, challenge_request, CHALLENGE_LEN);
              return;
            }
          break;
              
          default:
            msg.type = ERR;
            msg.data = "Invalid Header!";
            return;
          break;
        }
    }
  }
  
  else if(s.startsWith(UNENCRYPTED_ERROR)){
    s.replace(UNENCRYPTED_ERROR, "");
    msg.type = ERR;
    msg.data = s;
    return;
  }
}

char Message::typeToChar(MessageType t) {
  switch (t) {
    case HELLO:
      return HEADER_HELLO;
    case ACK:
      return HEADER_ACK;
    case NACK:
      return HEADER_NACK;
    case ERR:
      return HEADER_ERR;
    case DATA:
      return HEADER_DATA;
  }
  return HEADER_ERR;
}

MessageType Message::charToType(char type) {
  if (type == HEADER_HELLO) {
    return HELLO;
  }
  if (type == HEADER_ACK) {
    return ACK;
  }
  if (type == HEADER_NACK) {
    return NACK;
  }
  if (type == HEADER_ERR) {
    return ERR;
  }
  if (type == HEADER_DATA) {
    return DATA;
  }
  return NOPE;
}

String Message::wrap(String &message) {
  String out;
  out.reserve(strlen(MESSAGE_BEGIN) + strlen(MESSAGE_END) + message.length());
  out += MESSAGE_BEGIN;
  out += message;
  out += MESSAGE_END;
  
  return out;
}

String Message::unwrap(const char * message) { //REGEX: \[BEGIN\]\s*(.{0,MAX_MESSAGE_LEN}?)\s*\[END\]
  char * begin = strstr(message, MESSAGE_BEGIN);
  char * end = strstr(message, MESSAGE_END);

  if (begin != NULL && end != NULL){
    begin += strlen(MESSAGE_BEGIN);
    if (end > begin && (end - begin) <= MAX_MESSAGE_LEN) {
      *end = '\0';
      return begin;
    }
  }
  return "";
}

String Message::unwrap(String &message) { //REGEX: \[BEGIN\]\s*(.{0,MAX_MESSAGE_LEN}?)\s*\[END\]
  return unwrap(message.c_str());
}

String Message::getParam(String& message, int index, bool remaining) {
  int startindex = 0;
  int endindex = 0;

  for (int i = 0; i < index; i++) {
    startindex = message.indexOf(':', startindex)+1;
    if (startindex == 0) {
      startindex = -1;
      break;
    }
  }

  if (startindex != -1) {
    endindex = message.indexOf(':', startindex);
    if (endindex != -1 && !remaining) {
      return message.substring(startindex, endindex);
    } else {
      return message.substring(startindex);
    }
  }
  return "";
}
