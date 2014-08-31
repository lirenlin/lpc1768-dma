#include "mbed.h"
#include "C12832.h"
#include "LPC1768_dma.h"

Serial pc(USBTX, USBRX); // tx, rx
//AnalogIn pot1(p19);
C12832 lcd(p5, p7, p6, p8, p11);

volatile uint32_t ADCDMA0Done = 0;
volatile uint32_t DMAErrCount = 0;

void ADC_init();
uint32_t ADC_read(uint8_t channelNum );
void dma_test();
void DMA_IRQHandler (void);

static inline int div_round_up(int x, int y) {
  return (x + (y - 1)) / y;
}
 
int main()
{
    lcd.cls();
    lcd.locate(0,0);
    
    float data = 0;
    ADC_init();
    NVIC_DisableIRQ(ADC_IRQn);
    dma_test();
    
    while(1)
    {
        //data = pot1.read();
        data = ADC_read(4);
        pc.printf("Hello World!: %f\n", data);
        wait(1);
    }
/*
    lcd.cls();
    lcd.locate(0,0);
    lcd.printf("%d",pot1.read());
    */
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

uint32_t ADC_read(uint8_t channel) {
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

/*test P2P, send the data from ADC to UART;*/ 
void dma_test(void)
{
    uint32_t DMA_SRC = (uint32_t) &(LPC_ADC->ADDR4); // DIP19
    uint32_t DMA_DST = (uint32_t) &(LPC_UART0->THR);

    pc.printf("start to test DMA P2P test!\r\n"); 
    
    LPC_UART0->FCR |= 1<<3 ; //enable UART DMA mode
    
 /* Initialize structure */
    DMA_InitTypeDef dma_init_struct;
    DMA_StructInit(&dma_init_struct);
    
    dma_init_struct.DMA_DestAddr = DMA_DST;
    dma_init_struct.DMA_SrcAddr = DMA_SRC; 
    dma_init_struct.DMA_TransferSize = 8;
    dma_init_struct.DMA_SrcBurst = 0x00; // 1
    dma_init_struct.DMA_DestBurst = 0x00;
    dma_init_struct.DMA_SrcWidth = 2; // 4 byte
    dma_init_struct.DMA_DestWidth = 2;
    dma_init_struct.DMA_SrcInc = 1;
    dma_init_struct.DMA_DestInc = 0;
    dma_init_struct.DMA_TermInt = 1; // enable count interrupt
    dma_init_struct.DMA_TransferType = P2P;
    dma_init_struct.DMA_SrcPeripheral = 4; // ADC
    dma_init_struct.DMA_DestPeripheral = 8; // UART
 
    LPC_SC->PCONP |= (1 << 29);    // Enable GPDMA clock   
    DMA_init(LPC_GPDMACH0, &dma_init_struct); //initialize the channel
    LPC_GPDMA->DMACConfig = 0x01; // enable DMA and little endian; have to initial firstly then enable
    while ( (LPC_GPDMA->DMACConfig & 0x01) == 0);

    NVIC_SetVector(DMA_IRQn, (uint32_t)DMA_IRQHandler);
    NVIC_EnableIRQ(DMA_IRQn);
    
    DMA_Cmd(LPC_GPDMACH0, ENABLE); // enable channel
    DMA_ITConfig (LPC_GPDMACH0, DMA_ITC, ENABLE);
    
    while (!ADCDMA0Done);
    ADCDMA0Done = 0;
    
    //while ( DMA_EnabledChannels() & 1);
    lcd.printf("hello world\n");

    pc.printf("\nFinish!\n");
    while (1);
}

void DMA_IRQHandler (void)
{
    uint32_t regVal;
    regVal = LPC_GPDMA->DMACIntTCStat;
    if ( regVal & 0x01)
    {
        LPC_GPDMA->DMACIntTCClear |= regVal;
        ADCDMA0Done++;
    }
    LPC_ADC->ADCR &= ~(0x01<<16);
    regVal = LPC_GPDMA->DMACIntErrStat;
    if ( regVal )
    {
        DMAErrCount++;
        LPC_GPDMA->DMACIntErrClr |= regVal;
    }
    return;
}
