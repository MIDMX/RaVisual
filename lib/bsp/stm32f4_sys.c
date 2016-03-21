/*
 *	Club Robot ESEO 2009 - 2013
 *
 *	Fichier : QS_sys.c
 *	Package : Qualite Soft
 *	Description : Compatibilité avec la libc - redirection de _read et _write vers la liaison série choisie.
 *	Auteur : Gwenn, Alexis, Nirgal
 *	Version 2013
 */

#define QS_SYS_C

#include "stm32f4_sys.h"
#include "macro_types.h"
#include "stm32f4xx_hal.h"
#include <errno.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/unistd.h>
#include <malloc.h>


#ifndef STDOUT_USART
	#define DEFAULT_STDOUT_USART USART1
#endif

#ifndef STDERR_USART
	#define DEFAULT_STDERR_USART USART1
#endif

#ifndef STDIN_USART
	#define DEFAULT_STDIN_USART USART1
#endif

static USART_TypeDef * stdout_usart = DEFAULT_STDOUT_USART;
static USART_TypeDef * stderr_usart = DEFAULT_STDERR_USART;
static USART_TypeDef * stdin_usart 	= DEFAULT_STDIN_USART;


#define HCLK_FREQUENCY_HZ     168000000	//40Mhz, Max: 168Mhz
#define FLASH_WAIT_CYCLES (HCLK_FREQUENCY_HZ / 30000000)	//Voir page 62 du manuel de réference RM0090
#define PCLK1_FREQUENCY_HZ    42000000	//10Mhz, Max: 42Mhz
#define PCLK2_FREQUENCY_HZ    84000000	//40Mhz, Max: 84Mhz
#define CPU_EXTERNAL_CLOCK_HZ 8000000	//8Mhz, Fréquence de l'horloge externe

//L'entrée du VCO doit avoir une freqence entre 1Mhz et 2Mhz
#define VCO_INPUT_HZ  1000000	//1Mhz
//La sortie du VCO doit avoir une freqence entre 192Mhz et 432Mhz
#define VCO_OUTPUT_HZ (HCLK_FREQUENCY_HZ*FORCED_HCLK_DIV*FORCED_PLLP)
#define SYSCLK_HZ (HCLK_FREQUENCY_HZ*FORCED_HCLK_DIV)
#define USB_RNG_SDIO_CLK_HZ  48000000	//L'usb doit utiliser une fréquence de 48Mhz, RNG et SDIO <= 48Mhz, donc on prend 48Mhz

//Pour avoir un PLLP entier, on le force à une valeur, ce qui définira VCO_OUTPUT_HZ en conséquence
#define FORCED_PLLP 2
#define FORCED_HCLK_DIV	1

#define HCLK_DIV	FORCED_HCLK_DIV	//HCLK = SYSCLK_HZ / HCLK_DIV
#define PCLK1_DIV	(HCLK_FREQUENCY_HZ/PCLK1_FREQUENCY_HZ)	//PCLK1 = HCLK_DIV / PCLK1_DIV
#define PCLK2_DIV	(HCLK_FREQUENCY_HZ/PCLK2_FREQUENCY_HZ)	//PCLK2 = HCLK_DIV / PCLK2_DIV

#define PLLM1 (CPU_EXTERNAL_CLOCK_HZ / VCO_INPUT_HZ)
#define PLLN1 (VCO_OUTPUT_HZ  / CPU_EXTERNAL_CLOCK_HZ * PLLM1)	//On utilise pas directement VCO_INPUT_HZ car il peut ne pas pouvoir être exact
#define PLLP1 FORCED_PLLP
#define PLLQ1 (VCO_OUTPUT_HZ / USB_RNG_SDIO_CLK_HZ)

/*
 * @func SYS_init(void)
 * @brief	initialise les horloges du microcontroleur selon les fréquences indiquées en macro.
 */
void SYS_init(void)
{
	RCC_OscInitTypeDef RCC_OscInitStructure;
	RCC_ClkInitTypeDef RCC_ClkInitStructure;

	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	HAL_RCC_DeInit();

	/* Oscillateur externe */
	//ErrorStatus HSEStartUpStatus;
	//RCC_HSEConfig(RCC_HSE_ON);
	//HSEStartUpStatus = RCC_WaitForHSEStartUp();

	RCC_OscInitStructure.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStructure.HSEState = RCC_HSE_ON;

	RCC_OscInitStructure.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStructure.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStructure.PLL.PLLM = PLLM1;
	RCC_OscInitStructure.PLL.PLLN = PLLN1;
	RCC_OscInitStructure.PLL.PLLP = PLLP1;
	RCC_OscInitStructure.PLL.PLLQ = PLLQ1;

	if(HAL_RCC_OscConfig(&RCC_OscInitStructure) != HAL_OK)
	{
		// Erreur à gérer
	}


	//Voir page 60 du manuel de reference
	//FLASH_SetLatency(FLASH_WAIT_CYCLES);
	__HAL_FLASH_SET_LATENCY(FLASH_WAIT_CYCLES);

	//Défini la clock HSE pour avoir des valeurs correcte pour RCC_GetClocksFreq()
	RCC_SetHSEFreq(CPU_EXTERNAL_CLOCK_HZ);

	/* PCLK1 = HCLK/2, PCLK2 = HCLK | HCLK = SYSCLK */
	//Pour savoir si les valeurs sont correctes, veuillez changer HCLK_CHOOSEN_DIV, PCLK1_CHOOSEN_DIV et PCLK2_CHOOSEN_DIV. Une erreur de précompilation indiquera s'il y a un problème
	//RCC_HCLKConfig(RCC_SYSCLK_Div1);
	//RCC_PCLK1Config(RCC_HCLK_Div4);
	//RCC_PCLK2Config(RCC_HCLK_Div2);

	RCC_ClkInitStructure.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_SYSCLK;
	RCC_ClkInitStructure.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStructure.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStructure.APB2CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStructure.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	if(HAL_RCC_ClockConfig(&RCC_ClkInitStructure, FLASH_WAIT_CYCLES) != HAL_OK)
	{
		// Erreur à gérer
	}


	//RCC_PLLConfig(RCC_PLLSource_HSE, PLLM, PLLN, PLLP, PLLQ);

	/* Enable PLL1 */
	//RCC_PLLCmd(ENABLE);
	//while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET){}

	/* Select PLL as system clock source */
	//RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
	//while (RCC_GetSYSCLKSource() != 0x08){}


	SystemCoreClockUpdate();

	//Pas de subpriority sur les interruptions
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

	//Activation de l'exception Division par 0
	SCB->CCR |= SCB_CCR_DIV_0_TRP_Msk;	//

	//Config LibC: no buffering
	setvbuf(stdout, NULL, _IONBF, 0 );
	setvbuf(stderr, NULL, _IONBF, 0 );
	setvbuf(stdin, NULL, _IONBF, 0 );
}


/*  Fonctions appelées par la libc (comme printf)  */
// https://sites.google.com/site/stm32discovery/open-source-development-with-the-stm32-discovery/getting-newlib-to-work-with-stm32-and-code-sourcery-lite-eabi
/*
 environ
 A pointer to a list of environment variables and their values.
 For a minimal environment, this empty list is adequate:
 */
char *__env[1] = { 0 };
char **environ = __env;

__attribute__((weak))
int _write(int file, char *ptr, int len);

__attribute__((weak))
void _exit(int status) {
	_write(1, "exit", 4);
	while (1) {
		;
	}
}

__attribute__((weak))
int _close(int file) {
	return -1;
}

/*
 execve
 Transfer control to a new process. Minimal implementation (for a system without processes):
 */
__attribute__((weak))
int _execve(char *name, char **argv, char **env) {
	errno = ENOMEM;
	return -1;
}

/*
 fork
 Create a new process. Minimal implementation (for a system without processes):
 */
__attribute__((weak))
int _fork() {
	errno = EAGAIN;
	return -1;
}

/*
 fstat
 Status of an open file. For consistency with other minimal implementations in these examples,
 all files are regarded as character special devices.
 The `sys/stat.h' header file required is distributed in the `include' subdirectory for this C library.
 */
__attribute__((weak))
int _fstat(int file, struct stat *st) {
	st->st_mode = S_IFCHR;
	return 0;
}

/*
 getpid
 Process-ID; this is sometimes used to generate strings unlikely to conflict with other processes. Minimal implementation, for a system without processes:
 */
__attribute__((weak))
int _getpid() {
	return 1;
}

/*
 isatty
 Query whether output stream is a terminal. For consistency with the other minimal implementations,
 */
__attribute__((weak))
int _isatty(int file) {
	switch (file){
		case STDOUT_FILENO:
		case STDERR_FILENO:
		case STDIN_FILENO:
			return 1;
		default:
			//errno = ENOTTY;
			errno = EBADF;
			return 0;
	}
}

/*
 kill
 Send a signal. Minimal implementation:
 */
__attribute__((weak))
int _kill(int pid, int sig) {
	errno = EINVAL;
	return (-1);
}

/*
 link
 Establish a new name for an existing file. Minimal implementation:
 */
__attribute__((weak))
int _link(char *old, char *new) {
	errno = EMLINK;
	return -1;
}

/*
 lseek
 Set position in a file. Minimal implementation:
 */
__attribute__((weak))
int _lseek(int file, int ptr, int dir) {
	return 0;
}

/*
__attribute__((strong))
_PTR _malloc_r _PARAMS ((struct _reent * reent, size_t size))
{
	return NULL;
}
*/

/*
 sbrk
 Increase program data space.
 Malloc and related functions depend on this
 */
__attribute__((weak))
caddr_t _sbrk(int incr) {

	extern int heap_start; 	// Defined by the linker
	extern int heap_end;	// Defined by the linker

	static int * heap_current = 0;
	static int * heap_end_address = 0;

	int *prev_heap_end;

	if (heap_current == 0)
		heap_current = &heap_start;

	if(heap_end_address == 0)
		heap_end_address = &heap_end;

	prev_heap_end = heap_current;

	//char * stack = (char*) __get_MSP();

	if (heap_current + incr >  heap_end_address)
	{
		_write (STDERR_FILENO, "Heap overflow\n", 25);
		errno = ENOMEM;
		return  (caddr_t) -1;
	}

	heap_current += incr;
	return (caddr_t) prev_heap_end;
}



void * _sbrk_r(struct _reent *ptr, ptrdiff_t incr)
{
  char *ret;

  errno = 0;
  if ((ret = (char *)(_sbrk (incr))) == (void *) -1 && errno != 0)
    ptr->_errno = errno;
  return ret;
}


#define NB_MALLOC   40
void malloc_test()
{
	uint32_t * ptr[NB_MALLOC];
	uint8_t i;
	for(i=0;i<NB_MALLOC;i++)
	{
		ptr[i] = malloc(500);
		debug_printf("malloc %d : %s\n", i, (*ptr)?"ok":"ko!");
	}
	for(i=0;i<NB_MALLOC;i++)
		free(*ptr);
}



/*
caddr_t _sbrk ( int incr )
{
  static unsigned char *heap = NULL;
  unsigned char *prev_heap;

  if (heap == NULL) {
    heap = (unsigned char *)&__HEAP_START;
  }
  prev_heap = heap;
  // check removed to show basic approach

  heap += incr;

  return (caddr_t) prev_heap;
}
*/



void SYS_set_std_usart(USART_TypeDef * in, USART_TypeDef * out, USART_TypeDef * err)
{
	stdin_usart = in;
	stdout_usart = out;
	stderr_usart = err;
}


/*
 read
 Read a character to a file. `libc' subroutines will use this system routine for input from all files, including stdin
 Returns -1 on error or blocks until the number of characters have been read.
 */
__attribute__((weak))
int _read(int file, char *ptr, int len) {
	int n;
	int num = 0;
	switch (file) {
		case STDIN_FILENO:
			for (n = 0; n < len; n++)
			{
				while ((stdin_usart->SR & USART_FLAG_RXNE) == (uint16_t)RESET);
				char c = (char)(stdin_usart->DR & (uint16_t)0x01FF);
				*ptr++ = c;
				num++;
			}
			break;
		default:
			errno = EBADF;
			return -1;
	}
	return num;
}

/*
 stat
 Status of a file (by name). Minimal implementation:
 int    _EXFUN(stat,( const char *__path, struct stat *__sbuf ));
 */
__attribute__((weak))
int _stat(const char *filepath, struct stat *st) {
	st->st_mode = S_IFCHR;
	return 0;
}

/*
 times
 Timing information for current process. Minimal implementation:
 */
__attribute__((weak))
clock_t _times(struct tms *buf) {
	return -1;
}

/*
 unlink
 Remove a file's directory entry. Minimal implementation:
 */
__attribute__((weak))
int _unlink(char *name) {
	errno = ENOENT;
	return -1;
}

/*
 wait
 Wait for a child process. Minimal implementation:
 */
__attribute__((weak))
int _wait(int *status) {
	errno = ECHILD;
	return -1;
}

/*
 write
 Write a character to a file. `libc' subroutines will use this system routine for output to all files, including stdout
 Returns -1 on error or number of bytes sent
 */
__attribute__((weak))
int _write(int file, char *ptr, int len) {
	int n;
	switch (file) {
		case STDOUT_FILENO: /*stdout*/
			for (n = 0; n < len; n++)
			{
				while ((stdout_usart->SR & USART_FLAG_TC) == (uint16_t)RESET);
				stdout_usart->DR = (*ptr++ & (uint16_t)0x01FF);
			}
			break;
		case STDERR_FILENO: /* stderr */
			for (n = 0; n < len; n++)
			{
				while ((stderr_usart->SR & USART_FLAG_TC) == (uint16_t)RESET);
				stderr_usart->DR = (*ptr++ & (uint16_t)0x01FF);
			}
			break;
		default:
			errno = EBADF;
			return -1;
	}
	return len;
}

void assert_failed(uint8_t* file, uint32_t line)
{
	debug_printf("ASSERT FAILED file : %s, line %ld\n", file, line);
	while(1);
}


void dump_trap_info(uint32_t stack_ptr[], uint32_t lr) {
	extern char _estack;	//Defined by the linker, end of stack

	debug_printf("FATAL Error ! Exception %ld\n", __get_IPSR() & 0xFF);
	//See http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0553a/CHDBIBGJ.html
	/*  ISR_NUMBER
		This is the number of the current exception:
		0 = Thread mode
		1 = Reserved
		2 = NMI
		3 = HardFault
		4 = MemManage
		5 = BusFault
		6 = UsageFault
		7-10 = Reserved
		11 = SVCall
		12 = Reserved for Debug
		13 = Reserved
		14 = PendSV
		15 = SysTick
		16 = IRQ0.
	*/
	if(lr & 0x00000008)
		debug_printf("CPU was in thread mode\n");
	else debug_printf("CPU was in handler mode\n");

	int offset, i;
	offset = 0;

	debug_printf("CPU status was:\n");
	debug_printf("-  R0: 0x%08lX  R1: 0x%08lX\n", stack_ptr[offset], stack_ptr[offset + 1]); offset += 2;
	debug_printf("-  R2: 0x%08lX  R3: 0x%08lX\n", stack_ptr[offset], stack_ptr[offset + 1]); offset += 2;
	debug_printf("- R12: 0x%08lX\n", stack_ptr[offset++]);
	debug_printf("- LR:   0x%08lX\n", stack_ptr[offset++]);
	debug_printf("- PC:   0x%08lX\n", stack_ptr[offset++]);
	debug_printf("- xPSR: 0x%08lX\n\n", stack_ptr[offset++]);
	if(lr & 0x00000010) {
		debug_printf("FPU status was:\n");
		debug_printf("-  S0: 0x%08lX   S1: 0x%08lX   S2: 0x%08lX   S3: 0x%08lX\n", stack_ptr[offset], stack_ptr[offset + 1], stack_ptr[offset + 2], stack_ptr[offset + 3]); offset += 4;
		debug_printf("-  S4: 0x%08lX   S5: 0x%08lX   S6: 0x%08lX   S7: 0x%08lX\n", stack_ptr[offset], stack_ptr[offset + 1], stack_ptr[offset + 2], stack_ptr[offset + 3]); offset += 4;
		debug_printf("-  S8: 0x%08lX   S9: 0x%08lX  S10: 0x%08lX  S11: 0x%08lX\n", stack_ptr[offset], stack_ptr[offset + 1], stack_ptr[offset + 2], stack_ptr[offset + 3]); offset += 4;
		debug_printf("- S12: 0x%08lX  S13: 0x%08lX  S14: 0x%08lX  S15: 0x%08lX\n", stack_ptr[offset], stack_ptr[offset + 1], stack_ptr[offset + 2], stack_ptr[offset + 3]); offset += 4;
		debug_printf("- FPSCR: 0x%08lX\n", stack_ptr[offset++]);
		offset++; //empty value at end
	}

	debug_printf("Stack was: \n");
	for(i=0; i < 32 && (char*)&(stack_ptr[offset]) < &_estack; i++) {
		if(!((i + 1) % 4) && i)
			debug_printf("\n");
		debug_printf("0x%08lX ", stack_ptr[offset++]);
	}
	debug_printf("\n");


	//See http://infocenter.arm.com/help/topic/com.arm.doc.dui0553a/Cihdjcfc.html

	if((__get_IPSR() & 0xFF) == 3) {
		debug_printf("HardFault reason:\n");
		if(SCB->HFSR & SCB_HFSR_DEBUGEVT_Msk)
			debug_printf("- DEBUGEVT\n");
		if(SCB->HFSR & SCB_HFSR_FORCED_Msk)
			debug_printf("- Fault escalated to a hard fault\n");
		if(SCB->HFSR & SCB_HFSR_VECTTBL_Msk)
			debug_printf("- Bus error on a vector read\n");
	}


	//See http://infocenter.arm.com/help/topic/com.arm.doc.dui0553a/Cihcfefj.html#Cihgghei

	if((__get_IPSR() & 0xFF) == 4) {
		if((SCB->CFSR >> SCB_CFSR_MEMFAULTSR_Pos) & 0x80)
			debug_printf("MemManage fault at address 0x%08lX\n", SCB->MMFAR);
		else
			debug_printf("MemManage fault\n");

		if((SCB->CFSR >> SCB_CFSR_MEMFAULTSR_Pos) & 0x01)
			debug_printf("- Memory is not executable\n");
		if((SCB->CFSR >> SCB_CFSR_MEMFAULTSR_Pos) & 0x02)
			debug_printf("- Memory is not readable/writable\n");
		if((SCB->CFSR >> SCB_CFSR_MEMFAULTSR_Pos) & 0x08)
			debug_printf("- Exception when unstacking from exception\n");
		if((SCB->CFSR >> SCB_CFSR_MEMFAULTSR_Pos) & 0x10)
			debug_printf("- Exception when stacking for an exception\n");
		if((SCB->CFSR >> SCB_CFSR_MEMFAULTSR_Pos) & 0x20)
			debug_printf("- Fault when floating-point lazy state preservation\n");
	}


	//See http://infocenter.arm.com/help/topic/com.arm.doc.dui0553a/Cihcfefj.html#Cihbeigb

	if((__get_IPSR() & 0xFF) == 5) {
		if((SCB->CFSR >> SCB_CFSR_BUSFAULTSR_Pos) & 0x80)
			debug_printf("BusFault fault at address 0x%08lX\n", SCB->BFAR);
		else
			debug_printf("BusFault fault\n");

		if((SCB->CFSR >> SCB_CFSR_BUSFAULTSR_Pos) & 0x01)
			debug_printf("- Instruction bus error\n");
		if((SCB->CFSR >> SCB_CFSR_BUSFAULTSR_Pos) & 0x02)
			debug_printf("- Precise Data bus error\n");
		if((SCB->CFSR >> SCB_CFSR_BUSFAULTSR_Pos) & 0x04)
			debug_printf("- Imprecise Data bus error\n");
		if((SCB->CFSR >> SCB_CFSR_BUSFAULTSR_Pos) & 0x08)
			debug_printf("- Exception when unstacking from exception\n");
		if((SCB->CFSR >> SCB_CFSR_BUSFAULTSR_Pos) & 0x10)
			debug_printf("- Exception when stacking for an exception\n");
		if((SCB->CFSR >> SCB_CFSR_BUSFAULTSR_Pos) & 0x20)
			debug_printf("- Fault when floating-point lazy state preservation\n");
	}

	//See http://infocenter.arm.com/help/topic/com.arm.doc.dui0553a/Cihcfefj.html#Cihgbdbi

	if((__get_IPSR() & 0xFF) == 6) {
		debug_printf("UsageFault fault, return address: 0x%08lX\n", stack_ptr[6]);

		if((SCB->CFSR >> SCB_CFSR_USGFAULTSR_Pos) & 0x001)
			debug_printf("- Undefined instruction\n");
		if((SCB->CFSR >> SCB_CFSR_USGFAULTSR_Pos) & 0x002)
			debug_printf("- Illegal use of the EPSR\n");
		if((SCB->CFSR >> SCB_CFSR_USGFAULTSR_Pos) & 0x004)
			debug_printf("- Illegal load of the PC\n");
		if((SCB->CFSR >> SCB_CFSR_USGFAULTSR_Pos) & 0x008)
			debug_printf("- Attempt to access a coprocessor but not present\n");
		if((SCB->CFSR >> SCB_CFSR_USGFAULTSR_Pos) & 0x100)
			debug_printf("- Unaligned memory access\n");
		if((SCB->CFSR >> SCB_CFSR_USGFAULTSR_Pos) & 0x200)
			debug_printf("- Divide by zero\n");
	}
	debug_printf("END of Fault Handler\n");
}

__attribute__((naked)) void Fault_Handler(void)
{
	//On ne veux pas perdre l'état des registres, donc pas de C
	//l'attribut naked indique qu'on ne veux pas de prologue / epilogue générés par GCC
	__asm volatile
	(
		"TST LR, #4\n"		// Test for MSP or PSP
		"ITE EQ\n"			//If equal
		"MRSEQ R0, MSP\n"	//r0 = msp
		"MRSNE R0, PSP\n"	//else r0 = psp
		"MOV R1, LR\n"
		"B dump_trap_info\n"
	);
}
void HardFault_Handler(void) __attribute__((alias("Fault_Handler"))) 	__attribute__((unused));
void MemManage_Handler(void) __attribute__((alias("Fault_Handler"))) 	__attribute__((unused));
void BusFault_Handler(void) __attribute__((alias("Fault_Handler"))) 	__attribute__((unused));
void UsageFault_Handler(void) __attribute__((alias("Fault_Handler"))) 	__attribute__((unused));

void NMI_Handler(void)
{
}

void SVC_Handler(void)
{
	debug_printf("SVC interrupt: unimplemented\n");
}

void DebugMon_Handler(void)
{
	debug_printf("DebugMon: unimplemented\n");
}

void PendSV_Handler(void)
{
	debug_printf("Pending SVC interrupt: unimplemented\n");
}



static callback_fun_t usr_systick_handler = NULL;

void SysTick_Handler(void)
{
	HAL_IncTick();
	if(usr_systick_handler)
		(*usr_systick_handler)();
}


void SYS_set_usr_systick_handler(callback_fun_t func)
{
	usr_systick_handler = func;
}

/**
  * @brief  Inserts a delay time.
  * @func void Delay(uint32_t wait_duration_ms)
  * @param  wait_duration_ms: specifies the delay time length, in milliseconds
  * @retval None
  */
void Delay(uint32_t wait_duration_ms)
{
	uint32_t tick;
	tick = HAL_GetTick();
	while(HAL_GetTick() - tick < wait_duration_ms);
}


