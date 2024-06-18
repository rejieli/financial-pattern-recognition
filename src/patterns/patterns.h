
#include "../math/math.h"

enum Patterns {
    Pennant,
    Megaphone,
    Flag,
    AscendingChannel,
    DescendingChannel,
    SymmetricalTriangle,
    DescendingTriangle,
    AscendingTriangle,
    Diamond,
    LivermoreCylinder,
    DoubleTop,
    DoubleBottom,
    HeadAndShoulders,
    CupAndHandle,
    DescendingWedge,
    AscendingWedge,
    Gartley,
    Cypher
};

namespace patterns {


    struct AscendingChannel {
        stock::IndexAndPriceVector top_points;
        stock::IndexAndPriceVector bottom_points;
        algomath::Line top_line; // Regression line
        algomath::Line bottom_line; // Regression line
    };

    struct DoubleTop {
        uint16_t top_1_index;
        uint16_t top_2_index;
        algomath::Line neck_line;
    };

    std::vector<AscendingChannel>  IdentifyAscendingChannel(stock::StockMinuteDaily const& daily_chart, std::vector<stock::PeakAndTrough> const& peak_and_trough);
    
}