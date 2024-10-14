/*
* CryptoIoT - ChallengeManager
* 
* (implementation)
*/

#include "ChallengeManager.h"

void ChallengeManager::setChallengeTimeout(int challenge_timeout) {
  this->challenge_timeout = challenge_timeout;
}

int ChallengeManager::getChallengeTimeout(){
  return challenge_timeout;
}

void ChallengeManager::stateTick(void * context) {
  printDebug("[ChallengenMan] Challenge timed out");
  (*(ChallengeManager*)context).resetChallenge();
}

void ChallengeManager::getCurrentChallenge(uint8_t * challenge_out){
  memcpy(challenge_out, challenge, CHALLENGE_LEN);
}

void ChallengeManager::generateRandomChallenge(uint8_t * challenge_out){
  resetChallenge();
  Crypto::getRandom(challenge, CHALLENGE_LEN);
  memcpy(challenge_out, challenge, CHALLENGE_LEN);
  stateTicker.attach(challenge_timeout, stateTick, (void*)this);
}

bool ChallengeManager::verifyChallenge(uint8_t * challenge_response){
  if(memcmp(challenge, EMPTY_CHALLENGE, CHALLENGE_LEN) != 0){
    return memcmp(challenge, challenge_response, CHALLENGE_LEN) == 0;
  }
  return false;
}

void ChallengeManager::resetChallenge(){
  stateTicker.detach();
  memset(challenge, 0, CHALLENGE_LEN);;
}
