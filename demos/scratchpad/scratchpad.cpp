#define OOF_IMPL
#include <print>
#include <windows.h>

#include <incplot.hpp>


int main() {
    using json = nlohmann::json;

    // HANDLE hIn  = GetStdHandle(STD_INPUT_HANDLE);
    // DWORD  type = GetFileType(hIn);
    // switch (type) {
    //     case FILE_TYPE_PIPE: break;
    //     default:
    //         std::cout << "STD INPUT is not 'pipe' ... exiting";⠇
    //         return 1;
    //         break;
    // }

    // std::string input((std::istreambuf_iterator(std::cin)), std::istreambuf_iterator<char>());

    std::string testInput(R"({"name":"autosave232.whs","size":4117646}
{"name":"autosave233.whs","size":4116614}
{"name":"autosave234.whs","size":4110969}
{"name":"autosave235.whs","size":4079573}
{"name":"autosave236.whs","size":4092053}
{"name":"autosave237.whs","size":4211498}
{"name":"autosave238.whs","size":4166587}
{"name":"autosave240.whs","size":4156290}
{"name":"autosave242.whs","size":4236054}
{"name":"autosave244.whs","size":4202428}
{"name":"autosave245.whs","size":4207673}
{"name":"autosave246.whs","size":4208191}
{"name":"autosave247.whs","size":4232827}
{"name":"autosave248.whs","size":4245377}
{"name":"autosave250.whs","size":4181332}
{"name":"autosave251.whs","size":4219387}
{"name":"autosave253.whs","size":4223419}
{"name":"autosave255.whs","size":4195367}
{"name":"autosave256.whs","size":4245552}
{"name":"autosave257.whs","size":4292180}
{"name":"autosave259.whs","size":4138110}
{"name":"autosave260.whs","size":4150626}
{"name":"autosave261.whs","size":4200034}
{"name":"autosave263.whs","size":4250089}
{"name":"autosave265.whs","size":4295548}
{"name":"autosave266.whs","size":4389820}
{"name":"autosave267.whs","size":4280101}
{"name":"autosave269.whs","size":4231126}
{"name":"autosave270.whs","size":4262373}
{"name":"autosave272.whs","size":4249694}
{"name":"autosave274.whs","size":4239565}
{"name":"autosave275.whs","size":4146376}
{"name":"autosave278.whs","size":4206243}
{"name":"autosave280.whs","size":4162941}
{"name":"autosave282.whs","size":4144082}
{"name":"autosave284.whs","size":4293325}
{"name":"autosave285.whs","size":4345864}
{"name":"autosave286.whs","size":4340816}
{"name":"autosave288.whs","size":4342698}
{"name":"autosave289.whs","size":4338326}
{"name":"autosave290.whs","size":4420890}
{"name":"autosave291.whs","size":4417215}
{"name":"autosave292.whs","size":4392064}
{"name":"autosave294.whs","size":4432822}
{"name":"autosave295.whs","size":4408701}
{"name":"autosave296.whs","size":4262003}
{"name":"autosave297.whs","size":4291928}
{"name":"autosave299.whs","size":4215392}
{"name":"autosave300.whs","size":4230959}
{"name":"autosave301.whs","size":4245696}
{"name":"autosave302.whs","size":4227377}
{"name":"autosave303.whs","size":4253392}
{"name":"autosave304.whs","size":4262521}
{"name":"autosave305.whs","size":4330042}
{"name":"autosave307.whs","size":4291721}
{"name":"autosave308.whs","size":4310169}
{"name":"autosave309.whs","size":4295069}
{"name":"autosave311.whs","size":4302809}
{"name":"autosave312.whs","size":4322336}
{"name":"autosave313.whs","size":4336742}
{"name":"autosave316.whs","size":4277744}
{"name":"autosave317.whs","size":4199732}
{"name":"autosave318.whs","size":4216254}
{"name":"autosave321.whs","size":4289484}
{"name":"autosave322.whs","size":4289343}
{"name":"autosave323.whs","size":4267637}
{"name":"autosave324.whs","size":4214194}
{"name":"autosave326.whs","size":4213750}
{"name":"autosave327.whs","size":4195577}
{"name":"autosave328.whs","size":4256089}
{"name":"autosave330.whs","size":4354501}
{"name":"autosave331.whs","size":4350050}
{"name":"autosave332.whs","size":4279019}
{"name":"autosave335.whs","size":4333798}
{"name":"autosave336.whs","size":4340955}
{"name":"autosave338.whs","size":4220807}
{"name":"autosave339.whs","size":4233364}
{"name":"autosave340.whs","size":4221096}
{"name":"autosave341.whs","size":4179085}
{"name":"autosave344.whs","size":4283240}
{"name":"autosave345.whs","size":4226667}
{"name":"autosave346.whs","size":4258213}
{"name":"autosave347.whs","size":4291250}
{"name":"autosave348.whs","size":4326273}
{"name":"autosave349.whs","size":4372842}
{"name":"autosave351.whs","size":4362411}
{"name":"autosave353.whs","size":4332849}
{"name":"autosave354.whs","size":4363892}
{"name":"autosave355.whs","size":4403513}
{"name":"autosave356.whs","size":4367163}
{"name":"autosave357.whs","size":4239000}
{"name":"autosave360.whs","size":4455458}
{"name":"autosave361.whs","size":4471815}
{"name":"autosave363.whs","size":4323912}
{"name":"autosave364.whs","size":4294252}
{"name":"autosave365.whs","size":4294467}
{"name":"autosave366.whs","size":4285157}
{"name":"autosave367.whs","size":4261399}
{"name":"autosave368.whs","size":4245180}
{"name":"autosave370.whs","size":4168128}
{"name":"exit.whs","size":4450367}
{"name":"permanent001.whs","size":3136092}
{"name":"permanent002.whs","size":1114779}
{"name":"permanent007.whs","size":1202256}
{"name":"permanent011.whs","size":1239148}
{"name":"permanent016.whs","size":1267861}
{"name":"permanent040.whs","size":1339828}
{"name":"permanent090.whs","size":1632258}
{"name":"permanent097.whs","size":1674780}
{"name":"permanent105.whs","size":1712758}
{"name":"permanent112.whs","size":1679148}
{"name":"permanent120.whs","size":1870788}
{"name":"permanent125.whs","size":1726595}
{"name":"permanent134.whs","size":1723046}
{"name":"permanent148.whs","size":3428406}
{"name":"permanent151.whs","size":4059478}
{"name":"permanent160.whs","size":3736302}
{"name":"permanent183.whs","size":3988032}
{"name":"permanent239.whs","size":4151241}
{"name":"permanent262.whs","size":4229978}
{"name":"permanent283.whs","size":4146291}
{"name":"permanent293.whs","size":4427175}
{"name":"permanent298.whs","size":4264056}
{"name":"permanent310.whs","size":4300401}
{"name":"permanent334.whs","size":4316095}
{"name":"permanent342.whs","size":4223556}
{"name":"permanent359.whs","size":4456939}
{"name":"permanent369.whs","size":4121743}
{"name":"permanent371.whs","size":4175947}
{"name":"save027.whs","size":1387789}
{"name":"save050.whs","size":1383595}
{"name":"save065.whs","size":1459451}
{"name":"save075.whs","size":1505055}
{"name":"save077.whs","size":1545015}
{"name":"save128.whs","size":1787164}
{"name":"save137.whs","size":1731932}
{"name":"save140.whs","size":1738934}
{"name":"save147.whs","size":1773703}
{"name":"save191.whs","size":4011247}
{"name":"save207.whs","size":4012823}
{"name":"save211.whs","size":4036672}
{"name":"save230.whs","size":4160995}
{"name":"save241.whs","size":4213406}
{"name":"save252.whs","size":4230128}
{"name":"save258.whs","size":4178449}
{"name":"save268.whs","size":4234040}
{"name":"save271.whs","size":4237198}
{"name":"save273.whs","size":4243164}
{"name":"save276.whs","size":4145803}
{"name":"save277.whs","size":4168679}
{"name":"save279.whs","size":4148630}
{"name":"save281.whs","size":4111407}
{"name":"save314.whs","size":4348612}
{"name":"save319.whs","size":4211578}
{"name":"save320.whs","size":4318134}
{"name":"save325.whs","size":4210785}
{"name":"save329.whs","size":4343913}
{"name":"save333.whs","size":4337736}
{"name":"save343.whs","size":4279248}
{"name":"save350.whs","size":4343389})");

    auto ds = incplot::Parser::parse_NDJSON_intoDS(testInput);


    auto dp_autoGuessed = incplot::DesiredPlot(64).guess_missingParams(ds);

    if (not dp_autoGuessed.has_value()) {
        std::print("{0}{1}", "Autoguessing of 'DesiresPlot' parameters failed \n", "Exiting ...");
        return 1;
    }

    std::string aaar("\u25a0");


    auto plotDrawer2 = incplot::make_plotDrawer(dp_autoGuessed.value(), ds);

    auto outExp = plotDrawer2.validateAndDrawPlot();


    if (not outExp.has_value()) {
        std::print("{0}{1}", "Invalid plot structure", "Exiting ...");
        return 1;
    }

    auto [val, unit] = incplot::detail::rebase_2_SIPrefix(0.001);


    std::print("{}\n", outExp.value());

    std::print("{}\n", incplot::detail::format_toMax4length(0.00015836));

    std::print("{}Viridis {}Reset \n", incplot::TermColors::get_basicColor(incplot::Color_CVTS::Foreground_Green), incplot::TermColors::get_basicColor(incplot::Color_CVTS::Default));
    std::print("{}Reduta {}Reset \n", incplot::TermColors::get_basicColor(incplot::Color_CVTS::Foreground_Red), incplot::TermColors::get_basicColor(incplot::Color_CVTS::Default));
    std::print("{}Tanza {}Reset \n", incplot::TermColors::get_basicColor(incplot::Color_CVTS::Foreground_Blue), incplot::TermColors::get_basicColor(incplot::Color_CVTS::Default));

    return 0;
}