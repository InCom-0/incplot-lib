#pragma once

#include <expected>
#include <string_view>

#include <incplot/datastore.hpp>
#include <incplot/err.hpp>


namespace incom {
namespace terminal_plot {

// Encapsulates parsing of the input into DataStore
// Validates 'hard' errors during parsing
// Validates that input data is not structured 'impossibly' (missing values, different value names per record, etc.)
namespace parsers {

using incerr_c = incerr::incerr_code;

// ENUMS
enum class input_t {
    NDJSON,
    JSON,
    CSV,
    TSV
};
enum class CellType {
    null_like,
    double_like,
    ll_like,
    string_like
};

class INCPLOT_LIB_API Parser {

    // TYPE ALIAS
    using parser_return_t = std::expected<DataStore::vec_pr_varCol_t, incerr_c>;

    // HLPRS
    static std::string_view get_trimmedSV(std::string_view const &sv);

    static CellType assess_cellType(auto const &csvCell);

    static double      conv_cellToDouble(auto const &csvCell);
    static long long   conv_cellToLongLong(auto const &csvCell);
    static std::string conv_cellToString(auto const &csvCell);


    // COMPOSITION METHODS
    static std::expected<input_t, incerr_c>               assess_inputType(std::string_view const &sv);
    static std::expected<DataStore::DS_CtorObj, incerr_c> dispatch_toParsers(input_t const          &inp_t,
                                                                             std::string_view const &sv);

    // PARSE USING RANAV::CSV2
    static std::expected<DataStore::DS_CtorObj, incerr_c> parse_usingCSV2(auto                 &&csv2Reader,
                                                                          std::string_view const trimmed);


public:
    // MAIN INTENDED INTERFACE METHOD
    // Dispatches the string_view to the right parser and constructs DataStore
    static std::expected<DataStore, incerr_c> parse(std::string_view const sv);

    // JSON AND NDJSON
    static std::expected<DataStore::DS_CtorObj, incerr_c> parse_NDJSON(std::string_view const &trimmed);
    static std::expected<DataStore::DS_CtorObj, incerr_c> parse_JSON(std::string_view const &trimmed);

    // CSV AND TSV
    static std::expected<DataStore::DS_CtorObj, incerr_c> parse_CSV(std::string_view const sv_like);
    static std::expected<DataStore::DS_CtorObj, incerr_c> parse_TSV(std::string_view const sv_like);
};
} // namespace parsers
} // namespace terminal_plot
} // namespace incom