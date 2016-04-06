/* 

  Pine wood derby
  
  This code runs off of 4 Light Diodes and powers 4 8x8 matrix's and 4 4 number 7 segment displays.
  
  On the arduino plug in the light diodes to pings 2, 3, 4, 5
  
  The matrix displays will use the pins 9,10,11,12 (DIN,CLK,CS,#of displays)
  
  the 7 segment displays will uses the serial connection adjust the jumpers to use 0x70-0x73 respectivly 
  
*/

#include <avr/pgmspace.h>
#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"
#include "LedControl.h"
#include <Servo.h>

Servo sg;

int sg_pos = 0;

int sensor_lane_1 = 2;
int sensor_lane_2 = 3;
int sensor_lane_3 = 4;
int sensor_lane_4 = 5;

int pinStart = 6;
int pinReset = 7;

Adafruit_7segment time_lane_1 = Adafruit_7segment();
Adafruit_7segment time_lane_2 = Adafruit_7segment();
Adafruit_7segment time_lane_3 = Adafruit_7segment();
Adafruit_7segment time_lane_4 = Adafruit_7segment();

LedControl lc = LedControl(8,10,9,4);

unsigned long start, finish, elapsed;

unsigned long bufferLong [14] = {0};
const long scrollDelay = 75;   // adjust scrolling speed

prog_uchar scrollText[] PROGMEM = { " Pinewood Derby   Ready   Set   Race..       \0"};

//Numeric Font Matrix (Arranged as 7x font data + 1x kerning data) //Space (Char 0x20)
prog_uchar font5x7 [] PROGMEM = {
    B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, 6, // Space
    B10000000, B10000000, B10000000, B10000000, B00000000, B00000000, B10000000, 2, // !
    B10100000, B10100000, B10100000, B00000000, B00000000, B00000000, B00000000, 4, // "
    B01010000, B01010000, B11111000, B01010000, B11111000, B01010000, B01010000, 6, // #
    B00100000, B01111000, B10100000, B01110000, B00101000, B11110000, B00100000, 6, //$
    B11000000, B11001000, B00010000, B00100000, B01000000, B10011000, B00011000, 6, //% 
    B01100000, B10010000, B10100000, B01000000, B10101000, B10010000, B01101000, 6, //&
    B11000000, B01000000, B10000000, B00000000, B00000000, B00000000, B00000000, 3, //'
    B00100000, B01000000, B10000000, B10000000, B10000000, B01000000, B00100000, 4, //( 
    B10000000, B01000000, B00100000, B00100000, B00100000, B01000000, B10000000, 4, //)
    B00000000, B00100000, B10101000, B01110000, B10101000, B00100000, B00000000, 6, //* 
    B00000000, B00100000, B00100000, B11111000, B00100000, B00100000, B00000000, 6, //+
    B00000000, B00000000, B00000000, B00000000, B11000000, B01000000, B10000000, 3, //,
    B00000000, B00000000, B11111000, B00000000, B00000000, B00000000, B00000000, 6, //-
    B00000000, B00000000, B00000000, B00000000, B00000000, B11000000, B11000000, 3, //.
    B00000000, B00001000, B00010000, B00100000, B01000000, B10000000, B00000000, 6, ///
    B01110000, B10001000, B10011000, B10101000, B11001000, B10001000, B01110000, 6, //0
    B01000000, B11000000, B01000000, B01000000, B01000000, B01000000, B11100000, 4, //1 
    B01110000, B10001000, B00001000, B00010000, B00100000, B01000000, B11111000, 6, //2 
    B11111000, B00010000, B00100000, B00010000, B00001000, B10001000, B01110000, 6, //3
    B00010000, B00110000, B01010000, B10010000, B11111000, B00010000, B00010000, 6, //4
    B11111000, B10000000, B11110000, B00001000, B00001000, B10001000, B01110000, 6, //5
    B00110000, B01000000, B10000000, B11110000, B10001000, B10001000, B01110000, 6, //6
    B11111000, B10001000, B00001000, B00010000, B00100000, B00100000, B00100000, 6, //7
    B01110000, B10001000, B10001000, B01110000, B10001000, B10001000, B01110000, 6, //8
    B01110000, B10001000, B10001000, B01111000, B00001000, B00010000, B01100000, 6, //9
    B00000000, B11000000, B11000000, B00000000, B11000000, B11000000, B00000000, 3, //:
    B00000000, B11000000, B11000000, B00000000, B11000000, B01000000, B10000000, 3, //;
    B00010000, B00100000, B01000000, B10000000, B01000000, B00100000, B00010000, 5, //<
    B00000000, B00000000, B11111000, B00000000, B11111000, B00000000, B00000000, 6, //=
    B10000000, B01000000, B00100000, B00010000, B00100000, B01000000, B10000000, 5, //>
    B01110000, B10001000, B00001000, B00010000, B00100000, B00000000, B00100000, 6, //?
    B01110000, B10001000, B00001000, B01101000, B10101000, B10101000, B01110000, 6, //@
    B01110000, B10001000, B10001000, B10001000, B11111000, B10001000, B10001000, 6, //A
    B11110000, B10001000, B10001000, B11110000, B10001000, B10001000, B11110000, 6, //B
    B01110000, B10001000, B10000000, B10000000, B10000000, B10001000, B01110000, 6, //C
    B11100000, B10010000, B10001000, B10001000, B10001000, B10010000, B11100000, 6, //D
    B11111000, B10000000, B10000000, B11110000, B10000000, B10000000, B11111000, 6, //E
    B11111000, B10000000, B10000000, B11110000, B10000000, B10000000, B10000000, 6, //F 
    B01110000, B10001000, B10000000, B10111000, B10001000, B10001000, B01111000, 6, //G 
    B10001000, B10001000, B10001000, B11111000, B10001000, B10001000, B10001000, 6, //H 
    B11100000, B01000000, B01000000, B01000000, B01000000, B01000000, B11100000, 4, //I 
    B00111000, B00010000, B00010000, B00010000, B00010000, B10010000, B01100000, 6, //J 
    B10001000, B10010000, B10100000, B11000000, B10100000, B10010000, B10001000, 6, //K 
    B10000000, B10000000, B10000000, B10000000, B10000000, B10000000, B11111000, 6, //L 
    B10001000, B11011000, B10101000, B10101000, B10001000, B10001000, B10001000, 6, //M 
    B10001000, B10001000, B11001000, B10101000, B10011000, B10001000, B10001000, 6, //N 
    B01110000, B10001000, B10001000, B10001000, B10001000, B10001000, B01110000, 6, //O 
    B11110000, B10001000, B10001000, B11110000, B10000000, B10000000, B10000000, 6, //P 
    B01110000, B10001000, B10001000, B10001000, B10101000, B10010000, B01101000, 6, //Q 
    B11110000, B10001000, B10001000, B11110000, B10100000, B10010000, B10001000, 6, //R 
    B01111000, B10000000, B10000000, B01110000, B00001000, B00001000, B11110000, 6, //S 
    B11111000, B00100000, B00100000, B00100000, B00100000, B00100000, B00100000, 6, //T 
    B10001000, B10001000, B10001000, B10001000, B10001000, B10001000, B01110000, 6, //U 
    B10001000, B10001000, B10001000, B10001000, B10001000, B01010000, B00100000, 6, //V 
    B10001000, B10001000, B10001000, B10101000, B10101000, B10101000, B01010000, 6, //W
    B10001000, B10001000, B01010000, B00100000, B01010000, B10001000, B10001000, 6, //X 
    B10001000, B10001000, B10001000, B01010000, B00100000, B00100000, B00100000, 6, //Y 
    B11111000, B00001000, B00010000, B00100000, B01000000, B10000000, B11111000, 6, //Z 
    B11100000, B10000000, B10000000, B10000000, B10000000, B10000000, B11100000, 4, //[ 
    B00000000, B10000000, B01000000, B00100000, B00010000, B00001000, B00000000, 6, //(Backward Slash)
    B11100000, B00100000, B00100000, B00100000, B00100000, B00100000, B11100000, 4, //] 
    B00100000, B01010000, B10001000, B00000000, B00000000, B00000000, B00000000, 6, //^ 
    B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B11111000, 6, //_ 
    B10000000, B01000000, B00100000, B00000000, B00000000, B00000000, B00000000, 4, //` 
    B00000000, B00000000, B01110000, B00001000, B01111000, B10001000, B01111000, 6, //a 
    B10000000, B10000000, B10110000, B11001000, B10001000, B10001000, B11110000, 6, //b 
    B00000000, B00000000, B01110000, B10001000, B10000000, B10001000, B01110000, 6, //c 
    B00001000, B00001000, B01101000, B10011000, B10001000, B10001000, B01111000, 6, //d 
    B00000000, B00000000, B01110000, B10001000, B11111000, B10000000, B01110000, 6, //e 
    B00110000, B01001000, B01000000, B11100000, B01000000, B01000000, B01000000, 6, //f 
    B00000000, B01111000, B10001000, B10001000, B01111000, B00001000, B01110000, 6, //g
    B10000000, B10000000, B10110000, B11001000, B10001000, B10001000, B10001000, 6, //h
    B01000000, B00000000, B11000000, B01000000, B01000000, B01000000, B11100000, 4, //i 
    B00010000, B00000000, B00110000, B00010000, B00010000, B10010000, B01100000, 5, //j 
    B10000000, B10000000, B10010000, B10100000, B11000000, B10100000, B10010000, 5, //k 
    B11000000, B01000000, B01000000, B01000000, B01000000, B01000000, B11100000, 4, //l 
    B00000000, B00000000, B11010000, B10101000, B10101000, B10001000, B10001000, 6, //m 
    B00000000, B00000000, B10110000, B11001000, B10001000, B10001000, B10001000, 6, //n 
    B00000000, B00000000, B01110000, B10001000, B10001000, B10001000, B01110000, 6, //o 
    B00000000, B00000000, B11110000, B10001000, B11110000, B10000000, B10000000, 6, //p 
    B00000000, B00000000, B01101000, B10011000, B01111000, B00001000, B00001000, 6, //q 
    B00000000, B00000000, B10110000, B11001000, B10000000, B10000000, B10000000, 6, //r 
    B00000000, B00000000, B01110000, B10000000, B01110000, B00001000, B11110000, 6, //s 
    B01000000, B01000000, B11100000, B01000000, B01000000, B01001000, B00110000, 6, //t
    B00000000, B00000000, B10001000, B10001000, B10001000, B10011000, B01101000, 6, //u
    B00000000, B00000000, B10001000, B10001000, B10001000, B01010000, B00100000, 6, //v 
    B00000000, B00000000, B10001000, B10101000, B10101000, B10101000, B01010000, 6, //w
    B00000000, B00000000, B10001000, B01010000, B00100000, B01010000, B10001000, 6, //x 
    B00000000, B00000000, B10001000, B10001000, B01111000, B00001000, B01110000, 6, //y 
    B00000000, B00000000, B11111000, B00010000, B00100000, B01000000, B11111000, 6, //z 
    B00100000, B01000000, B01000000, B10000000, B01000000, B01000000, B00100000, 4, //{ 
    B10000000, B10000000, B10000000, B10000000, B10000000, B10000000, B10000000, 2, //| 
    B10000000, B01000000, B01000000, B00100000, B01000000, B01000000, B10000000, 4, //} 
    B00000000, B00000000, B00000000, B01101000, B10010000, B00000000, B00000000, 6, //~ 
    B01100000, B10010000, B10010000, B01100000, B00000000, B00000000, B00000000, 5  // (Char 0x7F)
};

byte four[] = { B00100100, B00100100, B00100100, B00111100, B00000100, B00000100, B00000100, B00000100 };
byte thrd[] = { B00011000, B00100100, B00000100, B00011000, B00000100, B00000100, B00100100, B00011000 };
byte scnd[] = { B00011000, B00100100, B00100100, B00001000, B00010000, B00100000, B00100000, B00111100 };
byte frst[] = { B00010000, B00110000, B00010000, B00010000, B00010000, B00010000, B00010000, B00111000 };
byte error[] = { B01111110, B01000010, B01000010, B01000010, B01000010, B01000010, B01000010, B01111110 };
byte dash[] = { B00000000, B00000000, B00000000, B11111111, B00000000, B00000000, B00000000, B00000000 };

/**************************************************************************/



void setup(){
  Serial.begin(9600); 

  sg.attach(11);

  pinMode(pinStart, INPUT);
  pinMode(pinReset, INPUT);

  time_lane_1.begin(0x70);
  time_lane_2.begin(0x71);
  time_lane_3.begin(0x72);
  time_lane_4.begin(0x73);
  
  time_lane_1.clear();
  time_lane_1.writeDisplay();
  
  time_lane_2.clear();
  time_lane_2.writeDisplay();
  
  time_lane_3.clear();
  time_lane_3.writeDisplay();
  
  time_lane_4.clear();
  time_lane_4.writeDisplay();

  lc.shutdown(0,false); // Wake them up
  lc.shutdown(1,false);
  lc.shutdown(2,false);
  lc.shutdown(3,false);
  
  lc.setIntensity(0,5);
  lc.setIntensity(1,5);
  lc.setIntensity(2,5);
  lc.setIntensity(3,5);

  lc.clearDisplay(0);
  lc.clearDisplay(1);
  lc.clearDisplay(2);
  lc.clearDisplay(3);
 
 pinMode(sensor_lane_1, INPUT);
 pinMode(sensor_lane_2, INPUT);
 pinMode(sensor_lane_3, INPUT);
 pinMode(sensor_lane_4, INPUT); 

  sg_pos = 0;
  sg.write(sg_pos);
  
}


void loop(){
   int btnStart = digitalRead(pinStart);
   int btnReset = digitalRead(pinReset);
   
   if(btnReset == LOW){
    //scrollMessage(scrollText); 
    resetTrack();
  }

  if(btnStart == LOW){
    startRace();
  }
 
}

/**************************************************************************/
void startRace(){
  int wc[] = {1,2,3,4};
  int f1 = 0;
  int f2 = 0;
  int f3 = 0;
  int f4 = 0;
  int pos = 0;

  int in_time = 0;


  Serial.println("Start Pushed");
  moveServo(1);
  showDash();

  do {
    if(in_time == 0){
      start = millis();
      in_time = 1;
    }

    if(digitalRead(sensor_lane_1) == HIGH && f1 == 0){
      showPlace(0,wc[pos]);
      pos++;
      f1++;
      double lap = pTime(0,start);
      setTime(lap,time_lane_1);
    }

    if(digitalRead(sensor_lane_2) == HIGH && f2 == 0){
      showPlace(1,wc[pos]);
      pos++;
      f2++;
      double lap = pTime(1,start);
      setTime(lap,time_lane_2);
    }

   if(digitalRead(sensor_lane_3) == HIGH && f3 == 0){
      showPlace(2,wc[pos]);
      pos++;
      f3++;
      double lap = pTime(2,start);
      setTime(lap,time_lane_3);
    }

   if(digitalRead(sensor_lane_4) == HIGH && f4 == 0){
      showPlace(3,wc[pos]);
      pos++;
      f4++;
      double lap = pTime(3,start);
      setTime(lap,time_lane_4);
    }

        finish = millis();
        elapsed = finish-start;
        if(elapsed > 15000){
          Serial.println("15 sec");
          Serial.println("zero it out");
          if(f1 == 0){
            showPlace(0, 9);
            f1++;
            setTime(9.999, time_lane_1);
          }
          if(f2 == 0){
            showPlace(1, 9);
            f2++;
            setTime(9.999, time_lane_2);
          }
          if(f3 == 0){
            showPlace(2, 9);
            f3++;
            setTime(9.999, time_lane_3);
          }
          if(f4 == 0){
            showPlace(3,9);
            f4++;
            setTime(9.999,time_lane_4);
          }
          pos = 4; 
        }

  }while(pos < 4);
  Serial.println("Raced Finished");
}

void resetTrack(){
  Serial.println("Reset Pushed");
  /*
  int c = 0;
  do {


    c++;
    delay(100);
  }while(c < 5);
  */
  moveServo(45);
  showDash();
  delay(100);

  //int myAry[5] = {82,101,97,100,121};
  //displayStatus(myAry);
  Serial.println("Reset Finished");
  
}

void moveServo(int x){

  // sg_pos

 if(sg_pos < x){
    do{
      sg_pos++;
      if(sg_pos > 180){
        sg_pos = 180;
      }
      sg.write(sg_pos);
    }while(sg_pos < x);
 }else{
   do{
     sg_pos--;
     if(sg_pos < 1){
       sg_pos = 1;
     }
     sg.write(sg_pos);
   }while(sg_pos > x);
  }
}

/**************************************************************************/

double pTime(int l, unsigned long start){

  float h,m,s,ms;
  unsigned long over;

        finish = millis();
        elapsed = finish-start;
        over=elapsed%60000;
        s=int(over/1000);
        ms=over%1000;

        Serial.print("Lane ");
        Serial.print(l);
        Serial.print(": ");
        Serial.print(s,0);
        Serial.print(".");
        Serial.println(ms,0);

        double mil_sec = ms/1000;
        double lap_time = s + mil_sec;
        Serial.println(lap_time);

  return lap_time;
}

void showDash(){
    setNum(0,dash);
    setNum(1,dash);
    setNum(2,dash);
    setNum(3,dash);

    time_lane_1.clear();
    time_lane_2.clear();
    time_lane_3.clear();
    time_lane_4.clear();
    time_lane_1.writeDisplay();
    time_lane_2.writeDisplay();
    time_lane_3.writeDisplay();
    time_lane_4.writeDisplay();


}

void showPlace(int lane, int place){

  if(place == 1){
    setNum(lane,frst);
  }else if(place == 2){
    setNum(lane,scnd);
  }else if(place == 3){
    setNum(lane,thrd);
  }else if(place == 4){
    setNum(lane,four);
  }else if(place == 9){
    setNum(lane,error);
  }

}

void setNum(int x, byte * ary){
  for(int i = 0; i < 8; i++){
    lc.setRow(x,i,ary[i]);
  }
}

/*
65 = A 97, 66 = B 98, 67 = C 99, 68 = D 100, 69 = E 101, 70 = F 102, 71 = G 103,
72 = H 104, 73 = I 105, 74 = J 106, 75 = K 107, 76 = L 108, 77 = M 109, 78 = N 110,
79 = O 111, 80 = P 112, 81 = Q 113, 82 = R 114, 83 = S 115, 84 = T 116, 85 = U 117,
86 = V 118, 87 = W 119, 88 = X 120, 89 = Y 121, 90 = Z 122, 
*/

void displayStatus(int * myAry){

  //int myAry[5] = {82,101,97,100,121};

  for(int i=0; i<5; i++){
    loadBufferLong(myAry[i]);
  }

  // blank it out
  for(int i=0; i < 6; i++){
    loadBufferLong(32);
  }

  for(int i=0; i < 4; i++){
    lc.clearDisplay(i);
  }
}



// Scroll Message
void scrollMessage(prog_uchar * messageString) {
    int counter = 0;
    int myChar=0;
    do {
        // read back a char 

        myChar =  pgm_read_byte_near(messageString + counter);
        Serial.println(myChar);
        if (myChar != 0){
            loadBufferLong(myChar);
        }
        counter++;
    }
    while (myChar != 0);
}
// Load character into scroll buffer
void loadBufferLong(int ascii){
    if (ascii >= 0x20 && ascii <=0x7f){
        for (int a=0;a<7;a++){                      // Loop 7 times for a 5x7 font
            unsigned long c = pgm_read_byte_near(font5x7 + ((ascii - 0x20) * 8) + a);     // Index into character table to get row data
            unsigned long x = bufferLong [a*2];     // Load current scroll buffer
            x = x | c;                              // OR the new character onto end of current
            bufferLong [a*2] = x;                   // Store in buffer
        }
        byte count = pgm_read_byte_near(font5x7 +((ascii - 0x20) * 8) + 7);     // Index into character table for kerning data
        for (byte x=0; x<count;x++){
            rotateBufferLong();
            printBufferLong();
            delay(scrollDelay);
        }
    }
}
// Rotate the buffer
void rotateBufferLong(){
    for (int a=0;a<7;a++){                      // Loop 7 times for a 5x7 font
        unsigned long x = bufferLong [a*2];     // Get low buffer entry
        byte b = bitRead(x,31);                 // Copy high order bit that gets lost in rotation
        x = x<<1;                               // Rotate left one bit
        bufferLong [a*2] = x;                   // Store new low buffer
        x = bufferLong [a*2+1];                 // Get high buffer entry
        x = x<<1;                               // Rotate left one bit
        bitWrite(x,0,b);                        // Store saved bit
        bufferLong [a*2+1] = x;                 // Store new high buffer
    }
}
// Display Buffer on LED matrix
void printBufferLong(){
  for (int a=0;a<7;a++){                    // Loop 7 times for a 5x7 font
    unsigned long x = bufferLong [a*2+1];   // Get high buffer entry
    byte y = x;                             // Mask off first character
    lc.setRow(0,a,y);                       // Send row to relevent MAX7219 chip
    x = bufferLong [a*2];                   // Get low buffer entry
    y = (x>>24);                            // Mask off second character
    lc.setRow(1,a,y);                       // Send row to relevent MAX7219 chip
    y = (x>>16);                            // Mask off third character
    lc.setRow(2,a,y);                       // Send row to relevent MAX7219 chip
    y = (x>>8);                             // Mask off forth character
    lc.setRow(3,a,y);                       // Send row to relevent MAX7219 chip
  }
}

void showPoll(int x, Adafruit_7segment m){

    m.writeDigitNum(1, x, 0);
      m.writeDisplay();

}

void setTime(double time, Adafruit_7segment m){

   m.printFloat(time,3);
    m.writeDisplay();

}



