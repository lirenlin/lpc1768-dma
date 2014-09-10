
#include "mbed.h"

#include "dma.h"

#define test_m2m 0
#define test_m2p 1
#define test_p2m 0
volatile uint32_t DMATCCount = 0;
volatile uint32_t DMAErrCount = 0;


DigitalOut myled1(LED1);
DigitalOut myled2(LED2) ;
DigitalOut myled3(LED3) ;


RawSerial pc (USBTX, USBRX);

void led_switchon_m2m(void);
void led_switchon_m2p (void);
void led_switchon_p2m (void);

void ADC_init();
uint32_t ADC_read(uint8_t channelNum );
void dma_test();
void DMA_IRQHandler (void);

static inline int div_round_up(int x, int y)
{
    return (x + (y - 1)) / y;
}


int main(void)
{

#if test_m2m
    /* test the DMA M2M, copy data from src to dest, and then print out the dest mem data */

    pc.printf("start to test DMA M2M test now!\r\n");

    char src[] = "Hello world\r\n";
    uint8_t size = sizeof (src);
    char *dst  = (char *) malloc(size);
    memset (dst, '\0', size);
    DMA dma1 (0) ;
    dma1.source (src,0,1);
    dma1.destination (dst,0,1);
    //dma1.destination (dst, 0, 1);
    dma1.attach_TC(led_switchon_m2m) ;
    dma1.start(size);
    dma1.wait();
    pc.printf("dst text: %s", dst);

    if (strcmp (src, dst) != 0)
        pc.printf("error! \r\n");
    else
        pc.printf("correct! \r\n");

    //while (1);
#endif

#if test_m2p
    /*test m2P, send the memory data to UART;*/ 
    pc.printf ("start to test m2p now\r\n");
   	wait(0.1);
    char src2[] = "This message was transmitted via UART DMA from memory\r\n";
    LPC_UART0->FCR =  1<<3 ; //enable UART DMA mode
	 	LPC_UART0->LCR &= ~(1<<3); // no parity bit genrated 
    DMA dma2(-1);
		
    // Initialize DMA 
    dma2.destination(&(LPC_UART0->THR),sizeof(char)*8,0);
    dma2.source(src2,sizeof(char)*8,1);
    dma2.TriggerDestination(_UART0_TX);
    dma2.attach_TC(led_switchon_m2p);
    dma2.start(sizeof(src2));
    dma2.wait();


#endif

#if test_p2m
    
    pc.printf("start to test DMA P2M test!\r\n");
    wait(1);
    LPC_UART0->FCR  = 1<<3 ; //enable UART DMA mode
    float data = 0;
		int *dst3  = (int *) malloc(8);
    ADC_init();
		NVIC_DisableIRQ(ADC_IRQn);
    
	
    DMA dma3(4);
    dma3.destination(dst3,32,0);
    dma3.source(&(LPC_ADC->ADDR4),32,0);
    dma3.TriggerSource(_ADC);
    dma3.attach_TC(led_switchon_p2m);
  
		
 
		while (1){
		dma3.start(8);
    dma3.wait();
		data= float(dst3[0]>>4 & 0xfff);	
    pc.printf("Hello World!: %f\n", data);
		wait (2);
		}	
    pc.printf("\nFinish!\n");
#endif
    while (1);
		return 0;
}



void ADC_init()
{
    // ensure power is turned on
    LPC_SC->PCONP |= (1 << 12);

    /* p19 pin is set to ADC input.*/
    LPC_PINCON->PINSEL3 |= 0x30000000;
    /* no pull-up, no pull-down */
    LPC_PINCON->PINMODE3 |= 0x20000000;

    // set PCLK of ADC to 1
    LPC_SC->PCLKSEL0 &= ~(0x3 << 24);
    LPC_SC->PCLKSEL0 |= (0x1 << 24);
    uint32_t PCLK = SystemCoreClock;

    // calculate minimum clock divider
    //  clkdiv = divider - 1
    uint32_t MAX_ADC_CLK = 13000000;
    uint32_t clkdiv = div_round_up(PCLK, MAX_ADC_CLK) - 1;

    // Set the generic software-controlled ADC settings
    LPC_ADC->ADCR = (0 << 0)      // SEL: 0 = no channels selected
                    | (clkdiv << 8) // CLKDIV: PCLK max ~= 25MHz, /25 to give safe 1MHz at fastest
                    | (0 << 16)     // BURST: 0 = software control
                    | (0 << 17)     // CLKS: not applicable
                    | (1 << 21)     // PDN: 1 = operational
                    | (0 << 24)     // START: 0 = no start
                    | (0 << 27);    // EDGE: not applicable

    LPC_ADC->ADINTEN = 0x1ff;

    // Select the appropriate channel and start conversion
    LPC_ADC->ADCR &= ~0xFF;
    LPC_ADC->ADCR |= 1 << 4;
    LPC_ADC->ADCR |= 1 << 24;
}

uint32_t ADC_read(uint8_t channel)
{
    // Select the appropriate channel and start conversion
    LPC_ADC->ADCR &= ~0xFF;
    LPC_ADC->ADCR |= 1 << channel;
    LPC_ADC->ADCR |= 1 << 24;
    // Repeatedly get the sample data until DONE bit
    unsigned int data;
    do {
        data = LPC_ADC->ADGDR;
    } while ((data & ((unsigned int)1 << 31)) == 0);
    // Stop conversion
    LPC_ADC->ADCR &= ~(1 << 24);
    return (data >> 4) & 0xfff; // 12 bit
}

void led_switchon_m2m(void)
{
    myled1=1;
}

void led_switchon_m2p(void)
{
    myled2=1;
}

void led_switchon_p2m(void)
{
    myled3=1;
}
