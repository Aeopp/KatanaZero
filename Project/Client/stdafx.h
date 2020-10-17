// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일:
#include <windows.h>

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <cassert>
#include <forward_list>
#include <deque>
#include <set>
#include <map>
#include <queue>
#include <list>
#include <vector>
#include <numeric>
#include <algorithm>
#include <functional>
#include <type_traits>
#include <fstream>
#include <istream>
#include <ostream>
#include <string>
#include <memory>
#include <utility>
#include <mutex>
#include <thread>
#include <future>
#include <utility>
#include <tuple>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <unordered_set>

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
#include <d3d9.h>
#pragma comment(lib, "d3d9.lib")

#include <d3dx9.h>
#pragma comment(lib, "d3dx9.lib")

#include "Function.h"
#include "Typedef.h"
#include "global.h"
#include "Struct.h"

using namespace std::string_view_literals;
using namespace std::chrono_literals;
using namespace std::string_literals;
