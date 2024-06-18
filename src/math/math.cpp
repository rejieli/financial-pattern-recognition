#include "math.h"

namespace algomath {

    LineSegment CalcLineSegment(stock::Bar const& bar1, stock::Bar const& bar2) {
        // Line segment starts and finishes at average.
        LineSegment line_segment{};
        line_segment.start_period = bar1.period;
        line_segment.end_period = bar2.period;
        line_segment.start_val = bar1.low < bar2.high ? bar1.low : bar1.high;
        line_segment.end_val = bar1.low < bar2.high ? bar2.high : bar2.low;
        line_segment.slope = CalcSlope((float)line_segment.start_period, (float)line_segment.end_period, (float)line_segment.start_val, (float)line_segment.end_val);
        return line_segment;
    }

    bool IsSlopeIntersectBar(stock::Bar const& bar, LineSegment const& line_segment) {
        // Check x (period) direction
        if (IsBetween(bar.period, line_segment.start_period, line_segment.end_period)) {
            // Check y (scale) direction
            uint16_t diff = bar.period - line_segment.start_period;
            // Calculate the y (scale) value at index diff
            // y = mx + b 
            float val_at_diff = line_segment.slope * diff + line_segment.start_val;
            // Check if val_at_diff is between bar's high and low.
            return IsBetween(val_at_diff, bar.low, bar.high);
        }
        return false;
    }

    std::vector<LineSegment> IdentifyTrends(stock::StockMinuteDaily const& bars) {
        std::vector<LineSegment> lines;

        uint16_t start_pointer = 0;
        while (start_pointer < bars.bars.size() - 1) {
            for (uint16_t i = bars.bars.size() - 1; i > start_pointer; i--) {
                LineSegment line_segment = CalcLineSegment(bars.bars.at(start_pointer), bars.bars.at(i));
                std::array<bool, stock::NUM_PERIODS> intersections = algomath::SlopeIntersectBars<stock::NUM_PERIODS>(bars, line_segment);
                if (ArrayTruePercent(intersections, start_pointer, i) >= 80) {
                    lines.push_back(line_segment);
                    start_pointer = i;
                    break;
                }
            }
        }

        return lines;
    }

    std::vector<uint16_t> IdentifyMovAvgDirectionChange(DoubleArray const& mov_avg) {
        // Since mov_avg is not represented in a continous line, rather a discrete one, calculus is not able to be used. 
        // Rather this uses method uses a simple rate of change formula to distigusion directional change
        std::vector<uint16_t> direction_changes;
        direction_changes.push_back(0);
        double slope = 0;
        for (uint16_t i = 1; i < mov_avg.len; i++) {
            double new_slope = CalcSlope(mov_avg[i - 1], mov_avg[i], (double)i - 1, (double)i);
            if (new_slope < 0 != slope < 0) { // If direction has changed
                direction_changes.push_back(i);
            }
            slope = new_slope;
        }
        return direction_changes;
    }

    const uint16_t FindHighestBar(stock::StockMinuteDaily const& daily_chart, int const& start_period, int const& end_period) {
        assert(start_period < daily_chart.bars.size());
        assert(end_period < daily_chart.bars.size() && end_period >= start_period);

        uint16_t highest = start_period;

        for (int i = start_period + 1; i <= end_period; i++) {
            if (daily_chart.bars.at(i).high > daily_chart.bars.at(highest).high) {
                highest = i;
            }
        }

        return highest;

    }

    const uint16_t FindLowestBar(stock::StockMinuteDaily const& daily_chart, int const& start_period, int const& end_period) {
        assert(start_period < daily_chart.bars.size());
        assert(end_period < daily_chart.bars.size() && end_period >= start_period);

        uint16_t lowest = start_period;

        for (int i = start_period + 1; i <= end_period; i++) {
            if (daily_chart.bars.at(i).low < daily_chart.bars.at(lowest).low) {
                lowest = i;
            }
        }

        return lowest;

    }

    const std::vector<uint16_t> IntersectionsWithNonLinearLine(stock::StockMinuteDaily const& daily_chart, DoubleArray const& line) {
        assert(line.len == daily_chart.bars.size());
        std::vector<uint16_t> intersections;
        for (uint16_t i = 0; i < daily_chart.bars.size(); i++) {
            if (IsBetween(line[i], (double)daily_chart.bars.at(i).low, (double)daily_chart.bars.at(i).high)) {
                intersections.push_back(i);
            }
        }
        return intersections;
    }

    DoubleArray CalcMovAverage(stock::StockMinuteDaily const& daily_chart, uint16_t const& period) {
        stock::StockMinuteDailyArray daily_chart_arr = stock::StdStockObjToStockArrayObj(daily_chart);
        return ArrayMath(DoubleArray(daily_chart_arr.closeData.data(), stock::NUM_PERIODS)).movAvg(period);
    }

    std::vector<PeakAndTrough> IdentifyKeyPeaksAndTroughs(stock::StockMinuteDaily const& daily_chart) { //TODO change to index instead of pointers
        DoubleArray mov_avg = CalcMovAverage(daily_chart, 4); // Calculate moving average of 5
        std::vector<PeakAndTrough> key_bars;
        uint16_t start_period = 0;
        float slope = 0;
        for (uint16_t i = 1; i < daily_chart.bars.size(); i++) {
            // If direction of moving average changes
            // Since mov_avg is not represented in a continous line, rather a discrete one, calculus is not able to be used. 
            // Rather this uses method uses a simple rate of change formula to distigusion directional change
            float new_slope = CalcSlope(mov_avg[i - 1], mov_avg[i], (double)i - 1, (double)i);
            uint16_t period = i - start_period;
            if ((new_slope <= 0 != slope <= 0) && period > 2) { // If direction has changed for at least 2 periods (confirmation)
                std::cout << new_slope << std::endl;
                // Identify if the previous trend was up or down
                if (slope < 0) { // Down Trend
                    // Find minimum bar
                    const uint16_t minimum_bar = FindLowestBar(daily_chart, start_period, i);
                    PeakAndTrough trough = { .is_peak = false, .index = minimum_bar };
                    key_bars.push_back(trough);
                }
                else { // Up Trend
                    // Find maximum bar
                    const uint16_t maximum_bar = FindHighestBar(daily_chart, start_period, i);
                    PeakAndTrough peak = { .is_peak = true, .index = maximum_bar };
                    key_bars.push_back(peak);
                }
                start_period = i;
                slope = new_slope;
            }
        }

        return key_bars;
    }

    // Linear Regression
    Line LinearRegression(std::vector<double>& time, std::vector<double>& price) {
        double m; // Slope
        double b; // Y-intercept

        // Calculations
        double mean_x = mean(time);
        double mean_y = mean(price);
        double numerator = 0;
        double denominator = 0;
        for (size_t i = 0; i < time.size(); i++) {
            numerator += (time[i] - mean_x) * (price[i] - mean_y);
            denominator += (time[i] - mean_x) * (time[i] - mean_x);
        }
        m = numerator / denominator;
        b = mean_y - m * mean_x;

        return Line{ m, b };
    }

    void AdjustSlope(Line& const reference_line, Line& line_to_adjust, float pivot_x) {
        float pivot_y = LinearPosition(line_to_adjust, pivot_x);
        // Adjust slope
        line_to_adjust.slope = reference_line.slope;
        // Get new y intercept
        float new_y_int = pivot_y - (line_to_adjust.slope * pivot_x);
        line_to_adjust.y_int = new_y_int;
    }

}