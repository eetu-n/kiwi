import nl.tue.id.oocsi.*;

OOCSI oocsi;

String tag = " 30 79 dd 89";
String card = " 16 73 a6 ac";

void setup() {
  size(200, 200);
  background(120);
  frameRate(10);

  oocsi = new OOCSI(this, "eetu2", "oocsi.id.tue.nl");
}

// Card:
// Tag:  30 79 dd 89

void draw() {
  oocsi
  .channel("esp-testchannel")
    .data("line", 3) // 0-7, line to write text on
    .data("message", "Things") // message to display, max 16 char
    .data("delay","50") // time to wait before vibrate in ms
    .data("time","1000") // time to vibrate
    .data("vibrate", 1) // vibrate yes or no, 1 for yes 0 for no
    .data("uid", card) // UID of the card
    .data("clearLines", 9) // Lines to be cleared, single int where each int represents a line to clear. Cleared before new message written, 8 to clear all, 9 to clear none.
      .send();
            exit();
}