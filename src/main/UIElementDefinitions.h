void UIElements( void )
{

	sManager= new screenManager();
	screens	= new Screen*[SCREENS_TOTAL];
	blocks	= new Block*[MAX_BLOCKS];
	
	// Create screens and attach to manager
	for(int i=0; i < SCREENS_TOTAL; i++)
	{
		screens[i] = new Screen( &lcd, NROWS, NCOLS );
		sManager->attach_screen(screens[i]);
	}

	// Add person blocks to the screens
	int i, s, r, c;
	for(i=0, s=0, r=0, c=0; i<persons.size(); i++)
	{
		s = (int) floor((double)N_BLOCKS/8);
		r = (int) floor((double)(N_BLOCKS %(s*8))/(NCOLS/2)) ;
		c = N_BLOCKS%(NCOLS/2);
		blocks[N_BLOCKS] = new personBlock(screens[s], c*2+1, r+1,	2,	1, persons.get(i));
		blocks[N_BLOCKS]->setAction(&increment);
		screens[s]->attach_block(blocks[N_BLOCKS]);
		N_BLOCKS ++;
	}
	// Navigation
	for(int s=0; s<SCREENS_P; s++)
	{
		for(int r=0; r<NROWS; r++)
		{
			void(*func_action)(Block*);
			void(*func_draw)(Block*);
			void(*func_clear)(Block*);
			uint16_t color;
			switch(r)
			{
				case 0:	func_action	= &nextItem;
						func_draw = NULL;
						func_clear = NULL;
						color = info_color;
						break; // Button for drinks
				case 1:	func_action	= &dialogScreen;
						func_draw = &fillDraw;
						func_clear = NULL;
						color = success_color;
						break; // Button for approval
				case 2:	func_action	= &regular_cancel;
						func_draw = &fillDraw;
						func_clear = NULL;
						color = danger_color;
						break; // Button for cancel
				case 3:	func_action	= &nextScreen;
						func_draw = NULL;
						func_clear = NULL;
						color = info_color;
						break; // Button for next screen
				default:func_action = NULL;
						func_draw = NULL;
						func_clear = NULL;
						color = info_color;
						break;
			}
			blocks[N_BLOCKS] = new Block(screens[s], NCOLS, r+1,	1,	1);
			blocks[N_BLOCKS]->setColor(color);
			blocks[N_BLOCKS]->setAction(func_action);
			blocks[N_BLOCKS]->setDrawfun(func_draw);
			blocks[N_BLOCKS]->setClearfun(func_clear);
			screens[s]->attach_block(blocks[N_BLOCKS]);
		}
	}

	// Dialog
	blocks[N_BLOCKS] = new Block(screens[2], NCOLS, NROWS-2,	1,	1);
	blocks[N_BLOCKS]->setDrawfun(&fillDraw);
	blocks[N_BLOCKS]->setColor(success_color);
	screens[2]->attach_block(blocks[N_BLOCKS]);
	N_BLOCKS ++;
	blocks[N_BLOCKS] = new Block(screens[2], NCOLS, NROWS-1,	1,	1);
	blocks[N_BLOCKS]->setAction(&dialog_cancel);
	blocks[N_BLOCKS]->setDrawfun(&fillDraw);
	blocks[N_BLOCKS]->setColor(danger_color);
	screens[2]->attach_block(blocks[N_BLOCKS]);
	N_BLOCKS ++;
	blocks[N_BLOCKS] = new Block(screens[2], 2, 1,	1,	1);
	blocks[N_BLOCKS]->setDrawfun(&drawList);
	blocks[N_BLOCKS]->setClearfun(&clearList);
	screens[2]->attach_block(blocks[N_BLOCKS]);
	blocks[N_BLOCKS]->setColor(success_color);
	N_BLOCKS ++;
}
