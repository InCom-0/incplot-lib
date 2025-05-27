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
// TODO: Would it be possible to cleave he parser from the library so that it is easier to customize later?
// TODO: How would one do the above in a reasonable manner?
namespace parsers {

using incerr_c = incerr::incerr_code;

// ENUMS
enum class input_t {
    NDJSON,
    JSON,
    CSV,
    TSV
};
enum class csvCellType {
    double_like,
    ll_like,
    string_like
};

class Parser {

    // TYPE ALIAS
    using parser_return_t = std::expected<DataStore::vec_pr_strVarVec_t, incerr_c>;

    // HLPRS
    static std::string_view get_trimmedSV(std::string_view const &sv);

    static csvCellType assess_cellType(auto const &csvCell);

    static double      conv_cellToDouble(auto const &csvCell);
    static long long   conv_cellToLongLong(auto const &csvCell);
    static std::string conv_cellToString(auto const &csvCell);


    // COMPOSITION METHODS
    // TODO: Maybe make this public for later use?
    static std::expected<input_t, incerr_c> assess_inputType(std::string_view const &sv);
    static parser_return_t                      dispatch_toParsers(input_t const &inp_t, std::string_view const &sv);

    // PARSE USING RANAV::CSV2
    static parser_return_t parse_usingCSV2(auto &&csv2Reader, std::string_view const trimmed);

public:
    // MAIN INTENDED INTERFACE METHOD
    // Dispatches the string_view to the right parser and constructs DataStore
    static std::expected<DataStore, incerr_c> parse(std::string_view const sv);

    // JSON AND NDJSON
    static parser_return_t parse_NDJSON(std::string_view const &trimmed);
    static parser_return_t parse_JSON(std::string_view const &trimmed);

    // CSV AND TSV
    static parser_return_t parse_CSV(std::string_view const sv_like);
    static parser_return_t parse_TSV(std::string_view const sv_like);
};
} // namespace parsers
} // namespace terminal_plot
} // namespace incom