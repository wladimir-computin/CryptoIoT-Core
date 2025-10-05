#include <Timeout.h>	
#include <WiFiUdp.h>
#include "ChallengeManager.h"
#include "Message.h"
#include "Crypto.h"

typedef void (*ciot_client_callback)(Msg& response, void * param);

class CryptoIoTClient{

  public:
	CryptoIoTClient();
	void loop();
	void setup(String& devicepass, String& name);
	void setup(String& devicepass, String& name, String& host, int port);
	void send(String& host, const int port, String& command, ciot_client_callback callback, void * param);
	void send(String& command, ciot_client_callback callback, void * param);


  private:
	enum CryptConPhase {OFF, CONNECT, PHASE1, PHASE2, RETURN};
	struct Argument {
      String command;
	  IPAddress host;
      int port;
	  ciot_client_callback callback;
	  void * param;
    };
	
	volatile CryptConPhase clientPhase = OFF;
	Timeout clientTimeout;
	ChallengeManager challengeManager;
	WiFiUDP udpclient;
	Crypto crypto;
	Argument argument;
	
	void send_Data_UDP(String data);
	void send_Data_UDP(String data, IPAddress ip, int port);
	String receive_Data_UDP();
	void doUDPClientStuff();
};
