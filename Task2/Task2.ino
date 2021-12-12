#include "Wire.h"
#include "nfc.h"
#include"AESLib.h"

#include <ThreeWire.h>  
#include <RtcDS1302.h>

// CONNECTIONS:
// DS1302 CLK/SCLK --> 5
// DS1302 DAT/IO --> 4
// DS1302 RST/CE --> 2
// DS1302 VCC --> 3.3v - 5v
// DS1302 GND --> GND
/** define a nfc class */
NFC_Module nfc;
uint8_t key[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};

ThreeWire myWire(4,5,2); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);

void setup(void)
{
  Serial.begin(19200);
  nfc.begin();
  Rtc.Begin();
  Serial.println("MF1S50 Reader Demo From Elechouse!");
  
  uint32_t versiondata = nfc.get_version();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
  
  /** Set normal mode, and disable SAM */
  nfc.SAMConfiguration();
  
}
int count=0;
void loop(void)
{
  u8 buf[32],sta;
 
  /** Polling the mifar card, buf[0] is the length of the UID */
  sta = nfc.InListPassiveTarget(buf);
  /** check state and UID length */
  if(sta && buf[0] == 4)
  {
    if(count==0)
    {
      /** the card may be Mifare Classic card, try to read the block */  
      Serial.print("UUID length:");
      Serial.print(buf[0], DEC);
      Serial.println();
      Serial.print("UUID:");
      nfc.puthex(buf+1, buf[0]);
      Serial.println();
      /** factory default KeyA: 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF */
      u8 key[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
      u8 blocknum = 8;
      
      char name[16]="Hitesh Khanna"; 
      char matriculation[16]="32921"; 
      /** Authentication blok 8 */
      sta = nfc.MifareAuthentication(0, blocknum, buf+1, buf[0], key);
      if(sta)
      {
        Serial.println("Authentication success.");
        aes128_enc_multiple(key, name,16);
        Serial.println("Encrypted:");
        sta = nfc.MifareWriteBlock(8, name);
        if(sta)
        {
          Serial.println("Write block 8 successfully:");
          nfc.puthex(name, 16);
          Serial.println("");
        }     
      }
      sta = nfc.MifareAuthentication(0, blocknum+5, buf+1, buf[0], key);
      if(sta)
      {
        Serial.println("Authentication success."); 
        aes128_enc_multiple(key, matriculation,16);
        Serial.println("Encrypted:");
        sta = nfc.MifareWriteBlock(13, matriculation);
        if(sta)
        {
          Serial.println("Write block 13 successfully:");
          nfc.puthex(matriculation, 16);
          Serial.println("");
        }
      }
      count=1;
    }
  }
    delay(3000);
    sta = nfc.InListPassiveTarget(buf);
  /** check state and UID length */
    if(sta && buf[0] == 4)
    {
      if(count==1)
      {
        Serial.print("UUID length:");
        Serial.print(buf[0], DEC);
        Serial.println();
        Serial.print("UUID:");
        nfc.puthex(buf+1, buf[0]);
        Serial.println();
        /** factory default KeyA: 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF */
        u8 key[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
        u8 blocknum = 8;
        char name[16]="";
        char matriculation[16]="";
        sta = nfc.MifareAuthentication(0, blocknum, buf+1, buf[0], key);
        if(sta)
        {
          /** save read block data */
          Serial.println("Authentication success.");  
          sta = nfc.MifareReadBlock(blocknum, name);
          if(sta)
          {
            Serial.println("Read block 8 successfully:");
            nfc.puthex(name, 16);
            Serial.println("");
          } 
          aes128_dec_single(key, name);
          Serial.print("decrypted: ");
          Serial.println(name);
        }
        
        sta = nfc.MifareAuthentication(0, blocknum+5, buf+1, buf[0], key);
        if(sta)
        {
          sta = nfc.MifareReadBlock(blocknum+5, matriculation);
          if(sta)
          {
            Serial.println("Read block 13 successfully:");
            nfc.puthex(matriculation, 16);
            Serial.println("");
          } 
          aes128_dec_single(key, matriculation);
          Serial.print("decrypted: ");
          Serial.println(matriculation);
        }
        count=count+1;
		}
	}
    delay(3000);
    sta = nfc.InListPassiveTarget(buf);
  /** check state and UID length */
    if(sta && buf[0] == 4)
    {
      if(count>=2)
      {
        Serial.print("UUID length:");
        Serial.print(buf[0], DEC);
        Serial.println();
        Serial.print("UUID:");
        nfc.puthex(buf+1, buf[0]);
        Serial.println();
        /** factory default KeyA: 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF */
        u8 key[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
        u8 blocknum = 8;
    		char name[16]="";
    		char matriculation[16]="";
    		sta = nfc.MifareAuthentication(0, blocknum, buf+1, buf[0], key);
    		if(sta)
    		{
          /** save read block data */
          Serial.println("Authentication success.");  
          sta = nfc.MifareReadBlock(blocknum, name);
          if(sta)
          {
            Serial.println("Read block 8 successfully:");
            nfc.puthex(name, 16);
            Serial.println("");
          }
          aes128_dec_multiple(key, name,16);
          Serial.print("decrypted: ");
          Serial.println(name);
		    }
    		sta = nfc.MifareAuthentication(0, blocknum+5, buf+1, buf[0], key);
    		if(sta)
    		{
    		    sta = nfc.MifareReadBlock(blocknum+5, matriculation);
            if(sta)
            {
              Serial.println("Read block 13 successfully:");
              nfc.puthex(matriculation, 16);
              Serial.println("");
            } 
            aes128_dec_multiple(key, matriculation,16);
            Serial.print("decrypted: ");
            Serial.println(matriculation);
        		RtcDateTime now = Rtc.GetDateTime();
        		printDateTime(now);
        		if (!now.IsValid())
        		{
                  // Common Causes:
                  //    1) the battery on the device is low or even missing and the power line was disconnected
                  Serial.println("RTC lost confidence in the DateTime!");
        		}
		        delay(10000);
        }
		    count=count+1;
     }
    }
}
#define countof(a) (sizeof(a) / sizeof(a[0]))

void printDateTime(const RtcDateTime& dt)
{
    char datestring[20];

    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
            dt.Month(),
            dt.Day(),
            dt.Year(),
            dt.Hour(),
            dt.Minute(),
            dt.Second() );
    Serial.print(datestring);
}
