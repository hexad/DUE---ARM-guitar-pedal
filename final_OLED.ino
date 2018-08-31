// Licensed under a Creative Commons Attribution 3.0 Unported License.
// Based on rcarduino.blogspot.com previous work.
// www.electrosmash.com/pedalshield
//https://www.arduino.cc/en/Reference/Sizeof
 
//Header and constructor for U8glib
#include "U8glib.h"
// HW SPI Com: CS = 8, A0 = 9, RST = 10 (Hardware Pins are  SCK = 13 and MOSI = 11)
U8GLIB_SSD1306_128X64 u8g(8, 9, 10);

  //GENERAL VARS
  int in_ADC0, in_ADC1;  //variables for 2 ADCs values (ADC0, ADC1)
  int POT0, POT1, POT2, out_DAC0, out_DAC1; //variables for 3 pots (ADC8, ADC9, ADC10)
  int LED = 3;
  int FOOTSWITCH = 7; 
  int TOGGLE = 2; 
  int toggle_value = 0;
  int effect=0;

/* MULTIEFFECT VARS */
  #define MAX_DELAY 40000
  uint16_t sDelayBuffer0[MAX_DELAY];
  unsigned int DelayCounter = 0;
  unsigned int Delay_Depth;
/*  END  */

//DISTORTION VARS
int upper_threshold, lower_threshold;

//TREMOLO VARS
int sample, accumulator, count, LFO;

//OCTAVER VARS
uint16_t sDelayBuffer1[3000-1];
unsigned int write_pt=0;
unsigned int read_pt_A=0, read_pt_B= 3000/2;
unsigned int increment, divider=0, buffer0, buffer1;

//CHORUS VARS
unsigned int Delay_Depth_B = 300;
unsigned int count_up=1;
int p;

//BITCRUSHER VARS
int counter = 0;
int interval = 0;
unsigned int triangle = 0;
unsigned int trianglecounter = 0;
boolean countup=true;
int toggle_switch = 2;
unsigned int lfo_speed = 0;
unsigned int lfo_depth = 0;


//Hacked Var
bool isSineRun = false;


//********************* OLED VARS ********************
int footswitch_value,old_footswitch_value;
int old_effect=-1;
unsigned long DrawMillis=0;

const unsigned char splashLogo [] U8G_PROGMEM = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x7C, 0x00, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8,
0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x00, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8,
0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8,
0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8,
0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x7C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x01, 0xE0, 0x00, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x01, 0xE0, 0x00, 0x78, 0xF8, 0x30, 0x18, 0xC7, 0xF8, 0xC0, 0x7F, 0x00,
0x00, 0x00, 0x00, 0x00, 0x01, 0xE0, 0x00, 0xF1, 0xFC, 0x30, 0x18, 0xC7, 0xF8, 0xC0, 0x7F, 0xC0,
0x00, 0x00, 0x00, 0x00, 0x01, 0xE0, 0x00, 0xF3, 0x8C, 0x30, 0x18, 0xC7, 0xF8, 0xC0, 0x7F, 0xE0,
0x00, 0x00, 0x00, 0x00, 0x03, 0xC0, 0x00, 0xF3, 0x00, 0x30, 0x18, 0xC6, 0x00, 0xC0, 0x60, 0xF0,
0x00, 0x00, 0x00, 0x00, 0x03, 0xC0, 0x00, 0xF3, 0x00, 0x30, 0x18, 0xC6, 0x00, 0xC0, 0x60, 0x70,
0x00, 0x3F, 0xC0, 0x00, 0x3F, 0xC0, 0x00, 0xE3, 0x80, 0x30, 0x18, 0xC6, 0x00, 0xC0, 0x60, 0x38,
0x00, 0xFF, 0xDF, 0xFC, 0xFF, 0xCF, 0xF9, 0xE3, 0xC0, 0x30, 0x18, 0xC6, 0x00, 0xC0, 0x60, 0x18,
0x01, 0xE7, 0xDE, 0x3D, 0xE7, 0x80, 0x79, 0xE1, 0xF0, 0x3F, 0xF8, 0xC7, 0xF8, 0xC0, 0x60, 0x18,
0x01, 0xE7, 0x9E, 0x79, 0xE7, 0x80, 0x79, 0xE0, 0xF8, 0x3F, 0xF8, 0xC7, 0xF8, 0xC0, 0x60, 0x18,
0x01, 0xEF, 0xBE, 0x7B, 0xE7, 0x80, 0x79, 0xC0, 0x3C, 0x30, 0x18, 0xC6, 0x00, 0xC0, 0x60, 0x18,
0x03, 0xCF, 0xBE, 0x7B, 0xE7, 0x80, 0xFB, 0xC0, 0x0E, 0x30, 0x18, 0xC6, 0x00, 0xC0, 0x60, 0x18,
0x03, 0xCF, 0xBD, 0xFB, 0xCF, 0x3F, 0xFB, 0xC0, 0x06, 0x30, 0x18, 0xC6, 0x00, 0xC0, 0x60, 0x18,
0x03, 0xCF, 0x7C, 0x03, 0xCF, 0x3C, 0xF3, 0xC0, 0x06, 0x30, 0x18, 0xC6, 0x00, 0xC0, 0x60, 0x38,
0x03, 0xDF, 0x7C, 0x07, 0xCF, 0x78, 0xF3, 0xC6, 0x06, 0x30, 0x18, 0xC6, 0x00, 0xC0, 0x60, 0x70,
0x07, 0x9F, 0x7C, 0x07, 0xCF, 0x79, 0xF7, 0x87, 0x06, 0x30, 0x18, 0xC6, 0x00, 0xC0, 0x60, 0xE0,
0x07, 0x9F, 0x78, 0x07, 0x9E, 0x79, 0xF7, 0x83, 0x8E, 0x30, 0x18, 0xC7, 0xF8, 0xFE, 0x7F, 0xE0,
0x07, 0x9E, 0x78, 0x07, 0x9E, 0x79, 0xE7, 0x81, 0xFC, 0x30, 0x18, 0xC7, 0xF8, 0xFE, 0x7F, 0xC0,
0x07, 0xBE, 0x78, 0x0F, 0xFE, 0x7B, 0xE7, 0x80, 0xF8, 0x30, 0x18, 0xC7, 0xF8, 0xFE, 0x7E, 0x00,
0x0F, 0xF8, 0x7F, 0xCF, 0xFC, 0x7F, 0xEF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x1E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x1E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x3E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
 
const unsigned char mixOn [] U8G_PROGMEM = {
  0x00, //00000000 ........
  0x00, //00000000 ........
  0x18, //00011000 ...##...
  0x24, //00100100 ..#..#..
  0x42, //01000010 .#....#.
  0x99, //10011001 #..##..#
  0x99, //10011001 #..##..#
  0x5a, //01011010 .#.##.#.
  0x3c, //00111100 ..####..
  0x3c, //00111100 ..####..
  0x3c, //00111100 ..####..
  0x18  //00011000 ...##...
};
 
const unsigned char mixOff [] U8G_PROGMEM = {
  0x18, //00011000 ...##...
  0x3c, //00111100 ..####..
  0x3c, //00111100 ..####..
  0x3c, //00111100 ..####..
  0x5a, //01011010 .#.##.#.
  0x99, //10011001 #..##..#
  0x99, //10011001 #..##..#
  0x42, //01000010 .#....#.
  0x24, //00100100 ..#..#..
  0x18, //00011000 ...##...
  0x00, //00000000 ........
  0x00  //00000000 ........
};
//********************* OLED VARS END ********************

// create the individual samples for our sinewave table
void createSineTable()
{
  for(uint32_t nIndex=0; nIndex<MAX_DELAY; nIndex++)
  {
    // normalised to 12 bit range 0-4095
    sDelayBuffer0[nIndex] = (uint16_t)  (((1+sin(((2.0*PI)/MAX_DELAY)*nIndex))*4095.0)/2);
  }
}


void setup()
{

  //FIRST THING IN SETUP()---------------------------------------
  //OLED init
  u8g.setColorIndex(1); // pixel on
  u8g.firstPage();  
  do {
     drawSplash();
  } while( u8g.nextPage() );
  delay(10000);
  //-------------------------------------------------------------

  /* turn on the timer clock in the power management controller */
  pmc_set_writeprotect(false);
  pmc_enable_periph_clk(ID_TC4);
 
  /* we want wavesel 01 with RC */
  TC_Configure(/* clock */TC1,/* channel */1, TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC 
  | TC_CMR_TCCLKS_TIMER_CLOCK2);
  TC_SetRC(TC1, 1, 238); // sets <> 44.1 Khz interrupt rate
  //TC_SetRC(TC1, 1, 109); // sets <>   96 Khz interrupt rate
 
  TC_Start(TC1, 1);
 
  // enable timer interrupts on the timer
  TC1->TC_CHANNEL[1].TC_IER=TC_IER_CPCS;
  TC1->TC_CHANNEL[1].TC_IDR=~TC_IER_CPCS;
 
  /* Enable the interrupt in the nested vector interrupt controller */
  /* TC4_IRQn where 4 is the timer number * timer channels (3) + the channel number 
  (=(1*3)+1) for timer1 channel1 */
  NVIC_EnableIRQ(TC4_IRQn);
 
  //ADC Configuration
  ADC->ADC_MR |= 0x80;   // DAC in free running mode.
  ADC->ADC_CR=2;         // Starts ADC conversion.
  ADC->ADC_CHER=0x1CC0;  // Enable ADC channels 0 and 1.  
 
  //DAC Configuration
  analogWrite(DAC0,0);  // Enables DAC0
  analogWrite(DAC1,0);  // Enables DAC0
 
  //configure pin2 as an input and enable the internal pull-up resistor
  pinMode(TOGGLE, INPUT_PULLUP);
  attachInterrupt(TOGGLE, switch_handler, CHANGE); //Toggle switch interruption
  
  pinMode(LED, OUTPUT);
  pinMode(FOOTSWITCH, INPUT); 
}
 
void loop()
{
  //Read the ADCs
  while((ADC->ADC_ISR & 0x1CC0)!=0x1CC0);   // wait for ADC 0, 1, 8, 9, 10 conversion complete.
  in_ADC0=ADC->ADC_CDR[7];             		  // read data from ADC0
  in_ADC1=ADC->ADC_CDR[6];             		  // read data from ADC1  
  POT0=ADC->ADC_CDR[10];                  	// read data from ADC8        
  POT1=ADC->ADC_CDR[11];                    // read data from ADC9   
  POT2=ADC->ADC_CDR[12];                    // read data from ADC10    

  if (digitalRead(FOOTSWITCH)) digitalWrite(LED, HIGH); 
  else  digitalWrite(LED, LOW);

  //********************************** OLED STUFF **********************************
  footswitch_value=digitalRead(FOOTSWITCH);
 
  //create sin tabe for tremolo
  if ((effect!=old_effect)&&(effect==7))   // CHANGED THIS
  {
       u8g.firstPage();  
       do {
         drawSinWait();
       } while( u8g.nextPage() );
       //createSineTable();  // REMOVE THIS ????????????????????
  }
 
  //lcd refresh on effect change or 
  if ( (effect!=old_effect)   ||  (footswitch_value!=old_footswitch_value)  ||  ((DrawMillis<millis())&&(footswitch_value==0))  )
  {
    u8g.firstPage();  
    do {
      draw();
    } while( u8g.nextPage() );
    DrawMillis=millis()+100;
    old_effect=effect;
    old_footswitch_value=footswitch_value;
  }
  //********************************** OLED STUFF END **********************************

}

//********************************************** DRAW FUNCTIONS *********************************************

void draw(void)
{
  // graphic commands to redraw the complete screen should be placed here  
  u8g.drawCircle(32,34,5);
  u8g.drawCircle(64,48,5);
  u8g.drawCircle(96,34,5);
  if (footswitch_value==0)
  {
    u8g.setFont(u8g_font_6x10);
    u8g.setPrintPos(26,50);
    u8g.print(POT0*100/4096); u8g.print('%');
    u8g.setPrintPos(58,64);
    u8g.print(POT1*100/4096); u8g.print('%');
    u8g.setPrintPos(90,50);
    u8g.print(POT2*100/4096); u8g.print('%');
  }
  u8g.setFont(u8g_font_8x13B);
  switch(effect)
  {
    case 0:  
      u8g.drawStr( 0, 10, " Volume-Booster"); //max 16 char
      u8g.setFont(u8g_font_6x10);
      u8g.drawStr( 74, 24, " Volume"); //max 8 char
      u8g.drawBitmapP( 4, 42, 1, 12, mixOff);
      u8g.drawStr( 0, 64, "Mix");
      break;
    case 1:  
      u8g.drawStr( 0, 10, "Asym. Distortion");
      u8g.setFont(u8g_font_6x10);
      u8g.drawStr( 10, 24, "Upp. Th."); //max 8 char
      u8g.drawStr( 42, 38, "Low. Th."); //max 8 char
      u8g.drawStr( 74, 24, " Volume");
      u8g.drawBitmapP( 4, 42, 1, 12, mixOff);
      u8g.drawStr( 0, 64, "Mix");
      break;
    case 2:  
      u8g.drawStr( 0, 10, "      Delay");
      u8g.setFont(u8g_font_6x10);
      u8g.drawStr( 10, 24, " Delay");
      u8g.drawStr( 74, 24, " Volume");
      u8g.drawBitmapP( 4, 42, 1, 12, mixOn);
      u8g.drawStr( 0, 64, "Mix");
      break;
    case 3:  
      u8g.drawStr( 0, 10, "     Reverb");
      u8g.setFont(u8g_font_6x10);
      u8g.drawStr( 10, 24, "1st Ref");
      u8g.drawStr( 42, 38, "2nd Ref");
      u8g.drawStr( 74, 24, " Volume");
      u8g.drawBitmapP( 4, 42, 1, 12, mixOn);
      u8g.drawStr( 0, 64, "Mix");
      break;
    case 4:  
      u8g.drawStr( 0, 10, "  Super-Reverb");
      u8g.setFont(u8g_font_6x10);
      u8g.drawStr( 10, 24, "1st Ref");
      u8g.drawStr( 42, 38, "2nd Ref");
      u8g.drawStr( 74, 24, " Volume");
      u8g.drawBitmapP( 4, 42, 1, 12, mixOn);
      u8g.drawStr( 0, 64, "Mix");
      break;
    case 5:  
      u8g.drawStr( 0, 10, "      Echo");
      u8g.setFont(u8g_font_6x10);
      u8g.drawStr( 10, 24, " Delay");
      u8g.drawStr( 74, 24, " Volume");
      u8g.drawBitmapP( 4, 42, 1, 12, mixOff);
      u8g.drawStr( 0, 64, "Mix");
      break;
    case 6:
      u8g.drawStr( 0, 10, " Chorus/Vibrato");
      u8g.setFont(u8g_font_6x10);
      u8g.drawStr( 10, 24, " Delay");
      u8g.drawStr( 74, 24, " Volume");
     break;
    case 7:
      u8g.drawStr( 0, 10, "     Tremolo");
      u8g.setFont(u8g_font_6x10);
      u8g.drawStr( 10, 24, "  Freq.");
      u8g.drawStr( 42, 38, "  Depth");
      u8g.drawStr( 74, 24, " Volume");
      u8g.drawBitmapP( 4, 42, 1, 12, mixOff);
      u8g.drawStr( 0, 64, "Mix");
      break;
    case 8:  
      u8g.drawStr( 0, 10, "     Octaver");
      u8g.setFont(u8g_font_6x10);
      u8g.drawStr( 12, 24, " - | +");
      u8g.drawStr( 74, 24, " Volume");
      break;
    case 9:  
      u8g.drawStr( 0, 10, "  Bitcrusher");
      u8g.setFont(u8g_font_6x10);
      u8g.drawStr( 10, 24, "  Freq.");
      u8g.drawStr( 74, 24, " Volume");
      u8g.drawBitmapP( 4, 42, 1, 12, mixOff);
      u8g.drawStr( 0, 64, "Mix");
      break;
    default:  
      u8g.drawStr( 0, 10, " Unknown-effect");
      break;
  }
}
 
void drawSinWait()
{
  u8g.setFont(u8g_font_8x13B);
  if (effect==7) u8g.drawStr( 0, 10, "    Tremolo");  //CHANGED THIS
  u8g.drawStr( 0, 32, "     Please");
  u8g.drawStr( 0, 48, "      wait");
}
 
void drawSplash()
{
  u8g.setFont(u8g_font_6x10);
  u8g.drawStr( 0, 46, " Arduino Guitar Pedal");
  u8g.drawStr( 0, 55, "    Multi Effects");
  u8g.drawStr( 0, 64, " www.electrosmash.com");
  u8g.drawBitmapP( 0, 0, 16, 36, splashLogo);
}

//********************************************** DRAW FUNCTIONS END ******************************************
 

void TC4_Handler()
{
  // We need to get the status to clear it and allow the interrupt to fire again
  TC_GetStatus(TC1, 1);

  if (effect==0) // Volume-Booster  (CLEAN)   
  {
    
    //Adjust the volume with POT2
    out_DAC0=map(in_ADC0,0,4095,1,POT2);
    out_DAC1=map(in_ADC1,0,4095,1,POT2);
   
    //Write the DACs
    dacc_set_channel_selection(DACC_INTERFACE, 0);          //select DAC channel 0
    dacc_write_conversion_data(DACC_INTERFACE, out_DAC0);//write on DAC
    dacc_set_channel_selection(DACC_INTERFACE, 1);          //select DAC channel 1
    dacc_write_conversion_data(DACC_INTERFACE, out_DAC1);//write on DAC
  }
    

      
  else if (effect==1) //DISTORTION
  {
        /*
        Potentiometer 0: Sets the distortion level.
        Potentiometer 1: Not used.
        Potentiometer 2: Adjust the effect volume.
        */

    upper_threshold=map(POT0,0,4095,4095,2047);
    lower_threshold=map(POT0,0,4095,0000,2047);
    
    if(in_ADC0>=upper_threshold) in_ADC0=upper_threshold;
    else if(in_ADC0<lower_threshold)  in_ADC0=lower_threshold;
   
    if(in_ADC1>=upper_threshold) in_ADC1=upper_threshold;
    else if(in_ADC1<lower_threshold)  in_ADC1=lower_threshold;
   
    //adjust the volume with POT2
    out_DAC0=map(in_ADC0,0,4095,1,POT2);
    out_DAC1=map(in_ADC1,0,4095,1,POT2);
   
    //Write the DACs
    dacc_set_channel_selection(DACC_INTERFACE, 0);          //select DAC channel 0
    dacc_write_conversion_data(DACC_INTERFACE, out_DAC0);   //write on DAC
    dacc_set_channel_selection(DACC_INTERFACE, 1);          //select DAC channel 1
    dacc_write_conversion_data(DACC_INTERFACE, out_DAC1);   //write on DAC
  }
  
  else if(effect==2) // DELAY LONG (but max delay is 40000 instead of 46000)          
  {

      /*
      Potentiometer 0: Adjusts the delay time from 0 to 450 ms
      Potentiometer 2: Sets the presence of the delayed signal
      Mix Switch: Should be ON (down) to mix the original an delayed signal.
      */

    //Store current readings  
    sDelayBuffer0[DelayCounter] = in_ADC0;
   
    //Adjust Delay Depth based in pot0 position.
    //POT0>>2 is the same as writing POT0/2.
    //To the values that this potentiometer produces (from 0 to 2097) are re-maped in values between 1 and 20000
    Delay_Depth=map(POT0>>2,0,1023,1,MAX_DELAY);
   
    //Increase/reset delay counter.
    //We use an index to save the data in the buffer, so we increment it,
    //the actual reading will be stored one sample before the index position.  
    DelayCounter++;
    //If we reach the end of the buffer, we reset the counter
    if(DelayCounter >= Delay_Depth) DelayCounter = 0;
    
   
    //We write on the DACs the current index buffer sample, which is indeed the one that was stored here 20000 samples before:
    out_DAC0 = ((sDelayBuffer0[DelayCounter]));
   
    //Add volume feature based in pot2 position.
    out_DAC0=map(out_DAC0,0,4095,1,POT2);
   
    //Write the DACs
    dacc_set_channel_selection(DACC_INTERFACE, 0);       //select DAC channel 0
    dacc_write_conversion_data(DACC_INTERFACE, out_DAC0);//write on DAC
    dacc_set_channel_selection(DACC_INTERFACE, 1);       //select DAC channel 1
    dacc_write_conversion_data(DACC_INTERFACE, 0);       //write on DAC
  }
   
  else if (effect==3)  //REVERB ECHOED
  {     /*
      Pot 0: Adjusts the first reflection of the sound.
      Pot 1: Adjusts the second reflection of the sound.
      Pot 2: Controls the output volume.
      Mix Switch: Shoud be ON (down) to add the direct sound trajectory.
      Toggle Switch: Switchs betseen reverb(delayed signals) and super-reverb(echoed signals). 
      */

      //Store current readings in ECHO mode
      sDelayBuffer0[DelayCounter]=(in_ADC0 + (sDelayBuffer0[DelayCounter]))>>1;
         
      //Adjust Delay Depth based in POT0 and POT1 position.
      Delay_Depth =map(POT0>>3,0,512,1,20000);
      
     
      //Increse/reset delay counter.   
      DelayCounter++;

      if(DelayCounter >= Delay_Depth) DelayCounter = 0;
      if(DelayCounter >= 20000) DelayCounter = 0;
      
     
      //Calculate the output as the sum of DelayBuffer_A + DelayBuffer_B 
      out_DAC0 = (sDelayBuffer0[DelayCounter]);
      
     
      //Add volume feature based in pot2 position.
      out_DAC0=map(out_DAC0,0,4095,1,POT2);
     
     
      //Write the DACs
      dacc_set_channel_selection(DACC_INTERFACE, 0);       //select DAC channel 0
      dacc_write_conversion_data(DACC_INTERFACE, out_DAC0);//write on DAC


  }

  else if (effect ==4) //REVERB DELAYED
  {
      //Store current readings in DELAY mode  
       sDelayBuffer0[DelayCounter]  = in_ADC0 ;
        
    
   
      //Adjust Delay Depth based in POT0 and POT1 position.
      Delay_Depth =map(POT0>>3,0,512,1,20000);

     
      //Increse/reset delay counter.   
      DelayCounter++;
     
      if(DelayCounter >= Delay_Depth) DelayCounter = 0; 
     if(DelayCounter >= 20000) DelayCounter = 0;
     
      //Calculate the output as the sum of DelayBuffer_A + DelayBuffer_B 
      out_DAC0 = (sDelayBuffer0[DelayCounter]);
      
     
      //Add volume feature based in pot2 position.
      out_DAC0=map(out_DAC0,0,4095,1,POT2);
     
     
      //Write the DACs
      dacc_set_channel_selection(DACC_INTERFACE, 0);       //select DAC channel 0
      dacc_write_conversion_data(DACC_INTERFACE, out_DAC0);//write on DAC
  }

  else if (effect==5) // Echo.
  {
     //Store current readings  
     sDelayBuffer0[DelayCounter]  = (in_ADC0 + (sDelayBuffer0[DelayCounter]))>>1;
   
     //Adjust Delay Depth based in pot0 position.
     Delay_Depth =map(POT0>>2,0,1023,1,MAX_DELAY);
   
     //Increse/reset delay counter.   
     DelayCounter++;
     if(DelayCounter >= Delay_Depth) DelayCounter = 0; 
     out_DAC0 = ((sDelayBuffer0[DelayCounter]));
   
     //Add volume feature based in POT2 position.
     out_DAC0=map(out_DAC0,0,4095,1,POT2);
   
     //Write the DACs
     dacc_set_channel_selection(DACC_INTERFACE, 0);          //select DAC channel 0
     dacc_write_conversion_data(DACC_INTERFACE, out_DAC0);//write on DAC
     dacc_set_channel_selection(DACC_INTERFACE, 1);          //select DAC channel 1
     dacc_write_conversion_data(DACC_INTERFACE, 0);          //write on DAC
  }

  else if (effect==6) // Chorus
  {
      
      //Store current readings  
      sDelayBuffer0[DelayCounter] = in_ADC0;
   
      //Adjust Delay Depth based in pot0 position.
      POT0=map(POT0>>2,0,1024,1,25); //25 empirically chosen
   
      DelayCounter++;
      if(DelayCounter >= Delay_Depth_B) 
       {
        DelayCounter = 0; 
        if(count_up)
         {
          
          for(p=0;p<POT0+1;p++) sDelayBuffer0[Delay_Depth_B+p]=sDelayBuffer0[Delay_Depth_B-1]; 
          Delay_Depth_B=Delay_Depth_B+POT0;
          if (Delay_Depth_B>=500) count_up=0;
         }
        else
         {
          
          Delay_Depth_B=Delay_Depth_B-POT0;
          if (Delay_Depth_B<=200)count_up=1;
         }
       }
   
     out_DAC0 = sDelayBuffer0[DelayCounter];
   
     //Add volume control based in POT2
     out_DAC0=map(out_DAC0,0,4095,1,POT2);
   
     //Write the DACs
     dacc_set_channel_selection(DACC_INTERFACE, 0);       //select DAC channel 0
     dacc_write_conversion_data(DACC_INTERFACE, out_DAC0);//write on DAC
     dacc_set_channel_selection(DACC_INTERFACE, 1);       //select DAC channel 1
     dacc_write_conversion_data(DACC_INTERFACE, 0);       //write on DAC
  }

  else  if (effect ==7) // TREMOLO
  {

      if (isSineRun == false)
      {
      createSineTable();
      isSineRun = true;
      } 
       
       //Increase the sinewave index and/or reset the value.
       POT0 = POT0>>1; //divide value by 2 (its too big) 
       count++; 
       if (count>=160) //160 chosen empirically
       {
         count=0;
         sample=sample+POT0;
         if(sample>=MAX_DELAY) sample=0;
       }
       
        //Create the Low Frequency Oscillator signal with depth control based in POT1.
        LFO=map(sDelayBuffer0[sample],0,4095,(4095-POT1),4095);
       
        //Modulate the output signals based on the sinetable.
        out_DAC0 =map(in_ADC0,1,4095,1, LFO);
        out_DAC1 =map(in_ADC1,1,4095,1, LFO);
       
        //Add volume feature with POT2
        out_DAC0 =map(out_DAC0,1,4095,1, POT2);
        out_DAC1 =map(out_DAC1,1,4095,1, POT2);
       
         //Write the DACs
        dacc_set_channel_selection(DACC_INTERFACE, 0);       //select DAC channel 0
        dacc_write_conversion_data(DACC_INTERFACE, out_DAC0);//write on DAC
        dacc_set_channel_selection(DACC_INTERFACE, 1);       //select DAC channel 1
        dacc_write_conversion_data(DACC_INTERFACE, out_DAC1);//write on DAC
  }

  else if (effect==8) //OCTAVER
  {
    
      //Store current readings  
      sDelayBuffer0[write_pt] = in_ADC0;
      sDelayBuffer1[write_pt] = in_ADC1;
     
      //Adjust Delay Depth based in pot2 position.
      Delay_Depth = 3000-1;
     
      //Increse/reset delay counter.
      write_pt++;
      if(write_pt >= Delay_Depth) write_pt = 0; 

     
      out_DAC0 = ((sDelayBuffer0[read_pt_A]));
      out_DAC1 = ((sDelayBuffer1[read_pt_B]));
     
      if (POT0>2700)
      { 
        read_pt_A = read_pt_A + 2;
        read_pt_B = read_pt_B + 2;
      }
      else if (POT0>1350)
      {
        read_pt_A = read_pt_A + 1;
        read_pt_B = read_pt_B + 1;
      }
      else
      {
       divider++;
       if (divider>=2)
        {
          read_pt_A = read_pt_A + 1;
          read_pt_B = read_pt_B + 1;
          divider=0;
        }
      }
     
      if(read_pt_A >= Delay_Depth) read_pt_A = 0; 
      if(read_pt_B >= Delay_Depth) read_pt_B = 0; 
     
      //Add volume control with POT2
      out_DAC0=map(out_DAC0,0,4095,1,POT2);
      out_DAC1=map(out_DAC1,0,4095,1,POT2);
     
      //Write the DACs
      dacc_set_channel_selection(DACC_INTERFACE, 0);       //select DAC channel 0
      dacc_write_conversion_data(DACC_INTERFACE, out_DAC0);//write on DAC
      dacc_set_channel_selection(DACC_INTERFACE, 1);       //select DAC channel 1
      dacc_write_conversion_data(DACC_INTERFACE, out_DAC1);//write on DAC
  }

  else if (effect==9)  //BITCRUSHER1     
  {
      lfo_speed = ADC->ADC_CDR[12];
      counter++;
      trianglecounter++;
      if (lfo_depth > 5)
      {
        if (trianglecounter >= map(lfo_speed, 0, 4095, 800, 1))
        {
            if (countup)
            {
              triangle++;
            }
            else
            {
              triangle--;
            }
          if (triangle >= 300)
          {
              countup = !countup;
          }
          else if (triangle <= 0)
          {
              countup = !countup;
          }
         trianglecounter = 0;
        }
        interval = map(POT1, 0, 4095, 0, 300) + triangle/map(lfo_depth, 0, 4095, 100, 1);
      }
      else
      {
        interval = map(POT1, 0, 4095, 0, 300);
      }
      if (counter >= interval)
      {
      out_DAC0 = in_ADC0<<map(POT0>>2,0,1024,0,12);
      //Write the DACs
      dacc_set_channel_selection(DACC_INTERFACE, 0); //select DAC channel 0
      dacc_write_conversion_data(DACC_INTERFACE, out_DAC0);//write on DAC
      dacc_set_channel_selection(DACC_INTERFACE, 1); //select DAC channel 1
      dacc_write_conversion_data(DACC_INTERFACE, 0);//write on DAC
      counter=0;
      }
  }
    

}


      void switch_handler()
      {
        delayMicroseconds(100000); //debouncing protection
        if (toggle_value!=digitalRead(TOGGLE)) effect++;
        delayMicroseconds(100000); //debouncing protection
        toggle_value=digitalRead(TOGGLE);
        if (effect==10) effect=0;
        isSineRun = false;
        DelayCounter = 0; //NOT TOO SURE ABOUT THIS
        Delay_Depth_B = 300; //reset the variable - for chorus only
        count_up=1; //reset the variable
        //NORMAL Delay_Depth SHOULD BE RESETED?????????????????????
      }
