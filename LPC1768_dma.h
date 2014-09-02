#ifndef LPC1768_DMA_H
#define LPC1768_DMA_H

/*
#ifdef __cplusplus
 extern "C" {
#endif
*/

#include "mbed.h"

/*DMA error interrupt register base address*/
#define DMA_IE                        ((uint32_t)0x00004000)
/*DMA terminal counter interrupt register base address*/
#define DMA_ITC                       ((uint32_t)0x00008000)

/*DMA channel number*/
#define channel_num 8

typedef enum {DISABLE = 0, ENABLE = !DISABLE} FunctionalState;
typedef enum {COUNTER, ERR} DMA_IT;

/*DMA transfer type*/
typedef enum {
    M2M = 0x00,
    M2P = 0x01,
    P2M = 0x02,
    P2P = 0x03
} TransferType;

/**
  * @brief  DMA Init structures definition
  */
typedef struct {
    /*!< Specifies the features set by channel control register */
    uint32_t DMA_DestAddr; /*!< Specifies the destination base address for DMAy Channelx. */
    uint32_t DMA_SrcAddr;     /*!< Specifies the source base address for DMAy Channelx. */
    uint32_t DMA_TransferSize;/*!< Specifies the source  transfer size    */
    uint32_t DMA_SrcBurst; /*!< Specifies the source  burst size    */
    uint32_t DMA_DestBurst; /*!< Specifies the destination burst size   */
    uint32_t DMA_SrcWidth; /*!< Specifies the source transfer width   */
    uint32_t DMA_DestWidth; /*!< Specifies the destination transfer width   */
    bool DMA_SrcInc;  /*!< Specifies whether the source is incremented or not */
    bool DMA_DestInc; /*!< Specifies whether the destination is incremented or not */
    bool DMA_TermInt; /*!< Specifies whether the terminal count interrupt enabled or not */

    /*!< Specifies the features set by channel config register */
    uint32_t  DMA_SrcPeripheral;
    uint32_t  DMA_DestPeripheral;
    TransferType  DMA_TransferType;
} DMA_InitTypeDef;


void DMA_init(LPC_GPDMACH_TypeDef* DMAy_Channelx, DMA_InitTypeDef* DMA_InitStruct);
void DMA_StructInit(DMA_InitTypeDef* DMA_InitStruct);
void DMA_Cmd(LPC_GPDMACH_TypeDef*  DMAy_Channelx, FunctionalState NewState);
void DMA_ITConfig (LPC_GPDMACH_TypeDef* DMAy_Channelx, uint32_t DMA_IT, FunctionalState NewState);
void DMA_ClearITPendingBit(LPC_GPDMACH_TypeDef* DMAy_Channelx, uint32_t DMA_IT);
bool DMA_ChannelActive (int channel);

/*
#ifdef __cplusplus
}
#endif
*/
#endif
