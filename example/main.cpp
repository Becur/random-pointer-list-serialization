#include "../src/list.h"
#include "../src/list_serializer.h"

#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

int main(int argc, char* argv[]){
    fs::path exe_path = fs::absolute(argv[0]).parent_path();

    fs::path input_path = exe_path / "inlet.in";
    fs::path output_path = exe_path / "outlet.out";

    List* f_list = DeserializeFromText(input_path);
    SerializeToBin(f_list, output_path);
    List* s_list = DeserializeFromBin(output_path);
    
    if(*f_list == *s_list){
        std::cout << "Congratulations!\n";
    }
    else{
        std::cout << "Defeat\n";
    }
}