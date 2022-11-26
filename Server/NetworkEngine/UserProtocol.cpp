#include "pch.h"
#include "UserProtocol.h"

DEFINE_PROTOCOL(UserProtocol::TEST, TEST);
DEFINE_PROTOCOL(UserProtocol::HEARTBEAT, HEATBEAT);
DEFINE_PROTOCOL(UserProtocol::LOGIN_REQUEST, LOGIN_REQ);
DEFINE_PROTOCOL(UserProtocol::LOGIN_RESPONSE, LOGIN_RES);
DEFINE_PROTOCOL(UserProtocol::ENTER_ROOM_REQUEST, ENTER_ROOM_REQ);
DEFINE_PROTOCOL(UserProtocol::ENTER_ROOM_RESPONSE, ENTER_ROOM_RES);