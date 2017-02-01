case ILI9327_8_UNO:
	LCD_Write_COM(0xE9);
	LCD_Write_DATA(0x00,0x20);
	LCD_Write_COM(0x11); //Exit Sleep
	delay(100);
	LCD_Write_COM(0xD1);
	LCD_Write_DATA(0x00,0x00);
	LCD_Write_DATA(0x00,0x71);
	LCD_Write_DATA(0x00,0x19);
	LCD_Write_COM(0xD0);
	LCD_Write_DATA(0x00,0x07);
	LCD_Write_DATA(0x00,0x01);
	LCD_Write_DATA(0x00,0x08);
	LCD_Write_COM(0x36);
	LCD_Write_DATA(0x00,0x48);
	LCD_Write_COM(0x3A);
	LCD_Write_DATA(0x00,0x05);
	LCD_Write_COM(0xC1);
	LCD_Write_DATA(0x00,0x10);
	LCD_Write_DATA(0x00,0x10);
	LCD_Write_DATA(0x00,0x02);
	LCD_Write_DATA(0x00,0x02);
	LCD_Write_COM(0xC0); //Set Default Gamma
	LCD_Write_DATA(0x00,0x00);
	LCD_Write_DATA(0x00,0x35);
	LCD_Write_DATA(0x00,0x00);
	LCD_Write_DATA(0x00,0x00);
	LCD_Write_DATA(0x00,0x01);
	LCD_Write_DATA(0x00,0x02);
	LCD_Write_COM(0xC5); //Set frame rate
	LCD_Write_DATA(0x00,0x04);
	LCD_Write_COM(0xD2); //power setting
	LCD_Write_DATA(0x00,0x01);
	LCD_Write_DATA(0x00,0x44);
	LCD_Write_COM(0xC8); //Set Gamma
	LCD_Write_DATA(0x00,0x04);
	LCD_Write_DATA(0x00,0x67);
	LCD_Write_DATA(0x00,0x35);
	LCD_Write_DATA(0x00,0x04);
	LCD_Write_DATA(0x00,0x08);
	LCD_Write_DATA(0x00,0x06);
	LCD_Write_DATA(0x00,0x24);
	LCD_Write_DATA(0x00,0x01);
	LCD_Write_DATA(0x00,0x37);
	LCD_Write_DATA(0x00,0x40);
	LCD_Write_DATA(0x00,0x03);
	LCD_Write_DATA(0x00,0x10);
	LCD_Write_DATA(0x00,0x08);
	LCD_Write_DATA(0x00,0x80);
	LCD_Write_DATA(0x00,0x00);
	LCD_Write_COM(0x2A); 
	LCD_Write_DATA(0x00,0x00);
	LCD_Write_DATA(0x00,0x00);
	LCD_Write_DATA(0x00,0x00);
	LCD_Write_DATA(0x00,0xeF);
	LCD_Write_COM(0x2B); 
	LCD_Write_DATA(0x00,0x00);
	LCD_Write_DATA(0x00,0x00);
	LCD_Write_DATA(0x00,0x01);
	LCD_Write_DATA(0x00,0x8F);
	LCD_Write_COM(0x29); //display on      
	LCD_Write_COM(0x2C); //display on
	break;
