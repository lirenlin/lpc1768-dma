
#include "dma.h"

namespace mbed {


DMA::DMA ( int priority )
{
    chan = chooseFreeChannel(priority) ;
    channel_num = _channel_num;
    DMA_reset(chan,&dma_init_struct);// reset the register value to default and clear the interrupts
    DMA_StructInit(&dma_init_struct); // initialize the dma data structure to default value
}


// if no channel choosed, choose whichever free channel, otherwise, wait until the channel is free
int DMA::chooseFreeChannel (int channel)
{
    int reval=channel;
    if (channel > (channel_num-1) || channel < 0) {
        reval = 0;
        while (1) { //if not chosen channel, round robin checked which channel is free
            if (!DMA_ChannelActive(reval))
                return reval;
            reval++;
            if (reval>(channel_num-1))
                reval=0;
        }
    } else
        while (DMA_ChannelActive(reval));//if has chosen the channel, wait until the channel is free
    return reval;
}

void DMA::TriggerSource(TriggerType trig)
{
    DMA_TriggerSource(&dma_init_struct, trig);
}

void DMA::TriggerDestination(TriggerType trig)
{
    DMA_TriggerDestination(&dma_init_struct, trig);
}

void DMA::start ( int length)
{
    DMA_TransferSize(&dma_init_struct, length);
    DMA_init(chan, &dma_init_struct);
    DMA_Cmd(chan, ENABLE); // enable channel
    DMA_ITConfig (chan, ENABLE); // enable interrupt
}

void DMA::wait()
{
    while (DMA_ChannelActive(chan));
}
}

