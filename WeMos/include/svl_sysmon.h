extern "C"{
#include <gos.h>
}

gos_result_t svl_sysmonInit (void_t);
gos_result_t svl_sysmonSendReceiveMessage (u16_t txMessageId, void_t* pTxPayload, u16_t txPayloadSize,
                                           u16_t* pRxMessageId, void_t* pRxPayload, u16_t* rxPayloadSize);