/*
  CryptoMX-3 - DualOTA

  (implementation)
*/

#include "DualOTA.h"

#ifdef ARDUINO_ARCH_ESP32
	WebServer httpUpdateServer(8266);

	static const char updateSite[] PROGMEM =
	R"(<html><body>
		<form method='POST' action='' enctype='multipart/form-data'>
			<input type='file' name='update'>
			<input type='submit' value='Update'>
		</form>
	</body></html>)";
			
	static const char successSite[] PROGMEM = 
	"<META http-equiv=\"refresh\" content=\"15;URL=/\">Update Success! Rebooting...\n";
#endif
    
DualOTA::DualOTA(StatusLED * led){
  this->led = led;
}

void DualOTA::start(const char * hostname, const char * url_path, int http_port, int arduino_port){
  printDebug("Enabling OTA!");
  enabled = true;
  ArduinoOTA.setPort(arduino_port);
  ArduinoOTA.setHostname(hostname);

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  ArduinoOTA.onStart([this]() {
      printDebug("Update started...");
      LittleFS.end();
      if(led!=nullptr){
          led->fadeStop();
          led->setVal(1);
      }
    });
  ArduinoOTA.onEnd([this]() {
      printDebug("Done!");
    });
  ArduinoOTA.onProgress([this](unsigned int progress, unsigned int total) {
      printfDebug("Progress: %u%%\r", (progress / (total / 100)));
      if(led!=nullptr){
        led->setVal(1.0 - (double)progress / (double)total);
      }
    });
  ArduinoOTA.onError([this](ota_error_t error) {
      printfDebug("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR){ printDebug("Auth Failed");}
      else if (error == OTA_BEGIN_ERROR){ printDebug("Begin Failed");}
      else if (error == OTA_CONNECT_ERROR){ printDebug("Connect Failed");}
      else if (error == OTA_RECEIVE_ERROR){ printDebug("Receive Failed");}
      else if (error == OTA_END_ERROR){ printDebug("End Failed");}
    });

  ArduinoOTA.begin();
#ifdef ARDUINO_ARCH_ESP8266
  httpUpdater.setup(&httpUpdateServer, url_path);
  httpUpdateServer.begin(http_port);
#endif
  
#ifdef ARDUINO_ARCH_ESP32
  httpUpdateServer.on(url_path, HTTP_GET, [&](){
    httpUpdateServer.send_P(200, "text/html", updateSite);
  });

  httpUpdateServer.on("/", HTTP_GET, [&](){
    httpUpdateServer.send_P(200, PSTR("text/html"), updateSite);
  });
  
  // handler for the /update form POST (once file upload finishes)
  httpUpdateServer.on(url_path, HTTP_POST, [&](){
    if (Update.hasError()) {
      httpUpdateServer.send(200, "text/html", String("Update error: ") + _updateError);
    } else {
      httpUpdateServer.client().setNoDelay(true);
      httpUpdateServer.send_P(200, "text/html", successSite);
      delay(100);
      httpUpdateServer.client().stop();
      ESP.restart();
    }
  },[&](){
    // handler for the file upload, get's the sketch bytes, and writes
    // them through the Update object
    HTTPUpload& upload = httpUpdateServer.upload();

    if(upload.status == UPLOAD_FILE_START){
      printDebug("Update: " + upload.filename);
      if(!Update.begin(UPDATE_SIZE_UNKNOWN)){//start with max available size
        setUpdaterError();
      }
    } else if(upload.status == UPLOAD_FILE_WRITE && !_updateError.length()){
      if(Update.write(upload.buf, upload.currentSize) != upload.currentSize){
        setUpdaterError();
      }
    } else if(upload.status == UPLOAD_FILE_END && !_updateError.length()){
      if(Update.end(true)){ //true to set the size to the current progress
        printDebug("Update Success! Rebooting...\n");
      } else {
        setUpdaterError();
      }
    } else if(upload.status == UPLOAD_FILE_ABORTED){
      Update.end();
      printDebug("Update was aborted");
    }
  });
  
  httpUpdateServer.begin(http_port);
#endif
  if(led!=nullptr){
    led->fade(StatusLED::PERIODIC_FADE, 2000);
  }
}

#ifdef ARDUINO_ARCH_ESP32
void DualOTA::setUpdaterError(){
  StreamString str;
  Update.printError(str);
  _updateError = str.c_str();
}
#endif

bool DualOTA::started(){
	return enabled;
}

void DualOTA::loop(){
	if(enabled){
	#ifdef ARDUINO_ARCH_ESP8266
		ETS_GPIO_INTR_DISABLE();
	#endif
	ArduinoOTA.handle();
	httpUpdateServer.handleClient();
	#ifdef ARDUINO_ARCH_ESP8266
		ETS_GPIO_INTR_ENABLE();
	#endif
	}
}
