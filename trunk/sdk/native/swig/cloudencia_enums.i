

// ca_common.h
typedef enum CADebugLevel_e {
	CADebugLevel_Info = 4,
	CADebugLevel_Warn = 3,
	CADebugLevel_Error = 2,
	CADebugLevel_Fatal = 1
}
CADebugLevel_t;

// ca_common.h
typedef enum CASignalingEventType_e {
	CASignalingEventType_NetConnected,
	CASignalingEventType_NetReady,
	CASignalingEventType_NetData, // "passthrough" data: https://code.google.com/p/sincity/issues/detail?id=2
	CASignalingEventType_NetDisconnected,
	CASignalingEventType_NetError,

	CASignalingEventType_Call
}
CASignalingEventType_t;