#include "mbed.h"
#include "C12832.h"
#include "LPC1768_dma.h"

volatile uint32_t DMATCCount = 0;
volatile uint32_t DMAErrCount = 0;

void DMA_IRQHandler (void);
uint32_t DMA_Init( uint32_t DMAMode );

C12832 lcd(p5, p7, p6, p8, p11);

int main(void)
{
    lcd.cls();
    lcd.locate(0,0);
    lcd.printf("DMA M2M test!");

    char src[] = "Hello world!";
    uint8_t size = sizeof (src);
    char *dst  = (char *) malloc(size);
    memset (dst, '\0', size);

    uint32_t DMA_SRC = (uint32_t)src;
    uint32_t DMA_DST = (uint32_t)dst;

    DMA_InitTypeDef dma_init_struct;
    DMA_StructInit(&dma_init_struct);

    /* Initialize structure */
    dma_init_struct.DMA_DestAddr = DMA_DST;
    dma_init_struct.DMA_SrcAddr = DMA_SRC;
    dma_init_struct.DMA_TransferSize = size;
    dma_init_struct.DMA_SrcBurst = 0x01; // 1 byte
    dma_init_struct.DMA_DestBurst = 0x01;
    dma_init_struct.DMA_SrcWidth = 0x00; // byte
    dma_init_struct.DMA_DestWidth = 0x00;
    dma_init_struct.DMA_SrcInc = 1;
    dma_init_struct.DMA_DestInc = 1;
    dma_init_struct.DMA_TermInt = 1; // enable count interrupt:w
    dma_init_struct.DMA_TransferType = M2M;

    LPC_SC->PCONP |= (1 << 29);    /* Enable GPDMA clock */
    DMA_init(LPC_GPDMACH0, &dma_init_struct); //initialize the channel
    LPC_GPDMA->DMACConfig = 0x01; // enable DMA and little endian
    while ( (LPC_GPDMA->DMACConfig & 0x01) == 0);

    // set the IRQ routine
    NVIC_SetVector(DMA_IRQn, (uint32_t)DMA_IRQHandler);
    NVIC_EnableIRQ(DMA_IRQn);

    // Have to be in this order, I don't know why
    DMA_Cmd(LPC_GPDMACH0, ENABLE); // enable channel
    DMA_ITConfig (LPC_GPDMACH0, DMA_ITC, ENABLE)

    lcd.locate(0,10);
    lcd.printf("src text: %s", src);

    while (!DMATCCount);  /* Wait untypedil DMA is done */

    lcd.locate(0,20);
    lcd.printf("dst text: %s", dst);

    lcd.locate(90,0);
    if (strcmp (src, dst) != 0)
        lcd.printf("error!");
    else
        lcd.printf("correct!");

    while (1);
}

void DMA_IRQHandler (void)
{
    uint32_t regVal;
    regVal = LPC_GPDMA->DMACIntTCStat;
    if ( regVal ) {
        DMATCCount++;
        LPC_GPDMA->DMACIntTCClear |= regVal;
    }
    regVal = LPC_GPDMA->DMACIntErrStat;
    if ( regVal ) {
        DMAErrCount++;
        LPC_GPDMA->DMACIntErrClr |= regVal;
    }
    return;
}
