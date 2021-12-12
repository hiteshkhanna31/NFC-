#include "Wire.h"
#include "nfc.h"
#include"AESLib.h"

/** define a nfc class */
NFC_Module nfc;
uint8_t key[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
void setup(void)
{
  Serial.begin(9600);
  nfc.begin();
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

void loop(void)
{
  u8 buf[32],sta;
  int count=0;
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
    delay(2000);
    sta = nfc.InListPassiveTarget(buf);
  /** check state and UID length */
    if(sta && buf[0] == 4)
    {
      if(count==1)
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
        Serial.print("Decrypted name: ");
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
        Serial.print("Decrypted matriculation number: ");
        Serial.println(matriculation);
      }
	  }
	}
}
