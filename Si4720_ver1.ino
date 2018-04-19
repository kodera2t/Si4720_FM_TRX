//////////////////////////////////////////////
// Silicon Labs. Si4720 FM transceiver
// kodera2t April 19, 2018
// No warranty, provided as is.
//////////////////////////////////////////////
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 9
Adafruit_SSD1306 display(OLED_RESET);
#define RADIO 0x11
int read_byte,raw_upper,upper,lower,mode,read_byte2;
    int rssi,stereo,infreq;
float freq, old_freq;
unsigned int channel_num,s_upper,s_lower;
unsigned char s_upper2, s_lower2, s_upper3;
unsigned int initial_num;
volatile int encorder_val;
volatile int mode_set=0; /// mode_set=0:AM, mode_set=1:FM
volatile int band_mode = LOW;
float listen_freq;
int terminal_1  = 2;
int terminal_2  = 4;
volatile char old_state = 0;
int ct,pt,event,event2;
int rssi_count;
int rssi_count2=0;
boolean extx=0;

void i2c_write(int device_address, int memory_address, int value, int value2)
{
  Wire.beginTransmission(device_address);
  Wire.write(memory_address);
    delay(5);
  Wire.write(value);
    delay(5);
  Wire.write(value2);
    delay(5);
  Wire.endTransmission();
}

void i2c_write0(int device_address, int memory_address)
{
  Wire.beginTransmission(device_address);
  Wire.write(memory_address);
  Wire.endTransmission();
}

void i2c_write1(int device_address, int memory_address, int value)
{
  Wire.beginTransmission(device_address);
  Wire.write(memory_address);
    delay(5);
  Wire.write(value);
  Wire.endTransmission();
}

void i2c_write3(int device_address, int memory_address, int value, int value2, int value3)
{
  Wire.beginTransmission(device_address);
  Wire.write(memory_address);
    delay(5);
  Wire.write(value);
    delay(5);
  Wire.write(value2);
    delay(5);
  Wire.write(value3);
    delay(5);
  Wire.endTransmission();
}

void i2c_write4(int device_address, int memory_address, int value, int value2,int value3,int value4)
{
  Wire.beginTransmission(device_address);
  Wire.write(memory_address);
    delay(5);
  Wire.write(value);
    delay(5);
  Wire.write(value2);
    delay(5);
  Wire.write(value3);
    delay(5);
  Wire.write(value4);
    delay(5);
  Wire.endTransmission();
}

void i2c_write5(int device_address, int memory_address, int value, int value2,int value3,int value4,int value5)
{
  Wire.beginTransmission(device_address);
  Wire.write(memory_address);
    delay(5);
  Wire.write(value);
    delay(5);
  Wire.write(value2);
    delay(5);
  Wire.write(value3);
    delay(5);
  Wire.write(value4);
    delay(5);
  Wire.write(value5);
    delay(5);
  Wire.endTransmission();
}

void i2c_read(int device_address, int memory_address)
{
Wire.beginTransmission(device_address);
Wire.write(memory_address);
Wire.endTransmission(false);
Wire.requestFrom(device_address, 2);
read_byte = Wire.read();
//Wire.requestFrom(device_address, 1);
read_byte2 = Wire.read();
Wire.endTransmission(true);
//delay(30);
}



void setup()
{
    unsigned int upper,lower,raw_upper;
    unsigned int mask,mode,mode_set;
    
    Wire.begin() ;
  attachInterrupt(0,Rotary_encorder,CHANGE);
  attachInterrupt(1,mode_setting,CHANGE);
   delay(100) ;  
////////////    lcd.begin(8,2);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); 
  pinMode(3, INPUT);
  pinMode(terminal_1, INPUT);
  pinMode(terminal_2, INPUT);
//  digitalWrite(terminal_1, HIGH);
//  digitalWrite(terminal_2, HIGH);
//  digitalWrite(3,HIGH);
    //i2c_write0(RADIO, 0x11);
  int temp;
  listen_freq=100.0;
  initial_num=listen_freq*100;
  channel_num=initial_num+encorder_val*2;
  s_upper2=(channel_num>>8);
  s_lower2=channel_num&0b11111111;
    temp=0;
//////////////////receiver initilize////////////////////////    
    i2c_write(RADIO,0x01,0b10010000,0b00000101);//receive boot up command, receiver, crystal enabled, analogoutput
    i2c_write4(RADIO,0x20,0x00,s_upper2,s_lower2,0x05);//receive set frequency  
encorder_val=0;
display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.clearDisplay();
  display.setCursor(32,32);
  display.print("FM");
  freq=channel_num/100.0;
  display.setCursor(32,40);  
 display.print(freq);
 display.setCursor(70,40);
 display.print("MHz");
   display.display();
}

void loop()
{
if(event==1){
  if(mode_set==0){         
  if(extx==1){
  i2c_write0(RADIO, 0x11);//powerdown
  extx=0;
  event=0;    
  }
  i2c_write(RADIO,0x01,0b10010000,0b00000101);//boot up command, receiver, crystal enabled, analogoutput
  listen_freq=100.0;
  initial_num=listen_freq*100;
  channel_num=initial_num+encorder_val*10;
  s_upper2=(channel_num>>8);
  s_lower2=channel_num&0b11111111;
//  delay(50);
  i2c_write5(RADIO,0x12,0x00,0x11,0x00,0x00,0x01); //De-emphasis 50us
  i2c_write5(RADIO,0x12,0x00,0x40,0x00,0x00,0x32); //audio volume set 
  i2c_write4(RADIO,0x20,0x00,s_upper2,s_lower2,0x05);//receive set frequency
  display.clearDisplay();
  display.setCursor(32,32);
  display.print("FM");
  freq=channel_num/100.0;
  display.setCursor(32,40);
 display.print(freq);
 display.setCursor(70,40);
 display.print("MHz");
   display.display();
  event=0;  
  }
  else{
  display.clearDisplay();
  display.display();
  //display.setCursor(32,32);
  //display.print("FM ONAIR");
  //display.display();
    i2c_write0(RADIO, 0x11);//powerdown
  i2c_write(RADIO,0x01,0xd2,0x50);
  i2c_write5(RADIO,0x12,0x00,0x21,0x01,0x1d,0x4c); //TX FM deviation to 75kHz
  i2c_write5(RADIO,0x12,0x00,0x21,0x04,0b00000010,0b01111111); //TX line level                                                                                                                                                                                                
  i2c_write5(RADIO,0x12,0x00,0x21,0x06,0x00,0x01); //TX pre-emphasis to 50us
  i2c_write5(RADIO,0x12,0x00,0x22,0x00,0x00,0x03); //TX limiter enable
  
  i2c_write5(RADIO,0x12,0x00,0x21,0x04,0x21,0x5E);
  i2c_write5(RADIO,0x12,0x00,0x00,0x01,0x00,0xC1);
  i2c_write5(RADIO,0x12,0x00,0x21,0x07,0x4a,0x38); 
  i2c_write4(RADIO,0x31,0x00,0x00,0x78,0x00);
  i2c_write3(RADIO,0x30,0x00,s_upper2,s_lower2);  
    extx=1;
    event=0;
  } 
}
}

void mode_setting(){
int sw,k;
ct=millis();
delay(1);
sw=digitalRead(3);
if(sw==LOW && (ct-pt)>50){
band_mode=HIGH;
mode_set=mode_set+1;
}
pt=ct;
if(mode_set>1){
  mode_set=0;
}
event=1;
}
    
void Rotary_encorder(void)
{
  if(!digitalRead(terminal_1)){
    if(digitalRead(terminal_2)){
      old_state = 'R';
    } else {
      old_state = 'L';
    }
  } else {
    if(digitalRead(terminal_2)){
      if(old_state == 'L'){ 
        encorder_val++;
      }
    } else {
      if(old_state == 'R'){
        encorder_val--;
      }
    }
    old_state = 0;
    event=1;
  }

}




