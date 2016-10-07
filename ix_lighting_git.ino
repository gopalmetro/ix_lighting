#include <IRremote.h> //Include IRremote library
#include <FastLED.h>  //Include FastLED library

#define NO_CORRECTION 1 //Attempting to fix flicker issues (seems to have worked!!!)

#define DEBUG 0

// IRremote setup
#define RECV_PIN 11   //set the IRremote receive pin
IRrecv irrecv(RECV_PIN);   //activate reception
decode_results results;   //decode any IR signal received
#define NEC   //Define the IR device type

// FastLED setup
int const LED_COUNT = 13;  //13 LEDs in the room
#define LED_DT 3  //FastLED signal pin on the Arduino
#define MAX_BRIGHTNESS 120 //DEFINES MAXIMUM BRIGHTNESS
#define MIN_BRIGHTNESS 0 //DEFINES MINIMUM BRIGHTNESS
#define KEY_PRESS_DELAY 800
#define KEY_PRESS_INCREMENT 10
int BRIGHTNESS = 90;  //Initial brightness. Keep this at 180 or below


//CRGB leds[LED_COUNT]; // Define the array of leds - THIS IS REPEATED IN THE SPI EFFECTS BELOW

//---FASTSPI--- SERIAL/SOFTWARE SERIAL SETUP STUFF
//#define SERIAL_BAUDRATE 9600
//#define SERIAL_BAUDRATE 57600
#define SERIAL_BAUDRATE 115200
#define SERIAL_TIMEOUT 5

//FASTSPI2_EFFECTS INFO
int BOTTOM_INDEX = 0;
int TOP_INDEX = int(LED_COUNT/2);
int EVENODD = LED_COUNT%2;
struct CRGB leds[LED_COUNT];
int ledsX[LED_COUNT][3];     //-ARRAY FOR COPYING WHATS IN THE LED STRIP CURRENTLY (FOR CELL-AUTOMATA, MARCH, ETC)
//int ledMode = 3;           //-START IN RAINBOW LOOP
//int ledMode = 888;         //-START IN DEMO MODE
int ledMode = 6;             //-MODE TESTING

int thisdelay = 800;          //-FX LOOPS DELAY VAR (ORIGINALLY 200)
int fxdelay = 400;
int thisstep = 10;           //-FX LOOPS DELAY VAR
int thishue = 0;             //-FX LOOPS DELAY VAR
int thissat = 255;           //-FX LOOPS DELAY VAR
int max_bright = 120;        //-SET MAX BRIGHTNESS TO 1/4

int thisindex = 0;           //-SET SINGLE LED VAR
int thisRED = 255;
int thisGRN = 255;
int thisBLU = 255;

//---FASTSPI LED FX VARS
int idex = 0;                //-LED INDEX (0 to LED_COUNT-1
int ihue = 255;                //-HUE (0-255)
int ibright = 255;             //-BRIGHTNESS (0-255)
int isat = 255;                //-SATURATION (0-255)
int bouncedirection = 0;     //-SWITCH FOR COLOR BOUNCE (0-1)
float tcount = 0.0;          //-INC VAR FOR SIN LOOPS
int lcount = 0;              //-ANOTHER COUNTING VAR


// IX CUSTOM INFO
int powerState = 0;
boolean loopFlag;

// REMOTE CODE VARIABLES
//int POWER
//int PLAY
//int BRIGHTNESS_UP
//int BRIGHTENESS_DOWN
//int R1
//int R2
//int R3
//int R4
//int R5
//int G1
//int G2
//int G3
//int G4
//int G5
//int B1
//int B2
//int B3
//int B4
//int B5
//int W1
//int W2
//int W3
//int W4
//int W5
//int R_UP
//int R_DOWN
//int G_UP
//int G_DOWN
//int B_UP
//int B_DOWN
//int QUICK
//int SLOW
//int DIY1
//int DIY2
//int DIY3
//int DIY4
//int DIY5
//int DIY6
//int AUTO
//int FLASH
//int JUMP3
//int JUMP7
//int FADE3
//int FADE7


//int POWER = 0xFF02FD;
//int PLAY;
//int BRIGHTNESS_UP;
//int BRIGHTENESS_DOWN;
//int RED1;
//int RED2;
//int RED3;
//int RED4;
//int RED5;
//int GREEN1;
//int GREEN2;
//int GREEN3;
//int GREEN4;
//int GREEN5;
//int BLUE1;
//int BLUE2;
//int BLUE3;
//int BLUE4;
//int BLUE5;
//int WHITE1;
//int WHITE2;
//int WHITE3;
//int WHITE4;
//int WHITE5;
//int R_UP;
//int R_DOWN;
//int G_UP;
//int G_DOWN;
//int B_UP;
//int B_DOWN;
//int QUICK;
//int SLOW;
//int DIY1;
//int DIY2;
//int DIY3;
//int DIY4;
//int DIY5;
//int DIY6;
//int AUTO;
//int FLASH;
//int JUMP3;
//int JUMP7;
//int FADE3;
//int FADE7;


//------------------------------------- UTILITY FXNS --------------------------------------
//---SET THE COLOR OF A SINGLE RGB LED
void set_color_led(int adex, int cred, int cgrn, int cblu) {  
  leds[adex].setRGB( cred, cgrn, cblu);
} 

//---FIND INDEX OF HORIZONAL OPPOSITE LED
int horizontal_index(int i) {
  //-ONLY WORKS WITH INDEX < TOPINDEX
  if (i == BOTTOM_INDEX) {return BOTTOM_INDEX;}
  if (i == TOP_INDEX && EVENODD == 1) {return TOP_INDEX + 1;}
  if (i == TOP_INDEX && EVENODD == 0) {return TOP_INDEX;}
  return LED_COUNT - i;  
}

//---FIND INDEX OF ANTIPODAL OPPOSITE LED
int antipodal_index(int i) {
  int iN = i + TOP_INDEX;
  if (i >= TOP_INDEX) {iN = ( i + TOP_INDEX ) % LED_COUNT; }
  return iN;
}

//---FIND ADJACENT INDEX CLOCKWISE
int adjacent_cw(int i) {
  int r;
  if (i < LED_COUNT - 1) {r = i + 1;}
  else {r = 0;}
  return r;
}

//---FIND ADJACENT INDEX COUNTER-CLOCKWISE
int adjacent_ccw(int i) {
  int r;
  if (i > 0) {r = i - 1;}
  else {r = LED_COUNT - 1;}
  return r;
}

void copy_led_array(){
  for(int i = 0; i < LED_COUNT; i++ ) {
    ledsX[i][0] = leds[i].r;
    ledsX[i][1] = leds[i].g;
    ledsX[i][2] = leds[i].b;
  }  
}


// ---IX--- TURN THE LIGHTS ON AND OFF
void powerCycle() {
   if (powerState == 1) {
         one_color_all(0,0,0); powerState = 0; LEDS.show(); delay(KEY_PRESS_DELAY);
       Serial.println("Turning off. Power state is: ");
       Serial.println (powerState);     
   } else if (powerState == 0) {
     BRIGHTNESS = MAX_BRIGHTNESS;
     FastLED.setBrightness(BRIGHTNESS);
       one_color_all(255,255,255); powerState = 1; LEDS.show(); delay(KEY_PRESS_DELAY);
     Serial.println("Turning on. Power state is: ");
     Serial.println (powerState);
   }
   FastLED.delay(500);
}


//------------------------LED EFFECT FUNCTIONS------------------------
void one_color_all(int cred, int cgrn, int cblu) {       //-SET ALL LEDS TO ONE COLOR
    for(int i = 0 ; i < LED_COUNT; i++ ) {
      leds[i].setRGB( cred, cgrn, cblu);
    }
}

void one_color_allHEX(int chex) {       //-SET ALL LEDS TO ONE COLOR
    for(int i = 0 ; i < LED_COUNT; i++ ) {
      leds[i] = chex;
    }
}

void one_color_allCRGB(int crgbin) {       //-SET ALL LEDS TO ONE COLOR
    for(int i = 0 ; i < LED_COUNT; i++ ) {
      leds[i] = crgbin;
    }
}

void one_color_allHSV(int ahue) {    //-SET ALL LEDS TO ONE COLOR (HSV)
    for(int i = 0 ; i < LED_COUNT; i++ ) {
      leds[i] = CHSV(ahue, thissat, 255);
    }
}

void rainbow_fade() {                         //-m2-FADE ALL LEDS THROUGH HSV RAINBOW
    ihue++;
    if (ihue > 255) {ihue = 0;}
    for(int idex = 0 ; idex < LED_COUNT; idex++ ) {
      leds[idex] = CHSV(ihue, thissat, 255);
    }
    LEDS.show();    
    delay(thisdelay);
}

void rainbow_loop() {                        //-m3-LOOP HSV RAINBOW
  idex++;
  ihue = ihue + thisstep;
  if (idex >= LED_COUNT) {idex = 0;}
  if (ihue > 255) {ihue = 0;}
  leds[idex] = CHSV(ihue, thissat, 255);
  LEDS.show();
  delay(thisdelay);
}

void random_burst() {                         //-m4-RANDOM INDEX/COLOR
  idex = random(0, LED_COUNT);
  ihue = random(0, 255);  
  leds[idex] = CHSV(ihue, thissat, 255);
  LEDS.show();
  delay(thisdelay);
}

void color_bounce() {                        //-m5-BOUNCE COLOR (SINGLE LED)
  if (bouncedirection == 0) {
    idex = idex + 1;
    if (idex == LED_COUNT) {
      bouncedirection = 1;
      idex = idex - 1;
    }
  }
  if (bouncedirection == 1) {
    idex = idex - 1;
    if (idex == 0) {
      bouncedirection = 0;
    }
  }  
  for(int i = 0; i < LED_COUNT; i++ ) {
    if (i == idex) {leds[i] = CHSV(thishue, thissat, 255);}
    else {leds[i] = CHSV(0, 0, 0);}
  }
  LEDS.show();
  delay(thisdelay);
}

void color_bounceFADE() {  //-m6-BOUNCE COLOR (SIMPLE MULTI-LED FADE)
  thishue = random(0, 255);
  if (bouncedirection == 0) {
    idex = idex + 1;
    if (idex == LED_COUNT) {
      bouncedirection = 1;
      idex = idex - 1;
    }
  }
  if (bouncedirection == 1) {
    idex = idex - 1;
    if (idex == 0) {
      bouncedirection = 0;
    }
  }
  int iL1 = adjacent_cw(idex);
  int iL2 = adjacent_cw(iL1);
  int iL3 = adjacent_cw(iL2);
  int iR1 = adjacent_ccw(idex);
  int iR2 = adjacent_ccw(iR1);
  int iR3 = adjacent_ccw(iR2);
  for(int i = 0; i < LED_COUNT; i++ ) {
    if (i == idex) {leds[i] = CHSV(thishue, thissat, 255);}
    else if (i == iL1) {leds[i] = CHSV(thishue, thissat, 150);}
    else if (i == iL2) {leds[i] = CHSV(thishue, thissat, 80);}
    else if (i == iL3) {leds[i] = CHSV(thishue, thissat, 20);}        
    else if (i == iR1) {leds[i] = CHSV(thishue, thissat, 150);}
    else if (i == iR2) {leds[i] = CHSV(thishue, thissat, 80);}
    else if (i == iR3) {leds[i] = CHSV(thishue, thissat, 20);}    
    else {leds[i] = CHSV(0, 0, 0);}
  }
  LEDS.show();
  delay(300);
}

void flicker() {                          //-m9-FLICKER EFFECT
  int random_bright = random(MIN_BRIGHTNESS,MAX_BRIGHTNESS);
  int random_delay = random(10,100);
  int random_bool = random(0,random_bright);
  if (random_bool < 10) {
    for(int i = 0 ; i < LED_COUNT; i++ ) {
      leds[i] = CHSV(thishue, thissat, random_bright);
    }
    LEDS.show();
    delay(random_delay);
  }
}

void pulse_one_color_all() {              //-m10-PULSE BRIGHTNESS ON ALL LEDS TO ONE COLOR 
  if (bouncedirection == 0) {
    ibright = ibright + 1;
    if (ibright >= MAX_BRIGHTNESS) {bouncedirection = 1;}
  }
  if (bouncedirection == 1) {
    ibright = ibright - 1;
    if (ibright <= (MIN_BRIGHTNESS + 1)) {bouncedirection = 0;}         
  }  
    for(int idex = 0 ; idex < LED_COUNT; idex++ ) {
      leds[idex] = CHSV(thishue, thissat, ibright);
      Serial.println(ibright);
    }
    LEDS.show();    
    delay(thisdelay);
}

void pulse_one_color_all_rev() {           //-m11-PULSE SATURATION ON ALL LEDS TO ONE COLOR 
  if (bouncedirection == 0) {
    isat++;
    if (isat >= 255) {bouncedirection = 1;}
  }
  if (bouncedirection == 1) {
    isat = isat - 1;
    if (isat <= 1) {bouncedirection = 0;}         
  }  
    for(int idex = 0 ; idex < LED_COUNT; idex++ ) {
      leds[idex] = CHSV(thishue, isat, 255);
    }
    LEDS.show();
    delay(thisdelay);
}

void fade_vertical() {                    //-m12-FADE 'UP' THE LOOP
  idex++;
  if (idex > TOP_INDEX) {idex = 0;}  
  int idexA = idex;
  int idexB = horizontal_index(idexA);
  ibright = ibright + 10;
  if (ibright > MAX_BRIGHTNESS) {ibright = 0;}
  leds[idexA] = CHSV(thishue, thissat, ibright);
  leds[idexB] = CHSV(thishue, thissat, ibright);
  LEDS.show();
  delay(thisdelay);
}

void color_loop_vardelay() {                    //-m17-COLOR LOOP (SINGLE LED) w/ VARIABLE DELAY
  idex++;
  if (idex > LED_COUNT) {idex = 0;}
  int di = abs(TOP_INDEX - idex);
  int t = constrain((10/di)*10, 10, 500);
  for(int i = 0; i < LED_COUNT; i++ ) {
    if (i == idex) {
      leds[i] = CHSV(0, thissat, 255);
    }
    else {
      leds[i].r = 0; leds[i].g = 0; leds[i].b = 0;
    }
  }
  LEDS.show();  
  delay(t);
}

void sin_bright_wave() {        //-m19-BRIGHTNESS SINE WAVE
  for(int i = 0; i < LED_COUNT; i++ ) {
    tcount = tcount + .1;
    if (tcount > 3.14) {tcount = 0.0;}
    ibright = int(sin(tcount)*255);
    leds[i] = CHSV(thishue, thissat, ibright);
    LEDS.show();    
    delay(thisdelay);
  }
}

void quad_bright_curve() {      //-m21-QUADRATIC BRIGHTNESS CURVER
  int ax;    
  for(int x = 0; x < LED_COUNT; x++ ) {
    if (x <= TOP_INDEX) {ax = x;}
    else if (x > TOP_INDEX) {ax = LED_COUNT-x;}
    int a = 1; int b = 1; int c = 0;
    int iquad = -(ax*ax*a)+(ax*b)+c; //-ax2+bx+c
    int hquad = -(TOP_INDEX*TOP_INDEX*a)+(TOP_INDEX*b)+c;
    ibright = int((float(iquad)/float(hquad))*255);
    leds[x] = CHSV(thishue, thissat, ibright);
  }
  LEDS.show();  
  delay(thisdelay);
}

void flame() {                                    //-m22-FLAMEISH EFFECT
  int idelay = random(0,35);
  float hmin = 0.1; float hmax = 45.0;
  float hdif = hmax-hmin;
  int randtemp = random(0,3);
  float hinc = (hdif/float(TOP_INDEX))+randtemp;
  int ihue = hmin;
  for(int i = 0; i <= TOP_INDEX; i++ ) {
    ihue = ihue + hinc;
    leds[i] = CHSV(ihue, thissat, 255);
    int ih = horizontal_index(i);    
    leds[ih] = CHSV(ihue, thissat, 255);    
    leds[TOP_INDEX].r = 255; leds[TOP_INDEX].g = 255; leds[TOP_INDEX].b = 255;    
    LEDS.show();    
    delay(idelay);
  }
}

void rainbow_vertical() {                        //-m23-RAINBOW 'UP' THE LOOP
  idex++;
  if (idex > TOP_INDEX) {idex = 0;}  
  ihue = ihue + thisstep;
  if (ihue > 255) {ihue = 0;}
  int idexA = idex;
  int idexB = horizontal_index(idexA);
  leds[idexA] = CHSV(ihue, thissat, 255);
  leds[idexB] = CHSV(ihue, thissat, 255);
  LEDS.show();  
  delay(thisdelay);
}

void random_color_pop() {                         //-m25-RANDOM COLOR POP
  idex = random(0, LED_COUNT);
  ihue = random(0, 255);
  one_color_all(0, 0, 0);
  leds[idex] = CHSV(ihue, thissat, 255);
  LEDS.show();
  delay(thisdelay);
}

void new_rainbow_loop(){                       //-m88-RAINBOW FADE FROM FAST_SPI2
  ihue -= 1;
  fill_rainbow( leds, LED_COUNT, ihue );
  LEDS.show();
  delay(thisdelay);
}


void setup()
{
  Serial.begin(SERIAL_BAUDRATE);      // SETUP HARDWARE SERIAL (USB)
  Serial.setTimeout(SERIAL_TIMEOUT);
  
  if (DEBUG == 0) {
    Serial.println("Debug is off");
  } else if (DEBUG == 1) {
    Serial.println("Debug is on");
  } else {
    Serial.println("Debug error");
  }
  
  irrecv.enableIRIn(); // Start the receiver
  irrecv.blink13(true);
  BRIGHTNESS = 0;
  FastLED.addLeds<WS2811, LED_DT, RGB>(leds, LED_COUNT); //configures FastLED for our setup
//  FastLED.clear(); //On reset, clear all LEDs values
  FastLED.setBrightness(BRIGHTNESS);
  one_color_all(0,0,0); LEDS.show(); delay(KEY_PRESS_DELAY); 
//  for (int i = 0; i < LED_COUNT; i++) {
//       leds[i] = CRGB::Black;
//       FastLED.show();
//  }
  FastLED.delay(1000);
  
}

void loop()   /*----( LOOP: RUNS CONSTANTLY )----*/
{
//  if (powerState == 0) {
//    one_color_all(0,0,0); powerState = 0; LEDS.show(); delay(KEY_PRESS_DELAY);
//    if (DEBUG == 1) {
//      Serial.println("Keeping the lights off.");
//      Serial.println (powerState);
//    }
//  }
    
  // BRIGHTNESS CHECKS
  if (BRIGHTNESS >= MAX_BRIGHTNESS) {
        BRIGHTNESS = MAX_BRIGHTNESS;
      }
  if (BRIGHTNESS <= MIN_BRIGHTNESS) {
        BRIGHTNESS = MIN_BRIGHTNESS;
      }
      
  // have we received an IR signal?
  if (irrecv.decode(&results)) {
    translateIR(); //commented out for testing
    irrecv.resume(); // receive the next value
    loopFlag = 0;
  }
}


void translateIR()
  {
  
    switch(results.value)
  
    {
  
    case 0xFF02FD:  
      Serial.println(" POWER          ");
      powerCycle(); 
      break;
  
    case 0xFF3AC5:  
      Serial.println(" BRIGHTNESS UP  ");
      BRIGHTNESS = BRIGHTNESS + KEY_PRESS_INCREMENT;
      if (BRIGHTNESS >= MAX_BRIGHTNESS) {
        BRIGHTNESS = MAX_BRIGHTNESS;
      }
      Serial.println (BRIGHTNESS);
      FastLED.setBrightness(BRIGHTNESS);
      FastLED.show(); 
      break;
  
    case 0xFFBA45:  
      Serial.println(" BRIGHTNESS DOWN");
      BRIGHTNESS = BRIGHTNESS - KEY_PRESS_INCREMENT;
      if (BRIGHTNESS <= MIN_BRIGHTNESS) {
        BRIGHTNESS = MIN_BRIGHTNESS;
      }
      Serial.println (BRIGHTNESS);
      FastLED.setBrightness(BRIGHTNESS);
      FastLED.show(); 
      break;
  
    case 0xFF1AE5:  
      Serial.println(" RED1             ");
      one_color_allHSV(255); LEDS.show(); delay(KEY_PRESS_DELAY);
      powerState = 1;
      break;
  
    case 0xFF2AD5:  
      Serial.println(" RED2             ");
      one_color_allHSV(15); LEDS.show(); delay(KEY_PRESS_DELAY);
      powerState = 1;
      break;
  
    case 0xFF0AF5:  
      Serial.println(" RED3            ");
      one_color_allHSV(31); LEDS.show(); delay(KEY_PRESS_DELAY);
      powerState = 1;
      break;
  
    case 0xFF38C7:  
      Serial.println(" RED4                ");
      one_color_allHSV(47); LEDS.show(); delay(KEY_PRESS_DELAY);
      powerState = 1;
      break;
  
    case 0xFF18E7:  
      Serial.println(" RED5                ");
      one_color_allHSV(63); LEDS.show(); delay(KEY_PRESS_DELAY);
      powerState = 1;
      break;
  
    case 0xFF9A65:  
      Serial.println(" GREEN1              ");
      one_color_allHSV(90); LEDS.show(); delay(KEY_PRESS_DELAY);
      powerState = 1; 
      break;
  
    case 0xFFAA55:  
      Serial.println(" GREEN2              ");
      one_color_allHSV(109); LEDS.show(); delay(KEY_PRESS_DELAY);
      powerState = 1; 
      break;
  
    case 0xFF8A75:  
      Serial.println(" GREEN3              ");
      one_color_allHSV(128); LEDS.show(); delay(KEY_PRESS_DELAY);
      powerState = 1; 
      break;
  
    case 0xFFB847:  
      Serial.println(" GREEN4             ");
      one_color_allHSV(138); LEDS.show(); delay(KEY_PRESS_DELAY);
      powerState = 1; 
      break;
  
    case 0xFF9867:  
      Serial.println(" GREEN5             ");
      one_color_allHSV(149); LEDS.show(); delay(KEY_PRESS_DELAY);
      powerState = 1; 
      break;
  
    case 0xFFA25D:
      Serial.println(" BLUE1              ");
      one_color_allHSV(165); LEDS.show(); delay(KEY_PRESS_DELAY);
      powerState = 1; 
      break;
  
    case 0xFF926D:  
      Serial.println(" BLUE2              ");
      one_color_allHSV(180); LEDS.show(); delay(KEY_PRESS_DELAY);
      powerState = 1; 
      break;
  
    case 0xFFB24D:  
      Serial.println(" BLUE3              ");
      one_color_allHSV(192); LEDS.show(); delay(KEY_PRESS_DELAY);
      powerState = 1; 
      break;
      
    case 0xFF7887:  
      Serial.println(" BLUE4              "); 
      one_color_allHSV(224); LEDS.show(); delay(KEY_PRESS_DELAY);
      powerState = 1;
      break;
  
    case 0xFF58A7:  
      Serial.println(" BLUE5              ");
      one_color_allHSV(235); LEDS.show(); delay(KEY_PRESS_DELAY);
      powerState = 1; 
      break;
  
    case 0xFF22DD:  
      Serial.println(" WHITE1             ");
      one_color_all(230,255,230); LEDS.show(); delay(KEY_PRESS_DELAY);
      powerState = 1; 
      break;
    
    case 0xFF12ED:  
      Serial.println(" WHITE2             ");
      one_color_all(200,200,255); LEDS.show(); delay(KEY_PRESS_DELAY);
      powerState = 1; 
      break;
    
    case 0xFF32CD:  
      Serial.println(" WHITE3             ");
      one_color_all(255,200,200); LEDS.show(); delay(KEY_PRESS_DELAY);
      powerState = 1; 
      break;
    
    case 0xFFF807:  
      Serial.println(" WHITE4             ");
      one_color_all(200,255,200); LEDS.show(); delay(KEY_PRESS_DELAY);
      powerState = 1; 
      break;
    
    case 0xFFD827:  
      Serial.println(" WHITE5             ");
      one_color_all(255,200,255); LEDS.show(); delay(KEY_PRESS_DELAY);
      powerState = 1; 
      break;
  
    case 0xFF28D7:  
      Serial.println(" RED_UP             ");
      for(int i = 0; i < LED_COUNT; i++ ) {
        leds[i] += CRGB( KEY_PRESS_INCREMENT, 0, 0);
      } 
      LEDS.show(); 
      delay(KEY_PRESS_DELAY);
      powerState = 1;
      break;
  
    case 0xFF08F7:  
      Serial.println(" RED_DOWN          ");
      for(int i = 0; i < LED_COUNT; i++ ) {
        leds[i] -= CRGB( KEY_PRESS_INCREMENT, 0, 0);
      } 
      LEDS.show(); 
      delay(KEY_PRESS_DELAY);
      powerState = 1;
      break;
  
    case 0xFFA857:  
      Serial.println(" GREEN_UP          ");
      for(int i = 0; i < LED_COUNT; i++ ) {
        leds[i] += CRGB( 0, KEY_PRESS_INCREMENT, 0);
      } 
      LEDS.show(); 
      delay(KEY_PRESS_DELAY); 
      powerState = 1;
      break;
  
    case 0xFF8877:  
      Serial.println(" GREEN_DOWN        ");
      for(int i = 0; i < LED_COUNT; i++ ) {
        leds[i] -= CRGB( 0, KEY_PRESS_INCREMENT, 0);
      } 
      LEDS.show(); 
      delay(KEY_PRESS_DELAY); 
      powerState = 1;
      break;
  
    case 0xFF6897:  
      Serial.println(" BLUE_UP           ");
      for(int i = 0; i < LED_COUNT; i++ ) {
        leds[i] += CRGB( 0, 0, KEY_PRESS_INCREMENT);
      } 
      LEDS.show(); 
      delay(KEY_PRESS_DELAY); 
      powerState = 1;
      break;
  
    case 0xFF48B7:  
      Serial.println(" BLUE_DOWN         ");
      for(int i = 0; i < LED_COUNT; i++ ) {
        leds[i] -= CRGB( 0, 0, KEY_PRESS_INCREMENT);
      } 
      LEDS.show(); 
      delay(KEY_PRESS_DELAY); 
      powerState = 1;
      break;
  
    case 0xFFE817:  
      Serial.println(" QUICK             ");
      fxdelay = fxdelay + 20;
      if (fxdelay <= 100) {
        fxdelay = 100;
      }
      if (fxdelay >= 1000) {
        fxdelay = 1000;
      }
      FastLED.show(); //DO I NEED THIS?  
      powerState = 1;
      break;
  
    case 0xFFC837:  
      Serial.println(" SLOW             ");
      fxdelay = fxdelay - 20;
      if (fxdelay <= 100) {
        fxdelay = 100;
      }
      if (fxdelay >= 1000) {
        fxdelay = 1000;
      }
      FastLED.show(); //DO I NEED THIS?  
      powerState = 1;
      break;
  
    case 0xFF30CF:  
      Serial.println(" DIY_1             ");
       
      powerState = 1;
      break;
  
    case 0xFFB04F:  
      Serial.println(" DIY_2             ");
       
      powerState = 1;
      break;
  
    case 0xFF708F:  
      Serial.println(" DIY_3             ");
       
      powerState = 1;
      break;
  
    case 0xFF10EF:  
      Serial.println(" DIY_4             ");
       
      powerState = 1;
      break;
  
    case 0xFF906F:  
      Serial.println(" DIY_5             ");
       
      powerState = 1;
      break;
  
    case 0xFF50AF:  
      Serial.println(" DIY_6             ");
      
      powerState = 1;
      break;
  
    case 0xFFF00F:  
      Serial.println(" AUTO              ");
      do {
        irrecv.resume(); //THIS RESETS THE IR READER
        Serial.println(results.value); //THIS IS THE CURRENTLY STORED IR VALUE
        color_bounceFADE(); // THIS IS THE EFFECT
        if (irrecv.decode(&results)) {
            translateIR();
            irrecv.resume(); // receive the next IR value
          }
        } while (results.value == 16764975 || results.value == 4294967295); // SET THIS TO THE CURRENTLY STORED IR VALUE
      powerState = 1;
      break;
  
    case 0xFFD02F: 
      Serial.println(" FLASH             ");
       do {
        irrecv.resume(); //THIS RESETS THE IR READER
        Serial.println(results.value); //THIS IS THE CURRENTLY STORED IR VALUE
        random_color_pop(); // THIS IS THE EFFECT
        if (irrecv.decode(&results)) {
            translateIR();
            irrecv.resume(); // receive the next IR value
          }
        } while (results.value == 16764975 || results.value == 4294967295); // SET THIS TO THE CURRENTLY STORED IR VALUE
      powerState = 1;
      break;
  
    case 0xFF20DF: 
      Serial.println(" JUMP3             ");
      do {
        irrecv.resume(); //THIS RESETS THE IR READER
        //Serial.println(results.value); //THIS IS THE CURRENTLY STORED IR VALUE
        pulse_one_color_all_rev(); // THIS IS THE EFFECT
        if (irrecv.decode(&results)) {
            translateIR();
            irrecv.resume(); // receive the next IR value
          }
        } while (results.value == 16720095 || results.value == 4294967295); // SET THIS TO THE CURRENTLY STORED IR VALUE
      powerState = 1;
      break;
  
    case 0xFFA05F:  
      Serial.println(" JUMP7             ");
      do {
        irrecv.resume(); //THIS RESETS THE IR READER
        Serial.println(results.value); //THIS IS THE CURRENTLY STORED IR VALUE
        pulse_one_color_all(); // THIS IS THE EFFECT
        if (irrecv.decode(&results)) {
            translateIR();
            irrecv.resume(); // receive the next IR value
          }
        } while (results.value == 16752735 || results.value == 4294967295); // SET THIS TO THE CURRENTLY STORED IR VALUE
      powerState = 1;
      break;
  
    case 0xFF609F:  
      Serial.println(" FADE3             ");
      do {
        irrecv.resume(); //THIS RESETS THE IR READER
        Serial.println(results.value); //THIS IS THE CURRENTLY STORED IR VALUE
        fade_vertical(); // THIS IS THE EFFECT
        if (irrecv.decode(&results)) {
            translateIR();
            irrecv.resume(); // receive the next IR value
          }
        } while (results.value == 16769055 || results.value == 4294967295); // SET THIS TO THE CURRENTLY STORED IR VALUE 
      powerState = 1;
      break;
  
    case 0xFFE01F:  
      Serial.println(" FADE7             ");
      do {
        irrecv.resume(); //THIS RESETS THE IR READER
        Serial.println(results.value); //THIS IS THE CURRENTLY STORED IR VALUE
        rainbow_fade(); // THIS IS THE EFFECT
        if (irrecv.decode(&results)) {
            translateIR();
            irrecv.resume(); // receive the next IR value
          }
        } while (results.value == 16769055 || results.value == 4294967295); // SET THIS TO THE CURRENTLY STORED IR VALUE
      powerState = 1;
      break;
  
    default:
      Serial.println(" other button");
      Serial.println(results.value);
  
    }  
  
} //END translateIR

