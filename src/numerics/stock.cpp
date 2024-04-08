#include "stock.h"

namespace stock {
    StockMinuteDailyArray StdStockObjToStockArrayObj(StockMinuteDaily const& chart) {
        StockMinuteDailyArray arr;
        for (int i = 0; i < stock::NUM_PERIODS; ++i) {
            arr.timestamp[i] = i;
            arr.openData[i] = chart.bars[i].open;
            arr.lowData[i] = chart.bars[i].low;
            arr.highData[i] = chart.bars[i].high;
            arr.closeData[i] = chart.bars[i].close;
            arr.volData[i] = chart.bars[i].volume;
        }
        return arr;
    }
}