import nl.tue.id.oocsi.*;

OOCSI oocsi;

String tag1 =   "30 79 dd 89";
String card1 =  "80 34 ec 00";
String tag2 =   "16 73 a6 ac";
String card2 =  "6f ac fa 29";

void setup() {
  size(200, 200);
  background(120);
  frameRate(10);

  oocsi = new OOCSI(this, "eetu2", "oocsi.id.tue.nl");
}

void message(String uid, String message0, String message1, String message2, String message3, 
int delay, int time, int repeat, int hue, int brightness, int ledMode){
  oocsi
  .channel("kiwiReceiver")
  .data("uid", uid) // UID of the card
  .data("message0", message0) // message to display, max 16 char
  .data("message1", message1)
  .data("message2", message2)
  .data("message3", message3)
  .data("delay", delay) // time to wait before vibrate in ms
  .data("time", time) // time to vibrate
  .data("repeat", repeat) // vibrate yes or no, 1 for yes 0 for no
  .data("hue", hue)
  .data("brightness", brightness)
  .data("ledMode", ledMode)
    .send();
}

void draw() {
  message(card1, "Message1", "Message2", "Message3", "Message4", 50, 100, 0, 100, 100, 1);
  message(card2, "Message1", "Message2", "Message3", "Message4", 50, 100, 0, 200, 100, 2);
  message(tag1, "Message1", "Message2", "Message3", "Message4", 500, 1000, 5, 150, 100, 3);
  message(tag2, "Message1", "Message2", "Message3", "Message4", 50, 100, 1, 50, 100, 1);
  
  exit();
}
