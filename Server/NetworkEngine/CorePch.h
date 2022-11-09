#pragma once

// 여기에 미리 컴파일하려는 헤더 추가
#include "framework.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>

#include <vector>
#include <queue>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <list>

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
#include "IoData.h"

#pragma comment(lib, "ws2_32.lib")

#define GETSET(field, type)\
type get##field() const { return field;}\
void set##field(type value) { field = value;}

#define GET(field)\
type get##field() const { return field;}

#define SET(field, type)\
void set##field(type value) { field = value;}