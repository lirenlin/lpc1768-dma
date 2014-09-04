
#include "mbed.h"

#include "dma.h"

#define test_m2m 0
#define test_m2p 1
volatile uint32_t DMATCCount = 0;
volatile uint32_t DMAErrCount = 0;


DigitalOut myled1(LED1);
DigitalOut myled2(LED2) ;
DigitalOut myled3(LED3) ;


RawSerial pc (USBTX, USBRX);

void led_switchon(void);
void led_switchoff (void);

int main(void)
{

#if test_m2m
    /* test the DMA M2M, copy data from src to dest, and then print out the dest mem data */

    printf("start to test DMA M2M test!\r\n");

    char src[] = "Hello world\r\n";
    uint8_t size = sizeof (src);
    char *dst  = (char *) malloc(size);
    memset (dst, '\0', size);
    DMA dma1 (0) ;
    dma1.source (src,0,1);
    //dma1.destination (dst,0,1);
    dma1.destination (dst, 0, 1);
    dma1.attach_TC(led_switchon) ;
    dma1.start(size);
    dma1.wait();
    printf("dst text: %s", dst);

    if (strcmp (src, dst) != 0)
        printf("error! \r\n");
    else
        printf("correct! \r\n");

    //while (1);
#endif

#if test_m2p
    /*test m2P, send the memory data to UART;*/
    pc.printf ("start to test M2P now \r\n");
    char src2[] = "This message was transmitted via UART DMA from memory \r\n";

    uint8_t size2 = sizeof(src2);

    DMA dma2(4);
    /* Initialize structure */
    LPC_UART0->FCR |= 1<<3 ; //enable UART DMA mode
    dma2.destination (&(LPC_UART0->THR),0,0);
    dma2.source(src2,0,1);
    dma2.trigger(_UART0_TX);
    dma2.attach_TC(led_switchoff);
    dma2.start(size2);
    dma2.wait();

#endif
    while (1);
}

void led_switchon(void)
{
    myled1=1;
    myled2=1;

}

void led_switchoff(void)
{
    myled1=1;
    myled2=0;
}
