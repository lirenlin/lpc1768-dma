
#include "dma_api.h"


int _channel_num = 8;

//Define LPC1768 DMA IRQ. Different platforms might have different name
IRQn_Type _DMA_IRQ = DMA_IRQn;

//DMA channel control register bit set
#define DMA_CCxControl_TransferSize_Pos 0
#define DMA_CCxControl_SBSize_Pos 12
#define DMA_CCxControl_DBSize_Pos 15
#define DMA_CCxControl_SWidth_Pos 18
#define DMA_CCxControl_DWidth_Pos 21
#define DMA_CCxControl_SI_Pos 26
#define DMA_CCxControl_DI_Pos 27
#define DMA_CCxControl_I_Pos 31

//DMA Channel config register bit set
#define DMA_CCxConfig_E_Pos 0
#define DMA_CCxConfig_SrcPeripheral_Pos 1
#define DMA_CCxConfig_DestPeripheral_Pos 6
#define DMA_CCxConfig_TransferType_Pos 11
#define DMA_CCxConfig_IE_Pos 14
#define DMA_CCxConfig_ITC_Pos 15
#define DMA_CCxConfig_L_Pos 16
#define DMA_CCxConfig_A_Pos 17
#define DMA_CCxConfig_H_Pos 18



static TransferType getTransferType (uint32_t src_addr, uint32_t dst_addr );


/**
  * @brief  Get the channel address according to the channel number
  * @param  channel. The chosen channel number
  * @retval Chosen channel address
  */
static LPC_GPDMACH_TypeDef* returnChannel(int channel)
{
    switch (channel) {
        case 0:
            return LPC_GPDMACH0;
        case 1:
            return LPC_GPDMACH1;
        case 2:
            return LPC_GPDMACH2;
        case 3:
            return LPC_GPDMACH3;
        case 4:
            return LPC_GPDMACH4;
        case 5:
            return LPC_GPDMACH5;
        case 6:
            return LPC_GPDMACH6;
        case 7:
            return LPC_GPDMACH7;
        default:
            return LPC_GPDMACH0;
    }
}


void DMA_init(int channel, DMA_InitTypeDef* DMA_InitStruct)
{

    LPC_GPDMACH_TypeDef* GPDMA_channel;
    // get DMA channel address
    GPDMA_channel = returnChannel(channel);
    // Calcuate transfer type according to source address and destination address
    DMA_InitStruct->DMA_TransferType = getTransferType(DMA_InitStruct->DMA_SrcAddr, DMA_InitStruct->DMA_DstAddr);
    // Reset interrupt pending bits for DMA Channel
    LPC_GPDMA->DMACIntTCClear = 1<<channel;
    LPC_GPDMA->DMACIntErrClr = 1<<channel;

    /*--------------------------- DMAy channelx request select register ----------------*/
    // Choose UART or Timer DMA request for DMA inputs 8 through 15

    if ( (DMA_InitStruct->DMA_TriggerSource & 0x10) || (DMA_InitStruct->DMA_TriggerDestination & 0x10))
        LPC_SC->DMAREQSEL |= 1 << ((DMA_InitStruct->DMA_TriggerSource || DMA_InitStruct->DMA_TriggerSource) - 24); // I don't know why. The user manual says DMAREQSET is GPDMA register, but in mbed, it is LPC_SC register
    else
        LPC_SC->DMAREQSEL &= ~(1 << ((DMA_InitStruct->DMA_TriggerSource - 24)&& (DMA_InitStruct->DMA_TriggerDestination - 24)));


    /*--------------------------- DMAy channelx source and destination ----------------*/
    // Write to DMA channel source address
    GPDMA_channel->DMACCSrcAddr = DMA_InitStruct->DMA_SrcAddr;
    // Write to DMA channel destination address
    GPDMA_channel->DMACCDestAddr = DMA_InitStruct->DMA_DstAddr;

    /*--------------------------- DMAy channelx control register ----------------*/
    // Set TransferSize
    // Set source burst size
    // Set destination burst size
    // Set source width size
    // Set destination width size
    // Set source increment
    // Set destination increment
    // Set destination increment
    // Enable terminal count interrupt
    GPDMA_channel->DMACCControl |= (DMA_InitStruct->DMA_TransferSize<<DMA_CCxControl_TransferSize_Pos) |
                                   (DMA_InitStruct->DMA_SrcBurst<<DMA_CCxControl_SBSize_Pos)  |
                                   (DMA_InitStruct->DMA_DestBurst<<DMA_CCxControl_DBSize_Pos) |
                                   (DMA_InitStruct->DMA_SrcWidth<<DMA_CCxControl_SWidth_Pos)  |
                                   (DMA_InitStruct->DMA_DestWidth<<DMA_CCxControl_DWidth_Pos) |
                                   (DMA_InitStruct->DMA_SrcInc<<DMA_CCxControl_SI_Pos)    |
                                   (DMA_InitStruct->DMA_DestInc<<DMA_CCxControl_DI_Pos)   |
                                   (DMA_InitStruct->DMA_TermInt<<DMA_CCxControl_I_Pos );


    /*--------------------------- DMAy channelx configuration register ----------------*/
    // Set source periheral trigger. If the source is memory, this bit is ignored. Set according to low 4 bit of DMA_Trigger
    // Set destination periheral trigger. If the destination is memory, this bit is ignored. Set according low 4 bit of DMA_Trigger
    // Set transfer type: M2M, M2P, P2M, M2M
    GPDMA_channel->DMACCConfig |= ((DMA_InitStruct->DMA_TriggerSource & 0x0f) <<DMA_CCxConfig_SrcPeripheral_Pos)|    //set SrcPeripheral according low 4 bit of DMA_Trigger
                                  ((DMA_InitStruct->DMA_TriggerDestination & 0x0f)<<DMA_CCxConfig_DestPeripheral_Pos)|      //set DstPeripheral according low 4 bit of DMA_Trigger
                                  (DMA_InitStruct->DMA_TransferType<<DMA_CCxConfig_TransferType_Pos);

}//end of DMA_init



void DMA_reset(int channel, DMA_InitTypeDef* DMA_InitStruct)
{

    LPC_SC->PCONP |= (1 << 29);
    LPC_GPDMACH_TypeDef* GPDMA_channel;
    //Get DMA channel address
    GPDMA_channel = returnChannel(channel);
    GPDMA_channel->DMACCConfig = 0;
    GPDMA_channel->DMACCControl = 0;
    GPDMA_channel->DMACCLLI = 0;
    GPDMA_channel->DMACCSrcAddr = 0;
    LPC_GPDMA->DMACIntErrClr |= 1 << channel;
    LPC_GPDMA->DMACIntTCClear |= 1 << channel;
}



void DMA_StructInit(DMA_InitTypeDef* DMA_InitStruct)
{
    /*-------------- Reset DMA init structure parameters values ------------------*/
    DMA_InitStruct->DMA_DstAddr = 0;
    DMA_InitStruct->DMA_SrcAddr = 0;
    DMA_InitStruct->DMA_TransferSize = 0;
    DMA_InitStruct->DMA_SrcBurst = 0;
    DMA_InitStruct->DMA_DestBurst = 0;
    DMA_InitStruct->DMA_SrcWidth = 0;
    DMA_InitStruct->DMA_DestWidth = 0;
    DMA_InitStruct->DMA_LLI = 0;
    DMA_InitStruct->DMA_SrcInc = 0;
    DMA_InitStruct->DMA_DestInc = 0;
    DMA_InitStruct->DMA_TermInt = 0;
    DMA_InitStruct->DMA_TriggerSource = ALWAYS;
    DMA_InitStruct->DMA_TriggerDestination = ALWAYS;
    DMA_InitStruct->DMA_TransferType = M2M;
} //end of DMA_StructInit


void DMA_destination (DMA_InitTypeDef* DMA_InitStruct, uint32_t dst, int width, bool inc)
{
    DMA_InitStruct->DMA_DstAddr = dst;
    DMA_InitStruct->DMA_SrcBurst = 0x00; // 1 byte To be done
    DMA_InitStruct->DMA_DestWidth = (width >>4);
    DMA_InitStruct->DMA_DestInc = inc;
}

void DMA_source (DMA_InitTypeDef* DMA_InitStruct, uint32_t src, int width, bool inc)
{
    DMA_InitStruct->DMA_SrcAddr = src;
    DMA_InitStruct->DMA_SrcBurst = 0x00; // 1 byte
    DMA_InitStruct->DMA_SrcInc = inc;
    DMA_InitStruct->DMA_SrcWidth = (width >> 4);
}

void DMA_TransferSize ( DMA_InitTypeDef* DMA_InitStruct,int len)
{
    DMA_InitStruct->DMA_TransferSize = (uint32_t)len  ;
}


void DMA_TriggerSource(DMA_InitTypeDef* DMA_InitStruct, TriggerType trig)
{
    DMA_InitStruct->DMA_TriggerSource = trig;
}

void DMA_TriggerDestination(DMA_InitTypeDef* DMA_InitStruct, TriggerType trig)
{
    DMA_InitStruct->DMA_TriggerDestination = trig;
}


void DMA_IRQ_handler0()
{
    uint32_t FinishStatus=0;
    uint32_t ErrStatus=0;
    FinishStatus = LPC_GPDMA->DMACIntTCStat;
    ErrStatus= LPC_GPDMA->DMACIntErrStat;
    LPC_GPDMA->DMACIntTCClear |= FinishStatus;
    LPC_GPDMA->DMACIntErrClr |=ErrStatus;

}

bool DMA_ITStatus (int channel, DMA_IT interrupt)
{
    uint32_t FinishStatus=0;
    uint32_t ErrStatus=0;

    if (interrupt == ERR) {
        ErrStatus= LPC_GPDMA->DMACIntErrStat;
        return ErrStatus && (1<<channel);
    } else {
        FinishStatus = LPC_GPDMA->DMACIntTCStat;
        return FinishStatus && (1<<channel);
    }
}

bool isMemory (uint32_t addr)
{
    if ((addr >> 28) == 0 || (addr >> 28)== 1)
        return 1;
    else
        return 0;

}

//Static function. Automatice cacluate the transfer type according to the source and destination address
static TransferType getTransferType (uint32_t src_addr, uint32_t dst_addr )
{
    if (isMemory(src_addr)) {   //if source is memory
        if(isMemory(dst_addr))  //if destination is memory
            return M2M;    //return M2M if source is memory and destination is memory.
        else    //if destination is peripheral
            return M2P;    //return M2P if source is memory and destination is peripheral
    } else {        //if source is peripheral
        if(isMemory(dst_addr))      //if destination is memory
            return P2M;     //return P2M if source is peripheral and destination is memory
        else        //if destination is peripheral
            return P2P;     //return P2P if source is peripheral and destination is peripheral
    }
}


bool DMA_ChannelActive (int channel)
{
    LPC_SC->PCONP |= (1 << 29);
    return (LPC_GPDMA->DMACEnbldChns && (1<<channel));
}



void DMA_Cmd(int channel, FunctionalState NewState)
{
    LPC_GPDMACH_TypeDef* GPDMA_channel;
    // get DMA channel address
    GPDMA_channel = returnChannel(channel);

    if (NewState != DISABLE) {
        LPC_GPDMA->DMACConfig = 0x01;
        // Enable the selected DMAy Channelx
        GPDMA_channel->DMACCConfig |= 1ul<<DMA_CCxConfig_E_Pos;

    } else {
        // Disable the selected DMAy Channelx
        GPDMA_channel->DMACCConfig &= (uint16_t)(~1ul<<DMA_CCxConfig_E_Pos);
    }
} // end of DMA_cmd



void DMA_ITConfig (int channel, FunctionalState NewState)
{
    LPC_GPDMACH_TypeDef* GPDMA_channel;
    // get DMA channel address
    GPDMA_channel = returnChannel(channel);

    if (NewState != DISABLE) {
        //unmask ITC
        GPDMA_channel->DMACCConfig |= 1ul <<DMA_CCxConfig_ITC_Pos;
        GPDMA_channel->DMACCControl |= 1ul <<DMA_CCxControl_I_Pos;
        //unmask IE
        GPDMA_channel->DMACCConfig |= 1ul<< DMA_CCxConfig_IE_Pos;
    } else { // to be consider: remove else?
        //mask ITC
        GPDMA_channel->DMACCConfig &= ~(1ul <<DMA_CCxConfig_ITC_Pos);
        GPDMA_channel->DMACCControl &= ~(1ul <<DMA_CCxControl_I_Pos);

        //mask IE
        GPDMA_channel->DMACCConfig &= ~(1ul <<DMA_CCxConfig_IE_Pos);
    }
} //end of DMA_ITConfig

void (*dma_irq_finish[8])(void);
void (*dma_irq_error[8])(void);

void  DMA_IRQ_handler( void)
{
    //only call the attached function when certain interrupt happened on the right channel
    unsigned i = 0;
    for (i; i < 8; ++i)
    {
        if (DMA_ITStatus(i, ERR) && dma_irq_error[i])
            dma_irq_error[i]();
        else if (DMA_ITStatus(i, FINISH) && dma_irq_finish[i])
            dma_irq_finish[i]();
    }
    DMA_IRQ_handler0 (); // the DMA handler implemented by per platform
}

void DMA_IRQ_set (int channel, int status, void *ptr(void))
{
    assert (channel <= 8 && channel>=0);
    assert (status == ERR || status == FINISH);

    if (status == ERR)
        dma_irq_error[channel] = ptr;
    else if (status == FINISH)
        dma_irq_finish[channel] = ptr;
}
