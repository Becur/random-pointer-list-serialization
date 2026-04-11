#pragma once

#include <string>

struct ListNode{
    ListNode* prev = nullptr;
    ListNode* next =  nullptr;
    ListNode* rand = nullptr;

    std::string data;

    ~ListNode();
};

bool operator==(const ListNode& lhs, const ListNode& rhs); // For tests