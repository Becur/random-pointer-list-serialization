#include "list_serializer.h"

#include "list.h"

#include <filesystem>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <cstdint>
#include <stdexcept>
#include <memory>

List* DeserializeFromText(const std::filesystem::path& input_path){
    std::ifstream input_file(input_path);
    if(!input_file.is_open()){
        throw std::runtime_error("No such file: " + input_path.string());
    }
    // if(std::filesystem::exists(input_path) &&
    //     std::filesystem::is_regular_file(input_path) &&
    //     (std::filesystem::file_size(input_path) == 0)){
    //         return new List();
    // }
    std::unique_ptr<List> res_list = std::make_unique<List>();
    std::vector<std::int32_t> num_rand_node;
    num_rand_node.reserve(10e5);
    std::vector<ListNode*> ptr_num_node;
    ptr_num_node.reserve(10e5);
    std::string line;
    for(ListNode* cur_node; std::getline(input_file, line);){
        cur_node = res_list->AddNode();
        std::size_t pos_sep = line.find_last_of(';');
        if (pos_sep == std::string::npos) {
            throw std::runtime_error("Invalid input line format");
        }
        std::int32_t num_rand = std::stoi(line.substr((pos_sep + 1)));
        line.erase(pos_sep);
        cur_node->data = std::move(line);
        num_rand_node.push_back(num_rand);
        ptr_num_node.push_back(cur_node);
    }
    std::uint32_t i = 0;
    for(ListNode* cur_node = res_list->Front(); cur_node != nullptr; cur_node = cur_node->next, ++i){
        if(num_rand_node[i] != -1){
            cur_node->rand = ptr_num_node[num_rand_node[i]];
        }
    }
    return res_list.release();
}

void SerializeToBin(const List* list, const std::filesystem::path& output_path){
    std::ofstream output_file(output_path, std::ios::binary);
    const ListNode* begin_node = list->Front();
    std::uint32_t size_list = list->Size();
    output_file.write(reinterpret_cast<const char*>(&size_list), sizeof(std::uint32_t));
    std::vector<const ListNode*> rand_ptr_node;
    rand_ptr_node.reserve(10e5);
    std::unordered_map<const ListNode*, std::int32_t> order_nodes;
    for(; begin_node != nullptr; begin_node = begin_node->next){
        std::uint16_t size_data = begin_node->data.size();
        output_file.write(reinterpret_cast<const char*>(&size_data), sizeof(std::uint16_t));
        output_file.write(begin_node->data.data(), static_cast<std::streamsize>(size_data));
        order_nodes[begin_node] = rand_ptr_node.size();
        rand_ptr_node.push_back(begin_node->rand);
    }
    std::vector<std::int32_t> rand_num_node;
    rand_num_node.reserve(rand_ptr_node.size());
    for(const ListNode* ptr_node : rand_ptr_node){
        rand_num_node.push_back((ptr_node == nullptr) ? -1 : order_nodes[ptr_node]);
    }
    output_file.write(reinterpret_cast<const char*>(rand_num_node.data()), rand_num_node.size() * sizeof(std::int32_t));
}

List* DeserializeFromBin(const std::filesystem::path& input_path){
    std::ifstream input_file(input_path, std::ios::binary);
    if(!input_file.is_open()){
        throw std::runtime_error("No such file: " + input_path.string());
    }
    std::uint32_t size_list;
    input_file.read(reinterpret_cast<char*>(&size_list), sizeof(std::uint32_t));
    std::unique_ptr<List> res_list = std::make_unique<List>();
    std::uint32_t i = 0;
    std::vector<ListNode*> order_nodes;
    order_nodes.reserve(size_list);
    for(ListNode* cur_node; i < size_list; ++i){
        cur_node = res_list->AddNode();
        std::uint16_t size_data;
        input_file.read(reinterpret_cast<char*>(&size_data), sizeof(std::uint16_t));
        cur_node->data = std::string(size_data, '\0');
        input_file.read(cur_node->data.data(), static_cast<std::streamsize>(size_data));
        order_nodes.push_back(cur_node);
    }
    std::vector<std::int32_t> rand_ptr_nodes(size_list);
    input_file.read(reinterpret_cast<char*>(rand_ptr_nodes.data()), size_list * sizeof(std::int32_t));
    i = 0;
    for(ListNode* cur_node = res_list->Front(); cur_node != nullptr; cur_node = cur_node->next, ++i){
        cur_node->rand = ((rand_ptr_nodes[i] == -1) ? nullptr : order_nodes[rand_ptr_nodes[i]]);
    }
    return res_list.release();
}