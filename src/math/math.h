#pragma once

#ifndef MATH_H
#define MATH_H

#include "../numerics/stock.h"
#include <iostream>
#include <chartdir.h>
#include <FinanceChart.h>
#include <vector>
#include <cassert>

namespace algomath {

    using PeakAndTrough = stock::PeakAndTrough;

    enum Change {
        up = 0,
        nc,
        down
    };

    struct Line {
        double slope;
        double y_int;
    };

    struct LineSegment {
        int start_period;
        int end_period;
        double start_val;
        double end_val;
        double slope;
    };

    template<typename T>
    bool IsBetween(T const& val, T const& lower_bound, T const& upper_bound) {
        return val >= lower_bound && val <= upper_bound;
    }

    template<typename T>
    T CalcSlope(T const& x1, T const& x2, T const& y1, T const& y2) {
        return (y2 - y1) / (x2 - x1);
    }

    template<typename T>
    T CalcAverage(T const& x, T const& y) {
        return (x + y) / 2;
    }

    LineSegment CalcLineSegment(stock::Bar const& bar1, stock::Bar const& bar2);

    bool IsSlopeIntersectBar(stock::Bar const& bar, LineSegment const& line_segment);

    std::vector<LineSegment> IdentifyTrends(stock::StockMinuteDaily const& bars);

    std::vector<uint16_t> IdentifyMovAvgDirectionChange(DoubleArray const& mov_avg);

    const uint16_t FindHighestBar(stock::StockMinuteDaily const& daily_chart, int const& start_period, int const& end_period);

    const uint16_t FindLowestBar(stock::StockMinuteDaily const& daily_chart, int const& start_period, int const& end_period);

    const std::vector<uint16_t> IntersectionsWithNonLinearLine(stock::StockMinuteDaily const& daily_chart, DoubleArray const& line);

    DoubleArray CalcMovAverage(stock::StockMinuteDaily const& daily_chart, uint16_t const& period);

    std::vector<PeakAndTrough> IdentifyKeyPeaksAndTroughs(stock::StockMinuteDaily const& daily_chart);

    template<size_t size>
    std::array<bool, size> SlopeIntersectBars(stock::StockMinuteDaily const& bars, LineSegment const& line_segment) {
        std::array<bool, size> ret;
        for (int i = 0; i < size; i++) {
            ret.at(i) = IsSlopeIntersectBar(bars.bars.at(i), line_segment);
        }
        return ret;
    }

    template<size_t size>
    float ArrayTruePercent(std::array<bool, size> arr, uint16_t start, uint16_t end) {
        int num_true = 0;
        for (int i = start; i <= end; i++) {
            if (arr.at(i)) {
                num_true++;
            }
        }
        return ((float)num_true) * 100.0F / ((float)(end - start + 1));
    }

    inline double mean(const std::vector<double>& v) {
        double sum = 0;
        for (double x : v) sum += x;
        return sum / v.size();
    }

    Line LinearRegression(std::vector<double>& time, std::vector<double>& price);

    std::vector<uint16_t> RollingWindowPeakAlgorithm(stock::StockMinuteDaily const& bars, uint16_t order);

    std::vector<uint16_t> DirectionalChangeAlgorithm(stock::StockMinuteDaily const& bars);

    std::vector<uint16_t> PerceptuallyImportantPointsAlgorithm(stock::StockMinuteDaily const& bars);

    void AdjustSlope(Line& const reference_line, Line& line_to_adjust, float pivot_x);

    inline double LinearPosition(Line& const reference_line, float x) {
        return reference_line.slope * x + reference_line.y_int;
    }

    inline double PercentChange(double x1, double x2) {
        return (x2 - x1) / x1 * 100;
    }

}

#endif //MATH_H