#pragma once

#include <memory>
#include <string>

struct ListNode{
    ListNode* prev = nullptr;
    ListNode* next =  nullptr;
    
    ListNode* rand = nullptr;
    std::string data;
};

class List{
public:

    List() = default;

    List(const List&) = delete;
    List(List&&) = delete;

    ListNode* AddNode();

    ListNode* Front();
    const ListNode* Front() const;

    std::uint32_t Size() const;

    ~List();

private:
    std::unique_ptr<ListNode> front_node;
    ListNode* back_node = nullptr;
    std::uint32_t size_ = 0;
};

bool operator==(const List& lhs, const List& rhs);