

// ca_common.h
typedef enum CADebugLevel_e {
	CADebugLevel_Info = 4,
	CADebugLevel_Warn = 3,
	CADebugLevel_Error = 2,
	CADebugLevel_Fatal = 1
}
CADebugLevel_t;

// ca_common.h
typedef enum CAMediaType_e {
	CAMediaType_None = 0x00,
	CAMediaType_Audio = (0x01 << 0),
	CAMediaType_Video = (0x01 << 1),
	CAMediaType_ScreenCast = (0x01 << 2),
	CAMediaType_AudioVideo = (CAMediaType_Audio | CAMediaType_Video),

	CAMediaType_All = 0xFF,
}
CAMediaType_t;

// ca_common.h
typedef enum CAIceState_e {
	CAIceState_None,
	CAIceState_Failed,
	CAIceState_GatheringDone,
	CAIceState_Connected,
	CAIceState_Teminated
}
CAIceState_t;

// ca_common.h
typedef enum CASessionType_e {
	CASessionType_None,
	CASessionType_Call
}
CASessionType_t;