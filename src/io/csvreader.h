#pragma once

#include "../numerics/stock.h"
#include "../../external/fast-cpp-csv-parser/csv.h"
#include <string>


namespace csvreader{

    stock::StockMinuteDaily read_bars(std::string file_path);

}