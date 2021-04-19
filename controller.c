#include "controller.h"

void wait_button(void) {
	ctrl_data_t ctl;
	int btn;

	btn=1;
	while(btn!=0){
		sceCtrlRead(&ctl,1);
		btn = ((ctl.buttons & 0xF000) != 0);
	}
	btn=0;
	while(btn==0){
		sceCtrlRead(&ctl,1);
		btn = ((ctl.buttons & 0xF000) != 0);
	}
}


static unsigned long control_bef_ctl  = 0;
static unsigned long control_bef_tick = 0;

unsigned long Read_Key(unsigned long RepeatTime) {
	ctrl_data_t ctl;

	sceCtrlRead(&ctl,1);
	if (ctl.buttons == control_bef_ctl) {
		if ((ctl.frame - control_bef_tick) > RepeatTime) {
			return control_bef_ctl;
		}
		return 0;
	}
	control_bef_ctl  = ctl.buttons;
	control_bef_tick = ctl.frame;
	return control_bef_ctl;
}


int Confirm_Control(void) {
	unsigned long key;

	while(1) {
		while(1) {
			key = Read_Key(REPEAT_TIME);
			if (key != 0) break;
			pgWaitV();
		}

		if (key & CTRL_CIRCLE) {
			return 1;
		}
		if (key & CTRL_CROSS) {
			return 0;
		}
	}
}

int Control(void) {
	unsigned long key;
	int i;

	// wait key
	while(1) {
		key = Read_Key(REPEAT_TIME);
		if (key != 0) break;
		pgWaitV();
	}
if (key & CTRL_SQUARE)   {return 1;}  
if (key & CTRL_TRIANGLE) {return 2;} 
if (key & CTRL_CIRCLE)   {return 3;}   
if (key & CTRL_CROSS)    {return 4;}
if (key & CTRL_UP)       {return 5;}       
if (key & CTRL_DOWN)     {return 6;}     
if (key & CTRL_LEFT)     {return 7;}     
if (key & CTRL_RIGHT)    {return 8;}    
if (key & CTRL_START)    {return 9;}    
if (key & CTRL_SELECT)   {return 10;}   
if (key & CTRL_LTRIGGER) {return 11;} 
if (key & CTRL_RTRIGGER) {return 12;} 
	
	return 0;
}

