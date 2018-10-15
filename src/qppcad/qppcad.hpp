#ifndef QPP_QPPCAD
#define QPP_QPPCAD
#include <cstdint>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <chrono>
#include <ctime>
#include <set>
#include <algorithm>
#include <functional>
#include <tuple>
#include <unordered_set>
#include <data/compiler_fallback.hpp>
#include <io/strfun.hpp>
#include <qppcad/file_formats.hpp>

namespace qpp {

  namespace cad {

    typedef std::uint8_t  uint8_t;
    typedef std::uint16_t uint16_t;
    typedef std::uint32_t uint32_t;
    typedef std::uint64_t uint64_t;

    typedef std::int8_t  int8_t;
    typedef std::int16_t int16_t;
    typedef std::int32_t int32_t;
    typedef std::int64_t int64_t;

    enum ws_edit_type {
      EDIT_WS_ITEM = 0,
      EDIT_WS_ITEM_CONTENT = 1
    };

  }

}



#endif
