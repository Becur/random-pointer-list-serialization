#include "list.h"

#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

ListNode* List::Front(){
    return front_node.get();
}

const ListNode* List::Front() const{
    return front_node.get();
}

std::uint32_t List::Size() const{
    return size_;
}

ListNode* List::AddNode(){
    if(!front_node){
        front_node = std::make_unique<ListNode>();
        back_node = front_node.get();
    }
    else{
        back_node->next = new ListNode();
        back_node->next->prev = back_node;
        back_node = back_node->next;
    }
    ++size_;
    return back_node;
}

List::~List() {
    ListNode* node = front_node.release();
    while (node != nullptr) {
        ListNode* next = node->next;
        delete node;
        node = next;
    }
}

bool operator==(const List& lhs, const List& rhs){
    if(&lhs == &rhs){
        return true;
    }
    if(lhs.Size() != rhs.Size()){
        return false;
    }
    std::vector<const ListNode*> rand_ptr_lhs;
    rand_ptr_lhs.reserve(lhs.Size());
    std::vector<const ListNode*> rand_ptr_rhs;
    rand_ptr_rhs.reserve(lhs.Size());
    std::unordered_map<const ListNode*, int> order_nodes_lhs, order_nodes_rhs;
    const ListNode* lhs_node = lhs.Front();
    const ListNode* rhs_node = rhs.Front();
    for(std::uint32_t i = 0; i < lhs.Size(); ++i){
        if(lhs_node->data != rhs_node->data){
            return false;
        }
        rand_ptr_lhs.push_back(lhs_node->rand);
        rand_ptr_rhs.push_back(rhs_node->rand);
        order_nodes_lhs[lhs_node] = i;
        order_nodes_rhs[rhs_node] = i;
        lhs_node = lhs_node->next;
        rhs_node = rhs_node->next;
    }
    for(std::uint32_t i = 0; i < lhs.Size(); ++i){
        if ((rand_ptr_lhs[i] == nullptr) && (rand_ptr_rhs[i] == nullptr)) {
            continue;
        }

        if ((rand_ptr_lhs[i] == nullptr) || (rand_ptr_rhs[i] == nullptr)) {
            return false;
        }

        if (order_nodes_lhs[rand_ptr_lhs[i]] != order_nodes_rhs[rand_ptr_rhs[i]]) {
            return false;
        }
    }
    return true;
}