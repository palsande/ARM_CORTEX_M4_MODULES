// RTOS Framework
// Sandeep Pal

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: EK-TM4C123GXL Evaluation Board
// Target uC:       TM4C123GH6PM
// System Clock:    40 MHz

// Hardware configuration:
// 4 Pushbuttons and 4 LEDs, UART

//-----------------------------------------------------------------------------
// Device includes, defines, and assembler directives
//-----------------------------------------------------------------------------

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "tm4c123gh6pm.h"

#define RED_LED_B        (*((volatile uint32_t *)(0x42000000 + (0x400253FC-0x40000000)*32 + 1*4)))//PF1_RED

#define BLUE_LED_B       (*((volatile uint32_t *)(0x42000000 + (0x400253FC-0x40000000)*32 + 2*4)))//PF2_BLUE
// REQUIRED: correct these bitbanding references for green and yellow LEDs (temporary to guarantee compilation)
//#define YELLOW_LED     (*((volatile uint32_t *)(0x42000000 + (0x400043FC-0x40000000)*32 + 2*4)))//PA2_YELLOW
//#define GREEN_LED      (*((volatile uint32_t *)(0x42000000 + (0x400253FC-0x40000000)*32 + 3*4)))
//#define GREEN_LED      (*((volatile uint32_t *)(0x42000000 + (0x400043FC-0x40000000)*32 + 3*4)))//PA3_GREEN
// REQUIRED: correct these bitbanding references for green and yellow LEDs (temporary to guarantee compilation)
#define ORANGE_LED  (*((volatile uint32_t *)(0x42000000 + (0x400053FC-0x40000000)*32 + 4*4)))
#define GREEN_LED   (*((volatile uint32_t *)(0x42000000 + (0x400053FC-0x40000000)*32 + 5*4)))
#define RED_LED     (*((volatile uint32_t *)(0x42000000 + (0x400053FC-0x40000000)*32 + 7*4)))
#define YELLOW_LED  (*((volatile uint32_t *)(0x42000000 + (0x400053FC-0x40000000)*32 + 6*4)))

#define PB_0           (*((volatile uint32_t *)(0x42000000 + (0x400063FC-0x40000000)*32 + 4*4)))//
#define PB_1           (*((volatile uint32_t *)(0x42000000 + (0x400063FC-0x40000000)*32 + 5*4)))//
#define PB_2           (*((volatile uint32_t *)(0x42000000 + (0x400063FC-0x40000000)*32 + 6*4)))//
#define PB_3           (*((volatile uint32_t *)(0x42000000 + (0x400063FC-0x40000000)*32 + 7*4)))//

#define ENTER_CRITICAL_SECTION   (NVIC_ST_CTRL_R = 0x00)
#define EXIT_CRITICAL_SECTION    (NVIC_ST_CTRL_R = 0x01)
#define POST_ISR_BUFFER_SIZE     16

void* postIsrBuffer[POST_ISR_BUFFER_SIZE];
uint8_t postIsrBufferReadIndex = 0;
uint8_t postIsrBufferWriteIndex = 0;

///
// SVCall Handler parameters, defines.
///
// SV Call action definitions
#define SVCALL_ACTION_YIELD 1
#define SVCALL_ACTION_SLEEP 2
#define SVCALL_ACTION_WAIT 	4
#define SVCALL_ACTION_POST  8
// SVCall parameters
uint32_t svcallHandlerParam1 = 0; // Parameter for storing action requested by RTOS.
uint32_t svcallHandlerParam2 = 0; // Parameter for storing input parameter required.

//-----------------------------------------------------------------------------
// RTOS Defines and Kernel Variables
//-----------------------------------------------------------------------------

// function pointer
typedef void (*_fn)();
void restoreMSP(uint32_t sp);
uint32_t saveMSP();
// semaphore
#define MAX_QUEUE_SIZE 10

struct semaphore
{
  unsigned int count;
  unsigned int queueSize;
  unsigned int processQueue[MAX_QUEUE_SIZE]; // store task index here
} *s, keyPressed, keyReleased, flashReq, printRTOSModeReq;

// task
#define STATE_INVALID    0 // no task
#define STATE_READY      1 // ready to run
#define STATE_BLOCKED    2 // has run, but now blocked by semaphore
#define STATE_DELAYED    3 // has run, but now awaiting timer

#define MAX_TASKS 10       // maximum number of valid tasks
uint8_t taskCurrent = 0;   // index of last dispatched task
uint8_t taskCount = 0;     // total number of valid tasks

int rtosMode;              // mode
#define MODE_COOPERATIVE 0
#define MODE_PREEMPTIVE  1

struct _tcb
{
  uint8_t state;                 // see STATE_ values above
  void *pid;                     // used to uniquely identify process
  void *sp;                      // location of stack pointer for process
  uint8_t priority;              // 0=highest, 7=lowest
  uint8_t skipCount;             //no of times task can be skipped
  uint8_t currentPriority;       // used for priority inheritance
  uint32_t ticks;                // ticks until sleep complete
} tcb[MAX_TASKS];

uint32_t stack[MAX_TASKS][256];

//----------------------------------------------------------------
void save_exception_return_value(uint32_t R0)
{
}
//If read index is not equal to write index then post item needs to be read
bool postIsrReqPending()
{
	return (postIsrBufferReadIndex != postIsrBufferWriteIndex);
}
void* readNextPostIsrReqPending()
{
    void* pSemaphore = postIsrBuffer[postIsrBufferReadIndex];
    postIsrBufferReadIndex = (postIsrBufferReadIndex + 1) % POST_ISR_BUFFER_SIZE;
    return pSemaphore;
}
void postIsr(void* pSemaphore)
{
	bool isBufferfull;
	isBufferfull = ((postIsrBufferWriteIndex+1) % POST_ISR_BUFFER_SIZE) == postIsrBufferReadIndex;
	if (!isBufferfull)
	{
		postIsrBuffer[postIsrBufferWriteIndex] = pSemaphore;
		postIsrBufferWriteIndex = (postIsrBufferWriteIndex + 1) % POST_ISR_BUFFER_SIZE;
	}
}

// Blocking function that writes a serial character when the UART buffer is not full
void putcUart0(char c)
{
	while (UART0_FR_R & UART_FR_BUSY);
	UART0_DR_R = c;
}

// Blocking function that writes a string when the UART buffer is not full
void putsUart0(char* str)
{
	int i;
    for (i = 0; i < strlen(str); i++)
	  putcUart0(str[i]);
}


//-----------------------------------------------------------------------------
// RTOS Kernel
//-----------------------------------------------------------------------------

void rtosInit(int mode,int reload)
{
  uint8_t i;
  rtosMode = mode;
  // no tasks running
  taskCount = 0;
  // clear out tcb records
  for (i = 0; i < MAX_TASKS; i++)
  {
    tcb[i].state = STATE_INVALID;
    tcb[i].pid = 0;
  }
  // REQUIRED: systick for 1ms system timer
  if(rtosMode==MODE_PREEMPTIVE)
  {
	  NVIC_ST_CTRL_R    = 0;         // disable SysTick during setup
	  NVIC_ST_RELOAD_R  = reload-1;  // reload value of system timer
	  NVIC_ST_CURRENT_R = 0;         // any write to current clears it
	  NVIC_SYS_PRI2_R   = 0;         // SVCALL priority 0
	  NVIC_SYS_PRI3_R   = 0;         // SysTick priority 0
	  NVIC_ST_CTRL_R   |= NVIC_ST_CTRL_ENABLE| NVIC_ST_CTRL_CLK_SRC | NVIC_ST_CTRL_INTEN;; // enable SysTick with core clock and interrupts
  }
}
//Original RTOS Scheduler
/*
int rtosScheduler()
{
  // REQUIRED: Implement prioritization to 8 levels
  bool ok;
  static uint8_t task = 0xFF;
  ok = false;
  while (!ok)
  {
    task++;
    if (task >= MAX_TASKS)
      task = 0;
    ok = (tcb[task].state == STATE_READY);
  }
  return task;
}
*/

int rtosScheduler()
{
  // REQUIRED: Implement prioritization to 8 levels(0-7)
  bool ok,priorityReset;
  static uint8_t task = 0xFF;
  ok = false;
  priorityReset = true;
  while (!ok)
  {
    task++;
    if (task >= MAX_TASKS)
        task = 0;
        ok = (tcb[task].state == STATE_READY && tcb[task].skipCount <= 7);//Also validating priority of tasks
        //tcb[task].skipCount--;
  }

  if (tcb[task].skipCount <= 7)
  {
	  tcb[task].skipCount++;
  }
  int i;
  for (i = 0; i < MAX_TASKS; i++)
    {
  	  if (tcb[i].state != STATE_INVALID)
  	  {
  		  if( (tcb[i].state == STATE_READY) && (tcb[i].skipCount == 8))
  		  {
  			 priorityReset = true;
  		  }
  		  else
  		  {
  			priorityReset = false;
  		  }
  	  }
    }

  if (priorityReset)
  {
	  int i;
      for (i = 0; i < MAX_TASKS; i++)
	   {
		    if (tcb[i].state == STATE_READY)
		  	{
		        tcb[i].skipCount = tcb[i].priority;
		  	}
	   }
  }

  return task;
}

bool createProcess(_fn fn, int priority)
{
  bool ok = false;
  uint8_t i = 0;
  bool found = false;
  // REQUIRED: take steps to ensure a task switch cannot occur
  ENTER_CRITICAL_SECTION;

  // save starting address if room in task list
  if (taskCount < MAX_TASKS)
  {
    // make sure fn not already in list (prevent reentrancy)
    while (!found && (i < MAX_TASKS))
    {
      found = (tcb[i++].pid ==  fn);
    }
    if (!found)
    {
      // find first available tcb record
      i = 0;
      while (tcb[i].state != STATE_INVALID) {i++;}
      tcb[i].state = STATE_READY;
      tcb[i].pid = fn;
      // REQUIRED: preload stack to look like the task had run before
      stack[i][255] = 0x01000000;   // xPSR
      stack[i][254] = (uint32_t)fn; // PC
      stack[i][253] = (uint32_t)fn; // LR
      stack[i][252] = 12; 	        // R12
      stack[i][251] = 3; 	        // R3
      stack[i][250] = 2; 	        // R2
      stack[i][249] = 1; 	        // R1
      stack[i][248] = 0;	        // R0
      stack[i][247] = 4;            // R4
      stack[i][246] = 5;	        // R5
      stack[i][245] = 6;	        // R6
      stack[i][244] = 7; 	        // R7
      stack[i][243] = 8; 	        // R8
      stack[i][242] = 9; 	        // R9
      stack[i][241] = 10; 	        // R10
      stack[i][240] = 11; 	        // R11
      tcb[i].sp = &stack[i][240]; // REQUIRED: + offset as needed for the pre-loaded stack
      tcb[i].priority = priority;
      tcb[i].skipCount = priority;
      tcb[i].currentPriority = priority;
      // increment task count
      taskCount++;
      ok = true;
    }
  }
  // REQUIRED: allow tasks switches again
  EXIT_CRITICAL_SECTION;
  return ok;
}
/*
bool createProcess(_fn fn, int priority)
{
  bool ok = false;
  uint8_t i = 0;
  bool found = false;
  // REQUIRED: take steps to ensure a task switch cannot occur
  // save starting address if room in task list
  if (taskCount < MAX_TASKS)
  {
    // make sure fn not already in list (prevent reentrancy)
    while (!found && (i < MAX_TASKS))
    {
      found = (tcb[i++].pid ==  fn);
    }
    if (!found)
    {
      // find first available tcb record
      i = 0;
      while (tcb[i].state != STATE_INVALID) {i++;}
      tcb[i].state = STATE_READY;
      tcb[i].pid = fn;
      // REQUIRED: preload stack to look like the task had run before
      stack[i][255] = (uint32_t)fn; // R4
      stack[i][254] = 5; 	// R5
      stack[i][253] = 6; 	// R6
      stack[i][252] = 7; 	// R7
      stack[i][251] = 8; 	// R8
      stack[i][250] = 9;	// R9
      stack[i][249] = 10;	// R10
      stack[i][248] = 11;	// R11
      stack[i][247] = 14; 	// LR(R14)
      tcb[i].sp = &stack[i][247]; // REQUIRED: + offset as needed for the pre-loaded stack
      tcb[i].priority = priority;
      tcb[i].skipCount = priority;
      tcb[i].currentPriority = priority;
      // increment task count
      taskCount++;
      ok = true;
    }
  }
  // REQUIRED: allow tasks switches again
  return ok;
}*/

// REQUIRED: modify this function to destroy a process
void destroyProcess(_fn fn)
{
}

void restoreMSP(uint32_t sp)
{
	__asm("             MOV   R1, #8 ");
	__asm("             SUB   R0,R0, R1 ");
	__asm("             MSR   MSP, R0 ");
}

uint32_t saveMSP()
{
	__asm("             MRS   R0, MSP ");
}

void rtosStart()
{
  // REQUIRED: add code to call the first task to be run, restoring the preloaded context
  //_fn fn;
  ENTER_CRITICAL_SECTION;

  taskCurrent = rtosScheduler();
  // Add code to initialize the MSP with tcb[task_current].sp;
  // Restore the stack to run the first process
  restoreMSP((uint32_t)tcb[taskCurrent].sp);
  __asm("             POP  {R11, R10, R9, R8, R7, R6, R5, R4}");
  __asm("             POP  {R0, R1, R2, R3, R12}");
  __asm("             POP  {R14}");
  __asm("             POP  {R4}");
  __asm("             POP  {R5}");
  __asm("             MOV  PC,R4");

  EXIT_CRITICAL_SECTION;
}

/*void rtosStart()
{
  // REQUIRED: add code to call the first task to be run, restoring the preloaded context
  //_fn fn;
  taskCurrent = rtosScheduler();
  // Add code to initialize the MSP with tcb[task_current].sp;
  // Restore the stack to run the first process
  restoreMSP((uint32_t)tcb[taskCurrent].sp);
  __asm("             POP  {R11, R10, R9, R8, R7, R6, R5, R4, PC}");
}*/

void init(void* p, int count)
{
  s = p;
  s->count = count;
  s->queueSize = 0;
}

void yield()
{
	svcallHandlerParam1 = SVCALL_ACTION_YIELD; // Set param for yield, used in SVCall
	__asm("    SVC   #32");
}

/*
// REQUIRED: modify this function to yield execution back to scheduler
void yield()
{
	//_fn fn;
	// push registers, call scheduler, pop registers, return to new function
	__asm("             PUSH  {LR}");
	__asm("             PUSH  {R4,R5,R6,R7,R8,R9,R10,R11}");
	tcb[taskCurrent].sp = (void*)saveMSP();//stack is pointing at preloaded at position from createProcess
	taskCurrent = rtosScheduler();
	restoreMSP((uint32_t)tcb[taskCurrent].sp);
	__asm("             POP  {R11, R10, R9, R8, R7, R6, R5, R4, PC}");
}
*/

// REQUIRED: modify this function to support 1ms system timer
// execution yielded back to scheduler until time elapses
void sleep(uint32_t tick)
{
	svcallHandlerParam2 = tick; // Set param for sleep, used in SVCall
	svcallHandlerParam1 = SVCALL_ACTION_SLEEP; // Set param for sleep, used in SVCall
	__asm(" SVC  #32");
}

// REQUIRED: modify this function to support 1ms system timer
// execution yielded back to scheduler until time elapses
/*void sleep(uint32_t tick)
{
	// push registers, set state to delayed, store timeout, call scheduler, pop registers,
	// return to new function (separate unrun or ready processing)
	tcb[taskCurrent].ticks = tick;
	tcb[taskCurrent].state = STATE_DELAYED;
	__asm("             PUSH  {LR}");
	__asm("             PUSH  {R4,R5,R6,R7,R8,R9,R10,R11}");
	tcb[taskCurrent].sp = (void*)saveMSP();
	taskCurrent = rtosScheduler();
	restoreMSP((uint32_t)tcb[taskCurrent].sp);
	__asm("             POP  {R11, R10, R9, R8, R7, R6, R5, R4, PC}");

}*/
/*
// REQUIRED: modify this function to wait a semaphore with priority inheritance
// return if avail (separate unrun or ready processing), else yield to scheduler
void wait(void* pSemaphore)
{
	s = pSemaphore;
	__asm("             PUSH  {LR}");
	__asm("             PUSH  {R4,R5,R6,R7,R8,R9,R10,R11}");
	tcb[taskCurrent].sp = (void*)saveMSP();
    if(s->count>0)
    {
    	s->count--;
    }
    else
    {
    	s->queueSize = s->queueSize++;
    	s->processQueue[s->queueSize-1] = (unsigned int)tcb[taskCurrent].pid;
    	tcb[taskCurrent].state = STATE_BLOCKED;
    	taskCurrent = rtosScheduler();
    }
    restoreMSP((uint32_t)tcb[taskCurrent].sp);
    __asm("             POP  {R11, R10, R9, R8, R7, R6, R5, R4, PC}");

}
*/

// REQUIRED: modify this function to wait a semaphore with priority inheritance
// return if avail (separate unrun or ready processing), else yield to scheduler
void wait(void* pSemaphore)
{
	svcallHandlerParam2 = (uint32_t)pSemaphore;
	svcallHandlerParam1 = SVCALL_ACTION_WAIT;
	__asm(" SVC   #32");
}
/*
// REQUIRED: modify this function to signal a semaphore is available
void post(void* pSemaphore)
{
	s = pSemaphore;
	s->count++;
	if(s->count==1)
	{
		int i;
		for(i=0;i<MAX_TASKS;i++)
		{
			if(s->processQueue[0] == (unsigned int)tcb[i].pid)
				tcb[i].state=STATE_READY;
		}
		for(i=0;i< s->queueSize; i++)
		{
			s->processQueue[i] = s->processQueue[i+1];

		}
		s->queueSize--;
		s->count--;
	}


}
*/
// REQUIRED: modify this function to signal a semaphore is available
void post(void* pSemaphore)
{
	svcallHandlerParam2 = (uint32_t)pSemaphore;
	svcallHandlerParam1 = SVCALL_ACTION_POST;
	__asm(" SVC   #32");
}

void post_uninterrupted(void* pSemaphore)
{
	s = pSemaphore;
	s->count++;
	if(s->count==1)
	{
		int i;
		for(i=0;i<MAX_TASKS;i++)
		{
			if(s->processQueue[0] == (unsigned int)tcb[i].pid)
				tcb[i].state=STATE_READY;
		}
		for(i=0;i< s->queueSize; i++)
		{
			s->processQueue[i] = s->processQueue[i+1];

		}
		s->queueSize--;
		s->count--;
	}
}

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

// Initialize Hardware
void initHw()
{
    // REQUIRED: Add initialization for blue, red, green, and yellow LEDs
	//       4 pushbuttons, and uart
	// Configure HW to work with 16 MHz XTAL, PLL enabled, system clock of 40 MHz
	SYSCTL_RCC_R = SYSCTL_RCC_USESYSDIV|SYSCTL_RCC_XTAL_16MHZ|SYSCTL_RCC_OSCSRC_MAIN|(4 << SYSCTL_RCC_SYSDIV_S);
	// Set GPIO ports to use APB (not needed since default configuration -- for clarity)
	// Note UART on port A must use APB
    SYSCTL_GPIOHBCTL_R  = 0;//use of APB bus
    SYSCTL_RCGC2_R      = SYSCTL_RCGC2_GPIOF | SYSCTL_RCGC2_GPIOA| SYSCTL_RCGC2_GPIOB| SYSCTL_RCGC2_GPIOC;//Enable port A and port F
    //port F Configuration
    GPIO_PORTF_DIR_R    = 0x06;//port F pins 1,2 are outputs and pins 0,4 are input
    GPIO_PORTF_DR2R_R   = 0x06;//pins 1,2 has 2mA current drive strength
    GPIO_PORTF_PUR_R    = 0x11;// enable internal pull-up for push button on pins 0,4
    GPIO_PORTF_DEN_R    = 0x17;//enable(or make LED's and push buttons digital) LEDs and pushbuttons
    //port F Configuration
    GPIO_PORTA_DIR_R    = 0x0C;//port A pins 3,2 are outputs and pins 5,4 are input
    GPIO_PORTA_DR2R_R   = 0x0C;//pins 3,2 has 2mA current drive strength
    GPIO_PORTA_PUR_R    = 0x30;// enable internal pull-up for push button on pins 5,4
    GPIO_PORTA_DEN_R    = 0x3C;//enable(or make LED's and push buttons digital) LEDs and pushbuttons

    // Configure External LED and pushbutton pins
    GPIO_PORTB_DIR_R  = 0xF0;                           // bits 1, 2, and 3 are outputs, other pins are inputs
    GPIO_PORTB_DR2R_R = 0xF0;                          // set drive strength to 2mA (not needed since default configuration -- for clarity)
    GPIO_PORTB_DEN_R  = 0xF0;                          // enable LEDs and pushbuttons

    GPIO_PORTC_DIR_R = 0x00;                         // Bits PC7, PC6, PC5, and PC4 are Inputs for Push Buttons
    GPIO_PORTC_DEN_R = 0xF0;                         // Enable digital functions for the Push Buttons
    GPIO_PORTC_PUR_R = 0xF0;                         // Enable internal pull-up for push buttons

    //-----------------------------------Init of SysTick Timer--------------------------
    NVIC_ST_CTRL_R = 0;         // disable SysTick during setup
    NVIC_ST_RELOAD_R = 40000-1;// reload value of system timer
    NVIC_ST_CURRENT_R = 0;      // any write to current clears it
    NVIC_ST_CTRL_R |= 0x07; // enable SysTick with down counting with interrupt enabled
    //-----------------------------------------------------------------------------------

    //---------------------------Init Uart0 Module---------------------------------------
    // Configure UART0 pins
    SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R0;         // turn-on UART0, leave other uarts in same status
    GPIO_PORTA_DEN_R |= 3;                           // default, added for clarity
    GPIO_PORTA_AFSEL_R |= 3;                         // default, added for clarity
    GPIO_PORTA_PCTL_R |= GPIO_PCTL_PA1_U0TX | GPIO_PCTL_PA0_U0RX;

    // Configure UART0 to 115200 baud, 8N1 format (must be 3 clocks from clock enable and config writes)
    UART0_CTL_R  = 0;                                 // turn-off UART0 to allow safe programming
    UART0_CC_R   = UART_CC_CS_SYSCLK;                  // use system clock (40 MHz)
    UART0_IBRD_R = 21;                               // r = 40 MHz / (Nx115.2kHz), set floor(r)=21, where N=16
    UART0_FBRD_R = 45;                               // round(fract(r)*64)=45
    UART0_LCRH_R = UART_LCRH_WLEN_8;// | UART_LCRH_FEN; // configure for 8N1 w/ 16-level FIFO
    UART0_IM_R   = UART_IM_RXIM;                       // turn-on RX interrupt
    NVIC_EN0_R   = 1<<5;                               // turn-on interrupt 21 (UART0)
    //NVIC_PRI1_R  |= 0x000040000                         //priority 2
    UART0_CTL_R = UART_CTL_TXE | UART_CTL_RXE | UART_CTL_UARTEN; // enable TX, RX, and module

    //-----------------------------------------------------------------------------------
}

// Approximate busy waiting (in units of microseconds), given a 40 MHz system clock
void waitMicrosecond(uint32_t us)
{
	                                            // Approx clocks per us
	__asm("WMS_LOOP0:   MOV  R1, #6");          // 1
    __asm("WMS_LOOP1:   SUB  R1, #1");          // 6
    __asm("             CBZ  R1, WMS_DONE1");   // 5+1*3
    __asm("             NOP");                  // 5
    __asm("             B    WMS_LOOP1");       // 5*3
    __asm("WMS_DONE1:   SUB  R0, #1");          // 1
    __asm("             CBZ  R0, WMS_DONE0");   // 1
    __asm("             B    WMS_LOOP0");       // 1*3
    __asm("WMS_DONE0:");                        // ---
                                                // 40 clocks/us + error
}

// REQUIRED: add code to return a value from 0-15 indicating which of 4 PBs are pressed
uint8_t readPbs()
{
	uint8_t pb_status=0;

	if(PB_0 == 0)
		pb_status = 1;
	else if(PB_1 == 0)
		pb_status = 2;
	else if(PB_2 == 0)//MODE_COOPERATIVE
		pb_status = 4;
	else if(PB_3 == 0)//MODE_PREEMPTIVE
		pb_status = 8;

	return pb_status;
}

// ------------------------------------------------------------------------------
//  Task functions
// ------------------------------------------------------------------------------

// one task must be ready at all times or the scheduler will fail
// the idle task is implemented for this purpose

void idle()
{
  while(true)
  {
    BLUE_LED_B = 1;
    waitMicrosecond(1000);
    BLUE_LED_B = 0;
    yield();
  }
}

void flash4Hz()
{
  while(true)
  {
    GREEN_LED ^= 1;
    sleep(125);
  }
}

void oneshot()
{
  while(true)
  {
    wait(&flashReq);
    YELLOW_LED = 1;
    sleep(1000);
    YELLOW_LED = 0;
  }
}

void partOfLengthyFn()
{
  // represent some lengthy operation
  waitMicrosecond(1000);
  // give another process a chance
  yield();
}

void lengthyFn()
{
  uint16_t i;
  while(true)
  {
    for (i = 0; i < 4000; i++)
    {
      partOfLengthyFn();
    }
    RED_LED ^= 1;
  }
}

void readKeys()
{
  uint8_t buttons;
  while(true)
  {
    wait(&keyReleased);
    buttons = 0;
    while (buttons == 0)
    {
      buttons = readPbs();
      yield();
    }
    post(&keyPressed);
    if ((buttons & 1) != 0)
    {
      YELLOW_LED ^= 1;
      RED_LED = 1;
    }
    if ((buttons & 2) != 0)
    {
      post(&flashReq);
      RED_LED = 0;
    }
    if ((buttons & 4) != 0)
    {
      createProcess(flash4Hz, 0);
    }
    if ((buttons & 8) != 0)
    {
      destroyProcess(flash4Hz);
	}

    yield();
  }
}

void debounce()
{
  uint8_t count;
  while(true)
  {
    wait(&keyPressed);
    count = 10;
    while (count != 0)
    {
      sleep(10);
      if (readPbs() == 0)
        count--;
      else
        count = 10;
    }
    post(&keyReleased);
  }
}

void uncooperative()
{
  while(true)
  {
    while (readPbs() == 8)
    {
    }
    yield();
  }
}
//New UART Task
void printRTOSMode(void)
{
  while(true)
  {
	  putsUart0("\r\nPress <SPACEBAR> to continue...\r\n");
	  wait(&printRTOSModeReq);
	  putsUart0("\r\n----------------------------------------------------------------------------------");
	  putsUart0("\r\n");

	  if (rtosMode)
	  {
		  putsUart0("RTOS-MODE: Preemptive");
	  }
	  else
	  {
		  putsUart0("RTOS-MODE: Cooperative");
	  }

	  /*putsUart0("\r\nTASK-COUNT: ");
	  putsUart0((char*)IntToStr(taskCount));
	  putsUart0("\r\n");

	  uint8_t localCount = topCnt;
	  putsUart0("\r\n::DEBUG-DEBUG::");
	  putsUart0("\r\nTOP-INDEX: ");
	  putsUart0((char*)IntToStr(localCount));
	  putsUart0("\r\n");

	  uint8_t i;
	  for (i = 0; i != 255 ; i++)
	  {
		  putsUart0(" ");
		  if (i == localCount)
		  {putsUart0("[");}
		  putsUart0((char*)IntToStr(top[i]));
		  if (i == localCount)
		  {putsUart0("]");}
	  }
	  putsUart0("\r\n----------------------------------------------------------------------------------");
  	  putsUart0("\r\n");
      */
	  yield();

  }
}


//-----------------------------------------------------------------------------
// Main
//-----------------------------------------------------------------------------

int main(void)
{
    bool ok;
    //uint8_t pb;

	// Initialize hardware
	initHw();

	// Power-up flash
   	RED_LED_B = 1;
	waitMicrosecond(250000);
	RED_LED_B = 0;
	waitMicrosecond(250000);

	// Initialize semaphores
	init(&keyPressed, 0);
	init(&keyReleased, 1);
	init(&flashReq, 5);
	init(&printRTOSModeReq, 0);

   // Initialize selected RTOS
	/*ok = false;
	while (!ok)
	{
	  pb = readPbs();
	  if (pb & 4)
	  {
	    ok = true;
	    waitMicrosecond(1000000);
	    rtosInit(MODE_COOPERATIVE,40000);
	  }
	  if (pb & 8)
	  {
	    ok = true;
	    waitMicrosecond(1000000);
	    rtosInit(MODE_PREEMPTIVE,40000);
	  }
	}*/
	rtosInit(MODE_PREEMPTIVE,40000);
	// Add required idle process
	ok =  createProcess(idle, 7);

	// Add other processes
	ok &= createProcess(flash4Hz, 0);
	//ok &= createProcess(lengthyFn, 6);
	//ok &= createProcess(oneshot, 3);
	//ok &= createProcess(readKeys, 1);
	//ok &= createProcess(debounce, 3);
	//ok &= createProcess(uncooperative, 5);
	ok &= createProcess(printRTOSMode, 4);
	// Start up RTOS
	if (ok)
	  rtosStart(); // never returns
	else
	  RED_LED = 1;

    return 0;
    // don't delete this unreachable code
    // if a function is only called once in your code, it will be
    // accessed with two goto instructions instead of call-return,
    // so any stack-based code will not function correctly
    yield(); sleep(0); wait(0); post(0);
}

void SysTick_Handler(void)
{
  	int i;
  	for(i=0;i<MAX_TASKS;i++)
  	{
  		if(tcb[i].state == STATE_DELAYED)
  		{
  			tcb[i].ticks--;
  			if(tcb[i].ticks == 0)
  			{
  				tcb[i].state = STATE_READY;
  			}
  		}
  	}

  	while(postIsrReqPending())
  	{
  		post_uninterrupted(readNextPostIsrReqPending());
  	}

  	if(rtosMode==MODE_PREEMPTIVE)
  	{
  	//When Interrupt is called from main program,it automatically saves R3 and LR Registers
  	//Since when using conforming way of handling interrupt,stack should exactly mimic interrupt
  		__asm("          POP  {R3}");
  		__asm("          POP  {LR}");
  		__asm("          PUSH {R4-R11}");
  		tcb[taskCurrent].sp = (void*)saveMSP();//stack is pointing at preloaded at position from createProcess
  		taskCurrent = rtosScheduler();
  		restoreMSP((uint32_t)tcb[taskCurrent].sp);
  		save_exception_return_value(0xFFFFFFF9);
  		__asm("          MOV R14, R0");
  		__asm("          POP {R4-R11}");
  		__asm("          BX  LR");

  	}
}

void uart0ISR_Handler(void)
{
	// Collect 16 bit data from UART data register at once.
	// Read only once to keep RX count intact.
	uint16_t data16 = UART0_DR_R;

	// Get the 8bit char data.
	uint8_t data8 = (uint8_t)data16;

	//Check if space bar is hit.
	if ((char)data8  == ' ')
	{
		//post a semaphore. consumed in printRTOSMode routine.
		postIsr(&printRTOSModeReq);
	}

}

void SVCallHandler(void)
{
	// push registers.
	__asm("           POP   {R3}");
	__asm("           POP   {LR}");
	__asm("           PUSH 	{R4-R11}");

	// save stack pointer.
	tcb[taskCurrent].sp = (void*)saveMSP();

	// yield
	if (svcallHandlerParam1 == SVCALL_ACTION_YIELD)
	{
		// call scheduler.
		taskCurrent = rtosScheduler();
	}
	// sleep
	else if (svcallHandlerParam1 == SVCALL_ACTION_SLEEP)
	{
		// copy sleep parameters.
		tcb[taskCurrent].ticks = svcallHandlerParam2;
		tcb[taskCurrent].state = STATE_DELAYED;
		// call scheduler.
		taskCurrent = rtosScheduler();
	}
	// wait
	else if (svcallHandlerParam1 == SVCALL_ACTION_WAIT)
	{
		// copy wait parameter.
		s = (struct semaphore *)svcallHandlerParam2;

		// check if count is greater than zero, decrement it, otherwsie add a task to wait queue
		if (s->count > 0)
		{
			s->count--;
		}
		else
		{
			s->queueSize = s->queueSize++;
			s->processQueue[s->queueSize-1] = (unsigned int)tcb[taskCurrent].pid;
			tcb[taskCurrent].state = STATE_BLOCKED;
			taskCurrent = rtosScheduler();
		}
	}
	// post
	else if (svcallHandlerParam1 == SVCALL_ACTION_POST)
	{
		// copy post parameters
		s = (struct semaphore *)svcallHandlerParam2;

		s->count++;
		if(s->count==1)
		{
			int i;
			for(i=0;i<MAX_TASKS;i++)
			{
				if(s->processQueue[0] == (unsigned int)tcb[i].pid)
					tcb[i].state=STATE_READY;
			}
			for(i=0;i< s->queueSize; i++)
			{
				s->processQueue[i] = s->processQueue[i+1];

			}
			s->queueSize--;
			s->count--;
		}

	}

	// Reset global action indicator
	svcallHandlerParam1 = 0;
	restoreMSP((uint32_t)tcb[taskCurrent].sp);
	save_exception_return_value(0xFFFFFFF9);
	__asm("           MOV  	R14, R0"); // Restore LR

	// pop registers.
	__asm("           POP  	{R4-R11}");

	// Branch to new task current.
	__asm("           BX  	LR");

}




