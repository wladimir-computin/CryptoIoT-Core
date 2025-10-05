/*
* CryptoIoT - ChallengeManager
* 
* Class for tracking the state of the challenge-response protocol.
* The state jumps back to NONE after CONNECTION_STATE_TIMEOUT seconds (defined in AllConfig.h)
* This makes replay attacks or brute forcing the 12 byte challenge IV practically impossible.
*/

#pragma once

#include <Ticker.h>

#include "Config.h"
#include <PrintDebug.h>

#include "Crypto.h"

const int CHALLENGE_LEN = 12; //Recommended value is 12.

class ChallengeManager {
  private:
    uint8_t challenge_local[CHALLENGE_LEN] = {0};
    uint8_t challenge_remote[CHALLENGE_LEN] = {0};
    uint8_t EMPTY_CHALLENGE[CHALLENGE_LEN] = {0};
    Ticker stateTicker;
    static void stateTick(void * context);
    int challenge_timeout = DEFAULT_CHALLENGE_VALIDITY_TIMEOUT;

  public:
    void setChallengeTimeout(int challenge_timeout);
    int getChallengeTimeout();
    uint8_t * generateRandomChallenge();
    void resetChallenge();
    bool verifyChallenge(uint8_t * challenge_local);
    void rememberLastChallengeRequest(uint8_t * challenge_in);
    uint8_t * getLastChallengeRequest();
};
