#include <print>
#include <string>
#include <windows.h>


#include <incplot.hpp>
#include <incplot/args.hpp>


int main(int argc, char *argv[]) {

    std::string testInput(R"({"name":"BioWare","size":750}
{"name":"CD Projekt Red","size":980}
{"name":"Cline","size":6500}
{"name":"DSP_Star_Sector_Resources_61571387-64.csv","size":7558}
{"name":"Dyson Sphere Program","size":4096}
{"name":"Filmy","size":4500}
{"name":"Hudba","size":8500}
{"name":"My Games","size":1500}
{"name":"Obrázky","size":1800}
{"name":"OpenIV","size":2600}
{"name":"PowerToys","size":1000}
{"name":"Project CARS 2","size":4096}
{"name":"Respawn","size":300}
{"name":"Rockstar Games","size":50}
{"name":"The Riftbreaker","size":4096}
{"name":"VTune","size":0})");

    std::string testInput_petal_OLD(
        R"({"ID":0,"bill_length_mm":39.1,"bill_depth_mm":18.7}
{"ID":1,"bill_length_mm":39.5,"bill_depth_mm":17.4}
{"ID":2,"bill_length_mm":40.3,"bill_depth_mm":18}
{"ID":3,"bill_length_mm":36.7,"bill_depth_mm":19.3}
{"ID":4,"bill_length_mm":39.3,"bill_depth_mm":20.6}
{"ID":5,"bill_length_mm":38.9,"bill_depth_mm":17.8}
{"ID":6,"bill_length_mm":39.2,"bill_depth_mm":19.6}
{"ID":7,"bill_length_mm":34.1,"bill_depth_mm":18.1}
{"ID":8,"bill_length_mm":42,"bill_depth_mm":20.2}
{"ID":9,"bill_length_mm":37.8,"bill_depth_mm":17.1})");


    std::string testInput_petal(
        R"({"sepal_length":5.1,"sepal_width":3.5,"petal_length":1.4,"petal_width":0.2}
{"sepal_length":4.9,"sepal_width":3,"petal_length":1.4,"petal_width":0.2}
{"sepal_length":4.7,"sepal_width":3.2,"petal_length":1.3,"petal_width":0.2}
{"sepal_length":4.6,"sepal_width":3.1,"petal_length":1.5,"petal_width":0.2}
{"sepal_length":5,"sepal_width":3.6,"petal_length":1.4,"petal_width":0.2}
{"sepal_length":5.4,"sepal_width":3.9,"petal_length":1.7,"petal_width":0.4}
{"sepal_length":4.6,"sepal_width":3.4,"petal_length":1.4,"petal_width":0.3}
{"sepal_length":5,"sepal_width":3.4,"petal_length":1.5,"petal_width":0.2}
{"sepal_length":4.4,"sepal_width":2.9,"petal_length":1.4,"petal_width":0.2}
{"sepal_length":4.9,"sepal_width":3.1,"petal_length":1.5,"petal_width":0.1}
{"sepal_length":5.4,"sepal_width":3.7,"petal_length":1.5,"petal_width":0.2}
{"sepal_length":4.8,"sepal_width":3.4,"petal_length":1.6,"petal_width":0.2}
{"sepal_length":4.8,"sepal_width":3,"petal_length":1.4,"petal_width":0.1}
{"sepal_length":4.3,"sepal_width":3,"petal_length":1.1,"petal_width":0.1}
{"sepal_length":5.8,"sepal_width":4,"petal_length":1.2,"petal_width":0.2}
{"sepal_length":5.7,"sepal_width":4.4,"petal_length":1.5,"petal_width":0.4}
{"sepal_length":5.4,"sepal_width":3.9,"petal_length":1.3,"petal_width":0.4}
{"sepal_length":5.1,"sepal_width":3.5,"petal_length":1.4,"petal_width":0.3}
{"sepal_length":5.7,"sepal_width":3.8,"petal_length":1.7,"petal_width":0.3}
{"sepal_length":5.1,"sepal_width":3.8,"petal_length":1.5,"petal_width":0.3}
{"sepal_length":5.4,"sepal_width":3.4,"petal_length":1.7,"petal_width":0.2}
{"sepal_length":5.1,"sepal_width":3.7,"petal_length":1.5,"petal_width":0.4}
{"sepal_length":4.6,"sepal_width":3.6,"petal_length":1,"petal_width":0.2}
{"sepal_length":5.1,"sepal_width":3.3,"petal_length":1.7,"petal_width":0.5}
{"sepal_length":4.8,"sepal_width":3.4,"petal_length":1.9,"petal_width":0.2}
{"sepal_length":5,"sepal_width":3,"petal_length":1.6,"petal_width":0.2}
{"sepal_length":5,"sepal_width":3.4,"petal_length":1.6,"petal_width":0.4}
{"sepal_length":5.2,"sepal_width":3.5,"petal_length":1.5,"petal_width":0.2}
{"sepal_length":5.2,"sepal_width":3.4,"petal_length":1.4,"petal_width":0.2}
{"sepal_length":4.7,"sepal_width":3.2,"petal_length":1.6,"petal_width":0.2}
{"sepal_length":4.8,"sepal_width":3.1,"petal_length":1.6,"petal_width":0.2}
{"sepal_length":5.4,"sepal_width":3.4,"petal_length":1.5,"petal_width":0.4}
{"sepal_length":5.2,"sepal_width":4.1,"petal_length":1.5,"petal_width":0.1}
{"sepal_length":5.5,"sepal_width":4.2,"petal_length":1.4,"petal_width":0.2}
{"sepal_length":4.9,"sepal_width":3.1,"petal_length":1.5,"petal_width":0.1}
{"sepal_length":5,"sepal_width":3.2,"petal_length":1.2,"petal_width":0.2}
{"sepal_length":5.5,"sepal_width":3.5,"petal_length":1.3,"petal_width":0.2}
{"sepal_length":4.9,"sepal_width":3.1,"petal_length":1.5,"petal_width":0.1}
{"sepal_length":4.4,"sepal_width":3,"petal_length":1.3,"petal_width":0.2}
{"sepal_length":5.1,"sepal_width":3.4,"petal_length":1.5,"petal_width":0.2}
{"sepal_length":5,"sepal_width":3.5,"petal_length":1.3,"petal_width":0.3}
{"sepal_length":4.5,"sepal_width":2.3,"petal_length":1.3,"petal_width":0.3}
{"sepal_length":4.4,"sepal_width":3.2,"petal_length":1.3,"petal_width":0.2}
{"sepal_length":5,"sepal_width":3.5,"petal_length":1.6,"petal_width":0.6}
{"sepal_length":5.1,"sepal_width":3.8,"petal_length":1.9,"petal_width":0.4}
{"sepal_length":4.8,"sepal_width":3,"petal_length":1.4,"petal_width":0.3}
{"sepal_length":5.1,"sepal_width":3.8,"petal_length":1.6,"petal_width":0.2}
{"sepal_length":4.6,"sepal_width":3.2,"petal_length":1.4,"petal_width":0.2}
{"sepal_length":5.3,"sepal_width":3.7,"petal_length":1.5,"petal_width":0.2}
{"sepal_length":5,"sepal_width":3.3,"petal_length":1.4,"petal_width":0.2}
{"sepal_length":7,"sepal_width":3.2,"petal_length":4.7,"petal_width":1.4}
{"sepal_length":6.4,"sepal_width":3.2,"petal_length":4.5,"petal_width":1.5}
{"sepal_length":6.9,"sepal_width":3.1,"petal_length":4.9,"petal_width":1.5}
{"sepal_length":5.5,"sepal_width":2.3,"petal_length":4,"petal_width":1.3}
{"sepal_length":6.5,"sepal_width":2.8,"petal_length":4.6,"petal_width":1.5}
{"sepal_length":5.7,"sepal_width":2.8,"petal_length":4.5,"petal_width":1.3}
{"sepal_length":6.3,"sepal_width":3.3,"petal_length":4.7,"petal_width":1.6}
{"sepal_length":4.9,"sepal_width":2.4,"petal_length":3.3,"petal_width":1}
{"sepal_length":6.6,"sepal_width":2.9,"petal_length":4.6,"petal_width":1.3}
{"sepal_length":5.2,"sepal_width":2.7,"petal_length":3.9,"petal_width":1.4}
{"sepal_length":5,"sepal_width":2,"petal_length":3.5,"petal_width":1}
{"sepal_length":5.9,"sepal_width":3,"petal_length":4.2,"petal_width":1.5}
{"sepal_length":6,"sepal_width":2.2,"petal_length":4,"petal_width":1}
{"sepal_length":6.1,"sepal_width":2.9,"petal_length":4.7,"petal_width":1.4}
{"sepal_length":5.6,"sepal_width":2.9,"petal_length":3.6,"petal_width":1.3}
{"sepal_length":6.7,"sepal_width":3.1,"petal_length":4.4,"petal_width":1.4}
{"sepal_length":5.6,"sepal_width":3,"petal_length":4.5,"petal_width":1.5}
{"sepal_length":5.8,"sepal_width":2.7,"petal_length":4.1,"petal_width":1}
{"sepal_length":6.2,"sepal_width":2.2,"petal_length":4.5,"petal_width":1.5}
{"sepal_length":5.6,"sepal_width":2.5,"petal_length":3.9,"petal_width":1.1}
{"sepal_length":5.9,"sepal_width":3.2,"petal_length":4.8,"petal_width":1.8}
{"sepal_length":6.1,"sepal_width":2.8,"petal_length":4,"petal_width":1.3}
{"sepal_length":6.3,"sepal_width":2.5,"petal_length":4.9,"petal_width":1.5}
{"sepal_length":6.1,"sepal_width":2.8,"petal_length":4.7,"petal_width":1.2}
{"sepal_length":6.4,"sepal_width":2.9,"petal_length":4.3,"petal_width":1.3}
{"sepal_length":6.6,"sepal_width":3,"petal_length":4.4,"petal_width":1.4}
{"sepal_length":6.8,"sepal_width":2.8,"petal_length":4.8,"petal_width":1.4}
{"sepal_length":6.7,"sepal_width":3,"petal_length":5,"petal_width":1.7}
{"sepal_length":6,"sepal_width":2.9,"petal_length":4.5,"petal_width":1.5}
{"sepal_length":5.7,"sepal_width":2.6,"petal_length":3.5,"petal_width":1}
{"sepal_length":5.5,"sepal_width":2.4,"petal_length":3.8,"petal_width":1.1}
{"sepal_length":5.5,"sepal_width":2.4,"petal_length":3.7,"petal_width":1}
{"sepal_length":5.8,"sepal_width":2.7,"petal_length":3.9,"petal_width":1.2}
{"sepal_length":6,"sepal_width":2.7,"petal_length":5.1,"petal_width":1.6}
{"sepal_length":5.4,"sepal_width":3,"petal_length":4.5,"petal_width":1.5}
{"sepal_length":6,"sepal_width":3.4,"petal_length":4.5,"petal_width":1.6}
{"sepal_length":6.7,"sepal_width":3.1,"petal_length":4.7,"petal_width":1.5}
{"sepal_length":6.3,"sepal_width":2.3,"petal_length":4.4,"petal_width":1.3}
{"sepal_length":5.6,"sepal_width":3,"petal_length":4.1,"petal_width":1.3}
{"sepal_length":5.5,"sepal_width":2.5,"petal_length":4,"petal_width":1.3}
{"sepal_length":5.5,"sepal_width":2.6,"petal_length":4.4,"petal_width":1.2}
{"sepal_length":6.1,"sepal_width":3,"petal_length":4.6,"petal_width":1.4}
{"sepal_length":5.8,"sepal_width":2.6,"petal_length":4,"petal_width":1.2}
{"sepal_length":5,"sepal_width":2.3,"petal_length":3.3,"petal_width":1}
{"sepal_length":5.6,"sepal_width":2.7,"petal_length":4.2,"petal_width":1.3}
{"sepal_length":5.7,"sepal_width":3,"petal_length":4.2,"petal_width":1.2}
{"sepal_length":5.7,"sepal_width":2.9,"petal_length":4.2,"petal_width":1.3}
{"sepal_length":6.2,"sepal_width":2.9,"petal_length":4.3,"petal_width":1.3}
{"sepal_length":5.1,"sepal_width":2.5,"petal_length":3,"petal_width":1.1}
{"sepal_length":5.7,"sepal_width":2.8,"petal_length":4.1,"petal_width":1.3}
{"sepal_length":6.3,"sepal_width":3.3,"petal_length":6,"petal_width":2.5}
{"sepal_length":5.8,"sepal_width":2.7,"petal_length":5.1,"petal_width":1.9}
{"sepal_length":7.1,"sepal_width":3,"petal_length":5.9,"petal_width":2.1}
{"sepal_length":6.3,"sepal_width":2.9,"petal_length":5.6,"petal_width":1.8}
{"sepal_length":6.5,"sepal_width":3,"petal_length":5.8,"petal_width":2.2}
{"sepal_length":7.6,"sepal_width":3,"petal_length":6.6,"petal_width":2.1}
{"sepal_length":4.9,"sepal_width":2.5,"petal_length":4.5,"petal_width":1.7}
{"sepal_length":7.3,"sepal_width":2.9,"petal_length":6.3,"petal_width":1.8}
{"sepal_length":6.7,"sepal_width":2.5,"petal_length":5.8,"petal_width":1.8}
{"sepal_length":7.2,"sepal_width":3.6,"petal_length":6.1,"petal_width":2.5}
{"sepal_length":6.5,"sepal_width":3.2,"petal_length":5.1,"petal_width":2}
{"sepal_length":6.4,"sepal_width":2.7,"petal_length":5.3,"petal_width":1.9}
{"sepal_length":6.8,"sepal_width":3,"petal_length":5.5,"petal_width":2.1}
{"sepal_length":5.7,"sepal_width":2.5,"petal_length":5,"petal_width":2}
{"sepal_length":5.8,"sepal_width":2.8,"petal_length":5.1,"petal_width":2.4}
{"sepal_length":6.4,"sepal_width":3.2,"petal_length":5.3,"petal_width":2.3}
{"sepal_length":6.5,"sepal_width":3,"petal_length":5.5,"petal_width":1.8}
{"sepal_length":7.7,"sepal_width":3.8,"petal_length":6.7,"petal_width":2.2}
{"sepal_length":7.7,"sepal_width":2.6,"petal_length":6.9,"petal_width":2.3}
{"sepal_length":6,"sepal_width":2.2,"petal_length":5,"petal_width":1.5}
{"sepal_length":6.9,"sepal_width":3.2,"petal_length":5.7,"petal_width":2.3}
{"sepal_length":5.6,"sepal_width":2.8,"petal_length":4.9,"petal_width":2}
{"sepal_length":7.7,"sepal_width":2.8,"petal_length":6.7,"petal_width":2}
{"sepal_length":6.3,"sepal_width":2.7,"petal_length":4.9,"petal_width":1.8}
{"sepal_length":6.7,"sepal_width":3.3,"petal_length":5.7,"petal_width":2.1}
{"sepal_length":7.2,"sepal_width":3.2,"petal_length":6,"petal_width":1.8}
{"sepal_length":6.2,"sepal_width":2.8,"petal_length":4.8,"petal_width":1.8}
{"sepal_length":6.1,"sepal_width":3,"petal_length":4.9,"petal_width":1.8}
{"sepal_length":6.4,"sepal_width":2.8,"petal_length":5.6,"petal_width":2.1}
{"sepal_length":7.2,"sepal_width":3,"petal_length":5.8,"petal_width":1.6}
{"sepal_length":7.4,"sepal_width":2.8,"petal_length":6.1,"petal_width":1.9}
{"sepal_length":7.9,"sepal_width":3.8,"petal_length":6.4,"petal_width":2}
{"sepal_length":6.4,"sepal_width":2.8,"petal_length":5.6,"petal_width":2.2}
{"sepal_length":6.3,"sepal_width":2.8,"petal_length":5.1,"petal_width":1.5}
{"sepal_length":6.1,"sepal_width":2.6,"petal_length":5.6,"petal_width":1.4}
{"sepal_length":7.7,"sepal_width":3,"petal_length":6.1,"petal_width":2.3}
{"sepal_length":6.3,"sepal_width":3.4,"petal_length":5.6,"petal_width":2.4}
{"sepal_length":6.4,"sepal_width":3.1,"petal_length":5.5,"petal_width":1.8}
{"sepal_length":6,"sepal_width":3,"petal_length":4.8,"petal_width":1.8}
{"sepal_length":6.9,"sepal_width":3.1,"petal_length":5.4,"petal_width":2.1}
{"sepal_length":6.7,"sepal_width":3.1,"petal_length":5.6,"petal_width":2.4}
{"sepal_length":6.9,"sepal_width":3.1,"petal_length":5.1,"petal_width":2.3}
{"sepal_length":5.8,"sepal_width":2.7,"petal_length":5.1,"petal_width":1.9}
{"sepal_length":6.8,"sepal_width":3.2,"petal_length":5.9,"petal_width":2.3}
{"sepal_length":6.7,"sepal_width":3.3,"petal_length":5.7,"petal_width":2.5}
{"sepal_length":6.7,"sepal_width":3,"petal_length":5.2,"petal_width":2.3}
{"sepal_length":6.3,"sepal_width":2.5,"petal_length":5,"petal_width":1.9}
{"sepal_length":6.5,"sepal_width":3,"petal_length":5.2,"petal_width":2}
{"sepal_length":6.2,"sepal_width":3.4,"petal_length":5.4,"petal_width":2.3}
{"sepal_length":5.9,"sepal_width":3,"petal_length":5.1,"petal_width":1.8})");

    std::string testInput_flights(
        R"({"period":1,"value":112,"value2":432}
{"period":2,"value":118,"value2":390}
{"period":3,"value":132,"value2":461}
{"period":4,"value":129,"value2":508}
{"period":5,"value":121,"value2":606}
{"period":6,"value":135,"value2":622}
{"period":7,"value":148,"value2":535}
{"period":8,"value":148,"value2":472}
{"period":9,"value":136,"value2":461}
{"period":10,"value":119,"value2":419}
{"period":11,"value":104,"value2":391}
{"period":12,"value":118,"value2":417}
{"period":13,"value":115,"value2":405}
{"period":14,"value":126,"value2":362}
{"period":15,"value":141,"value2":407}
{"period":16,"value":135,"value2":463}
{"period":17,"value":125,"value2":559}
{"period":18,"value":149,"value2":548}
{"period":19,"value":170,"value2":472}
{"period":20,"value":170,"value2":420}
{"period":21,"value":158,"value2":396}
{"period":22,"value":133,"value2":406}
{"period":23,"value":114,"value2":342}
{"period":24,"value":140,"value2":360}
{"period":25,"value":145,"value2":337}
{"period":26,"value":150,"value2":310}
{"period":27,"value":178,"value2":359}
{"period":28,"value":163,"value2":404}
{"period":29,"value":172,"value2":505}
{"period":30,"value":178,"value2":491}
{"period":31,"value":199,"value2":435}
{"period":32,"value":199,"value2":363}
{"period":33,"value":184,"value2":348}
{"period":34,"value":162,"value2":362}
{"period":35,"value":146,"value2":318}
{"period":36,"value":166,"value2":340}
{"period":37,"value":171,"value2":336}
{"period":38,"value":180,"value2":305}
{"period":39,"value":193,"value2":347}
{"period":40,"value":181,"value2":404}
{"period":41,"value":183,"value2":467}
{"period":42,"value":218,"value2":465}
{"period":43,"value":230,"value2":422}
{"period":44,"value":242,"value2":355}
{"period":45,"value":209,"value2":348}
{"period":46,"value":191,"value2":356}
{"period":47,"value":172,"value2":301}
{"period":48,"value":194,"value2":315}
{"period":49,"value":196,"value2":306}
{"period":50,"value":196,"value2":271}
{"period":51,"value":236,"value2":306}
{"period":52,"value":235,"value2":355}
{"period":53,"value":229,"value2":405}
{"period":54,"value":243,"value2":413}
{"period":55,"value":264,"value2":374}
{"period":56,"value":272,"value2":318}
{"period":57,"value":237,"value2":313}
{"period":58,"value":211,"value2":317}
{"period":59,"value":180,"value2":277}
{"period":60,"value":201,"value2":284}
{"period":61,"value":204,"value2":278}
{"period":62,"value":188,"value2":237}
{"period":63,"value":235,"value2":274}
{"period":64,"value":227,"value2":312}
{"period":65,"value":234,"value2":347}
{"period":66,"value":264,"value2":364}
{"period":67,"value":302,"value2":315}
{"period":68,"value":293,"value2":270}
{"period":69,"value":259,"value2":269}
{"period":70,"value":229,"value2":267}
{"period":71,"value":203,"value2":233}
{"period":72,"value":229,"value2":242}
{"period":73,"value":242,"value2":229}
{"period":74,"value":233,"value2":203}
{"period":75,"value":267,"value2":229}
{"period":76,"value":269,"value2":259}
{"period":77,"value":270,"value2":293}
{"period":78,"value":315,"value2":302}
{"period":79,"value":364,"value2":264}
{"period":80,"value":347,"value2":234}
{"period":81,"value":312,"value2":227}
{"period":82,"value":274,"value2":235}
{"period":83,"value":237,"value2":188}
{"period":84,"value":278,"value2":204}
{"period":85,"value":284,"value2":201}
{"period":86,"value":277,"value2":180}
{"period":87,"value":317,"value2":211}
{"period":88,"value":313,"value2":237}
{"period":89,"value":318,"value2":272}
{"period":90,"value":374,"value2":264}
{"period":91,"value":413,"value2":243}
{"period":92,"value":405,"value2":229}
{"period":93,"value":355,"value2":235}
{"period":94,"value":306,"value2":236}
{"period":95,"value":271,"value2":196}
{"period":96,"value":306,"value2":196}
{"period":97,"value":315,"value2":194}
{"period":98,"value":301,"value2":172}
{"period":99,"value":356,"value2":191}
{"period":100,"value":348,"value2":209}
{"period":101,"value":355,"value2":242}
{"period":102,"value":422,"value2":230}
{"period":103,"value":465,"value2":218}
{"period":104,"value":467,"value2":183}
{"period":105,"value":404,"value2":181}
{"period":106,"value":347,"value2":193}
{"period":107,"value":305,"value2":180}
{"period":108,"value":336,"value2":171}
{"period":109,"value":340,"value2":166}
{"period":110,"value":318,"value2":146}
{"period":111,"value":362,"value2":162}
{"period":112,"value":348,"value2":184}
{"period":113,"value":363,"value2":199}
{"period":114,"value":435,"value2":199}
{"period":115,"value":491,"value2":178}
{"period":116,"value":505,"value2":172}
{"period":117,"value":404,"value2":163}
{"period":118,"value":359,"value2":178}
{"period":119,"value":310,"value2":150}
{"period":120,"value":337,"value2":145}
{"period":121,"value":360,"value2":140}
{"period":122,"value":342,"value2":114}
{"period":123,"value":406,"value2":133}
{"period":124,"value":396,"value2":158}
{"period":125,"value":420,"value2":170}
{"period":126,"value":472,"value2":170}
{"period":127,"value":548,"value2":149}
{"period":128,"value":559,"value2":125}
{"period":129,"value":463,"value2":135}
{"period":130,"value":407,"value2":141}
{"period":131,"value":362,"value2":126}
{"period":132,"value":405,"value2":115}
{"period":133,"value":417,"value2":118}
{"period":134,"value":391,"value2":104}
{"period":135,"value":419,"value2":119}
{"period":136,"value":461,"value2":136}
{"period":137,"value":472,"value2":148}
{"period":138,"value":535,"value2":148}
{"period":139,"value":622,"value2":135}
{"period":140,"value":606,"value2":121}
{"period":141,"value":508,"value2":129}
{"period":142,"value":461,"value2":132}
{"period":143,"value":390,"value2":118}
{"period":144,"value":432,"value2":112})");


    /*     auto ds = incplot::Parser::parse_NDJSON_intoDS(testInput);


        auto dp_autoGuessed =
            incplot::DesiredPlot(incplot::DesiredPlot::DP_CtorStruct{.tar_width = 96}).guess_missingParams(ds);

        if (not dp_autoGuessed.has_value()) {
            std::print("{0}{1}", "Autoguessing of 'DesiresPlot' parameters failed \n", "Exiting ...");
            return 1;
        }

        auto plotDrawer2 = incplot::make_plotDrawer(dp_autoGuessed.value(), ds);

        auto outExp = plotDrawer2.validateAndDrawPlot();

        if (not outExp.has_value()) {
            std::print("{0}{1}", "Invalid plot structure", "Exiting ...");
            return 1;
        }

        std::print("{}\n", outExp.value());


        std::print("{}Viridis {}Reset \n", incplot::TermColors::get_basicColor(incplot::Color_CVTS::Foreground_Green),
                   incplot::TermColors::get_basicColor(incplot::Color_CVTS::Default));
        std::print("{}Rhodo {}Reset \n", incplot::TermColors::get_basicColor(incplot::Color_CVTS::Foreground_Red),
                   incplot::TermColors::get_basicColor(incplot::Color_CVTS::Default));
        std::print("{}Tanza {}Reset \n", incplot::TermColors::get_basicColor(incplot::Color_CVTS::Foreground_Blue),
                   incplot::TermColors::get_basicColor(incplot::Color_CVTS::Default));


        // std::print("{0}{1}\n", "\x1b[38;2;125;125;0m", "TESTEST");


        auto ds2 = incplot::Parser::parse_NDJSON_intoDS(testInput_petal);

        auto dp2_autoGuessed =
            incplot::DesiredPlot(incplot::DesiredPlot::DP_CtorStruct{.tar_width = 48, .plot_type_name = "Scatter"})
                .guess_missingParams(ds2);


        if (not dp2_autoGuessed.has_value()) {
            std::print("{0}{1}", "Autoguessing of 'DesiresPlot_2' parameters failed \n", "Exiting ...");
            return 1;
        }

        auto plotDrawer3 = incplot::make_plotDrawer(dp2_autoGuessed.value(), ds2);

        auto outExp2 = plotDrawer3.validateAndDrawPlot();

        if (not outExp2.has_value()) {
            std::print("{0}{1}", "Invalid plot structure 2", "Exiting ...");
            return 1;
        }

        std::print("{}\n", outExp2.value()); */

    argparse::ArgumentParser ap("incplot", "1.0", argparse::default_arguments::help);
    incplot::CL_Args::finishAp(ap);
    auto dpCtor_Structs             = incplot::CL_Args::get_dpCtorStruct(ap, argc, argv);
    // dpCtor_Structs.front().tar_width = 120uz;
    dpCtor_Structs.front().v_colIDs = {2};
    dpCtor_Structs.front().lts_colID  = 1;

    auto ds = incplot::Parser::parse_NDJSON_intoDS(testInput_petal_OLD);


    for (auto const &dpctr : dpCtor_Structs) {
        auto dp_autoGuessed = incplot::DesiredPlot(dpctr).guess_missingParams(ds);
        if (not dp_autoGuessed.has_value()) {
            std::print("{0}{1}{2}", "Autoguessing of 'DesiresPlot' parameters for: ",
                       dpctr.plot_type_name.has_value() ? dpctr.plot_type_name.value() : "[Unspecified plot type]",
                       " failed.\n");
            continue;
        }

        auto plotDrawer = incplot::make_plotDrawer(dp_autoGuessed.value(), ds);
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