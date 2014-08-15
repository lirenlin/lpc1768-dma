#include "mbed.h"
#include "LPC1768_dma.h"
DigitalOut myled(LED1);

int main() {
    uint32_t busyChannels;
    printf("Start\r\n");
    
    printf("Use DMA to send 10 characters from buffer to buffer\r\n");
    printf("Then send them to UART0 (PC)\r\n");
    
    char characters[] = "abcdefgh\r\n";     //Characters we send
    char characters2[11];                   //Second buffer
   // LPC_SC->PCONP |= 1<<29;//power on
    DMA_InitTypeDef* new_dma;
    DMA_StructInit (new_dma);
    
    new_dma->DMA_DestAddr= (uint32_t)characters2; /*!< Specifies the destination base address for DMAy Channelx. */
    new_dma->DMA_SrcAddr= (uint32_t)characters;     /*!< Specifies the source base address for DMAy Channelx. */
    new_dma->DMA_TransferSize=10;/*!< Specifies the source  transfer size    */
  //  new_dma->DMA_SrcBurst; /*!< Specifies the source  burst size    */
  //  new_dma->DMA_DestBurst; /*!< Specifies the destination burst size   */
  //  new_dma->DMA_SrcWidth; /*!< Specifies the source transfer width   */
  //  new_dma->DMA_DestWidth; /*!< Specifies the destination transfer width   */
  //  new_dma->DMA_SrcInc;  /*!< Specifies whether the source is incremented or not */
  //  new_dma->DMA_DestInc; /*!< Specifies whether the destination is incremented or not */
    new_dma->DMA_TermInt = 1; /*!< Specifies whether the terminal count interrupt enabled or not */
    
    /*!< Specifies the features set by channel config register */
    new_dma->DMA_SrcPeripheral=0;
    new_dma->DMA_DestPeripheral=0;
    new_dma->DMA_TransferType=0;
    
    
    busyChannels= DMA_EnabledChannels(); 
    if (!(busyChannels && 0x1))
    {
        DMA_init(LPC_GPDMACH0, new_dma);
        DMA_ClearITPendingBit(LPC_GPDMACH0,DMA_ITC);
        DMA_ClearITPendingBit(LPC_GPDMACH0,DMA_IE);
        DMA_ITConfig (LPC_GPDMACH0,DMA_ITC,DISABLE);
        DMA_ITConfig (LPC_GPDMACH0, DMA_IE,DISABLE);
        printf("I am going to start");
        DMA_Cmd(LPC_GPDMACH0, ENABLE);
        
        //while (DMA_ChannelActive(LPC_GPDMACH0));
    }
    wait(10);
    printf ("the second buffer now is %s", characters2 );
}
