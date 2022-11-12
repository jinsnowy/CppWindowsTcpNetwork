#pragma once

// ���⿡ �̸� �������Ϸ��� ��� �߰�
#include "framework.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>

#include <algorithm>
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

#include "Stl.h"

using namespace std;

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>
#include <rpc.h>

#include "Singleton.h"
#include "Logger.h"
#include "Define.h"
#include "Common.h"
#include "IoEvent.h"
#include "MemoryPool.h"

#pragma comment(lib, "ws2_32.lib")

#define GETSET(field, type)\
type get##field() const { return field;}\
void set##field(type value) { field = value;}

#define GET(field)\
type get##field() const { return field;}

#define SET(field, type)\
void set##field(type value) { field = value;}