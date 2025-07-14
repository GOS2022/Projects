#include "gos.h"

typedef enum
{
    APP_STATE_UNINITIALIZED,
    APP_STATE_CONFIGURATION,
    APP_STATE_CONNECTING,
    APP_STATE_CONNECTED,
    APP_STATE_CONNECTION_LOST
}app_state_t;

gos_result_t app_init (void_t);

void_t app_setStateFeedBack (app_state_t requiredState);