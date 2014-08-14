#include "LPC1768_dma.h"

#define DMA_SRC 0x2007C000
#define DMA_DST 0x20080000
#define DMA_SIZE 0x1000

#define M2M 0x00
#define M2P 0x01
#define P2M 0x02
#define P2P 0x03

volatile uint32_t DMATCCount = 0;
volatile uint32_t DMAErrCount = 0;

void DMA_IRQHandler (void);
uint32_t DMA_Init( uint32_t DMAMode );

int main(void)
{
    uint8_t i;
    uint8_t* src_addr, *dest_addr;

    /* SystemClockUpdate() updates the SystemFrequency variable */
    SystemClockUpdate();
    src_addr = (uint8_t*)DMA_SRC;
    dest_addr = (uint8_t*)DMA_DST;

    for (i = 0; i < DMA_SIZE / 4; i++)
    {
        *src_addr++ = i;
        *dest_addr++ = 0;
    }

    DMA_Init(M2M);
    LPC_GPDMACH0->CConfig |= 0x08001;/* Enable channel and IE bit */

    while (!DMATCCount);  /* Wait untypedil DMA is done */

    /* Verify result */
    src_addr = (uint32_t*)DMA_SRC;
    dest_addr = (uint32_t*)DMA_DST;

    for (i = 0; i < DMA_SIZE / 4; i++)
    {
        if (*src_addr++ != *dest_addr++)
        {
            while (1);  /* fatal error */
        }
    }

    while (1);
}

void DMA_IRQHandler (void)
{
    uint32_t regVal;
    regVal = LPC_GPDMA->IntTCStat;
    if ( regVal )
    {
        DMATCCount++;
        LPC_GPDMA->IntTCClear |= regVal;
    }
    regVal = LPC_GPDMA->IntErrStat;
    if ( regVal )
    {
        DMAErrCount++;
        LPC_GPDMA->IntErrClr |= regVal;
    }
    return;
}
