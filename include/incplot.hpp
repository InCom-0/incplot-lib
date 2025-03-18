#pragma once

#include <expected>
#include <iostream>
#include <iterator>
#include <more_concepts/more_concepts.hpp>
#include <nlohmann/detail/value_t.hpp>
#include <nlohmann/json.hpp>
#include <oof.h>
#include <ranges>
#include <source_location>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>


namespace incom {
namespace terminal_plot {
// FORWARD DELCARATIONS
struct DataStore;
struct Parser;
class DesiredPlot;

namespace plot_structures {
class Base;
class BarV;
class BarH;
class Line;
class Multiline;
class Scatter;
class Bubble;
} // namespace plot_structures

enum class Err_plotSpecs {
    plotType,
    labelCol,
    valCols,
    namesIntoIDs_label,
    namesIntoIDs_vals,
    guessValCols,
    axisTicks
};
enum class Err_drawer {
    plotStructureInvalid,
    barVplot_tooWide
};

namespace detail {
constexpr inline std::size_t strlen_utf8(const std::string &str) {
    std::size_t length = 0;
    for (char c : str) {
        if ((c & 0xC0) != 0x80) { ++length; }
    }
    return length;
}

/*
Quasi compile time reflection for typenames
*/
template <typename T>
constexpr auto TypeToString() {
    auto EmbeddingSignature = std::string{std::source_location::current().function_name()};
    auto firstPos           = EmbeddingSignature.rfind("::") + 2;
    return EmbeddingSignature.substr(firstPos, EmbeddingSignature.size() - firstPos - 1);
}

template <typename... Ts>
constexpr bool __none_sameLastLevelTypeName_HLPR() {
    std::vector<std::string> vect;
    (vect.push_back(TypeToString<Ts>()), ...);
    std::ranges::sort(vect, std::less());
    auto [beg, end] = std::ranges::unique(vect);
    vect.erase(beg, end);
    return vect.size() == sizeof...(Ts);
}

// Just the 'last level' type name ... not the fully qualified typename
template <typename... Ts>
concept none_sameLastLevelTypeName = __none_sameLastLevelTypeName_HLPR<Ts...>();

constexpr inline std::string middleTrim2Size(std::string const &str, size_t maxSize) {
    if (str.size() > maxSize) {
        size_t cutPoint = str.size() / 2;
        return std::string(str.begin(), str.begin() + cutPoint)
            .append("...")
            .append(str.begin() + (maxSize - str.size()) + cutPoint + 3, str.end());
    }
    else { return std::string(maxSize - str.size(), ' ').append(str); }
}
constexpr inline std::vector<std::string> create_tickAxis(std::string filler, std::string tick, size_t steps,
                                                          size_t totalWidth) {
    size_t fillerSize = (totalWidth - steps) / (steps + 1);

    std::vector<std::string> res;
    for (size_t si = 0; si < steps; ++si) {
        for (size_t fi = 0; fi < fillerSize; ++fi) { res.push_back(std::string(filler)); }
        res.push_back(tick);
    }
    for (size_t i = 0; i < fillerSize; ++i) { res.push_back(std::string(filler)); }
    return res;
}

// Ring vector for future usage in 'scrolling plot' scenarios
template <typename T>
class RingVector {
private:
    std::vector<T> _m_buf;
    size_t         head        = 0;
    size_t         nextRead_ID = 0;

public:
    RingVector(std::vector<T> &&t) : _m_buf(t) {};
    RingVector(std::vector<T> &t) : _m_buf(t) {};

    // TODO: Might not need to create a copy here or below once std::views::concatenate from C++26 exists
    std::vector<T> create_copy() {
        std::vector<T> res(_m_buf.begin() + head, _m_buf.end());
        for (int i = 0; i < head; ++i) { res.push_back(_m_buf[i]); }
        return res;
    }

    std::vector<T> create_copy_reversed() {
        std::vector<T> res(_m_buf.rbegin() + (_m_buf.size() - head), _m_buf.rend());
        for (int i = (_m_buf.size() - 1); i >= head; --i) { res.push_back(_m_buf[i]); }
        return res;
    }

    T get_cur() const { return _m_buf[nextRead_ID]; }
    T get_cur_and_next() {
        T res = get_cur();
        advanceByOne();
        return res;
    }
    T get_cur_and_advanceBy(size_t by = 1) {
        T res = get_cur();
        advanceBy(by);
        return res;
    }

    void inline advanceByOne() { nextRead_ID = (nextRead_ID + 1) % _m_buf.size(); }
    void inline advanceBy(int by = 1) { nextRead_ID = (nextRead_ID + by) % _m_buf.size(); }

    // On insertion resets nextRead_ID to head as well
    void insertAtHead(T &&item) { insertAtHead(item); }
    void insertAtHead(T const &item) {
        _m_buf[head] = item;
        head         = (head + 1) % _m_buf.size();
        nextRead_ID  = head;
    }
};


} // namespace detail


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

        // Trim input 'string like' of all 'newline' chars at the end
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
                // TODO: Finally figure out how to handle exceptions somewhat professionally
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
        if (dp.plot_type_name.has_value()) { return dp; }

        if (dp.values_colIDs.size() > 3) { return std::unexpected(Err_plotSpecs::valCols); }
        else if (dp.values_colIDs.size() < 2) { dp.plot_type_name = detail::TypeToString<plot_structures::BarV>(); }
        else if (dp.values_colIDs.size() == 3 && (not dp.label_colID.has_value())) {
            dp.plot_type_name = detail::TypeToString<plot_structures::Bubble>();
        }
        else if (dp.values_colIDs.size() == 2 && (not dp.label_colID.has_value())) {
            dp.plot_type_name = detail::TypeToString<plot_structures::Scatter>();
        }
        else { dp.plot_type_name = detail::TypeToString<plot_structures::Multiline>(); }

        return std::move(dp);
    }
    static std::expected<DesiredPlot, Err_plotSpecs> guess_labelCol(DesiredPlot &dp, DataStore const &ds) {
        if (dp.label_colID.has_value()) { return std::move(dp); }
        else if (dp.plot_type_name != detail::TypeToString<plot_structures::BarV>()) { return std::move(dp); }

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
        if (dp.plot_type_name == detail::TypeToString<plot_structures::BarV>()) {
            if (dp.values_colIDs.size() > 1) { return std::unexpected(Err_plotSpecs::valCols); }
            else if (not addValColsUntil(1).has_value()) { return std::unexpected(Err_plotSpecs::guessValCols); }
        }
        if (dp.plot_type_name == detail::TypeToString<plot_structures::BarH>()) {
            if (dp.values_colIDs.size() > 1) { return std::unexpected(Err_plotSpecs::valCols); }
            else if (not addValColsUntil(1).has_value()) { return std::unexpected(Err_plotSpecs::guessValCols); }
        }
        // LINE PLOTS
        else if (dp.plot_type_name == detail::TypeToString<plot_structures::Line>()) {
            if (dp.values_colIDs.size() > 1) { return std::unexpected(Err_plotSpecs::valCols); }
            else if (not addValColsUntil(1).has_value()) { return std::unexpected(Err_plotSpecs::guessValCols); }
        }
        else if (dp.plot_type_name == detail::TypeToString<plot_structures::Multiline>()) {
            if (dp.values_colIDs.size() > 5) { return std::unexpected(Err_plotSpecs::valCols); }
            else if (not addValColsUntil(2).has_value()) { return std::unexpected(Err_plotSpecs::guessValCols); }
        }

        // SCATTER PLOT
        else if (dp.plot_type_name == detail::TypeToString<plot_structures::Scatter>()) {
            if (dp.values_colIDs.size() > 2) { return std::unexpected(Err_plotSpecs::valCols); }
            else if (not addValColsUntil(2).has_value()) { return std::unexpected(Err_plotSpecs::guessValCols); }
        }
        // BUBBLE PLOT
        else if (dp.plot_type_name == detail::TypeToString<plot_structures::Bubble>()) {
            if (dp.values_colIDs.size() > 3) { return std::unexpected(Err_plotSpecs::valCols); }
            else if (not addValColsUntil(3).has_value()) { return std::unexpected(Err_plotSpecs::guessValCols); }
        }
        return std::move(dp);
    }

public:
    std::optional<std::string> plot_type_name;

    std::optional<size_t>      label_colID; // ID in colTypes
    std::optional<std::string> label_colName;

    std::vector<size_t>      values_colIDs; // IDs in colTypes
    std::vector<std::string> values_colNames;


    DesiredPlot() {}
    DesiredPlot(std::string plot_type_name, std::optional<size_t> l_colID, std::optional<std::string> l_colName,
                std::vector<size_t> v_colIDs, std::vector<std::string> v_colNames)
        : plot_type_name(std::move(plot_type_name)), label_colID(std::move(l_colID)),
          label_colName(std::move(l_colName)), values_colIDs(std::move(v_colIDs)),
          values_colNames(std::move(v_colNames)) {}

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
// it) and overriding pure virtual functions. The types properly derived from 'Base' can then be used inside
// 'PlotDrawer' inside std::variant<...>. The idea is to be able to easily customize and also possibly 'partially
// customize' as needed You always have to make the 'Base' class a friend ... this enables really nice dynamic
// polymorphism coupled with 'deducing this' feature of C++23
class Base {
protected:
    // Descriptors - First thing to be computed
    size_t areaWidth = 0, areaHeight = 0;
    size_t labels_verLeftWidth = 0, labels_verRightWidth = 0;

    size_t axis_verLeftSteps = 1, axis_varRightSteps = 1, axis_horTopSteps = 1, axis_horBottomSteps = 1;

    size_t pad_left = 2, pad_right = 0, pad_top = 0, pad_bottom = 0;

    bool labels_horTop_bool = false, labels_horBottom_bool = false;
    bool axisName_horTop_bool = false, axisName_horBottom_bool = false;


    // Actual structure
    std::vector<std::string> labels_verLeft;
    std::vector<std::string> axis_verLeft;

    std::vector<std::string> axis_verRight;
    std::vector<std::string> labels_verRight;

    std::string              axisName_horTop;
    std::string              label_horTop;
    std::vector<std::string> axis_horTop;

    std::vector<std::string> axis_horBottom;
    std::string              label_horBottom;
    std::string              axisName_horBottom;

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
        size_t lngth = pad_top + pad_bottom;

        // Horizontal top axis name and axis labels lines
        lngth += axisName_horTop_bool ? (axisName_horTop.size() + corner_topLeft.front().size() +
                                         corner_topRight.front().size() + pad_left + pad_right)
                                      : 0;
        lngth += labels_horTop_bool ? (label_horTop.size() + corner_topLeft.front().size() +
                                       corner_topRight.front().size() + pad_left + pad_right)
                                    : 0;

        // First and last vertical labels
        lngth += labels_verLeft.front().size() + labels_verRight.front().size() + labels_verLeft.back().size() +
                 labels_verRight.back().size();

        // The 'corners'
        lngth += sizeof("┌") + sizeof("┐") + sizeof("└") + sizeof("┘") - 4;

        // All top and bottom axes
        for (int i = 0; i < areaWidth; i++) { lngth += (axis_horTop.at(i).size() + axis_horBottom.at(i).size()); }

        // Main plot area
        for (int i = 0; i < areaHeight; ++i) {
            lngth += labels_verLeft.at(i + 1).size() + labels_verRight.at(i + 1).size();
            lngth += axis_verLeft.at(i).size();
            lngth += axis_verRight.at(i).size();
            lngth += plotArea.at(i).size();
        }

        // Horizontal bottom axis name and axis labels lines
        lngth += labels_horBottom_bool ? (label_horBottom.size() + corner_bottomLeft.front().size() +
                                          corner_bottomRight.front().size() + pad_left + pad_right)
                                       : 0;
        lngth += axisName_horBottom_bool ? (axisName_horBottom.size() + corner_bottomLeft.front().size() +
                                            corner_bottomRight.front().size() + pad_left + pad_right)
                                         : 0;
        return lngth;
    }

    std::string build_plotAsString() const {
        std::string result;
        result.reserve(compute_lengthOfSelf());

        for (int i = 0; i < pad_top; ++i) { result.push_back('\n'); }

        // Build the heading lines of the plot
        if (axisName_horTop_bool) {
            result.append(std::string(pad_left, ' '));
            result.append(corner_topLeft.at(0));
            result.append(label_horTop);
            result.append(corner_topRight.at(0));
            result.append(std::string(pad_right, ' '));
            result.push_back('\n');
        }
        if (labels_horTop_bool) {
            result.append(std::string(pad_left, ' '));
            result.append(corner_topLeft.at(1));
            result.append(label_horTop);
            result.append(corner_topRight.at(1));
            result.append(std::string(pad_right, ' '));
            result.push_back('\n');
        }

        result.append(std::string(pad_left, ' '));
        result.append(labels_verLeft.front());
        result.append("┌");
        for (auto const &toAppend : axis_horTop) { result.append(toAppend); }
        result.append("┐");
        result.append(labels_verRight.front());
        result.append(std::string(pad_right, ' '));
        result.push_back('\n');


        for (size_t i = 1; i < (areaHeight + 1); ++i) {
            result.append(std::string(pad_left, ' '));
            result.append(labels_verLeft.at(i));
            result.append(axis_verLeft.at(i - 1));
            result.append(plotArea.at(i - 1));
            result.append(axis_verRight.at(i - 1));
            result.append(labels_verRight.at(i));
            result.append(std::string(pad_right, ' '));
            result.push_back('\n');
        }

        result.append(std::string(pad_left, ' '));
        result.append(labels_verLeft.back());
        result.append("└");
        for (auto const &toAppend : axis_horBottom) { result.append(toAppend); }
        result.append("┘");
        result.append(labels_verRight.back());
        result.append(std::string(pad_right, ' '));
        result.push_back('\n');

        if (labels_horBottom_bool) {
            result.append(std::string(pad_left, ' '));
            result.append(corner_bottomLeft.at(0));
            result.append(label_horBottom);
            result.append(corner_bottomRight.at(0));
            result.append(std::string(pad_right, ' '));
            result.push_back('\n');
        }
        if (axisName_horBottom_bool) {
            result.append(std::string(pad_left, ' '));
            result.append(corner_bottomLeft.at(1));
            result.append(axisName_horBottom);
            result.append(corner_bottomRight.at(1));
            result.append(std::string(pad_right, ' '));
            result.push_back('\n');
        }

        for (int i = 0; i < pad_bottom; ++i) { result.push_back('\n'); }
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

    virtual void compute_axis_ht(DesiredPlot const &dp, DataStore const &ds)       = 0;
    virtual void compute_axisName_ht(DesiredPlot const &dp, DataStore const &ds)   = 0;
    virtual void compute_axisLabels_ht(DesiredPlot const &dp, DataStore const &ds) = 0;

    virtual void compute_axis_hb(DesiredPlot const &dp, DataStore const &ds)       = 0;
    virtual void compute_axisName_hb(DesiredPlot const &dp, DataStore const &ds)   = 0;
    virtual void compute_axisLabels_hb(DesiredPlot const &dp, DataStore const &ds) = 0;

    virtual void compute_plot_area(DesiredPlot const &dp, DataStore const &ds) = 0;
};

class BarV : public Base {
    friend class Base;

    virtual void compute_descriptors(DesiredPlot const &dp, DataStore const &ds, size_t const &tar_width,
                                     size_t const &tar_height) override {
        // Vertical left labels
        if (dp.plot_type_name == detail::TypeToString<plot_structures::BarV>()) {
            auto const &labelColRef = ds.stringCols.at(ds.colTypes.at(dp.label_colID.value()).second);
            auto const  labelSizes  = std::views::transform(labelColRef, [](auto const &a) { return a.size(); });
            labels_verLeftWidth =
                std::min(30uz, std::min(std::ranges::max(labelSizes), (tar_width - pad_left - pad_right) / 4));
        }
        // TODO: Computation for numeric labels
        else {}

        // Vertical right labels ... probably nothing so keeping 0 size
        // ...

        // Plot area width (-2 is for the 2 vertical axes positions)
        areaWidth = tar_width - pad_left - labels_verLeftWidth - 2 - labels_verRightWidth - pad_right;

        // Labels and axis name bottom
        if (dp.plot_type_name == detail::TypeToString<plot_structures::BarH>() ||
            (dp.plot_type_name == detail::TypeToString<plot_structures::Line>() ||
             dp.plot_type_name == detail::TypeToString<plot_structures::Multiline>()) &&
                true) {} // TODO:  Proper assessment for Line and ML
        else {
            labels_horBottom_bool = true;
            axisName_horBottom    = true;
        }

        // Labels and axis name top ... probably nothing so keeping 0 size
        // ...

        // Plot area height (-2 is for the 2 horizontal axes positions)
        if (dp.plot_type_name == detail::TypeToString<plot_structures::BarV>()) {
            areaHeight = ds.stringCols.at(ds.colTypes.at(dp.label_colID.value()).second).size();
        }
        else {
            areaHeight = tar_height - pad_top - axisName_horTop_bool - labels_horTop_bool - 2 - labels_horBottom_bool -
                         axisName_horBottom_bool - pad_bottom;
        }

        // Axes steps
        if (dp.plot_type_name == detail::TypeToString<plot_structures::BarV>()) { axis_verLeftSteps = areaHeight; }
        else {} // TODO: Computation for numeric labels


        if (dp.plot_type_name == detail::TypeToString<plot_structures::BarH>()) { axis_horBottomSteps = areaWidth; }
        else {} // TODO: Computation for numeric labels

        // Top and Right axes steps keeping as-is
    }

    virtual void compute_labels_vl(DesiredPlot const &dp, DataStore const &ds) override {
        for (auto const &rawLabel : ds.stringCols.at(ds.colTypes.at(dp.label_colID.value()).second)) {
            labels_verLeft.push_back(detail::middleTrim2Size(rawLabel, labels_verLeftWidth));
        }
    }
    virtual void compute_labels_vr(DesiredPlot const &dp, DataStore const &ds) override {
        for (auto const &_ : ds.stringCols.at(ds.colTypes.at(dp.label_colID.value()).second)) {
            labels_verRight.push_back("");
        }
    }

    virtual void compute_axis_vl(DesiredPlot const &dp, DataStore const &ds) override {
        if (dp.plot_type_name == detail::TypeToString<plot_structures::BarV>()) {
            axis_verLeft = detail::create_tickAxis("│", "┤", areaHeight, areaHeight);
        }
        // All else should have vl axis ticks according to numeric values
        else {}
    }
    virtual void compute_axis_vr(DesiredPlot const &dp, DataStore const &ds) override {
        if (dp.plot_type_name == detail::TypeToString<plot_structures::BarV>()) {
            axis_verRight = std::vector(areaHeight, std::string(" "));
        }
    }

    // All corners are simply empty as default ... but can possibly be used for something later if overrided in derived
    virtual void compute_corner_tl(DesiredPlot const &dp, DataStore const &ds) override {
        if (axisName_horTop_bool) { corner_topLeft.push_back(std::string(labels_verLeftWidth, ' ')); }
        if (labels_horTop_bool) { corner_topLeft.push_back(std::string(labels_verLeftWidth, ' ')); }
    }
    virtual void compute_corner_bl(DesiredPlot const &dp, DataStore const &ds) override {
        if (axisName_horBottom_bool) { corner_bottomLeft.push_back(std::string(labels_verLeftWidth, ' ')); }
        if (labels_horBottom_bool) { corner_bottomLeft.push_back(std::string(labels_verLeftWidth, ' ')); }
    }
    virtual void compute_corner_br(DesiredPlot const &dp, DataStore const &ds) override {
        if (axisName_horTop_bool) { corner_topRight.push_back(std::string(labels_verRightWidth, ' ')); }
        if (labels_horTop_bool) { corner_topRight.push_back(std::string(labels_verRightWidth, ' ')); }
    }
    virtual void compute_corner_tr(DesiredPlot const &dp, DataStore const &ds) override {
        if (axisName_horBottom_bool) { corner_bottomRight.push_back(std::string(labels_verRightWidth, ' ')); }
        if (labels_horBottom_bool) { corner_bottomRight.push_back(std::string(labels_verRightWidth, ' ')); }
    }

    virtual void compute_axis_ht(DesiredPlot const &dp, DataStore const &ds) override {
        axis_horTop = std::vector(areaWidth, std::string(" "));
    }
    virtual void compute_axisName_ht(DesiredPlot const &dp, DataStore const &ds) override {}
    virtual void compute_axisLabels_ht(DesiredPlot const &dp, DataStore const &ds) override {}


    virtual void compute_axis_hb(DesiredPlot const &dp, DataStore const &ds) override {
        if (dp.plot_type_name == detail::TypeToString<plot_structures::BarH>()) {
            axis_horBottom = detail::create_tickAxis("─", "┬", areaWidth, areaWidth);
        }
        // All else should be values according to value col #1
        // TODO: Fix this so there are ticks by values ... must make specialized method for that
        else { axis_horBottom = std::vector(areaWidth, std::string(" ")); }
    }

    virtual void compute_axisName_hb(DesiredPlot const &dp, DataStore const &ds) override {
        if (dp.plot_type_name == detail::TypeToString<plot_structures::BarH>()) {
            // TODO: What to do with BarHs axisName bottom
        }
        else { std::string const anRef = ds.colNames.at(dp.values_colIDs.front()); }
    }
    virtual void compute_axisLabels_hb(DesiredPlot const &dp, DataStore const &ds) override {}

    virtual void compute_plot_area(DesiredPlot const &dp, DataStore const &ds) override {
        auto const &valColTypeRef = ds.colTypes.at(dp.values_colIDs.front());
        if (valColTypeRef.first == nlohmann::detail::value_t::number_float) {
            auto const &valColRef   = ds.doubleCols.at(valColTypeRef.second);
            auto const [minV, maxV] = std::ranges::minmax(valColRef);
            double stepSize         = (maxV - minV) / areaWidth;
            for (auto const &val : valColRef) {
                plotArea.push_back(std::string());
                size_t rpt = static_cast<size_t>((val - minV) / stepSize);
                for (size_t i = rpt; i > 0; --i) { plotArea.back().append("■"); }
                for (size_t i = rpt; i < areaWidth; ++i) { plotArea.back().push_back(' '); }
            }
        }
        else {
            auto const &valColRef   = ds.llCols.at(valColTypeRef.second);
            auto const [minV, maxV] = std::ranges::minmax(valColRef);
            long long scalingFactor = LONG_LONG_MAX / (std::max(std::abs(maxV), std::abs(minV)));
            long long maxV_adj      = maxV * scalingFactor;
            long long minV_adj      = minV * scalingFactor;
            long long stepSize      = (maxV_adj - minV_adj) / areaWidth;

            for (auto const &val : valColRef) {
                plotArea.push_back(std::string());
                long long rpt = (val * scalingFactor - minV_adj) / stepSize;
                for (long long i = rpt; i > 0; --i) { plotArea.back().append("■"); }
                for (long long i = rpt; i < areaWidth; ++i) { plotArea.back().push_back(' '); }
            }
        }
    }
};

class BarH : public BarV {
    friend class Base;

    virtual void compute_axis_ht(DesiredPlot const &dp, DataStore const &ds) override {
        axis_horBottomSteps = areaWidth;
    }
};

class Line : public BarV {
    friend class Base;
};

class Multiline : public Line {
    friend class Base;
};

class Scatter : public BarV {
    friend class Base;
};

class Bubble : public Scatter {
    friend class Base;
};

} // namespace plot_structures


template <typename PS>
requires std::is_base_of_v<plot_structures::Base, PS>
class PlotDrawer {
private:
    PS plotStructure;

public:
    constexpr PlotDrawer() {};
    PlotDrawer(DesiredPlot const &dp, DataStore const &ds, size_t tar_width, size_t tar_height) {
        plotStructure.build_self(dp, ds, tar_width, tar_height);
    }

    void update_newPlotStructure(DesiredPlot const &dp, DataStore const &ds, size_t tar_width, size_t tar_height) {
        plotStructure = PS();
        plotStructure.build_self(dp, ds, tar_width, tar_height);
    }

    std::expected<std::string, Err_drawer> validateAndDrawPlot() const {
        // TODO: Add some validation before drawing
        if (plotStructure.validate_self() == false) { return std::unexpected(Err_drawer::plotStructureInvalid); }
        else { return drawPlot(); }
    }
    std::string drawPlot() const { return plotStructure.build_plotAsString(); }
};


template <typename... Ts>
requires(std::is_base_of_v<plot_structures::Base, Ts>, ...) && detail::none_sameLastLevelTypeName<Ts...>
constexpr auto generate_PD_PS_variantTypeMap() {

    std::unordered_map<std::string, std::variant<PlotDrawer<Ts>...>> res;
    (res.insert({detail::TypeToString<Ts>(), std::variant<PlotDrawer<Ts>...>(PlotDrawer<Ts>())}), ...);
    return res;
}

// Pass the 'plot_structure' template types that should be used by the library
// This is the only where one 'selects' these template types
static const auto mp_names2Types =
    generate_PD_PS_variantTypeMap<plot_structures::BarV, plot_structures::BarH, plot_structures::Line,
                                  plot_structures::Multiline, plot_structures::Scatter, plot_structures::Bubble>();

inline decltype(mp_names2Types)::mapped_type make_plotDrawer(DesiredPlot const &dp, DataStore const &ds,
                                                             size_t tar_width, size_t tar_height) {
    auto ref          = mp_names2Types.at(dp.plot_type_name.value());
    auto overload_set = [&]<typename T>(T &variantItem) -> decltype(ref) { return T(dp, ds, tar_width, tar_height); };
    return std::visit(overload_set, ref);
}


} // namespace terminal_plot
} // namespace incom

namespace incplot = incom::terminal_plot;