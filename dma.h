#include "LPC1768_dma.h"
#include "mbed.h"


enum TriggerType;

class DMA
{
public:
    DMA_InitTypeDef dma_init_struct;
    LPC_GPDMACH_TypeDef* GPDMA_Channel;
    DMA(int prority); // constructor
    DMA ();
    void source (uint32_t src, int width, bool inc);
    void destination (uint32_t dst, int width, bool inc);
    void getTransferType (TransferType type);
    void start ( int length);
    bool wait();
    void trigger(TriggerType trig);

    void attach_TC(void (*fptr)(void)) {
        _TCCallback.attach(fptr);
        NVIC_SetVector(DMA_IRQn, (uint32_t)&IRQ_handler);
        NVIC_EnableIRQ(DMA_IRQn);
    }
    template<typename T> //has to use template in the header file
    void attach_TC(T* tptr, void (T::*mptr)(void)) {
        if((mptr != NULL) && (tptr != NULL)) {
            _TCCallback.attach(tptr, mptr);
        }
        NVIC_SetVector(DMA_IRQn, (uint32_t)&IRQ_handler);
        NVIC_EnableIRQ(DMA_IRQn);
    }

    void attach_Err(void (*fptr)(void)) {
        _ErrCallback.attach(fptr);
        NVIC_SetVector(DMA_IRQn, (uint32_t)&IRQ_handler);
        NVIC_EnableIRQ(DMA_IRQn);
    }
    template<typename T> //has to use template in the header file
    void attach_Err(T* tptr, void (T::*mptr)(void)) {
        if((mptr != NULL) && (tptr != NULL)) {
            _ErrCallback.attach(tptr, mptr);
        }
        NVIC_SetVector(DMA_IRQn, (uint32_t)&IRQ_handler);
        NVIC_EnableIRQ(DMA_IRQn);
    }


private:
    FunctionPointer _ErrCallback; // if declare as a function pointer, it will hang when attach
    FunctionPointer _TCCallback;
    int channel;
    static DMA* dma;
    TransferType type;
    int chooseFreeChannel(int channel);
    static void IRQ_handler(void); // use static because this method will be pased as a pointer, it has to be static. un-static method in Class will only have address in runtime.


};


#ifdef TARGET_LPC1768

enum TriggerType {
    ALWAYS = -1,
    SSP0_TX,
    SSP0_RX,
    SSP1_TX,
    SSP1_RX,
    ADC,
    I2S0,
    I2S1,
    DAC,
    UART0_TX,
    UART0_RX,
    UART1_TX,
    UART1_RX,
    UART2_TX,
    UART2_RX,
    UART3_TX,
    UART3_RX,
    MATCH0_0 = 24,
    MATCH0_1,
    MATCH1_0,
    MATCH1_1,
    MATCH2_0,
    MATCH2_1,
    MATCH3_0,
    MATCH3_1
};

#endif

//extern "C" void IRQ_handler(void);



/*
template <typename T>
void DMA::init (T src, T dst, TransferType type)
{
    dma_init_struct.DMA_DestAddr = (uint32_t)dst;
    dma_init_struct.DMA_SrcAddr = (uint32_t)src;
    //dma_init_struct.DMA_TransferSize = size;
    dma_init_struct.DMA_SrcBurst = 0x01; // 1 byte
    dma_init_struct.DMA_DestBurst = 0x01;
    dma_init_struct.DMA_SrcWidth = 0x00;//sizeof(*src); // caculate the width according to the input
    dma_init_struct.DMA_DestWidth = 0x00;//sizeof(*dst);
		{
		if (type==M2M || type==M2P)
				dma_init_struct.DMA_SrcInc = 1;
		else
			  dma_init_struct.DMA_SrcInc = 0;
		}
		{
		if (type==M2M || type==P2M)
r			 dma_init_struct.DMA_DestInc = 1;
		else
			 dma_init_struct.DMA_DestInc = 0;
		}


    dma_init_struct.DMA_TransferType = type;
}
*/
