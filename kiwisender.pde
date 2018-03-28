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

void draw() {
  oocsi
  .channel("esp-testchannel")
  .data("uid", card1) // UID of the card
  .data("line", 0) // 0-7, line to write text on
  .data("message", "Message 1") // message to display, max 16 char
  .data("clearLines", 8) // Lines to be cleared, single int where each int represents a line to clear. Cleared before new message written, 8 to clear all, 9 to clear none.
  .data("delay","50") // time to wait before vibrate in ms
  .data("time","1000") // time to vibrate
  .data("repeat", 1) // vibrate yes or no, 1 for yes 0 for no
  .data("hue", 10)
  .data("brightness", 100)
  .data("ledMode", 1)
    .send();
  oocsi
  .channel("esp-testchannel")
  .data("uid", card2) // 3UID of the card
  .data("line", 1) // 0-7, line to write text on
  .data("message", "Message 2") // message to display, max 16 char
  .data("clearLines", 8) // Lines to be cleared, single int where each int represents a line to clear. Cleared before new message written, 8 to clear all, 9 to clear none.
  .data("delay","50") // time to wait before vibrate in ms
  .data("time","1000") // time to vibrate
  .data("repeat", 1) // vibrate yes or no, 1 for yes 0 for no
  .data("hue", 150)
  .data("brightness", 255)
  .data("ledMode", 2)
    .send();
  oocsi
  .channel("esp-testchannel")
  .data("uid", tag1) // UID of the card
  .data("line", 2) // 0-7, line to write text on
  .data("message", "Message 3") // message to display, max 16 char
  .data("clearLines", 8) // Lines to be cleared, single int where each int represents a line to clear. Cleared before new message written, 8 to clear all, 9 to clear none.
  .data("delay","50") // time to wait before vibrate in ms
  .data("time","1000") // time to vibrate
  .data("repeat", 2) // vibrate yes or no, 1 for yes 0 for no
  .data("hue", 100)
  .data("brightness", 255)
  .data("ledMode", 0)
    .send();
  oocsi
  .channel("esp-testchannel")
  .data("uid", tag2) // UID of the card
  .data("line", 3) // 0-7, line to write text on
  .data("message", "Message 4") // message to display, max 16 char
  .data("clearLines", 8) // Lines to be cleared, single int where each int represents a line to clear. Cleared before new message written, 8 to clear all, 9 to clear none.
  .data("delay","50") // time to wait before vibrate in ms
  .data("time","1000") // time to vibrate
  .data("repeat", 3) // vibrate yes or no, 1 for yes 0 for no
  .data("hue", 175)
  .data("brightness", 255)
  .data("ledMode", 0)
    .send();
  exit();
}
