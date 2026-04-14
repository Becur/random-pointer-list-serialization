#pragma once

#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <string>

template <typename Element>
std::ostream& Print(std::ostream& out, const Element container){
    bool is_first = 1;
    for(auto element : container){
        if (is_first){
            out << element;
            is_first = false;
        }
        else{
            out << ", " << element;
        }
    }
    return out;
}

template <typename Element_l, typename Element_r>
std::ostream& operator<< (std::ostream& out, const std::map<Element_l, Element_r> container){
    out << "{";
    bool is_first = true;
    for(const auto& pair : container){
        if(!is_first){
            out << ", ";
        }
        else{
            is_first = false;
        }
        out << pair;
    }
    out << "}";
    return out;
}

template <typename Element_l, typename Element_r>
std::ostream& operator<< (std::ostream& out, const std::pair<Element_l, Element_r> container){
    out << container.first << ": " << container.second;
    return out;
}

template <typename Element>
std::ostream& operator<< (std::ostream& out, const std::set<Element> container){
    out << "{";
    Print(out, container); 
    out << "}";
    return out;
}

template <typename Element>
std::ostream& operator<< (std::ostream& out, const std::vector<Element> container){
    out << "[";
    Print(out, container);
    out << "]";
    return out;
}

template <typename T, typename U>
void AssertEqualImpl(const T& t, const U& u, const std::string& t_str, const std::string& u_str, const std::string& file,
                     const std::string& func, unsigned line, const std::string& hint) {
    if (t != u) {
        std::cerr << std::boolalpha;
        std::cerr << file << "(" << line << "): " << func << ": ";
        std::cerr << "ASSERT_EQUAL(" << t_str << ", " << u_str << ") failed: ";
        std::cerr << t << " != " << u << ".";
        if (!hint.empty()) {
            std::cerr << " Hint: " << hint;
        }
        std::cerr << std::endl;
        abort();
    }
}

#define ASSERT_EQUAL(a, b) AssertEqualImpl((a), (b), #a, #b, __FILE__, __FUNCTION__, __LINE__, "")

#define ASSERT_EQUAL_HINT(a, b, hint) AssertEqualImpl((a), (b), #a, #b, __FILE__, __FUNCTION__, __LINE__, (hint))

void AssertImpl(bool value, const std::string& expr_str, const std::string& file, const std::string& func, unsigned line,
                const std::string& hint) {
    if (!value) {
        std::cerr << file << "(" << line << "): " << func << ": ";
        std::cerr << "ASSERT(" << expr_str << ") failed.";
        if (!hint.empty()) {
            std::cerr << " Hint: " << hint;
        }
        std::cerr << std::endl;
        abort();
    }
}

#define ASSERT(expr) AssertImpl(!!(expr), #expr, __FILE__, __FUNCTION__, __LINE__, "")

#define ASSERT_HINT(expr, hint) AssertImpl(!!(expr), #expr, __FILE__, __FUNCTION__, __LINE__, (hint))