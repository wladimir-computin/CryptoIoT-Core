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

uint8_t * ChallengeManager::generateRandomChallenge(){
  stateTicker.detach();
  Crypto::getRandom(challenge_local, CHALLENGE_LEN);
  stateTicker.attach(challenge_timeout, stateTick, (void*)this);
  return challenge_local;
}

bool ChallengeManager::verifyChallenge(uint8_t * challenge_response){
  if(memcmp(challenge_local, EMPTY_CHALLENGE, CHALLENGE_LEN) != 0){
    return memcmp(challenge_local, challenge_response, CHALLENGE_LEN) == 0;
  }
  return false;
}

void ChallengeManager::rememberLastChallengeRequest(uint8_t * challenge_in){
  memcpy(challenge_remote, challenge_in, CHALLENGE_LEN);
}

uint8_t * ChallengeManager::getLastChallengeRequest(){
  return challenge_remote;
}

void ChallengeManager::resetChallenge(){
  stateTicker.detach();
  memset(challenge_local, 0, CHALLENGE_LEN);
  memset(challenge_remote, 0, CHALLENGE_LEN);
}
