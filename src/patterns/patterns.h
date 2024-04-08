
#include "../math/math.h"


namespace patterns {
    //enum Patterns {
    //    Pennant,
    //    Megaphone,
    //    Flag,
    //    AscendingChannel,
    //    DescendingChannel,
    //    SymmetricalTriangle,
    //    DescendingTriangle,
    //    AscendingTriangle,
    //    Diamond,
    //    LivermoreCylinder,
    //    DoubleTop,
    //    DoubleBottom,
    //    HeadAndShoulders,
    //    CupAndHandle,
    //    DescendingWedge,
    //    AscendingWedge,
    //    Gartley,
    //    Cypher
    //};

    //struct Pattern {
    //    Patterns type;

    //};

    struct AscendingChannel {
        stock::IndexAndPriceVector top_points;
        stock::IndexAndPriceVector bottom_points;
        algomath::Line top_line; // Regression line
        algomath::Line bottom_line; // Regression line
    };

    std::vector<AscendingChannel>  IdentifyAscendingChannel(stock::StockMinuteDaily const& daily_chart, std::vector<stock::PeakAndTrough> const& peak_and_trough);
    
}

// To Idenfify Patterns

