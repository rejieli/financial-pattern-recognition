#include "chart.h"
#include <vector>
#include <chartdir.h>
#include "../numerics/stock.h"
#include <FinanceChart.h>
#include "../math/math.h"
#include "../numerics/stock.h"
#include "../patterns/patterns.h"
#include "iostream"

namespace chart{

    void DrawBarChart(stock::StockMinuteDaily const& daily_chart, std::vector<algomath::LineSegment> const& trends) {

        stock::StockMinuteDailyArray daily_chart_arr = stock::StdStockObjToStockArrayObj(daily_chart);
        FinanceChart c(3000);

        // Add a title to the chart
        c.addTitle("Daily OHLC Chart with Volume");

        //// Add the main chart with 240 pixels in height
        XYChart* mainChart =  c.addMainChart(720);

        //// Add an HLOC layer to the main chart, using green/red for up/down days
        c.setData(DoubleArray(daily_chart_arr.timestamp.data(), stock::NUM_PERIODS), DoubleArray(daily_chart_arr.highData.data(), stock::NUM_PERIODS), DoubleArray(daily_chart_arr.lowData.data(), stock::NUM_PERIODS),
            DoubleArray(daily_chart_arr.openData.data(), stock::NUM_PERIODS), DoubleArray(daily_chart_arr.closeData.data(), stock::NUM_PERIODS), DoubleArray(daily_chart_arr.volData.data(), daily_chart_arr.volData.size()), 0);
        c.addSimpleMovingAvg(8, 0xA020F0);
        c.addSimpleMovingAvg(4, 0x20F0F0);
        c.addCandleStick(0x00ff00, 0xff0000);
        c.addVolBars(150, 0x00ff00f, 0xff0000f, 0x8080ff);

        
        c.addRSI(320, 10, 0xA020F0, 4.0f, 0xff0000f, 0x8080ff);


        DrawPeakAndTrough(mainChart, daily_chart);
        DrawTrendLines(c, trends);
        DrawAscendingChannel(mainChart, daily_chart, algomath::IdentifyKeyPeaksAndTroughs(daily_chart));
        DoubleArray arr = ArrayMath(DoubleArray(daily_chart_arr.closeData.data(), stock::NUM_PERIODS)).movAvg(12);

        //// Output the chart
        c.makeChart("financial_chart.png");
        
    }

    void DrawTrendLines(FinanceChart& c, std::vector<algomath::LineSegment> const& trends) {
        BaseChart* baseChart = c.getChart();
        // Cast it to an XYChart pointer
        XYChart* xyChart = dynamic_cast<XYChart*>(baseChart);
        for (int i = 0; i < trends.size(); i++) {
            LineLayer* lineLayer = xyChart->addLineLayer();
            lineLayer->addDataSet(DoubleArray(new double[2] {trends.at(i).start_val, trends.at(i).end_val}, 2), 0xff0000);
            lineLayer->setXData(DoubleArray(new double[2] {(double)trends.at(i).start_period, (double)trends.at(i).end_period}, 2));
        }
    }

    void DrawPeakAndTrough(XYChart* mainChart, stock::StockMinuteDaily const& daily_chart) {
        std::vector<stock::PeakAndTrough> peak_and_trough = algomath::IdentifyKeyPeaksAndTroughs(daily_chart);
        
        double* buySignal = new double[daily_chart.bars.size()];
        double* sellSignal = new double[daily_chart.bars.size()];
        for (int i = 0; i < daily_chart.bars.size(); ++i) {
            buySignal[i] = Chart::NoValue;
            sellSignal[i] = Chart::NoValue;
        }

        for (int i = 0; i < peak_and_trough.size(); i++) {
            if (peak_and_trough.at(i).is_peak) {
                const uint16_t index = peak_and_trough.at(i).index;
                sellSignal[index] = daily_chart.bars.at(index).high;
            }
            else {
                const uint16_t index = peak_and_trough.at(i).index;
                buySignal[index] = daily_chart.bars.at(index).low;
            }
        }

        ScatterLayer* buyLayer = mainChart->addScatterLayer(DoubleArray(), DoubleArray(buySignal,
            daily_chart.bars.size()), "Buy", Chart::ArrowShape(0, 1, 0.4, 0.4), 11, 0x00ffff);
        // Shift the symbol lower by 20 pixels
        buyLayer->getDataSet(0)->setSymbolOffset(0, 20);

        ScatterLayer* sellLayer = mainChart->addScatterLayer(DoubleArray(), DoubleArray(sellSignal,
            daily_chart.bars.size()), "Sell", Chart::ArrowShape(180, 1, 0.4, 0.4), 11, 0x9900cc);
        // Shift the symbol lower by 20 pixels
        sellLayer->getDataSet(0)->setSymbolOffset(0, -20);

    }

    void DrawAscendingChannel(XYChart* mainChart, stock::StockMinuteDaily const& daily_chart, std::vector<stock::PeakAndTrough> const& peak_and_trough) {
        std::vector<patterns::AscendingChannel> ascendingChannels = patterns::IdentifyAscendingChannel(daily_chart, peak_and_trough);
        for (int i = 0; i < ascendingChannels.size(); i++) {
           
            uint16_t size = ascendingChannels.at(i).top_points.index.size();
            double start_pos = ascendingChannels.at(i).top_points.index.at(0) - 20;
            double end_pos = ascendingChannels.at(i).top_points.index.at(size-1) + 20;

            LineLayer* lineLayer = mainChart->addLineLayer();
            lineLayer->addDataSet(DoubleArray(new double[2] {algomath::LinearPosition(ascendingChannels.at(i).top_line, start_pos), algomath::LinearPosition(ascendingChannels.at(i).top_line, end_pos)}, 2), 0x000000);
            lineLayer->setXData(DoubleArray(new double[2] {(double)start_pos, (double)end_pos}, 2));
            lineLayer->setLineWidth(2);

            uint16_t size_bottom = ascendingChannels.at(i).bottom_points.index.size();
            double start_pos_bottom = ascendingChannels.at(i).bottom_points.index.at(0) - 20;
            double end_pos_bottom = ascendingChannels.at(i).bottom_points.index.at(size_bottom - 1) + 20;

            LineLayer* lineLayer_bottom = mainChart->addLineLayer();
            lineLayer_bottom->addDataSet(DoubleArray(new double[2] {algomath::LinearPosition(ascendingChannels.at(i).bottom_line, start_pos_bottom), algomath::LinearPosition(ascendingChannels.at(i).bottom_line, end_pos_bottom)}, 2), 0x000000);
            lineLayer_bottom->setXData(DoubleArray(new double[2] {(double)start_pos_bottom, (double)end_pos_bottom}, 2));
            lineLayer_bottom->setLineWidth(2);
        }

        
    }

}