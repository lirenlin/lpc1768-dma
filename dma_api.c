#include "LPC1768_dma.h"


/*DMA channel control register*/
#define DMA_CCxControl_TransferSize_Pos 0
#define DMA_CCxControl_SBSize_Pos 12
#define DMA_CCxControl_DBSize_Pos 15
#define DMA_CCxControl_SWidth_Pos 18
#define DMA_CCxControl_DWidth_Pos 21
#define DMA_CCxControl_SI_Pos 26
#define DMA_CCxControl_DI_Pos 27
#define DMA_CCxControl_I_Pos 31

/*DMA Channel config register*/
#define DMA_CCxConfig_E_Pos 0
#define DMA_CCxConfig_SrcPeripheral_Pos 1
#define DMA_CCxConfig_DestPeripheral_Pos 6
#define DMA_CCxConfig_TransferType_Pos 11
#define DMA_CCxConfig_IE_Pos 14
#define DMA_CCxConfig_ITC_Pos 15
#define DMA_CCxConfig_L_Pos 16
#define DMA_CCxConfig_A_Pos 17
#define DMA_CCxConfig_H_Pos 18


/**
  * @brief  Initializes the DMAy Channelx according to the specified parameters
  *         in the DMA_InitStruct.
  * @param  DMAyChannelx.Channel base address. x can be 1 to 7 for DMA to select the DMA Channel.
  * @param  DMA_InitStruct: pointer to a DMA_InitTypeDef structure that contains
  *         the configuration information for the specified DMA Channel.
  * @retval None
  */
void DMA_init(LPC_GPDMACH_TypeDef* DMAy_Channelx, DMA_InitTypeDef* DMA_InitStruct)
{
    /*------------------------- clear the pending interrupts on the chosen channel ----------------*/
    if (DMAy_Channelx == LPC_GPDMACH0) {
        /* Reset interrupt pending bits for DMA Channel0 */
        LPC_GPDMA->DMACIntTCClear = 1<<0;
        LPC_GPDMA->DMACIntErrClr = 1<<0;
    } else if (DMAy_Channelx == LPC_GPDMACH1) {
        /* Reset interrupt pending bits for DMA Channel */
        LPC_GPDMA->DMACIntTCClear = 1<<1;
        LPC_GPDMA->DMACIntErrClr = 1<<1;
    } else if (DMAy_Channelx == LPC_GPDMACH2) {
        /* Reset interrupt pending bits for DMA Channel2 */
        LPC_GPDMA->DMACIntTCClear = 1<<2;
        LPC_GPDMA->DMACIntErrClr = 1<<2;
    } else if (DMAy_Channelx == LPC_GPDMACH3) {
        /* Reset interrupt pending bits for DMA Channel3 */
        LPC_GPDMA->DMACIntTCClear = 1<<3;
        LPC_GPDMA->DMACIntErrClr = 1<<3;
    } else if (DMAy_Channelx == LPC_GPDMACH4) {
        /* Reset interrupt pending bits for DMA Channel4 */
        LPC_GPDMA->DMACIntTCClear = 1<<4;
        LPC_GPDMA->DMACIntErrClr = 1<<4;
    } else if (DMAy_Channelx == LPC_GPDMACH5) {
        /* Reset interrupt pending bits for DMA Channel5 */
        LPC_GPDMA->DMACIntTCClear = 1<<5;
        LPC_GPDMA->DMACIntErrClr = 1<<5;
    } else if (DMAy_Channelx == LPC_GPDMACH6) {
        /* Reset interrupt pending bits for DMA Channel6 */
        LPC_GPDMA->DMACIntTCClear = 1<<6;
        LPC_GPDMA->DMACIntErrClr = 1<<6;
    } else if (DMAy_Channelx == LPC_GPDMACH7) {
        /* Reset interrupt pending bits for DMA Channel7 */
        LPC_GPDMA->DMACIntTCClear = 1<<7;
        LPC_GPDMA->DMACIntErrClr = 1<<7;
    }
    /*--------------------------- DMAy channelx source and destination ----------------*/
    /* Write to DMA channel source address */
    DMAy_Channelx->DMACCSrcAddr = DMA_InitStruct->DMA_SrcAddr;
    /* Write to DMA channel destination address */
    DMAy_Channelx->DMACCDestAddr = DMA_InitStruct->DMA_DestAddr;
    /*--------------------------- DMAy channelx control register ----------------*/
    /* Set TransferSize */
    /* Set source burst size */
    /* Set destination burst size */
    /* Set source width size */
    /* Set destination width size */
    /* Set source increment */
    /* Set destination increment */
    /* Set destination increment */
    /* Enable terminal count interrupt */
    DMAy_Channelx->DMACCControl |= (DMA_InitStruct->DMA_TransferSize<<DMA_CCxControl_TransferSize_Pos) |
                                   (DMA_InitStruct->DMA_SrcBurst<<DMA_CCxControl_SBSize_Pos)  |
                                   (DMA_InitStruct->DMA_DestBurst<<DMA_CCxControl_DBSize_Pos) |
                                   (DMA_InitStruct->DMA_SrcWidth<<DMA_CCxControl_SWidth_Pos)  |
                                   (DMA_InitStruct->DMA_DestWidth<<DMA_CCxControl_DWidth_Pos) |
                                   (DMA_InitStruct->DMA_SrcInc<<DMA_CCxControl_SI_Pos)    |
                                   (DMA_InitStruct->DMA_DestInc<<DMA_CCxControl_DI_Pos)   |
                                   (DMA_InitStruct->DMA_TermInt<<DMA_CCxControl_I_Pos );
    /*--------------------------- DMAy channelx configuration register ----------------*/
    /* Set source periheral trigger. If the source is memory, this bit is ignored */
    /* Set destination periheral trigger. If the destination is memory, this bit is ignored */
    /* Set transfer type: M2M, M2P, P2M, M2M */
    DMAy_Channelx->DMACCConfig |= (DMA_InitStruct->DMA_SrcPeripheral<<DMA_CCxConfig_SrcPeripheral_Pos)|
                                  (DMA_InitStruct->DMA_DestPeripheral<<DMA_CCxConfig_DestPeripheral_Pos)|
                                  (DMA_InitStruct->DMA_TransferType<< DMA_CCxConfig_TransferType_Pos);
}//end of DMA_init

/**
  * @brief  Fills each DMA_InitStruct member with its default value.
  * @param  DMA_InitStruct: pointer to a DMA_InitTypeDef structure which will
  *         be initialized.
  * @retval None
  */
void DMA_StructInit(DMA_InitTypeDef* DMA_InitStruct)
{
    /*-------------- Reset DMA init structure parameters values ------------------*/
    DMA_InitStruct->DMA_DestAddr=0;
    DMA_InitStruct->DMA_SrcAddr=0;
    DMA_InitStruct->DMA_TransferSize=0;
    DMA_InitStruct->DMA_SrcBurst=0;
    DMA_InitStruct->DMA_DestBurst=0;
    DMA_InitStruct->DMA_SrcWidth=0;
    DMA_InitStruct->DMA_DestWidth=0;
    DMA_InitStruct->DMA_SrcInc=0;
    DMA_InitStruct->DMA_DestInc=0;
    DMA_InitStruct->DMA_TermInt=0;
    DMA_InitStruct->DMA_SrcPeripheral=0;
    DMA_InitStruct->DMA_DestPeripheral=0;
    DMA_InitStruct->DMA_TransferType=M2M;
} //end of DMA_StructInit

/**
  * @brief  Enables or disables the specified DMAy Channelx.
  * @param  DMAy_Channelx. Channel base address. x can be 1 to 7 for DMA1 to select the DMA Channel.
  * @param  NewState: new state of the DMAy Channelx.
  *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void DMA_Cmd(LPC_GPDMACH_TypeDef*  DMAy_Channelx, FunctionalState NewState)
{
    if (NewState != DISABLE) {
        /* Enable the selected DMAy Channelx */
        DMAy_Channelx->DMACCConfig |= 1ul<<DMA_CCxConfig_E_Pos;
    } else {
        /* Disable the selected DMAy Channelx */
        DMAy_Channelx->DMACCConfig &= (uint16_t)(~1ul<<DMA_CCxConfig_E_Pos);
    }
} // end of DMA_cmd

/**
  * @brief  Mask or unmask selected interrupt
  * @param  DMAy_Channelx. Channel base address. x can be 1 to 7 for DMA1 to select the DMA Channel.
  * @param  DMA_IT: choose which interrupt to operate
            This parameter can be terminal counter Interrupr ITC or err interrupt ITE
  * @param  NewState: new state of the DMAy Channelx Interrupt.
  *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void DMA_ITConfig (LPC_GPDMACH_TypeDef* DMAy_Channelx, uint32_t DMA_IT, FunctionalState NewState)
{
    if (NewState != DISABLE) {
        /*unmask ITC*/
        if(DMA_IT == DMA_ITC ) {
            DMAy_Channelx->DMACCConfig |= 1ul <<DMA_CCxConfig_ITC_Pos;
            DMAy_Channelx->DMACCControl |= 1ul <<DMA_CCxControl_I_Pos;
        }
        /*unmask IE*/
        else if (DMA_IT == DMA_IE ) {
            DMAy_Channelx->DMACCConfig |= 1ul<< DMA_CCxConfig_IE_Pos;
        }
    } else {
        /*mask ITC*/
        if(DMA_IT == DMA_ITC ) {
            DMAy_Channelx->DMACCConfig &= ~(1ul <<DMA_CCxConfig_ITC_Pos);
            DMAy_Channelx->DMACCControl &= ~(1ul <<DMA_CCxControl_I_Pos);
        }
        /*mask IE*/
        else if (DMA_IT == DMA_IE) {
            DMAy_Channelx->DMACCConfig &= ~(1ul <<DMA_CCxConfig_IE_Pos);
        }
    }
} //end of DMA_ITConfig

/**
  * @brief  Clear pending interrupt on the channel
  * @param  DMAy_Channelx. Channel base address. x can be 1 to 7 for DMA1 to select the DMA Channel.
  * @param  DMA_IT: choose which interrupt to clear
            This parameter can be terminal counter Interrupr ITC or err interrupt ITE
  * @retval None
  */
void DMA_ClearITPendingBit(LPC_GPDMACH_TypeDef* DMAy_Channelx, uint32_t DMA_IT)
{

    /* clear the pending interrupts on the chosen channel by writing to the DMACIntTCClear and DMACIntErrClear register. */
    if (DMAy_Channelx == LPC_GPDMACH0) {
        /* Reset interrupt pending bits for DMA1 Channel0 */
        LPC_GPDMA->DMACIntTCClear = 1<<0;
        LPC_GPDMA->DMACIntErrClr = 1<<0;
    } else if (DMAy_Channelx == LPC_GPDMACH1) {
        /* Reset interrupt pending bits for DMA1 Channel1 */
        LPC_GPDMA->DMACIntTCClear = 1<<1;
        LPC_GPDMA->DMACIntErrClr = 1<<1;
    } else if (DMAy_Channelx == LPC_GPDMACH2) {
        /* Reset interrupt pending bits for DMA1 Channel2 */
        LPC_GPDMA->DMACIntTCClear = 1<<2;
        LPC_GPDMA->DMACIntErrClr = 1<<2;
    } else if (DMAy_Channelx== LPC_GPDMACH3) {
        /* Reset interrupt pending bits for DMA1 Channel3 */
        LPC_GPDMA->DMACIntTCClear = 1<<3;
        LPC_GPDMA->DMACIntErrClr = 1<<3;
    } else if (DMAy_Channelx == LPC_GPDMACH4) {
        /* Reset interrupt pending bits for DMA1 Channel4 */
        LPC_GPDMA->DMACIntTCClear = 1<<4;
        LPC_GPDMA->DMACIntErrClr = 1<<4;
    } else if (DMAy_Channelx == LPC_GPDMACH5) {
        /* Reset interrupt pending bits for DMA1 Channel5 */
        LPC_GPDMA->DMACIntTCClear = 1<<5;
        LPC_GPDMA->DMACIntErrClr = 1<<5;
    } else if (DMAy_Channelx == LPC_GPDMACH6) {
        /* Reset interrupt pending bits for DMA1 Channel6 */
        LPC_GPDMA->DMACIntTCClear = 1<<6;
        LPC_GPDMA->DMACIntErrClr = 1<<6;
    } else if (DMAy_Channelx == LPC_GPDMACH7) {
        /* Reset interrupt pending bits for DMA1 Channel7 */
        LPC_GPDMA->DMACIntTCClear = 1<<7;
        LPC_GPDMA->DMACIntErrClr = 1<<7;
    }
} // end of DMA_ClearITPendingBit

/**
  * @brief  Check whether channel is active or not
  * @param  channel. The channel number.
  * @retval 0 or 1
  */
bool DMA_ChannelActive (int channel)
{
    /* check whether channel number is valid. If not valid, use default channel 0. */
    if (channel > (channel_num-1) || channel < 0)
        channel = 0;
    return (LPC_GPDMA->DMACEnbldChns && (1<<channel));
}
