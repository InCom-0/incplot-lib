#include <incplot/plot_structures.hpp>


namespace incom {
namespace terminal_plot {
namespace plot_structures {

using incerr_c = incerr::incerr_code;
using enum Unexp_plotSpecs;
using enum Unexp_plotDrawer;

// BASE

size_t Base::compute_lengthOfSelf() const {

    size_t lngth = pad_top + pad_bottom;

    // Horizontal top axis name and axis labels lines
    lngth += axisName_horTop_bool ? (axisName_horTop.size() + corner_topLeft.front().size() +
                                     corner_topRight.front().size() + pad_left + pad_right)
                                  : 0;
    for (size_t i = 0; i < labels_horTop.size(); ++i) {
        lngth += (labels_horTop.at(i).size() + corner_topLeft.at(i).size() + corner_topRight.at(i).size());
    }

    // First and last vertical labels + padding + vert axes names if present
    lngth += labels_verLeft.front().size() + labels_verRight.front().size() + labels_verLeft.back().size() +
             labels_verRight.back().size();

    // The 'area corner symbols'
    lngth += Config::areaCorner_tl.size() + Config::areaCorner_tr.size() + Config::areaCorner_bl.size() +
             Config::areaCorner_br.size();

    // All top and bottom axes
    for (int i = 0; i < areaWidth; i++) { lngth += (axis_horTop.at(i).size() + axis_horBottom.at(i).size()); }

    // Main plot area
    for (int i = 0; i < areaHeight; ++i) {
        lngth += labels_verLeft.at(i + 1).size() + labels_verRight.at(i + 1).size();
        lngth += axis_verLeft.at(i).size();
        lngth += axis_verRight.at(i).size();
        lngth += plotArea.at(i).size();
    }

    lngth += ((areaHeight + 2) * (pad_left + pad_right + (Config::axis_verName_width_vl * (axisName_verLeft_bool)) +
                                  (Config::axis_verName_width_vr * (axisName_verRight_bool))));

    // Horizontal bottom axis name and axis labels lines
    for (size_t i = 0; i < labels_horBottom.size(); ++i) {
        lngth += (labels_horBottom.at(i).size() + corner_bottomLeft.at(i).size() + corner_bottomRight.at(i).size());
    }
    lngth += labels_horBottom.size() * (pad_left + pad_right);

    lngth += axisName_horBottom_bool ? (axisName_horBottom.size() + corner_bottomLeft.back().size() +
                                        corner_bottomRight.back().size() + pad_left + pad_right)
                                     : 0;
    return lngth;
}
std::string Base::build_plotAsString() const {
    std::string result;
    result.reserve(compute_lengthOfSelf());

    // Add padding on top
    for (int i = 0; i < pad_top; ++i) { result.push_back('\n'); }

    // Build the heading lines of the plot
    if (axisName_horTop_bool) {
        result.append(std::string(pad_left, Config::space));
        result.append(corner_topLeft.front());
        result.append(axisName_horTop);
        result.append(corner_topRight.front());
        result.append(std::string(pad_right, Config::space));
        result.push_back('\n');
    }
    for (size_t i = 0; auto const &oneLabelLine : labels_horTop) {
        result.append(std::string(pad_left, Config::space));
        result.append(corner_topLeft.at(i));
        result.append(oneLabelLine);
        result.append(corner_topRight.at(i++));
        result.append(std::string(pad_right, Config::space));
        result.push_back('\n');
    }

    // Build horizontal top axis line
    result.append(std::string(pad_left + (Config::axis_verName_width_vl * axisName_verLeft_bool), Config::space));
    result.append(labels_verLeft.front());
    result.append(Config::color_Axes);
    result.append(areaCorner_tl);
    for (auto const &toAppend : axis_horTop) { result.append(toAppend); }
    result.append(Config::color_Axes);
    result.append(areaCorner_tr);
    result.append(Config::term_setDefault);
    result.append(labels_verRight.front());
    result.append(std::string(pad_right + (Config::axis_verName_width_vr * axisName_verRight_bool), Config::space));
    result.push_back('\n');

    // Add plot area lines
    for (long long i = 0; i < areaHeight; ++i) {
        result.append(std::string(pad_left, Config::space));
        if (axisName_verLeft_bool) { result.push_back(axisName_verLeft.at(i)); }
        result.append(std::string((Config::axis_verName_width_vl - 1) * axisName_verLeft_bool, Config::space));
        result.append(labels_verLeft.at(i + 1));
        result.append(axis_verLeft.at(i));
        result.append(plotArea.at(i));
        result.append(axis_verRight.at(i));
        result.append(labels_verRight.at(i + 1));
        result.append(std::string((Config::axis_verName_width_vr - 1) * axisName_verRight_bool, Config::space));
        if (axisName_verRight_bool) { result.push_back(axisName_verRight.at(i)); }
        result.append(std::string(pad_right, Config::space));
        result.push_back('\n');
    }

    // Add horizontal bottom axis line
    result.append(std::string(pad_left + (Config::axis_verName_width_vl * axisName_verLeft_bool), Config::space));
    result.append(labels_verLeft.back());
    result.append(Config::color_Axes);
    result.append(areaCorner_bl);
    for (auto const &toAppend : axis_horBottom) { result.append(toAppend); }
    result.append(Config::color_Axes);
    result.append(areaCorner_br);
    result.append(Config::term_setDefault);
    result.append(labels_verRight.back());
    result.append(std::string(pad_right + (Config::axis_verName_width_vr * axisName_verRight_bool), Config::space));
    result.push_back('\n');

    // Add the bottom lines of the plot

    for (size_t i = 0; auto const &oneLabelLine : labels_horBottom) {
        result.append(std::string(pad_left, Config::space));
        result.append(corner_bottomLeft.at(i));
        result.append(oneLabelLine);
        result.append(corner_bottomRight.at(i++));
        result.append(std::string(pad_right, Config::space));
        result.push_back('\n');
    }

    if (axisName_horBottom_bool) {
        result.append(std::string(pad_left, Config::space));
        result.append(corner_bottomLeft.back());
        result.append(axisName_horBottom);
        result.append(corner_bottomRight.back());
        result.append(std::string(pad_right, Config::space));
        result.push_back('\n');
    }

    result.append(footer);

    // Add padding on bottom
    for (int i = 0; i < pad_bottom; ++i) { result.push_back('\n'); }
    return result;
}
// ### END BASE ###

// BAR V

guess_retType BarV::guess_sizes(guess_firstParamType &&dp, DataStore const &ds) {
    return std::unexpected(incerr_c::make(TEST_t1));
}
guess_retType BarV::guess_plotType(guess_firstParamType &&dp, DataStore const &ds) {
    return std::unexpected(incerr_c::make(TEST_t1));
}
guess_retType BarV::guess_TSCol(guess_firstParamType &&dp, DataStore const &ds) {
    return std::unexpected(incerr_c::make(TEST_t1));
}
guess_retType BarV::guess_catCol(guess_firstParamType &&dp, DataStore const &ds) {
    return std::unexpected(incerr_c::make(TEST_t1));
}
guess_retType BarV::guess_valueCols(guess_firstParamType &&dp, DataStore const &ds) {
    return std::unexpected(incerr_c::make(TEST_t1));
}
guess_retType BarV::guess_TFfeatures(guess_firstParamType &&dp, DataStore const &ds) {
    return std::unexpected(incerr_c::make(TEST_t1));
}
// ### END BAR V ###


// BAR VM

guess_retType BarVM::guess_sizes(guess_firstParamType &&dp, DataStore const &ds) {
    return std::unexpected(incerr_c::make(TEST_t1));
}
guess_retType BarVM::guess_plotType(guess_firstParamType &&dp, DataStore const &ds) {
    return std::unexpected(incerr_c::make(TEST_t1));
}
guess_retType BarVM::guess_TSCol(guess_firstParamType &&dp, DataStore const &ds) {
    return std::unexpected(incerr_c::make(TEST_t1));
}
guess_retType BarVM::guess_catCol(guess_firstParamType &&dp, DataStore const &ds) {
    return std::unexpected(incerr_c::make(TEST_t1));
}
guess_retType BarVM::guess_valueCols(guess_firstParamType &&dp, DataStore const &ds) {
    return std::unexpected(incerr_c::make(TEST_t1));
}
guess_retType BarVM::guess_TFfeatures(guess_firstParamType &&dp, DataStore const &ds) {
    return std::unexpected(incerr_c::make(TEST_t1));
}

// ### END BAR VM ###


// SCATTER
guess_retType Scatter::guess_sizes(guess_firstParamType &&dp, DataStore const &ds) {
    return std::unexpected(incerr_c::make(TEST_t1));
}
guess_retType Scatter::guess_plotType(guess_firstParamType &&dp, DataStore const &ds) {
    return std::unexpected(incerr_c::make(TEST_t1));
}
guess_retType Scatter::guess_TSCol(guess_firstParamType &&dp, DataStore const &ds) {
    return std::unexpected(incerr_c::make(TEST_t1));
}
guess_retType Scatter::guess_catCol(guess_firstParamType &&dp, DataStore const &ds) {
    return std::unexpected(incerr_c::make(TEST_t1));
}
guess_retType Scatter::guess_valueCols(guess_firstParamType &&dp, DataStore const &ds) {
    return std::unexpected(incerr_c::make(TEST_t1));
}
guess_retType Scatter::guess_TFfeatures(guess_firstParamType &&dp, DataStore const &ds) {
    return std::unexpected(incerr_c::make(TEST_t1));
}

// ### END SCATTER ###


// MULTILINE

guess_retType Multiline::guess_sizes(guess_firstParamType &&dp, DataStore const &ds) {
    return std::unexpected(incerr_c::make(TEST_t1));
}
guess_retType Multiline::guess_plotType(guess_firstParamType &&dp, DataStore const &ds) {
    return std::unexpected(incerr_c::make(TEST_t1));
}
guess_retType Multiline::guess_TSCol(guess_firstParamType &&dp, DataStore const &ds) {
    return std::unexpected(incerr_c::make(TEST_t1));
}
guess_retType Multiline::guess_catCol(guess_firstParamType &&dp, DataStore const &ds) {
    return std::unexpected(incerr_c::make(TEST_t1));
}
guess_retType Multiline::guess_valueCols(guess_firstParamType &&dp, DataStore const &ds) {
    return std::unexpected(incerr_c::make(TEST_t1));
}
guess_retType Multiline::guess_TFfeatures(guess_firstParamType &&dp, DataStore const &ds) {
    return std::unexpected(incerr_c::make(TEST_t1));
}
// ### END MULTILINE ###


// BAR HM

guess_retType BarHM::guess_sizes(guess_firstParamType &&dp, DataStore const &ds) {
    return std::unexpected(incerr_c::make(TEST_t1));
}
guess_retType BarHM::guess_plotType(guess_firstParamType &&dp, DataStore const &ds) {
    return std::unexpected(incerr_c::make(TEST_t1));
}
guess_retType BarHM::guess_TSCol(guess_firstParamType &&dp, DataStore const &ds) {
    return std::unexpected(incerr_c::make(TEST_t1));
}
guess_retType BarHM::guess_catCol(guess_firstParamType &&dp, DataStore const &ds) {
    return std::unexpected(incerr_c::make(TEST_t1));
}
guess_retType BarHM::guess_valueCols(guess_firstParamType &&dp, DataStore const &ds) {
    return std::unexpected(incerr_c::make(TEST_t1));
}
guess_retType BarHM::guess_TFfeatures(guess_firstParamType &&dp, DataStore const &ds) {
    return std::unexpected(incerr_c::make(TEST_t1));
}
// ### END BAR HM ###


// BAR HS

guess_retType BarHS::guess_sizes(guess_firstParamType &&dp, DataStore const &ds) {
    return std::unexpected(incerr_c::make(TEST_t1));
}
guess_retType BarHS::guess_plotType(guess_firstParamType &&dp, DataStore const &ds) {
    return std::unexpected(incerr_c::make(TEST_t1));
}
guess_retType BarHS::guess_TSCol(guess_firstParamType &&dp, DataStore const &ds) {
    return std::unexpected(incerr_c::make(TEST_t1));
}
guess_retType BarHS::guess_catCol(guess_firstParamType &&dp, DataStore const &ds) {
    return std::unexpected(incerr_c::make(TEST_t1));
}
guess_retType BarHS::guess_valueCols(guess_firstParamType &&dp, DataStore const &ds) {
    return std::unexpected(incerr_c::make(TEST_t1));
}
guess_retType BarHS::guess_TFfeatures(guess_firstParamType &&dp, DataStore const &ds) {
    return std::unexpected(incerr_c::make(TEST_t1));
}

// ### END BAR HS ###


} // namespace plot_structures
} // namespace terminal_plot
} // namespace incom