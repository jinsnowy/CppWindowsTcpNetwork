#pragma once

// 여기에 미리 컴파일하려는 헤더 추가
#include "framework.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>

#include <algorithm>
#include <bitset>
#include <mutex>
#include <memory>
#include <functional>
#include <thread>
#include <future>
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <type_traits>
#include <chrono>
#include <random>

#include "Stl.h"

using namespace std;

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>
#include <rpc.h>

#include "TlsMemory.h"
#include "Singleton.h"
#include "Logger.h"
#include "Define.h"
#include "Common.h"
#include "IoEvent.h"
#include "MemoryPool.h"
#include "Task.h"
#include "TaskAsync.h"
#include "Alarm.h"

#define OUT

#pragma comment(lib, "ws2_32.lib")
#ifdef _DEBUG
#pragma comment(lib, "libprotobufd.lib")
#else
#pragma comment(lib, "libprotobuf.lib")
#endif