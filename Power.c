
// ----------------------------------------------------------------------------------------
int exit_callback(void) 
{ 
	char *p;

// Cleanup the games resources etc (if required) 

// Exit game 
   sceKernelExitGame(); 

   return 0; 
} 

#define POWER_CB_POWER		0x80000000
#define POWER_CB_HOLDON		0x40000000
#define POWER_CB_STANDBY	0x00080000
#define POWER_CB_RESCOMP	0x00040000
#define POWER_CB_RESUME		0x00020000
#define POWER_CB_SUSPEND	0x00010000
#define POWER_CB_EXT		0x00001000
#define POWER_CB_BATLOW		0x00000100
#define POWER_CB_BATTERY	0x00000080
#define POWER_CB_BATTPOWER	0x0000007F
void power_callback(int unknown, int pwrflags) 
{ 
	char *p;

   // Combine pwrflags and the above defined masks 
} 

// Thread to create the callbacks and then begin polling 
int CallbackThread(void *arg) 
{ 
   int cbid; 

   cbid = sceKernelCreateCallback("Exit Callback", exit_callback); 
   SetExitCallback(cbid); 
   cbid = sceKernelCreateCallback("Power Callback", power_callback); 
   PowerSetCallback(0, cbid); 

   KernelPollCallbacks(); 
} 

/* Sets up the callback thread and returns its thread id */ 
int SetupCallbacks(void) 
{ 
   int thid = 0; 

   thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0x4000, 0, 0); 
   if(thid >= 0) 
   { 
      sceKernelStartThread(thid, 0, 0); 
   } 

   return thid; 
} 

