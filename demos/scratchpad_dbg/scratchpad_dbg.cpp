#include <print>
#include <string>

#include <incplot.hpp>
#include <string_view>


int main(int argc, char *argv[]) {

    std::string irisTSV_2025(R"({"sepal_length":5.1,"sepal_width":3.5,"petal_length":1.4,"petal_width":0.2,"species":"Iris-setosa"}
{"sepal_length":4.9,"sepal_width":3,"petal_length":1.4,"petal_width":0.2,"species":"Iris-setosa"}
{"sepal_length":4.7,"sepal_width":3.2,"petal_length":1.3,"petal_width":0.2,"species":"Iris-setosa"}
{"sepal_length":4.6,"sepal_width":3.1,"petal_length":1.5,"petal_width":0.2,"species":"Iris-setosa"}
{"sepal_length":5,"sepal_width":3.6,"petal_length":1.4,"petal_width":0.2,"species":"Iris-setosa"}
{"sepal_length":5.4,"sepal_width":3.9,"petal_length":1.7,"petal_width":0.4,"species":"Iris-setosa"}
{"sepal_length":4.6,"sepal_width":3.4,"petal_length":1.4,"petal_width":0.3,"species":"Iris-setosa"}
{"sepal_length":5,"sepal_width":3.4,"petal_length":1.5,"petal_width":0.2,"species":"Iris-setosa"}
{"sepal_length":4.4,"sepal_width":2.9,"petal_length":1.4,"petal_width":0.2,"species":"Iris-setosa"}
{"sepal_length":4.9,"sepal_width":3.1,"petal_length":1.5,"petal_width":0.1,"species":"Iris-setosa"}
{"sepal_length":5.4,"sepal_width":3.7,"petal_length":1.5,"petal_width":0.2,"species":"Iris-setosa"}
{"sepal_length":4.8,"sepal_width":3.4,"petal_length":1.6,"petal_width":0.2,"species":"Iris-setosa"}
{"sepal_length":4.8,"sepal_width":3,"petal_length":1.4,"petal_width":0.1,"species":"Iris-setosa"}
{"sepal_length":4.3,"sepal_width":3,"petal_length":1.1,"petal_width":0.1,"species":"Iris-setosa"}
{"sepal_length":5.8,"sepal_width":4,"petal_length":1.2,"petal_width":0.2,"species":"Iris-setosa"}
{"sepal_length":5.7,"sepal_width":4.4,"petal_length":1.5,"petal_width":0.4,"species":"Iris-setosa"}
{"sepal_length":5.4,"sepal_width":3.9,"petal_length":1.3,"petal_width":0.4,"species":"Iris-setosa"}
{"sepal_length":5.1,"sepal_width":3.5,"petal_length":1.4,"petal_width":0.3,"species":"Iris-setosa"}
{"sepal_length":5.7,"sepal_width":3.8,"petal_length":1.7,"petal_width":0.3,"species":"Iris-setosa"}
{"sepal_length":5.1,"sepal_width":3.8,"petal_length":1.5,"petal_width":0.3,"species":"Iris-setosa"}
{"sepal_length":5.4,"sepal_width":3.4,"petal_length":1.7,"petal_width":0.2,"species":"Iris-setosa"}
{"sepal_length":5.1,"sepal_width":3.7,"petal_length":1.5,"petal_width":0.4,"species":"Iris-setosa"}
{"sepal_length":4.6,"sepal_width":3.6,"petal_length":1,"petal_width":0.2,"species":"Iris-setosa"}
{"sepal_length":5.1,"sepal_width":3.3,"petal_length":1.7,"petal_width":0.5,"species":"Iris-setosa"}
{"sepal_length":4.8,"sepal_width":3.4,"petal_length":1.9,"petal_width":0.2,"species":"Iris-setosa"}
{"sepal_length":5,"sepal_width":3,"petal_length":1.6,"petal_width":0.2,"species":"Iris-setosa"}
{"sepal_length":5,"sepal_width":3.4,"petal_length":1.6,"petal_width":0.4,"species":"Iris-setosa"}
{"sepal_length":5.2,"sepal_width":3.5,"petal_length":1.5,"petal_width":0.2,"species":"Iris-setosa"}
{"sepal_length":5.2,"sepal_width":3.4,"petal_length":1.4,"petal_width":0.2,"species":"Iris-setosa"}
{"sepal_length":4.7,"sepal_width":3.2,"petal_length":1.6,"petal_width":0.2,"species":"Iris-setosa"}
{"sepal_length":4.8,"sepal_width":3.1,"petal_length":1.6,"petal_width":0.2,"species":"Iris-setosa"}
{"sepal_length":5.4,"sepal_width":3.4,"petal_length":1.5,"petal_width":0.4,"species":"Iris-setosa"}
{"sepal_length":5.2,"sepal_width":4.1,"petal_length":1.5,"petal_width":0.1,"species":"Iris-setosa"}
{"sepal_length":5.5,"sepal_width":4.2,"petal_length":1.4,"petal_width":0.2,"species":"Iris-setosa"}
{"sepal_length":4.9,"sepal_width":3.1,"petal_length":1.5,"petal_width":0.1,"species":"Iris-setosa"}
{"sepal_length":5,"sepal_width":3.2,"petal_length":1.2,"petal_width":0.2,"species":"Iris-setosa"}
{"sepal_length":5.5,"sepal_width":3.5,"petal_length":1.3,"petal_width":0.2,"species":"Iris-setosa"}
{"sepal_length":4.9,"sepal_width":3.1,"petal_length":1.5,"petal_width":0.1,"species":"Iris-setosa"}
{"sepal_length":4.4,"sepal_width":3,"petal_length":1.3,"petal_width":0.2,"species":"Iris-setosa"}
{"sepal_length":5.1,"sepal_width":3.4,"petal_length":1.5,"petal_width":0.2,"species":"Iris-setosa"}
{"sepal_length":5,"sepal_width":3.5,"petal_length":1.3,"petal_width":0.3,"species":"Iris-setosa"}
{"sepal_length":4.5,"sepal_width":2.3,"petal_length":1.3,"petal_width":0.3,"species":"Iris-setosa"}
{"sepal_length":4.4,"sepal_width":3.2,"petal_length":1.3,"petal_width":0.2,"species":"Iris-setosa"}
{"sepal_length":5,"sepal_width":3.5,"petal_length":1.6,"petal_width":0.6,"species":"Iris-setosa"}
{"sepal_length":5.1,"sepal_width":3.8,"petal_length":1.9,"petal_width":0.4,"species":"Iris-setosa"}
{"sepal_length":4.8,"sepal_width":3,"petal_length":1.4,"petal_width":0.3,"species":"Iris-setosa"}
{"sepal_length":5.1,"sepal_width":3.8,"petal_length":1.6,"petal_width":0.2,"species":"Iris-setosa"}
{"sepal_length":4.6,"sepal_width":3.2,"petal_length":1.4,"petal_width":0.2,"species":"Iris-setosa"}
{"sepal_length":5.3,"sepal_width":3.7,"petal_length":1.5,"petal_width":0.2,"species":"Iris-setosa"}
{"sepal_length":5,"sepal_width":3.3,"petal_length":1.4,"petal_width":0.2,"species":"Iris-setosa"}
{"sepal_length":7,"sepal_width":3.2,"petal_length":4.7,"petal_width":1.4,"species":"Iris-versicolor"}
{"sepal_length":6.4,"sepal_width":3.2,"petal_length":4.5,"petal_width":1.5,"species":"Iris-versicolor"}
{"sepal_length":6.9,"sepal_width":3.1,"petal_length":4.9,"petal_width":1.5,"species":"Iris-versicolor"}
{"sepal_length":5.5,"sepal_width":2.3,"petal_length":4,"petal_width":1.3,"species":"Iris-versicolor"}
{"sepal_length":6.5,"sepal_width":2.8,"petal_length":4.6,"petal_width":1.5,"species":"Iris-versicolor"}
{"sepal_length":5.7,"sepal_width":2.8,"petal_length":4.5,"petal_width":1.3,"species":"Iris-versicolor"}
{"sepal_length":6.3,"sepal_width":3.3,"petal_length":4.7,"petal_width":1.6,"species":"Iris-versicolor"}
{"sepal_length":4.9,"sepal_width":2.4,"petal_length":3.3,"petal_width":1,"species":"Iris-versicolor"}
{"sepal_length":6.6,"sepal_width":2.9,"petal_length":4.6,"petal_width":1.3,"species":"Iris-versicolor"}
{"sepal_length":5.2,"sepal_width":2.7,"petal_length":3.9,"petal_width":1.4,"species":"Iris-versicolor"}
{"sepal_length":5,"sepal_width":2,"petal_length":3.5,"petal_width":1,"species":"Iris-versicolor"}
{"sepal_length":5.9,"sepal_width":3,"petal_length":4.2,"petal_width":1.5,"species":"Iris-versicolor"}
{"sepal_length":6,"sepal_width":2.2,"petal_length":4,"petal_width":1,"species":"Iris-versicolor"}
{"sepal_length":6.1,"sepal_width":2.9,"petal_length":4.7,"petal_width":1.4,"species":"Iris-versicolor"}
{"sepal_length":5.6,"sepal_width":2.9,"petal_length":3.6,"petal_width":1.3,"species":"Iris-versicolor"}
{"sepal_length":6.7,"sepal_width":3.1,"petal_length":4.4,"petal_width":1.4,"species":"Iris-versicolor"}
{"sepal_length":5.6,"sepal_width":3,"petal_length":4.5,"petal_width":1.5,"species":"Iris-versicolor"}
{"sepal_length":5.8,"sepal_width":2.7,"petal_length":4.1,"petal_width":1,"species":"Iris-versicolor"}
{"sepal_length":6.2,"sepal_width":2.2,"petal_length":4.5,"petal_width":1.5,"species":"Iris-versicolor"}
{"sepal_length":5.6,"sepal_width":2.5,"petal_length":3.9,"petal_width":1.1,"species":"Iris-versicolor"}
{"sepal_length":5.9,"sepal_width":3.2,"petal_length":4.8,"petal_width":1.8,"species":"Iris-versicolor"}
{"sepal_length":6.1,"sepal_width":2.8,"petal_length":4,"petal_width":1.3,"species":"Iris-versicolor"}
{"sepal_length":6.3,"sepal_width":2.5,"petal_length":4.9,"petal_width":1.5,"species":"Iris-versicolor"}
{"sepal_length":6.1,"sepal_width":2.8,"petal_length":4.7,"petal_width":1.2,"species":"Iris-versicolor"}
{"sepal_length":6.4,"sepal_width":2.9,"petal_length":4.3,"petal_width":1.3,"species":"Iris-versicolor"}
{"sepal_length":6.6,"sepal_width":3,"petal_length":4.4,"petal_width":1.4,"species":"Iris-versicolor"}
{"sepal_length":6.8,"sepal_width":2.8,"petal_length":4.8,"petal_width":1.4,"species":"Iris-versicolor"}
{"sepal_length":6.7,"sepal_width":3,"petal_length":5,"petal_width":1.7,"species":"Iris-versicolor"}
{"sepal_length":6,"sepal_width":2.9,"petal_length":4.5,"petal_width":1.5,"species":"Iris-versicolor"}
{"sepal_length":5.7,"sepal_width":2.6,"petal_length":3.5,"petal_width":1,"species":"Iris-versicolor"}
{"sepal_length":5.5,"sepal_width":2.4,"petal_length":3.8,"petal_width":1.1,"species":"Iris-versicolor"}
{"sepal_length":5.5,"sepal_width":2.4,"petal_length":3.7,"petal_width":1,"species":"Iris-versicolor"}
{"sepal_length":5.8,"sepal_width":2.7,"petal_length":3.9,"petal_width":1.2,"species":"Iris-versicolor"}
{"sepal_length":6,"sepal_width":2.7,"petal_length":5.1,"petal_width":1.6,"species":"Iris-versicolor"}
{"sepal_length":5.4,"sepal_width":3,"petal_length":4.5,"petal_width":1.5,"species":"Iris-versicolor"}
{"sepal_length":6,"sepal_width":3.4,"petal_length":4.5,"petal_width":1.6,"species":"Iris-versicolor"}
{"sepal_length":6.7,"sepal_width":3.1,"petal_length":4.7,"petal_width":1.5,"species":"Iris-versicolor"}
{"sepal_length":6.3,"sepal_width":2.3,"petal_length":4.4,"petal_width":1.3,"species":"Iris-versicolor"}
{"sepal_length":5.6,"sepal_width":3,"petal_length":4.1,"petal_width":1.3,"species":"Iris-versicolor"}
{"sepal_length":5.5,"sepal_width":2.5,"petal_length":4,"petal_width":1.3,"species":"Iris-versicolor"}
{"sepal_length":5.5,"sepal_width":2.6,"petal_length":4.4,"petal_width":1.2,"species":"Iris-versicolor"}
{"sepal_length":6.1,"sepal_width":3,"petal_length":4.6,"petal_width":1.4,"species":"Iris-versicolor"}
{"sepal_length":5.8,"sepal_width":2.6,"petal_length":4,"petal_width":1.2,"species":"Iris-versicolor"}
{"sepal_length":5,"sepal_width":2.3,"petal_length":3.3,"petal_width":1,"species":"Iris-versicolor"}
{"sepal_length":5.6,"sepal_width":2.7,"petal_length":4.2,"petal_width":1.3,"species":"Iris-versicolor"}
{"sepal_length":5.7,"sepal_width":3,"petal_length":4.2,"petal_width":1.2,"species":"Iris-versicolor"}
{"sepal_length":5.7,"sepal_width":2.9,"petal_length":4.2,"petal_width":1.3,"species":"Iris-versicolor"}
{"sepal_length":6.2,"sepal_width":2.9,"petal_length":4.3,"petal_width":1.3,"species":"Iris-versicolor"}
{"sepal_length":5.1,"sepal_width":2.5,"petal_length":3,"petal_width":1.1,"species":"Iris-versicolor"}
{"sepal_length":5.7,"sepal_width":2.8,"petal_length":4.1,"petal_width":1.3,"species":"Iris-versicolor"}
{"sepal_length":6.3,"sepal_width":3.3,"petal_length":6,"petal_width":2.5,"species":"Iris-virginica"}
{"sepal_length":5.8,"sepal_width":2.7,"petal_length":5.1,"petal_width":1.9,"species":"Iris-virginica"}
{"sepal_length":7.1,"sepal_width":3,"petal_length":5.9,"petal_width":2.1,"species":"Iris-virginica"}
{"sepal_length":6.3,"sepal_width":2.9,"petal_length":5.6,"petal_width":1.8,"species":"Iris-virginica"}
{"sepal_length":6.5,"sepal_width":3,"petal_length":5.8,"petal_width":2.2,"species":"Iris-virginica"}
{"sepal_length":7.6,"sepal_width":3,"petal_length":6.6,"petal_width":2.1,"species":"Iris-virginica"}
{"sepal_length":4.9,"sepal_width":2.5,"petal_length":4.5,"petal_width":1.7,"species":"Iris-virginica"}
{"sepal_length":7.3,"sepal_width":2.9,"petal_length":6.3,"petal_width":1.8,"species":"Iris-virginica"}
{"sepal_length":6.7,"sepal_width":2.5,"petal_length":5.8,"petal_width":1.8,"species":"Iris-virginica"}
{"sepal_length":7.2,"sepal_width":3.6,"petal_length":6.1,"petal_width":2.5,"species":"Iris-virginica"}
{"sepal_length":6.5,"sepal_width":3.2,"petal_length":5.1,"petal_width":2,"species":"Iris-virginica"}
{"sepal_length":6.4,"sepal_width":2.7,"petal_length":5.3,"petal_width":1.9,"species":"Iris-virginica"}
{"sepal_length":6.8,"sepal_width":3,"petal_length":5.5,"petal_width":2.1,"species":"Iris-virginica"}
{"sepal_length":5.7,"sepal_width":2.5,"petal_length":5,"petal_width":2,"species":"Iris-virginica"}
{"sepal_length":5.8,"sepal_width":2.8,"petal_length":5.1,"petal_width":2.4,"species":"Iris-virginica"}
{"sepal_length":6.4,"sepal_width":3.2,"petal_length":5.3,"petal_width":2.3,"species":"Iris-virginica"}
{"sepal_length":6.5,"sepal_width":3,"petal_length":5.5,"petal_width":1.8,"species":"Iris-virginica"}
{"sepal_length":7.7,"sepal_width":3.8,"petal_length":6.7,"petal_width":2.2,"species":"Iris-virginica"}
{"sepal_length":7.7,"sepal_width":2.6,"petal_length":6.9,"petal_width":2.3,"species":"Iris-virginica"}
{"sepal_length":6,"sepal_width":2.2,"petal_length":5,"petal_width":1.5,"species":"Iris-virginica"}
{"sepal_length":6.9,"sepal_width":3.2,"petal_length":5.7,"petal_width":2.3,"species":"Iris-virginica"}
{"sepal_length":5.6,"sepal_width":2.8,"petal_length":4.9,"petal_width":2,"species":"Iris-virginica"}
{"sepal_length":7.7,"sepal_width":2.8,"petal_length":6.7,"petal_width":2,"species":"Iris-virginica"}
{"sepal_length":6.3,"sepal_width":2.7,"petal_length":4.9,"petal_width":1.8,"species":"Iris-virginica"}
{"sepal_length":6.7,"sepal_width":3.3,"petal_length":5.7,"petal_width":2.1,"species":"Iris-virginica"}
{"sepal_length":7.2,"sepal_width":3.2,"petal_length":6,"petal_width":1.8,"species":"Iris-virginica"}
{"sepal_length":6.2,"sepal_width":2.8,"petal_length":4.8,"petal_width":1.8,"species":"Iris-virginica"}
{"sepal_length":6.1,"sepal_width":3,"petal_length":4.9,"petal_width":1.8,"species":"Iris-virginica"}
{"sepal_length":6.4,"sepal_width":2.8,"petal_length":5.6,"petal_width":2.1,"species":"Iris-virginica"}
{"sepal_length":7.2,"sepal_width":3,"petal_length":5.8,"petal_width":1.6,"species":"Iris-virginica"}
{"sepal_length":7.4,"sepal_width":2.8,"petal_length":6.1,"petal_width":1.9,"species":"Iris-virginica"}
{"sepal_length":7.9,"sepal_width":3.8,"petal_length":6.4,"petal_width":2,"species":"Iris-virginica"}
{"sepal_length":6.4,"sepal_width":2.8,"petal_length":5.6,"petal_width":2.2,"species":"Iris-virginica"}
{"sepal_length":6.3,"sepal_width":2.8,"petal_length":5.1,"petal_width":1.5,"species":"Iris-virginica"}
{"sepal_length":6.1,"sepal_width":2.6,"petal_length":5.6,"petal_width":1.4,"species":"Iris-virginica"}
{"sepal_length":7.7,"sepal_width":3,"petal_length":6.1,"petal_width":2.3,"species":"Iris-virginica"}
{"sepal_length":6.3,"sepal_width":3.4,"petal_length":5.6,"petal_width":2.4,"species":"Iris-virginica"}
{"sepal_length":6.4,"sepal_width":3.1,"petal_length":5.5,"petal_width":1.8,"species":"Iris-virginica"}
{"sepal_length":6,"sepal_width":3,"petal_length":4.8,"petal_width":1.8,"species":"Iris-virginica"}
{"sepal_length":6.9,"sepal_width":3.1,"petal_length":5.4,"petal_width":2.1,"species":"Iris-virginica"}
{"sepal_length":6.7,"sepal_width":3.1,"petal_length":5.6,"petal_width":2.4,"species":"Iris-virginica"}
{"sepal_length":6.9,"sepal_width":3.1,"petal_length":5.1,"petal_width":2.3,"species":"Iris-virginica"}
{"sepal_length":5.8,"sepal_width":2.7,"petal_length":5.1,"petal_width":1.9,"species":"Iris-virginica"}
{"sepal_length":6.8,"sepal_width":3.2,"petal_length":5.9,"petal_width":2.3,"species":"Iris-virginica"}
{"sepal_length":6.7,"sepal_width":3.3,"petal_length":5.7,"petal_width":2.5,"species":"Iris-virginica"}
{"sepal_length":6.7,"sepal_width":3,"petal_length":5.2,"petal_width":2.3,"species":"Iris-virginica"}
{"sepal_length":6.3,"sepal_width":2.5,"petal_length":5,"petal_width":1.9,"species":"Iris-virginica"}
{"sepal_length":6.5,"sepal_width":3,"petal_length":5.2,"petal_width":2,"species":"Iris-virginica"}
{"sepal_length":6.2,"sepal_width":3.4,"petal_length":5.4,"petal_width":2.3,"species":"Iris-virginica"}
{"sepal_length":5.9,"sepal_width":3,"petal_length":5.1,"petal_width":1.8,"species":"Iris-virginica"})");

    std::string nileCSV(R"("id","time","value"
"1",1871,1120
"2",1872,1160
"3",1873,963
"4",1874,1210
"5",1875,1160
"6",1876,1160
"7",1877,813
"8",1878,1230
"9",1879,1370
"10",1880,1140
"11",1881,995
"12",1882,935
"13",1883,1110
"14",1884,994
"15",1885,1020
"16",1886,960
"17",1887,1180
"18",1888,799
"19",1889,958
"20",1890,1140
"21",1891,1100
"22",1892,1210
"23",1893,1150
"24",1894,1250
"25",1895,1260
"26",1896,1220
"27",1897,1030
"28",1898,1100
"29",1899,774
"30",1900,840
"31",1901,874
"32",1902,694
"33",1903,940
"34",1904,833
"35",1905,701
"36",1906,916
"37",1907,692
"38",1908,1020
"39",1909,1050
"40",1910,969
"41",1911,831
"42",1912,726
"43",1913,456
"44",1914,824
"45",1915,702
"46",1916,1120
"47",1917,1100
"48",1918,832
"49",1919,764
"50",1920,821
"51",1921,768
"52",1922,845
"53",1923,864
"54",1924,862
"55",1925,698
"56",1926,845
"57",1927,744
"58",1928,796
"59",1929,1040
"60",1930,759
"61",1931,781
"62",1932,865
"63",1933,845
"64",1934,944
"65",1935,984
"66",1936,897
"67",1937,822
"68",1938,1010
"69",1939,771
"70",1940,676
"71",1941,649
"72",1942,846
"73",1943,812
"74",1944,742
"75",1945,801
"76",1946,1040
"77",1947,860
"78",1948,874
"79",1949,848
"80",1950,890
"81",1951,744
"82",1952,749
"83",1953,838
"84",1954,1050
"85",1955,918
"86",1956,986
"87",1957,797
"88",1958,923
"89",1959,975
"90",1960,815
"91",1961,1020
"92",1962,906
"93",1963,901
"94",1964,1170
"95",1965,912
"96",1966,746
"97",1967,919
"98",1968,718
"99",1969,714
"100",1970,740)");


    auto dpCtor_Structs              = incplot::CL_Args::get_dpCtorStruct();
    // dpCtor_Structs.front().tar_width = 120uz;
    dpCtor_Structs.front().plot_type_name = "Scatter";
    dpCtor_Structs.front().tar_width = 60;
    // dpCtor_Structs.front().v_colIDs = {0};

    auto ds = incplot::parsers::Parser::parse(std::string_view(nileCSV));
    // auto ds_t5 = incplot::Parser::parse(std::string_view(irisJSON_t5));


    for (auto const &dpctr : dpCtor_Structs) {
        auto dp_autoGuessed = incplot::DesiredPlot(dpctr).guess_missingParams(ds.value());
        if (not dp_autoGuessed.has_value()) {
            std::print("{0}{1}{2}", "Autoguessing of 'DesiresPlot' parameters for: ",
                       dpctr.plot_type_name.has_value() ? dpctr.plot_type_name.value() : "[Unspecified plot type]",
                       " failed.\n");
            continue;
        }

        auto plotDrawer = incplot::make_plotDrawer(dp_autoGuessed.value(), ds.value());
        if (not plotDrawer.has_value()) {
            std::print("{0}{1}{2}", "Creating 'Plot Structure' for: ",
                       dpctr.plot_type_name.has_value() ? dpctr.plot_type_name.value() : "[Unspecified plot type]",
                       " failed.\n");
            continue;
        }

        auto outExp = plotDrawer.value().validateAndDrawPlot();
        if (not outExp.has_value()) {
            std::print("{0}{1}{2}", "Invalid plot structure for: ",
                       dpctr.plot_type_name.has_value() ? dpctr.plot_type_name.value() : "[Unspecified plot type]",
                       ".\n");
            continue;
        }
        std::print("{}\n", outExp.value());

        
    }

    return 0;
}