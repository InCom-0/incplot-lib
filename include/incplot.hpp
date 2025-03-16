#pragma once

#include <algorithm>
#include <expected>
#include <functional>
#include <iostream>
#include <more_concepts/more_concepts.hpp>
#include <nlohmann/json.hpp>
#include <oof.h>
#include <optional>
#include <string_view>


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
using NLMjson = nlohmann::json;

enum class PlotType {
    barV,
    barH,
    line,
    multiline,
    scatter,
    bubble
};

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
public:
    enum class specErr {
        plotType,
        labelCol,
        valCols,
        namesIntoIDs_label,
        namesIntoIDs_vals,
        guessValCols,
    };

private:
    static std::expected<DesiredPlot, specErr> transform_namedColsIntoIDs(DesiredPlot &dp, DataStore const &ds) {
        if (dp.label_colName.has_value()) {
            auto it = std::ranges::find(ds.colNames, dp.label_colName.value());
            if (it == ds.colNames.end()) { return std::unexpected(specErr::namesIntoIDs_label); }
            else if (not dp.label_colID.has_value()) { dp.label_colID = it - ds.colNames.begin(); }
            else if ((it - ds.colNames.begin()) == dp.label_colID.value()) { dp.label_colName = std::nullopt; }
            else { return std::unexpected(specErr::namesIntoIDs_label); }
        }

        for (auto const &v_colName : dp.values_colNames) {
            auto it = std::ranges::find(ds.colNames, v_colName);
            if (it == ds.colNames.end()) { return std::unexpected(specErr::namesIntoIDs_label); }

            auto it2 = std::ranges::find(dp.values_colIDs, it - ds.colNames.begin());
            if (it2 == dp.values_colIDs.end()) { dp.values_colIDs.push_back(it2 - dp.values_colIDs.begin()); }

            dp.values_colNames.clear();
        }
        return std::move(dp);
    }
    static std::expected<DesiredPlot, specErr> guess_plotType(DesiredPlot &dp, DataStore const &ds) {
        if (dp.plot_type.has_value()) { return dp; }

        if (dp.values_colIDs.size() > 5) { return std::unexpected(specErr::valCols); }
        else if (dp.values_colIDs.size() < 2) { dp.plot_type = PlotType::barV; }
        else if (dp.values_colIDs.size() == 3 && (not dp.label_colID.has_value())) { dp.plot_type = PlotType::bubble; }
        else if (dp.values_colIDs.size() == 2 && (not dp.label_colID.has_value())) { dp.plot_type = PlotType::scatter; }
        else { dp.plot_type = PlotType::multiline; }

        return std::move(dp);
    }
    static std::expected<DesiredPlot, specErr> guess_labelCol(DesiredPlot &dp, DataStore const &ds) {
        if (dp.label_colID.has_value()) { return std::move(dp); }
        else if (dp.plot_type != PlotType::barV) { return std::move(dp); }

        else {
            auto it = std::ranges::find_if(ds.colTypes, [](auto &&a) { return a.first == NLMjson::value_t::string; });
            if (it == ds.colTypes.end()) { return std::unexpected(specErr::labelCol); }
            else { dp.label_colID = it->second; }
            return std::move(dp);
        }
    }
    static std::expected<DesiredPlot, specErr> guess_valueCols(DesiredPlot &dp, DataStore const &ds) {

        auto addValColsUntil = [&](size_t count) -> std::expected<size_t, specErr> {
            auto getAnotherValColID = [&]() -> std::expected<size_t, specErr> {
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
                return std::unexpected(specErr::guessValCols);
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
            if (dp.values_colIDs.size() > 1) { return std::unexpected(specErr::valCols); }
            else if (not addValColsUntil(1).has_value()) { return std::unexpected(specErr::guessValCols); }
        }
        if (dp.plot_type == PlotType::barH) {
            if (dp.values_colIDs.size() > 1) { return std::unexpected(specErr::valCols); }
            else if (not addValColsUntil(1).has_value()) { return std::unexpected(specErr::guessValCols); }
        }
        // LINE PLOTS
        else if (dp.plot_type == PlotType::line) {
            if (dp.values_colIDs.size() > 1) { return std::unexpected(specErr::valCols); }
            else if (not addValColsUntil(1).has_value()) { return std::unexpected(specErr::guessValCols); }
        }
        else if (dp.plot_type == PlotType::multiline) {
            if (dp.values_colIDs.size() > 5) { return std::unexpected(specErr::valCols); }
            else if (not addValColsUntil(2).has_value()) { return std::unexpected(specErr::guessValCols); }
        }

        // SCATTER PLOT
        else if (dp.plot_type == PlotType::scatter) {
            if (dp.values_colIDs.size() > 2) { return std::unexpected(specErr::valCols); }
            else if (not addValColsUntil(2).has_value()) { return std::unexpected(specErr::guessValCols); }
        }
        // BUBBLE PLOT
        else if (dp.plot_type == PlotType::bubble) {
            if (dp.values_colIDs.size() > 3) { return std::unexpected(specErr::valCols); }
            else if (not addValColsUntil(3).has_value()) { return std::unexpected(specErr::guessValCols); }
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

    // This guesses the missing 'desired parameters' and returns a new DesiredPlot with those filled in
    // If impossible to guess or otherwise the user desires something impossible returns specErr.
    std::expected<DesiredPlot, specErr> make_autoGuessedDP(this auto selfCopy, DataStore const &ds) {

        auto tnc = [&](DesiredPlot &dp) -> std::expected<DesiredPlot, specErr> {
            return DesiredPlot::transform_namedColsIntoIDs(dp, ds);
        };
        auto gpt = [&](DesiredPlot &&dp) -> std::expected<DesiredPlot, specErr> {
            return DesiredPlot::guess_plotType(dp, ds);
        };
        auto glc = [&](DesiredPlot &&dp) -> std::expected<DesiredPlot, specErr> {
            return DesiredPlot::guess_labelCol(dp, ds);
        };
        auto gvc = [&](DesiredPlot &&dp) -> std::expected<DesiredPlot, specErr> {
            return DesiredPlot::guess_valueCols(dp, ds);
        };

        return tnc(selfCopy).and_then(gpt).and_then(glc).and_then(gvc);
    }
};

class PlotDrawer {
public:
    enum class drawerErr {
        plotStructureInvalid,
        barVplot_tooWide
    };

private:
    struct PlotStructure {
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

        std::string str_cutToSize(std::string const &toCut, size_t const &maxLabelLength) {
            size_t      cutChars = toCut.size() - maxLabelLength + 3;
            std::string res(toCut.begin(), toCut.begin() + 21);
            res.append("...");
            res.append(toCut.begin() + 21 + cutChars, toCut.end());
            return res;
        };

        void compute_labels_vl(DesiredPlot const &dp, DataStore const &ds) {
            std::vector<std::string> res;

            if (dp.plot_type == PlotType::barV) {
                constexpr size_t const maxLabelLength = 30;

                auto &ds_labelsREF = ds.stringCols.at(ds.colTypes.at(dp.label_colID.value()).second);
                auto  labelLengthView =
                    std::ranges::transform_view(ds_labelsREF, [](auto &&item) { return item.size(); });
                auto maxLL = std::ranges::max(labelLengthView, std::less());

                size_t const maxAllowedLL = std::min(maxLL, maxLabelLength);
                // Top horizontal axis label
                res.push_back(std::string(maxAllowedLL+2, ' '));
                // Plot area labels
                for (auto const &lbl : ds_labelsREF) {
                    if (lbl.size() <= maxAllowedLL) {
                        res.push_back(std::string(maxAllowedLL - lbl.size() + 2, ' ').append(lbl));
                    }
                    else { res.push_back(str_cutToSize(lbl, maxLabelLength)); }
                }
                // Bottom horizontal axis label
                res.push_back(std::string(maxAllowedLL+2, ' '));
            }
            else {
                size_t id = ds.colTypes.at(dp.values_colIDs.front()).second;
                // Top horizontal axis label
                res.push_back(std::string(2, ' '));
                // Plot area labels
                if (ds.colTypes.at(dp.values_colIDs.front()).first == NLMjson::value_t::number_float) {
                    auto &ds_valColREF = ds.doubleCols.at(id);
                    for (auto const &_ : ds_valColREF) { res.push_back(std::string(2, ' ')); }
                }
                else {
                    auto &ds_valColREF = ds.llCols.at(id);
                    for (auto const &_ : ds_valColREF) { res.push_back(std::string(2, ' ')); }
                }
                // Bottom horizontal axis label
                res.push_back(std::string(2, ' '));
            }


            labels_verticalLeft = std::move(res);
        }

        void compute_labels_vr(DesiredPlot const &dp, DataStore const &ds) {
            labels_verticalRight = std::vector<std::string>(labels_verticalLeft.size(), std::string());
        }
        void compute_axis_vl() {
            std::string res;
            res.push_back(' ');
            res.push_back(' ');
            res.append("┌");

            for (int i = 0; i < labels_verticalLeft.size() - 2; i++) { res.append("┤"); }

            res.append("└");
            res.push_back(' ');
            res.push_back(' ');
            axis_verticalLeft = std::move(res);
        }
        void compute_axis_vr() {
            using namespace std::literals;
            std::string res;
            res.push_back(' ');
            res.push_back(' ');
            res.append("┐"sv);

            for (int i = 0; i < labels_verticalLeft.size() - 2; i++) { res.push_back(' '); }

            res.append("┘"sv);
            res.push_back(' ');
            res.push_back(' ');
            axis_verticalRight = std::move(res);
        }

        void compute_corner_tl() {
            corner_topLeft = std::vector(2, std::string(labels_verticalLeft.front().size(), ' '));
        }
        void compute_corner_bl() {
            corner_bottomLeft = std::vector(2, std::string(labels_verticalLeft.front().size(), ' '));
        }
        void compute_corner_br() {
            corner_bottomRight = std::vector(2, std::string(labels_verticalRight.front().size(), ' '));
        }
        void compute_corner_tr() {
            corner_topRight = std::vector(2, std::string(labels_verticalRight.front().size(), ' '));
        }
        void compute_axis_ht(size_t const &tar_width) {
            size_t const plotWidth =
                tar_width - labels_verticalLeft.front().size() - labels_verticalRight.front().size() - 2;
            axis_horizontalTop = std::string(plotWidth, ' ');
        }
        void compute_axis_hb(size_t const &tar_width) {
            size_t const plotWidth =
                tar_width - labels_verticalLeft.front().size() - labels_verticalRight.front().size() - 2;

            axis_horizontalBottom = std::string(plotWidth, ' ');
            // for (int i = 0; i < plotWidth; ++i) { axis_horizontalBottom.append("─"); }
        }


        void compute_plotArea(DesiredPlot const &dp, DataStore const &ds, size_t const &tar_width) {
            static constexpr std::string const barString("■");
            size_t const                       plotWidth =
                tar_width - labels_verticalLeft.front().size() - labels_verticalRight.front().size() - 2;

            if (dp.plot_type == PlotType::barV) {
                auto colTypePair = ds.colTypes.at(dp.values_colIDs.front());

                if (colTypePair.first == NLMjson::value_t::number_float) {}

                else {
                    auto const &valColRef       = ds.llCols.at(colTypePair.second);
                    auto const [minVal, maxVal] = std::ranges::minmax(valColRef);
                    long long stepSize          = (maxVal - minVal) / plotWidth;

                    for (auto const &oneVal : valColRef) {
                        size_t const steps = (oneVal - minVal) / stepSize;
                        std::string  inputToPA;
                        for (size_t i = 0; i < steps; ++i) { inputToPA.append("■"); }
                        for (size_t i = steps; i < plotWidth; ++i) { inputToPA.push_back(' '); }
                        plotArea.push_back(inputToPA);
                    }
                }
            }
        }


        PlotStructure(DesiredPlot const &dp, DataStore const &ds, size_t tar_width) {
            compute_labels_vl(dp, ds);
            compute_labels_vr(dp, ds);

            compute_axis_vl();
            compute_axis_vr();
            compute_corner_tl();
            compute_corner_bl();
            compute_corner_br();
            compute_corner_tr();

            compute_axis_ht(tar_width);
            compute_axis_hb(tar_width);
            compute_plotArea(dp, ds, tar_width);

            auto view1      = std::ranges::transform_view(ds.llCols, [](auto &&item) { return item.size(); });
            auto [min, max] = std::ranges::minmax(view1, std::less());
        }

        bool validate_self() { return true; }

        size_t compute_lengthOfSelf() {
            size_t lngth = 0;

            lngth += labels_verticalLeft.empty() == true
                         ? 0
                         : labels_verticalLeft.size() * labels_verticalLeft.front().size();
            lngth += labels_verticalRight.empty() == true
                         ? 0
                         : labels_verticalRight.size() * labels_verticalRight.front().size();
            lngth += axis_verticalLeft.size() + axis_verticalRight.size();

            lngth += axis_horizontalBottom.size() + label_horizontalBottom.size() + axis_horizontalBottom.size();
            lngth += axis_horizontalTop.size() + label_horizontalTop.size() + axisName_horizontalTop.size();

            lngth += 6;

            lngth +=
                corner_bottomLeft.empty() == true ? 0 : corner_bottomLeft.size() * corner_bottomLeft.front().size();
            lngth +=
                corner_bottomRight.empty() == true ? 0 : corner_bottomRight.size() * corner_bottomRight.front().size();
            lngth += corner_topLeft.empty() == true ? 0 : corner_topLeft.size() * corner_topLeft.front().size();
            lngth += corner_topRight.empty() == true ? 0 : corner_topRight.size() * corner_topRight.front().size();

            lngth += plotArea.empty() == true ? 0 : plotArea.size() * (plotArea.front().size() + 1);

            return lngth;
        }

        std::string build_plotAsString() {
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
    };

    PlotStructure ps;

public:
    PlotDrawer(DesiredPlot const &dp, DataStore const &ds, size_t tar_width) : ps(dp, ds, tar_width) {}

    void update_newPlotStructure(DesiredPlot const &dp, DataStore const &ds, size_t tar_width) {
        ps = PlotStructure(dp, ds, tar_width);
    }

    std::expected<std::string, drawerErr> validateAndDrawPlot() {
        if (ps.validate_self() == false) { return std::unexpected(drawerErr::plotStructureInvalid); }
        else { return drawPlot(); }
    }
    std::string drawPlot() { return ps.build_plotAsString(); }

    std::string getSSST() { return ps.axis_verticalRight; }
};


} // namespace terminal_plot
} // namespace incom

namespace incplot = incom::terminal_plot;