#pragma once

#ifndef STOCK_H
#define STOCK_H

#include <array>
#include <vector>

namespace stock {

    constexpr uint16_t NUM_PERIODS = 390;


    struct Bar {
        int period;
        float open;
        float low;
        float high;
        float close;
        int volume;
    };

    struct IndexAndPriceVector {
        std::vector<double> index;
        std::vector<double> price;

        void clear() {
            index.clear();
            price.clear();
        }

    };

    struct IndexAndPrice {
        uint16_t index;
        double price;
    };

    struct StockMinuteDaily{
        std::array<Bar, NUM_PERIODS> bars;
    };

    struct StockMinuteDailyArray {
        std::array<double, stock::NUM_PERIODS> timestamp;
        std::array<double, stock::NUM_PERIODS> highData;
        std::array<double, stock::NUM_PERIODS> lowData;
        std::array<double, stock::NUM_PERIODS> openData;
        std::array<double, stock::NUM_PERIODS> closeData;
        std::array<double, stock::NUM_PERIODS > volData;
    };

    struct PeakAndTrough {
        bool is_peak;
        uint16_t index;
    };

    StockMinuteDailyArray StdStockObjToStockArrayObj(StockMinuteDaily const& chart);
}

#endif //STOCK_H