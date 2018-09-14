#include <Arduino.h>
#include <DFMiniMp3.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>


// network ettings --------------
//

char ssid[] = "TC";//"C3P";//
char psswd[] = "chOc0l4t1n4";//"trespatios";//

WiFiUDP Udp;
const IPAddress outIp(192,168,0,131);
const unsigned int outPort = 4000;
const unsigned int localPort = 4001;

// osc settings
OSCErrorCode error;

// handlers for samples
#define NUM_TRACKS 4
bool playIt = false;
bool notPlaying = true;
int wich_track;

class Mp3Notify
{
  public:
    static void OnError(uint16_t errorCode)
    {
      // see DfMp3_Error for code meaning
      Serial.println();
      Serial.print("Com Error ");
      Serial.println(errorCode);
    }

    static void OnPlayFinished(uint16_t globalTrack)
    {
      Serial.println();
      Serial.print("Play finished for #");
      Serial.println(globalTrack);   
      playIt = false;
      notPlaying = true;
      Serial.print("tocala  ");
      Serial.print(playIt);
      Serial.print("NO esta sonando ");
      Serial.println(notPlaying);
      //      mp3.stop();
      delay(20);
      yield();
    }

    static void OnCardOnline(uint16_t code)
    {
      Serial.println();
      Serial.print("Card online ");
      Serial.println(code);     
    }

    static void OnCardInserted(uint16_t code)
    {
      Serial.println();
      Serial.print("Card inserted ");
      Serial.println(code); 
    }

    static void OnCardRemoved(uint16_t code)
    {
      Serial.println();
      Serial.print("Card removed ");
      Serial.println(code);  
    }
};

// instance DFPlayer
SoftwareSerial soft_serial(D6,D5);
DFMiniMp3<SoftwareSerial, Mp3Notify>mp3(soft_serial);
//----
void setup(){
  Serial.begin(115200);
  Serial.println("initializing.....");

  mp3.begin();
  mp3.reset();

  //  mp3.setRepeatPlay(false);
  mp3.setVolume(80);
  Serial.print("NUMERO TRACKS");
  Serial.println(mp3.getTotalTrackCount());

  Serial.println("starting........");

//Serial.println(mp3.getPlaybackMode());
  //-- start wifi and udp
  WiFi.begin(ssid,psswd);

  while(WiFi.status() != WL_CONNECTED) {

    delay(500);
    Serial.print(".");

  }

  Serial.println("wifi connected");
  Serial.println(WiFi.localIP());
  Serial.println("starting udp");
  Udp.begin(localPort);



}
//--------- osc message listener 
void listen_osc_messages(OSCMessage &inc_msg) {

  int value = inc_msg.getInt(0);
  /*    Serial.print("tocala  ");
        Serial.print(playIt);
        Serial.print("NO esta sonando ");
        Serial.println(notPlaying);*/
  if(value > 0){
 //   Serial.println("play the fucking song");
    playIt = true;
  }else {
    //  Serial.println("dont play nothing");
    playIt = false;
  }
}
//----------------
void loop() {

  //Serial.println("track 1");
  /* Serial.print("tocala  ");
     Serial.print(playIt);
     Serial.print("NO esta sonando ");
     Serial.println(notPlaying);*/

   if(notPlaying) {
     mp3.pause();
     }
  if(playIt && notPlaying){
    Serial.println("where it really happens");

    wich_track = (int)random(NUM_TRACKS)+1;
    yield();
//      mp3.playMp3FolderTrack(wich_track);//plays :sd/mp3/003.mp3
   //   delay(100);
    mp3.playRandomTrackFromAll();
    playIt = false;
    notPlaying = false;

    yield();
  }
  mp3.loop();
  delay(100);
  //--- listen for the osc address /playSong i
  //-- if there is no song playing
  //  if(!playIt && notPlaying) {

  //Serial.println("gonna liten......");
  // incoming osc messages are dispatched 
  // to the listener
  OSCMessage inc_msg;
  int sizeOfmsg = Udp.parsePacket();
  if (sizeOfmsg > 0) {

    while (sizeOfmsg--) {


      inc_msg.fill(Udp.read());
      yield();
    }
    if (!inc_msg.hasError()) {
      inc_msg.dispatch("/playSong",listen_osc_messages );
      delay(2);
      yield();
    } else {
      error = inc_msg.getError();
      Serial.print("error: ");
      Serial.println(error);
    }
  }

  //}
}
