// XGGerrmann All Rights Reserved

#include <stdio.h>
#include <Base64.h>

void setup()
{
  Serial.begin(9600);
  //username = "turf";
  //password = "Turf21";
  // encoding
  char input[] = "turf:Turf21";
  int inputLen = sizeof(input);
  
  int encodedLen = base64_enc_len(inputLen);
  char encoded[encodedLen];
  
  Serial.print(input); Serial.print(" = ");
  
  // note input is consumed in this step: it will be empty afterwards
  base64_encode(encoded, input, inputLen); 
  
  Serial.println(encoded);

  String totalString = "Basic"+String(encoded);
  Serial.println(totalString);
  //// decoding
  //char input2[] = "Zm9vYmFy";
  //int input2Len = sizeof(input2);
  
//  int decodedLen = base64_dec_len(input2, input2Len);
 // char decoded[decodedLen];
  
//  base64_decode(decoded, input2, input2Len);
  
//  Serial.print(input2); Serial.print(" = "); Serial.println(decoded);
}

void loop()
{
 
}

