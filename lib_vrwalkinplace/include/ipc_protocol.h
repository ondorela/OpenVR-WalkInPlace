#pragma once

#include "vrwalkinplace_types.h"
#include <utility>


#define IPC_PROTOCOL_VERSION 1

namespace vrwalkinplace {
namespace ipc {


enum class RequestType : uint32_t {
	None,

	// IPC connection handling
	IPC_ClientConnect,
	IPC_ClientDisconnect,
	IPC_Ping,

	// These are indented to inject events into OpenVR and require an OpenVR device id.
	// These are "fire and forget".
	OpenVR_PoseUpdate,
	OpenVR_ButtonEvent,
	OpenVR_AxisEvent,
	OpenVR_ProximitySensorEvent,
	OpenVR_VendorSpecificEvent,

	// These are indented to manage virtual devices and require the internal device id.
	// The Reply is send to the client's message queue.
	VirtualDevices_AddDevice,
	VirtualDevices_PublishDevice,
	VirtualDevices_GetDeviceCount,
	VirtualDevices_GetDeviceInfo,
	VirtualDevices_GetDevicePose,
	VirtualDevices_GetControllerState,
	VirtualDevices_SetDeviceProperty,
	VirtualDevices_RemoveDeviceProperty,
	VirtualDevices_SetDevicePose,
	VirtualDevices_SetControllerState,

	DeviceManipulation_GetDeviceInfo,
	DeviceManipulation_ButtonMapping,
	DeviceManipulation_GetDeviceOffsets,
	DeviceManipulation_SetDeviceOffsets,
	DeviceManipulation_DefaultMode,
	DeviceManipulation_RedirectMode,
	DeviceManipulation_SwapMode,
	DeviceManipulation_MotionCompensationMode,
	DeviceManipulation_StepDetectionMode,
	DeviceManipulation_FakeDisconnectedMode,
	DeviceManipulation_TriggerHapticPulse,
	DeviceManipulation_SetMotionCompensationProperties
};


enum class ReplyType : uint32_t {
	None,
	
	IPC_ClientConnect,
	IPC_Ping,

	GenericReply,

	VirtualDevices_GetDeviceCount,
	VirtualDevices_GetDeviceInfo,
	VirtualDevices_GetDevicePose,
	VirtualDevices_GetControllerState,
	VirtualDevices_AddDevice,

	DeviceManipulation_GetDeviceInfo,
	DeviceManipulation_GetDeviceOffsets
};


enum class ReplyStatus : uint32_t {
	None,
	Ok,
	UnknownError,
	InvalidId,
	AlreadyInUse,
	InvalidType,
	NotFound,
	TooManyDevices,
	InvalidVersion,
	MissingProperty,
	InvalidOperation
};


struct Request_IPC_ClientConnect {
	uint32_t messageId;
	uint32_t ipcProcotolVersion;
	char queueName[128];
};


struct Request_IPC_ClientDisconnect {
	uint32_t clientId;
	uint32_t messageId;
};


struct Request_IPC_Ping {
	uint32_t clientId;
	uint32_t messageId;
	uint64_t nonce;
};


struct Request_OpenVR_PoseUpdate {
	uint32_t deviceId;
	vr::DriverPose_t pose;
};


#define REQUEST_OPENVR_BUTTONEVENT_MAXCOUNT 12

struct Request_OpenVR_ButtonEvent {
	unsigned eventCount;
	struct {
		ButtonEventType eventType;
		uint32_t deviceId;
		vr::EVRButtonId buttonId;
		double timeOffset;
	} events[REQUEST_OPENVR_BUTTONEVENT_MAXCOUNT];
};


#define REQUEST_OPENVR_AXISEVENT_MAXCOUNT 12

struct Request_OpenVR_AxisEvent {
	unsigned eventCount;
	struct {
		uint32_t deviceId;
		uint32_t axisId;
		vr::VRControllerAxis_t axisState;
	} events[REQUEST_OPENVR_AXISEVENT_MAXCOUNT];
};


struct Request_OpenVR_ProximitySensorEvent {
	uint32_t deviceId;
	bool sensorTriggered;
};


struct Request_OpenVR_VendorSpecificEvent {
	uint32_t deviceId;
	vr::EVREventType eventType;
	vr::VREvent_Data_t eventData;
	double timeOffset;
};


struct Request_VirtualDevices_GenericClientMessage {
	uint32_t clientId;
	uint32_t messageId; // Used to associate with Reply
};


struct Request_VirtualDevices_GenericDeviceIdMessage {
	uint32_t clientId;
	uint32_t messageId; // Used to associate with Reply
	uint32_t deviceId;
};


struct Request_VirtualDevices_AddDevice {
	uint32_t clientId;
	uint32_t messageId; // Used to associate with Reply
	VirtualDeviceType deviceType;
	char deviceSerial[256];
};


struct Request_VirtualDevices_SetDeviceProperty {
	uint32_t clientId;
	uint32_t messageId; // Used to associate with Reply
	uint32_t virtualDeviceId;
	vr::ETrackedDeviceProperty deviceProperty;
	DevicePropertyValueType valueType;
	union {
		int32_t int32Value;
		uint64_t uint64Value;
		float floatValue; 
		bool boolValue; 
		char stringValue[256];
		vr::HmdMatrix34_t matrix34Value;
		vr::HmdMatrix44_t matrix44Value;
		vr::HmdVector3_t vector3Value;
		vr::HmdVector4_t vector4Value;
	} value;
};

struct Request_VirtualDevices_RemoveDeviceProperty {
	uint32_t clientId;
	uint32_t messageId; // Used to associate with Reply
	uint32_t virtualDeviceId;
	vr::ETrackedDeviceProperty deviceProperty;
};

struct Request_VirtualDevices_SetDevicePose {
	uint32_t clientId;
	uint32_t messageId; // Used to associate with Reply
	uint32_t virtualDeviceId;
	vr::DriverPose_t pose;
};

struct Request_VirtualDevices_SetControllerState {
	uint32_t clientId;
	uint32_t messageId; // Used to associate with Reply
	uint32_t virtualDeviceId;
	vr::VRControllerState_t controllerState;
};

struct Request_DeviceManipulation_ButtonMapping {
	uint32_t clientId;
	uint32_t messageId; // Used to associate with Reply
	uint32_t deviceId;
	uint32_t enableMapping; // 0 .. don't change, 1 .. enable, 2 .. disable
	uint32_t mappingOperation; // 0 .. do nothing, 1 .. add given mappings, 2 .. remove given mappings, 3 .. remove all mappings
	uint32_t mappingCount;
	vr::EVRButtonId buttonMappings[32];
};

struct Request_DeviceManipulation_SetDeviceOffsets {
	uint32_t clientId;
	uint32_t messageId; // Used to associate with Reply
	uint32_t deviceId;
	uint32_t enableOffsets; // 0 .. don't change, 1 .. enable, 2 .. disable
	uint32_t offsetOperation; // 0 .. set, 1 .. add
	// world-from-driver universe offsets
	bool worldFromDriverRotationOffsetValid;
	vr::HmdQuaternion_t worldFromDriverRotationOffset;
	bool worldFromDriverTranslationOffsetValid;
	vr::HmdVector3d_t worldFromDriverTranslationOffset;
	// driver-from-head universe offsets
	bool driverFromHeadRotationOffsetValid;
	vr::HmdQuaternion_t driverFromHeadRotationOffset;
	bool driverFromHeadTranslationOffsetValid;
	vr::HmdVector3d_t driverFromHeadTranslationOffset;
	// device offsets
	bool deviceRotationOffsetValid;
	vr::HmdQuaternion_t deviceRotationOffset;
	bool deviceTranslationOffsetValid;
	vr::HmdVector3d_t deviceTranslationOffset;
};

struct Request_DeviceManipulation_RedirectMode {
	uint32_t clientId;
	uint32_t messageId; // Used to associate with Reply
	uint32_t deviceId;
	uint32_t targetId;
};

struct Request_DeviceManipulation_SwapMode {
	uint32_t clientId;
	uint32_t messageId; // Used to associate with Reply
	uint32_t deviceId;
	uint32_t targetId;
};

struct Request_DeviceManipulation_MotionCompensationMode {
	uint32_t clientId;
	uint32_t messageId; // Used to associate with Reply
	uint32_t deviceId;
	uint32_t velAccCompensationMode;
};

struct Request_DeviceManipulation_StepDetectionMode {
	uint32_t clientId;
	uint32_t messageId; // Used to associate with Reply
	uint32_t deviceId;
	uint32_t enableStepDetect;
	uint32_t useEulerForStep;
	uint32_t stepDetectOperation;
	vr::HmdVector3d_t hmdThreshold;
	vr::HmdVector3d_t handThreshold;
	float stepAcceleration;
	float stepSpeed;
	float stepIntSec;
};

struct Request_DeviceManipulation_TriggerHapticPulse {
	uint32_t clientId;
	uint32_t messageId; // Used to associate with Reply
	uint32_t deviceId;
	uint32_t axisId;
	uint16_t durationMicroseconds;
	bool directMode;
};

struct Request_DeviceManipulation_SetMotionCompensationProperties {
	uint32_t clientId;
	uint32_t messageId; // Used to associate with Reply
	uint32_t velAccCompensationMode;
};


struct Request {
	Request() {}
	Request(RequestType type) : type(type) {
		timestamp = std::chrono::duration_cast <std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	}
	Request(RequestType type, uint64_t timestamp) : type(type), timestamp(timestamp) {}

	void refreshTimestamp() {
		timestamp = std::chrono::duration_cast <std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	}

	RequestType type = RequestType::None;
	int64_t timestamp = 0; // milliseconds since epoch
	union {
		Request_IPC_ClientConnect ipc_ClientConnect;
		Request_IPC_ClientDisconnect ipc_ClientDisconnect;
		Request_IPC_Ping ipc_Ping;
		Request_OpenVR_PoseUpdate ipc_PoseUpdate;
		Request_OpenVR_ButtonEvent ipc_ButtonEvent;
		Request_OpenVR_AxisEvent ipc_AxisEvent;
		Request_OpenVR_ProximitySensorEvent ovr_ProximitySensorEvent;
		Request_OpenVR_VendorSpecificEvent ovr_VendorSpecificEvent;
		Request_VirtualDevices_GenericClientMessage vd_GenericClientMessage;
		Request_VirtualDevices_GenericDeviceIdMessage vd_GenericDeviceIdMessage;
		Request_VirtualDevices_AddDevice vd_AddDevice;
		Request_VirtualDevices_SetDeviceProperty vd_SetDeviceProperty;
		Request_VirtualDevices_RemoveDeviceProperty vd_RemoveDeviceProperty;
		Request_VirtualDevices_SetDevicePose vd_SetDevicePose;
		Request_VirtualDevices_SetControllerState vd_SetControllerState;
		Request_DeviceManipulation_ButtonMapping dm_ButtonMapping;
		Request_DeviceManipulation_SetDeviceOffsets dm_DeviceOffsets;
		Request_DeviceManipulation_RedirectMode dm_RedirectMode;
		Request_DeviceManipulation_SwapMode dm_SwapMode;
		Request_DeviceManipulation_MotionCompensationMode dm_MotionCompensationMode;
		Request_DeviceManipulation_StepDetectionMode dm_StepDetectionMode;
		Request_DeviceManipulation_TriggerHapticPulse dm_triggerHapticPulse;
		Request_DeviceManipulation_SetMotionCompensationProperties dm_SetMotionCompensationProperties;
	} msg;
};



struct Reply_IPC_ClientConnect {
	uint32_t clientId;
	uint32_t ipcProcotolVersion;
};

struct Reply_IPC_Ping {
	uint64_t nonce;
};

struct Reply_VirtualDevices_GetDeviceCount {
	uint32_t deviceCount;
};

struct Reply_VirtualDevices_GetDeviceInfo {
	uint32_t virtualDeviceId;
	uint32_t openvrDeviceId;
	VirtualDeviceType deviceType;
	char deviceSerial[128];
};

struct Reply_VirtualDevices_GetDevicePose {
	vr::DriverPose_t pose;
};

struct Reply_VirtualDevices_GetControllerState {
	vr::VRControllerState_t controllerState;
};

struct Reply_VirtualDevices_AddDevice {
	uint32_t virtualDeviceId;
};


struct Reply_DeviceManipulation_GetDeviceInfo {
	uint32_t deviceId;
	vr::ETrackedDeviceClass deviceClass;
	int deviceMode;
	bool offsetsEnabled;
	bool buttonMappingEnabled;
	bool redirectSuspended;
};



struct Reply_DeviceManipulation_GetDeviceOffsets {
	uint32_t deviceId;
	bool offsetsEnabled;
	vr::HmdQuaternion_t worldFromDriverRotationOffset;
	vr::HmdVector3d_t worldFromDriverTranslationOffset;
	vr::HmdQuaternion_t driverFromHeadRotationOffset;
	vr::HmdVector3d_t driverFromHeadTranslationOffset;
	vr::HmdQuaternion_t deviceRotationOffset;
	vr::HmdVector3d_t deviceTranslationOffset;
};


struct Reply {
	Reply() {}
	Reply(ReplyType type) : type(type) {
		timestamp = std::chrono::duration_cast <std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	}
	Reply(ReplyType type, uint64_t timestamp) : type(type), timestamp(timestamp) {}

	ReplyType type = ReplyType::None;
	uint64_t timestamp = 0; // milliseconds since epoch
	uint32_t messageId;
	ReplyStatus status;
	union {
		Reply_IPC_ClientConnect ipc_ClientConnect;
		Reply_IPC_Ping ipc_Ping;
		Reply_VirtualDevices_GetDeviceCount vd_GetDeviceCount;
		Reply_VirtualDevices_GetDeviceInfo vd_GetDeviceInfo;
		Reply_VirtualDevices_GetDevicePose vd_GetDevicePose;
		Reply_VirtualDevices_GetControllerState vd_GetControllerState;
		Reply_VirtualDevices_AddDevice vd_AddDevice;
		Reply_DeviceManipulation_GetDeviceInfo dm_deviceInfo;
		Reply_DeviceManipulation_GetDeviceOffsets dm_deviceOffsets;
	} msg;
};


} // end namespace ipc
} // end namespace vrwalkinplace