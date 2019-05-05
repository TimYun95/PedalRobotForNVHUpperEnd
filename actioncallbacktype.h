#ifndef ACTIONCALLBACKTYPE_H
#define ACTIONCALLBACKTYPE_H

enum eActionCallbackType{
    Callback_UpdateGoHomeResult,
    Callback_UpdateStopWeld,
    Callback_UpdateOutControlStatus,
    Callback_UpdateInSwitchStatus,
    Callback_UpdateMessageInform,
    Callback_UpdateUnifiedInform,
    Callback_UpdateReInit,
    Callback_UpdateFinishAction,
    Callback_UpdateAtSuspendPoint,

    MaxActionCallbackType
};

#endif // ACTIONCALLBACKTYPE_H
