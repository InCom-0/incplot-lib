#pragma once

#include <expected>
#include <iostream>
#include <more_concepts/more_concepts.hpp>
#include <nlohmann/json.hpp>
#include <oof.h>
#include <optional>
#include <string_view>
#include <type_traits>
#include <variant>


namespace incom {
namespace terminal_plot {
namespace detail {
inline std::size_t strlen_utf8(const std::string &str) {
    std::size_t length = 0;
    for (char c : str) {
        if ((c & 0xC0) != 0x80) { ++length; }
    }
    return length;
}
} // namespace detail

enum class PlotType {
    barV,
    barH,
    line,
    multiline,
    scatter,
    bubble
};

enum class Err_plotSpecs {
    plotType,
    labelCol,
    valCols,
    namesIntoIDs_label,
    namesIntoIDs_vals,
    guessValCols,
};

enum class Err_drawer {
    plotStructureInvalid,
    barVplot_tooWide
};

using NLMjson = nlohmann::json;


struct DataStore {
    // The json this was constucted with/from ... possibly not strictly necessary to keep, but whatever
    std::vector<NLMjson> constructedWith;

    // Data descriptors
    std::vector<std::string>                         colNames;
    std::vector<std::pair<NLMjson::value_t, size_t>> colTypes; // First =  ColType, Second = ID in data vector

    // Actual data storage
    std::vector<std::vector<std::string>> stringCols;
    std::vector<std::vector<long long>>   llCols; // Don't care about signed unsigned, etc. ... all will be long long
    std::vector<std::vector<double>>      doubleCols;


    // CONSTRUCTION
    DataStore(std::vector<NLMjson> &&jsonVec) : constructedWith(std::move(jsonVec)) {

        // Create the requisite data descriptors and the structure
        for (auto const &[key, val] : constructedWith.front().items()) {
            colNames.push_back(key);

            if (val.type() == NLMjson::value_t::string) {
                colTypes.push_back({NLMjson::value_t::string, stringCols.size()});
                stringCols.push_back(std::vector<std::string>());
            }
            else if (val.type() == NLMjson::value_t::number_float) {
                colTypes.push_back({NLMjson::value_t::number_float, doubleCols.size()});
                doubleCols.push_back(std::vector<double>());
            }
            else if (val.type() == NLMjson::value_t::number_integer ||
                     val.type() == NLMjson::value_t::number_unsigned) {
                colTypes.push_back({val.type(), llCols.size()});
                llCols.push_back(std::vector<long long>());
            }
        }
        append_data(constructedWith);
    }

    // APPENDING
    void append_data(std::vector<NLMjson> const &toAppend) {
        for (auto const &oneJson : toAppend) {
            auto oneJsonIT = oneJson.items().begin();
            for (auto const &colTypesPair : colTypes) {
                if (colTypesPair.first == NLMjson::value_t::string) {
                    stringCols[colTypesPair.second].push_back(oneJsonIT.value());
                }
                else if (colTypesPair.first == NLMjson::value_t::number_integer ||
                         colTypesPair.first == NLMjson::value_t::number_unsigned) {
                    llCols[colTypesPair.second].push_back(static_cast<long long>(oneJsonIT.value()));
                }
                else if (colTypesPair.first == NLMjson::value_t::number_float) {
                    doubleCols[colTypesPair.second].push_back(oneJsonIT.value());
                }
                ++oneJsonIT;
            }
        }
    }
    void append_jsonAndData(std::vector<NLMjson> const &toAppend) {
        constructedWith.insert(constructedWith.end(), toAppend.begin(), toAppend.end());
        append_data(toAppend);
    }
};

// Encapsulates parsing of the input into custom data structure
// Validates 'hard' errors during parsing
// Validates that input data is not structured 'impossibly' (missing values, different value names per record, etc.)
struct Parser {

    static bool validate_jsonSameness(std::vector<NLMjson> const &jsonVec) {
        // Validate that all the JSON objects parsed above have the same structure
        for (auto const &js : jsonVec) {
            // Different number of items in this line vs the firstline
            if (js.size() != jsonVec.front().size()) { return false; } // throw something here

            auto firstLineIT = jsonVec.front().items().begin();
            for (auto const &[key, val] : js.items()) {

                // Key is not the same as in the first line
                if (key != firstLineIT.key()) { return false; } // throw something here

                // Type is not the same as in the first line
                if (val.type() != firstLineIT.value().type()) { return false; } // throw something here
                ++firstLineIT;
            }
        }
        return true;
    }

    static bool validate_jsonSameness(NLMjson const &json_A, NLMjson const &json_B) {
        if (json_A.size() != json_B.size()) { return false; }

        auto json_A_IT = json_A.items().begin();
        auto json_B_IT = json_B.items().begin();
        for (size_t i = 0; i < json_A.size(); ++i) {
            if (json_A_IT.key() != json_B_IT.key()) { return false; }
            if (json_B_IT.value().type() != json_B_IT.value().type()) { return false; }
            json_A_IT++, json_B_IT++;
        }
        return true;
    }

    template <typename T>
    requires std::is_convertible_v<T, std::string_view>
    static std::vector<NLMjson> parse_NDJSON(T const &stringLike) {

        auto const it =
            std::ranges::find_if_not(stringLike.rbegin(), stringLike.rend(), [](auto &&chr) { return chr == '\n'; });
        std::string_view const trimmed(stringLike.begin(), stringLike.end() - (it - stringLike.rbegin()));

        std::vector<NLMjson> parsed;
        for (auto const &oneLine : std::views::split(trimmed, '\n') |
                                       std::views::transform([](auto const &in) { return std::string_view(in); })) {
            NLMjson oneLineJson;
            try {
                oneLineJson = NLMjson::parse(oneLine);
            }
            catch (const NLMjson::exception &e) {
                std::cout << e.what() << '\n';
            }
            parsed.push_back(std::move(oneLineJson));
        }
        return parsed;
    }

    template <typename T>
    requires std::is_convertible_v<T, std::string_view>
    static void parse_NDJSON_andAddTo(T const &stringLike, DataStore &out_DS_toAppend) {

        auto parsed = parse_NDJSON(stringLike);
        if (not validate_jsonSameness(parsed)) {} // Throw something here
        if (not validate_jsonSameness(parsed.front(), out_DS_toAppend.constructedWith.front())) {
        } // Throw something here

        out_DS_toAppend.append_jsonAndData(parsed);
    }


    template <typename T>
    requires std::is_convertible_v<T, std::string_view>
    static DataStore parse_NDJSON_intoDS(T const &stringLike) {

        auto parsed = parse_NDJSON(stringLike);
        if (not validate_jsonSameness(parsed)) {} // Throw something here

        return DataStore(std::move(parsed));
    }

    template <typename T>
    requires more_concepts::container<T> && std::is_convertible_v<typename T::value_type, std::string_view>
    static DataStore parse_NDJSON(T containerOfStringLike) {

        for (auto &oneStr : containerOfStringLike) {
            while (oneStr.back() == '\n') { oneStr.popback(); }
        }

        std::vector<NLMjson> parsed;
        for (auto const &contItem : containerOfStringLike) {
            for (auto const oneLine : std::views::split(contItem, '\n') |
                                          std::views::transform([](auto const &in) { return std::string_view(in); })) {

                NLMjson oneLineJson;
                try {
                    oneLineJson = NLMjson::parse(oneLine);
                }
                catch (const NLMjson::exception &e) {
                    std::cout << e.what() << '\n';
                }
                parsed.push_back(std::move(oneLineJson));
            }
        }
        return DataStore(std::move(parsed));
    }
};


class DesiredPlot {
private:
    static std::expected<DesiredPlot, Err_plotSpecs> transform_namedColsIntoIDs(DesiredPlot &dp, DataStore const &ds) {
        if (dp.label_colName.has_value()) {
            auto it = std::ranges::find(ds.colNames, dp.label_colName.value());
            if (it == ds.colNames.end()) { return std::unexpected(Err_plotSpecs::namesIntoIDs_label); }
            else if (not dp.label_colID.has_value()) { dp.label_colID = it - ds.colNames.begin(); }
            else if ((it - ds.colNames.begin()) == dp.label_colID.value()) { dp.label_colName = std::nullopt; }
            else { return std::unexpected(Err_plotSpecs::namesIntoIDs_label); }
        }

        for (auto const &v_colName : dp.values_colNames) {
            auto it = std::ranges::find(ds.colNames, v_colName);
            if (it == ds.colNames.end()) { return std::unexpected(Err_plotSpecs::namesIntoIDs_label); }

            auto it2 = std::ranges::find(dp.values_colIDs, it - ds.colNames.begin());
            if (it2 == dp.values_colIDs.end()) { dp.values_colIDs.push_back(it2 - dp.values_colIDs.begin()); }

            dp.values_colNames.clear();
        }
        return std::move(dp);
    }
    static std::expected<DesiredPlot, Err_plotSpecs> guess_plotType(DesiredPlot &dp, DataStore const &ds) {
        if (dp.plot_type.has_value()) { return dp; }

        if (dp.values_colIDs.size() > 5) { return std::unexpected(Err_plotSpecs::valCols); }
        else if (dp.values_colIDs.size() < 2) { dp.plot_type = PlotType::barV; }
        else if (dp.values_colIDs.size() == 3 && (not dp.label_colID.has_value())) { dp.plot_type = PlotType::bubble; }
        else if (dp.values_colIDs.size() == 2 && (not dp.label_colID.has_value())) { dp.plot_type = PlotType::scatter; }
        else { dp.plot_type = PlotType::multiline; }

        return std::move(dp);
    }
    static std::expected<DesiredPlot, Err_plotSpecs> guess_labelCol(DesiredPlot &dp, DataStore const &ds) {
        if (dp.label_colID.has_value()) { return std::move(dp); }
        else if (dp.plot_type != PlotType::barV) { return std::move(dp); }

        else {
            auto it = std::ranges::find_if(ds.colTypes, [](auto &&a) { return a.first == NLMjson::value_t::string; });
            if (it == ds.colTypes.end()) { return std::unexpected(Err_plotSpecs::labelCol); }
            else { dp.label_colID = it->second; }
            return std::move(dp);
        }
    }
    static std::expected<DesiredPlot, Err_plotSpecs> guess_valueCols(DesiredPlot &dp, DataStore const &ds) {

        auto addValColsUntil = [&](size_t count) -> std::expected<size_t, Err_plotSpecs> {
            auto getAnotherValColID = [&]() -> std::expected<size_t, Err_plotSpecs> {
                auto valColTypes = std::views::filter(ds.colTypes, [](auto &&a) {
                    return (a.first == NLMjson::value_t::number_float || a.first == NLMjson::value_t::number_integer ||
                            a.first == NLMjson::value_t::number_unsigned);
                });

                for (auto const &vct : valColTypes) {
                    if (std::ranges::find(dp.values_colIDs, vct.second) == dp.values_colIDs.end()) {
                        return vct.second;
                    }
                }
                // Cannot find another one
                return std::unexpected(Err_plotSpecs::guessValCols);
            };
            while (dp.values_colIDs.size() < count) {
                auto expID = getAnotherValColID();
                if (expID.has_value()) { dp.values_colIDs.push_back(expID.value()); }
                else { return std::unexpected(expID.error()); }
            }
            return 0uz;
        };

        // BAR PLOTS
        if (dp.plot_type == PlotType::barV) {
            if (dp.values_colIDs.size() > 1) { return std::unexpected(Err_plotSpecs::valCols); }
            else if (not addValColsUntil(1).has_value()) { return std::unexpected(Err_plotSpecs::guessValCols); }
        }
        if (dp.plot_type == PlotType::barH) {
            if (dp.values_colIDs.size() > 1) { return std::unexpected(Err_plotSpecs::valCols); }
            else if (not addValColsUntil(1).has_value()) { return std::unexpected(Err_plotSpecs::guessValCols); }
        }
        // LINE PLOTS
        else if (dp.plot_type == PlotType::line) {
            if (dp.values_colIDs.size() > 1) { return std::unexpected(Err_plotSpecs::valCols); }
            else if (not addValColsUntil(1).has_value()) { return std::unexpected(Err_plotSpecs::guessValCols); }
        }
        else if (dp.plot_type == PlotType::multiline) {
            if (dp.values_colIDs.size() > 5) { return std::unexpected(Err_plotSpecs::valCols); }
            else if (not addValColsUntil(2).has_value()) { return std::unexpected(Err_plotSpecs::guessValCols); }
        }

        // SCATTER PLOT
        else if (dp.plot_type == PlotType::scatter) {
            if (dp.values_colIDs.size() > 2) { return std::unexpected(Err_plotSpecs::valCols); }
            else if (not addValColsUntil(2).has_value()) { return std::unexpected(Err_plotSpecs::guessValCols); }
        }
        // BUBBLE PLOT
        else if (dp.plot_type == PlotType::bubble) {
            if (dp.values_colIDs.size() > 3) { return std::unexpected(Err_plotSpecs::valCols); }
            else if (not addValColsUntil(3).has_value()) { return std::unexpected(Err_plotSpecs::guessValCols); }
        }
        return std::move(dp);
    }


public:
    std::optional<PlotType> plot_type;

    std::optional<size_t>      label_colID;
    std::optional<std::string> label_colName;

    std::vector<size_t>      values_colIDs;
    std::vector<std::string> values_colNames;


    DesiredPlot() {}
    DesiredPlot(PlotType pt, std::optional<size_t> l_colID, std::optional<std::string> l_colName,
                std::vector<size_t> v_colIDs, std::vector<std::string> v_colNames)
        : plot_type(std::move(pt)), label_colID(std::move(l_colID)), label_colName(std::move(l_colName)),
          values_colIDs(std::move(v_colIDs)), values_colNames(std::move(v_colNames)) {}

    // Guesses the missing 'desired parameters' and returns a new DesiredPlot with those filled in
    // If impossible to guess or otherwise the user desires something impossible returns specErr.
    std::expected<DesiredPlot, Err_plotSpecs> make_autoGuessedDP(this auto selfCopy, DataStore const &ds) {

        // Could use std::bind for these ... had some trouble with that ... maybe return to it later
        auto tnc = [&](DesiredPlot &dp) -> std::expected<DesiredPlot, Err_plotSpecs> {
            return DesiredPlot::transform_namedColsIntoIDs(dp, ds);
        };
        auto gpt = [&](DesiredPlot &&dp) -> std::expected<DesiredPlot, Err_plotSpecs> {
            return DesiredPlot::guess_plotType(dp, ds);
        };
        auto glc = [&](DesiredPlot &&dp) -> std::expected<DesiredPlot, Err_plotSpecs> {
            return DesiredPlot::guess_labelCol(dp, ds);
        };
        auto gvc = [&](DesiredPlot &&dp) -> std::expected<DesiredPlot, Err_plotSpecs> {
            return DesiredPlot::guess_valueCols(dp, ds);
        };

        return tnc(selfCopy).and_then(gpt).and_then(glc).and_then(gvc);
    }
};


namespace plot_structures {
// Classes derived from base represent 'plot structures' of particular types of plots (such as bar vertical, scatter
// etc.)
// Create your own 'plot structure' ie. type of plot by deriving from 'Base' class (or from other classes derived from
// it) and overriding pure virtual functions The types properly derived from 'Base' can then be used inside 'PlotDrawer'
// inside std::variant<...>.
// The idea is to be able to easily customize and also possibly 'partially customize' as needed
// You always have to make the 'Base' class a friend ... this enables really nice dynamic polymorphism coupled with
// 'deducing this' feature of C++23
class Base {
protected:
    // Descriptors - First thing to be computed
    size_t areaWidth                = 0;
    size_t areaHeight               = 0;
    size_t labels_vertialWidth      = 0;
    size_t labels_vertialLeftWidth  = 0;
    size_t labels_vertialRightWidth = 0;

    size_t padding_left   = 2;
    size_t padding_right  = 0;
    size_t padding_top    = 0;
    size_t padding_bottom = 0;

    bool labels_horizontalTop    = false;
    bool labels_horizontalBottom = false;

    bool axisName_horizontalTop_bool    = false;
    bool axisName_horizontalBottom_bool = false;


    // Actual structure
    std::vector<std::string> labels_verticalLeft;
    std::string              axis_verticalLeft;

    std::string              axis_verticalRight;
    std::vector<std::string> labels_verticalRight;

    std::string axisName_horizontalTop;
    std::string label_horizontalTop;
    std::string axis_horizontalTop;

    std::string axis_horizontalBottom;
    std::string label_horizontalBottom;
    std::string axisName_horizontalBottom;

    std::vector<std::string> corner_topLeft;
    std::vector<std::string> corner_bottomLeft;
    std::vector<std::string> corner_bottomRight;
    std::vector<std::string> corner_topRight;

    std::vector<std::string> plotArea;


public:
    // This needs to get called after default construction
    bool build_self(this auto &&self, DesiredPlot const &dp, DataStore const &ds, size_t const &tar_width,
                    size_t const &tar_height) {

        self.compute_descriptors(dp, ds, tar_width, tar_height);

        if (not self.validate_descriptors()) { return false; }

        self.compute_labels_vl(dp, ds);
        self.compute_labels_vr(dp, ds);

        self.compute_axis_vl(dp, ds);
        self.compute_axis_vr(dp, ds);

        self.compute_corner_tl(dp, ds);
        self.compute_corner_bl(dp, ds);
        self.compute_corner_br(dp, ds);
        self.compute_corner_tr(dp, ds);

        self.compute_axis_ht(dp, ds);
        self.compute_axis_hb(dp, ds);
        self.compute_plot_area(dp, ds);

        return true;
    }

    bool validate_self() const { return true; }

    size_t compute_lengthOfSelf() const {
        size_t lngth = 0;

        lngth +=
            labels_verticalLeft.empty() == true ? 0 : labels_verticalLeft.size() * labels_verticalLeft.front().size();
        lngth += labels_verticalRight.empty() == true
                     ? 0
                     : labels_verticalRight.size() * labels_verticalRight.front().size();
        lngth += axis_verticalLeft.size() + axis_verticalRight.size();

        lngth += axis_horizontalBottom.size() + label_horizontalBottom.size() + axis_horizontalBottom.size();
        lngth += axis_horizontalTop.size() + label_horizontalTop.size() + axisName_horizontalTop.size();

        lngth += 6;

        lngth += corner_bottomLeft.empty() == true ? 0 : corner_bottomLeft.size() * corner_bottomLeft.front().size();
        lngth += corner_bottomRight.empty() == true ? 0 : corner_bottomRight.size() * corner_bottomRight.front().size();
        lngth += corner_topLeft.empty() == true ? 0 : corner_topLeft.size() * corner_topLeft.front().size();
        lngth += corner_topRight.empty() == true ? 0 : corner_topRight.size() * corner_topRight.front().size();

        lngth += plotArea.empty() == true ? 0 : plotArea.size() * (plotArea.front().size() + 1);

        return lngth;
    }

    std::string build_plotAsString() const {
        std::string result;
        result.reserve(compute_lengthOfSelf());

        // Build the heading lines of the plot
        size_t i = 0;
        if (not axisName_horizontalTop.empty()) {
            result.append(corner_topLeft.at(i));
            result.append(axis_verticalLeft.substr(0, 1));
            result.append(axisName_horizontalTop);
            result.append(axis_verticalRight.substr(0, 1));
            result.append(corner_topRight.at(i++));
            result.push_back('\n');
        }
        if (not label_horizontalTop.empty()) {
            result.append(corner_topLeft.at(i));
            result.append(axis_verticalLeft.substr(1, 1));
            result.append(label_horizontalTop);
            result.append(axis_verticalRight.substr(1, 1));
            result.append(corner_topRight.at(i++));
            result.push_back('\n');
        }
        if (not axis_horizontalTop.empty()) {
            result.append(labels_verticalLeft.front());
            result.append(axis_verticalLeft.substr(2, 3));
            result.append(axis_horizontalTop);
            result.append(axis_verticalRight.substr(2, 3));
            result.append(labels_verticalRight.front());
            result.push_back('\n');
        }

        // Build the main lines of the plot
        for (size_t lineID = 0; lineID < plotArea.size(); ++lineID) {
            result.append(labels_verticalLeft.at(lineID + 1));
            result.append(axis_verticalLeft.substr(5 + lineID * 3, 3));
            result.append(plotArea.at(lineID));
            result.append(axis_verticalRight.substr(5 + lineID * 3, 3));
            result.append(labels_verticalRight.at(lineID + 1));
            result.push_back('\n');
        }

        // Build the tail lines of the plot
        size_t leftSide_i = i;
        i                 = 0;
        if (not axis_horizontalBottom.empty()) {
            result.append(labels_verticalLeft.back());
            result.append(axis_verticalLeft.substr(axis_verticalLeft.size() - 5, 3));
            result.append(axis_horizontalBottom);
            result.append(axis_verticalRight.substr(axis_verticalRight.size() - 5, 3));
            result.append(labels_verticalRight.back());
            result.push_back('\n');
        }
        if (not label_horizontalBottom.empty()) {
            result.append(corner_bottomLeft.at(i));
            result.push_back(' ');
            result.append(label_horizontalBottom);
            result.push_back(' ');
            result.append(corner_bottomLeft.at(i++));
            result.push_back('\n');
        }

        if (not axisName_horizontalBottom.empty()) {
            result.append(corner_bottomLeft.at(i));
            result.push_back(' ');
            result.append(axisName_horizontalBottom);
            result.push_back(' ');
            result.append(corner_bottomLeft.at(i++));
            result.push_back('\n');
        }

        return result;
    }

private:
    bool validate_descriptors() { return true; }


    // Pure virtual methods that need be defined in derived and are then called during construction through 'self
    // builder'
    virtual void compute_descriptors(DesiredPlot const &dp, DataStore const &ds, size_t const &tar_width,
                                     size_t const &tar_height) = 0;

    virtual void compute_labels_vl(DesiredPlot const &dp, DataStore const &ds) = 0;
    virtual void compute_labels_vr(DesiredPlot const &dp, DataStore const &ds) = 0;

    virtual void compute_axis_vl(DesiredPlot const &dp, DataStore const &ds) = 0;
    virtual void compute_axis_vr(DesiredPlot const &dp, DataStore const &ds) = 0;

    virtual void compute_corner_tl(DesiredPlot const &dp, DataStore const &ds) = 0;
    virtual void compute_corner_bl(DesiredPlot const &dp, DataStore const &ds) = 0;
    virtual void compute_corner_br(DesiredPlot const &dp, DataStore const &ds) = 0;
    virtual void compute_corner_tr(DesiredPlot const &dp, DataStore const &ds) = 0;

    virtual void compute_axis_ht(DesiredPlot const &dp, DataStore const &ds) = 0;
    virtual void compute_axis_hb(DesiredPlot const &dp, DataStore const &ds) = 0;

    virtual void compute_plot_area(DesiredPlot const &dp, DataStore const &ds) = 0;
};

class BarV : public Base {
    friend class Base;

    virtual void compute_descriptors(DesiredPlot const &dp, DataStore const &ds, size_t const &tar_width,
                                     size_t const &tar_height) override {}

    virtual void compute_labels_vl(DesiredPlot const &dp, DataStore const &ds) override {}
    virtual void compute_labels_vr(DesiredPlot const &dp, DataStore const &ds) override {}

    virtual void compute_axis_vl(DesiredPlot const &dp, DataStore const &ds) override {}
    virtual void compute_axis_vr(DesiredPlot const &dp, DataStore const &ds) override {}

    virtual void compute_corner_tl(DesiredPlot const &dp, DataStore const &ds) override {}
    virtual void compute_corner_bl(DesiredPlot const &dp, DataStore const &ds) override {}
    virtual void compute_corner_br(DesiredPlot const &dp, DataStore const &ds) override {}
    virtual void compute_corner_tr(DesiredPlot const &dp, DataStore const &ds) override {}

    virtual void compute_axis_ht(DesiredPlot const &dp, DataStore const &ds) override {}
    virtual void compute_axis_hb(DesiredPlot const &dp, DataStore const &ds) override {}

    virtual void compute_plot_area(DesiredPlot const &dp, DataStore const &ds) override {}
};

class BarH : public BarV {
    friend class Base;
};

class Line : public BarV {
    friend class Base;
};

class Multiline : public BarV {
    friend class Base;
};

class Scatter : public BarV {
    friend class Base;
};

class Bubble : public BarV {
    friend class Base;
};

} // namespace plot_structures


template <typename T>
requires std::is_base_of_v<plot_structures::Base, T>
class PlotDrawer {
private:
    T ps;

public:
    PlotDrawer(DesiredPlot const &dp, DataStore const &ds, size_t tar_width, size_t tar_height) {
        ps.build_self(dp, ds, tar_width, tar_height);
    }

    void update_newPlotStructure(DesiredPlot const &dp, DataStore const &ds, size_t tar_width, size_t tar_height) {
        ps = T(dp, ds, tar_width, tar_height);
    }

    std::expected<std::string, Err_drawer> validateAndDrawPlot() const {
        if (ps.validate_self() == false) { return std::unexpected(Err_drawer::plotStructureInvalid); }
        else { return drawPlot(); }
    }
    std::string drawPlot() const { return ps.build_plotAsString(); }
};


inline std::variant<PlotDrawer<plot_structures::BarV>, PlotDrawer<plot_structures::BarH>,
                    PlotDrawer<plot_structures::Line>, PlotDrawer<plot_structures::Multiline>,
                    PlotDrawer<plot_structures::Scatter>, PlotDrawer<plot_structures::Bubble>>
make_plotDrawer(DesiredPlot const &dp, DataStore const &ds, size_t tar_width, size_t tar_height) {
    switch (dp.plot_type.value()) {
        case PlotType::barV:      return PlotDrawer<plot_structures::BarV>(dp, ds, tar_width, tar_height);
        case PlotType::barH:      return PlotDrawer<plot_structures::BarH>(dp, ds, tar_width, tar_height);
        case PlotType::line:      return PlotDrawer<plot_structures::Line>(dp, ds, tar_width, tar_height);
        case PlotType::multiline: return PlotDrawer<plot_structures::Multiline>(dp, ds, tar_width, tar_height);
        case PlotType::scatter:   return PlotDrawer<plot_structures::Scatter>(dp, ds, tar_width, tar_height);
        case PlotType::bubble:    return PlotDrawer<plot_structures::Bubble>(dp, ds, tar_width, tar_height);
        default:                  return PlotDrawer<plot_structures::BarV>(dp, ds, tar_width, tar_height);
    }
}


} // namespace terminal_plot
} // namespace incom

namespace incplot = incom::terminal_plot;