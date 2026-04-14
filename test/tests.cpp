#include "assert.h"
#include "../src/list.h"
#include "../src/list_serializer.h"

#include <filesystem>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include <iostream>

using namespace std;
namespace fs = std::filesystem;

// Создает временную директорию для тестовых файлов.
fs::path GetTestDir() {
    const fs::path dir = fs::temp_directory_path() / "list_serializer_tests";
    fs::create_directories(dir);
    return dir;
}

// Удаляет файл, если он существует.
void RemoveIfExists(const fs::path& path) {
    std::error_code ec;
    fs::remove(path, ec);
}

// Строит список через новый интерфейс List.
unique_ptr<List> BuildList(const vector<string>& data, const vector<int>& rand_indices) {
    ASSERT_EQUAL(data.size(), rand_indices.size());

    auto list = make_unique<List>();
    vector<ListNode*> nodes;
    nodes.reserve(data.size());

    for (size_t i = 0; i < data.size(); ++i) {
        ListNode* node = list->AddNode();
        node->data = data[i];
        nodes.push_back(node);
    }

    for (size_t i = 0; i < data.size(); ++i) {
        if (rand_indices[i] >= 0) {
            ASSERT_HINT(
                static_cast<size_t>(rand_indices[i]) < nodes.size(),
                "Индекс rand выходит за границы списка"
            );
            nodes[i]->rand = nodes[static_cast<size_t>(rand_indices[i])];
        }
    }

    return list;
}

// Извлекает логическое представление списка: данные и индексы rand.
pair<vector<string>, vector<int>> ExtractLogicalModel(const List& list) {
    vector<const ListNode*> nodes;
    nodes.reserve(list.Size());

    const ListNode* cur = list.Front();
    while (cur != nullptr) {
        nodes.push_back(cur);
        cur = cur->next;
    }

    ASSERT_EQUAL(nodes.size(), static_cast<size_t>(list.Size()));

    vector<string> data;
    vector<int> rand_indices;
    data.reserve(nodes.size());
    rand_indices.reserve(nodes.size());

    for (const ListNode* node : nodes) {
        data.push_back(node->data);

        if (node->rand == nullptr) {
            rand_indices.push_back(-1);
        } else {
            int index = -1;
            for (size_t i = 0; i < nodes.size(); ++i) {
                if (nodes[i] == node->rand) {
                    index = static_cast<int>(i);
                    break;
                }
            }
            ASSERT_HINT(index != -1, "rand должен указывать на элемент этого же списка либо быть nullptr");
            rand_indices.push_back(index);
        }
    }

    return {data, rand_indices};
}

// Проверяет, что список логически совпадает с ожидаемыми данными и rand-индексами.
void AssertListMatchesModel(const List& list, const vector<string>& expected_data, const vector<int>& expected_rand_indices) {
    const auto [actual_data, actual_rand_indices] = ExtractLogicalModel(list);
    ASSERT_EQUAL(actual_data, expected_data);
    ASSERT_EQUAL(actual_rand_indices, expected_rand_indices);
}

// Записывает входной текстовый файл в формате ТЗ.
void WriteTextInput(const fs::path& path, const vector<string>& data, const vector<int>& rand_indices) {
    ASSERT_EQUAL(data.size(), rand_indices.size());

    ofstream out(path, ios::binary);
    for (size_t i = 0; i < data.size(); ++i) {
        out << data[i] << ';' << rand_indices[i];
        if (i + 1 < data.size()) {
            out << '\n';
        }
    }
}

// Проверяет, что функция выбрасывает std::runtime_error.
template <typename Func>
void AssertThrowsRuntimeError(Func func, const string& hint = {}) {
    bool thrown = false;
    try {
        func();
    } catch (const runtime_error&) {
        thrown = true;
    } catch (...) {
        ASSERT_HINT(false, hint.empty() ? "Ожидался std::runtime_error, но выброшено исключение другого типа" : hint);
    }
    ASSERT_HINT(thrown, hint.empty() ? "Ожидался std::runtime_error, но исключение не было выброшено" : hint);
}

// Тестирует равенство двух пустых списков.
void TestEquality_EmptyLists() {
    List lhs;
    List rhs;

    ASSERT(lhs == rhs);
    ASSERT(rhs == lhs);
}

// Тестирует равенство двух одинаковых одноэлементных списков без rand.
void TestEquality_SingleNodeWithoutRand() {
    auto lhs = BuildList({"single"}, {-1});
    auto rhs = BuildList({"single"}, {-1});

    ASSERT(*lhs == *rhs);
    ASSERT(*rhs == *lhs);
}

// Тестирует равенство двух одинаковых многоэлементных списков с rand-ссылками.
void TestEquality_MultiNodeWithRand() {
    vector<string> data = {"apple", "banana", "carrot", "date"};
    vector<int> rand_indices = {2, -1, 1, 3};

    auto lhs = BuildList(data, rand_indices);
    auto rhs = BuildList(data, rand_indices);

    ASSERT(*lhs == *rhs);
    ASSERT(*rhs == *lhs);
}

// Тестирует, что списки не равны при различии данных в одном из узлов.
void TestEquality_DifferentData() {
    auto lhs = BuildList(
        {"apple", "banana", "carrot"},
        {2, -1, 1}
    );

    auto rhs = BuildList(
        {"apple", "BANANA", "carrot"},
        {2, -1, 1}
    );

    ASSERT(!(*lhs == *rhs));
    ASSERT(!(*rhs == *lhs));
}

// Тестирует, что списки не равны при различии rand-ссылок.
void TestEquality_DifferentRand() {
    auto lhs = BuildList(
        {"apple", "banana", "carrot"},
        {2, -1, 1}
    );

    auto rhs = BuildList(
        {"apple", "banana", "carrot"},
        {1, -1, 1}
    );

    ASSERT(!(*lhs == *rhs));
    ASSERT(!(*rhs == *lhs));
}

// Тестирует десериализацию пустого текстового файла как пустого списка.
void TestDeserializeFromText_EmptyList() {
    const fs::path path = GetTestDir() / "empty_text_list.txt";
    RemoveIfExists(path);

    {
        ofstream out(path, ios::binary);
    }

    unique_ptr<List> actual(DeserializeFromText(path));
    ASSERT_HINT(actual != nullptr, "DeserializeFromText не должен возвращать nullptr");
    ASSERT_EQUAL(actual->Size(), 0u);
    AssertListMatchesModel(*actual, {}, {});

    RemoveIfExists(path);
}

// Тестирует корректную десериализацию списка из текстового файла.
void TestDeserializeFromText_NormalCase() {
    const fs::path path = GetTestDir() / "normal_text_list.txt";
    RemoveIfExists(path);

    const vector<string> data = {
        "apple",
        "banana with spaces",
        "carrot_#42",
        "Привет UTF-8"
    };
    const vector<int> rand_indices = {2, -1, 1, 0};

    WriteTextInput(path, data, rand_indices);

    unique_ptr<List> actual(DeserializeFromText(path));
    ASSERT_HINT(actual != nullptr, "DeserializeFromText не должен возвращать nullptr");

    AssertListMatchesModel(*actual, data, rand_indices);

    RemoveIfExists(path);
}

// Тестирует выброс runtime_error при невозможности открыть текстовый файл.
void TestDeserializeFromText_FileOpenError() {
    const fs::path path = GetTestDir() / "missing_text_file.txt";
    RemoveIfExists(path);

    AssertThrowsRuntimeError(
        [&]() {
            unique_ptr<List> tmp(DeserializeFromText(path));
        },
        "DeserializeFromText должен выбрасывать std::runtime_error, если файл не удалось открыть"
    );
}

// Тестирует бинарный round-trip для пустого списка.
void TestBinaryRoundTrip_EmptyList() {
    const fs::path path = GetTestDir() / "empty_roundtrip.bin";
    RemoveIfExists(path);

    List source;
    SerializeToBin(&source, path);

    unique_ptr<List> restored(DeserializeFromBin(path));
    ASSERT_HINT(restored != nullptr, "DeserializeFromBin не должен возвращать nullptr");

    ASSERT_EQUAL(restored->Size(), 0u);
    AssertListMatchesModel(*restored, {}, {});

    RemoveIfExists(path);
}

// Тестирует бинарный round-trip для обычного списка с rand-ссылками.
void TestBinaryRoundTrip_NormalCase() {
    const fs::path path = GetTestDir() / "normal_roundtrip.bin";
    RemoveIfExists(path);

    const vector<string> data = {"one", "two", "three", "four", "five"};
    const vector<int> rand_indices = {4, 0, -1, 2, 1};

    auto source = BuildList(data, rand_indices);

    SerializeToBin(source.get(), path);

    unique_ptr<List> restored(DeserializeFromBin(path));
    ASSERT_HINT(restored != nullptr, "DeserializeFromBin не должен возвращать nullptr");

    AssertListMatchesModel(*restored, data, rand_indices);
    ASSERT(*source == *restored);

    RemoveIfExists(path);
}

// Тестирует выброс runtime_error при невозможности открыть бинарный файл.
void TestDeserializeFromBin_FileOpenError() {
    const fs::path path = GetTestDir() / "missing_binary_file.bin";
    RemoveIfExists(path);

    AssertThrowsRuntimeError(
        [&]() {
            unique_ptr<List> tmp(DeserializeFromBin(path));
        },
        "DeserializeFromBin должен выбрасывать std::runtime_error, если файл не удалось открыть"
    );
}

// Тестирует сквозной сценарий на большом объеме данных.
// При необходимости можно уменьшить размер для локального прогона.
void TestBinaryRoundTrip_LargeVolume() {
    const fs::path path = GetTestDir() / "large_volume_roundtrip.bin";
    RemoveIfExists(path);

    constexpr size_t NODE_COUNT = 1000000;
    constexpr size_t DATA_LENGTH = 1000;

    vector<string> data;
    data.reserve(NODE_COUNT);

    vector<int> rand_indices;
    rand_indices.reserve(NODE_COUNT);

    const string payload(DATA_LENGTH, 'x');

    for (size_t i = 0; i < NODE_COUNT; ++i) {
        data.push_back(payload);
        rand_indices.push_back(static_cast<int>(NODE_COUNT - 1 - i));
    }

    auto source = BuildList(data, rand_indices);

    SerializeToBin(source.get(), path);

    unique_ptr<List> restored(DeserializeFromBin(path));
    ASSERT_HINT(restored != nullptr, "DeserializeFromBin не должен возвращать nullptr");

    ASSERT_EQUAL(restored->Size(), static_cast<uint32_t>(NODE_COUNT));
    ASSERT(*source == *restored);

    RemoveIfExists(path);
}

// Запускает все тесты.
void RunAllTests() {
    TestEquality_EmptyLists();
    TestEquality_SingleNodeWithoutRand();
    TestEquality_MultiNodeWithRand();
    TestEquality_DifferentData();
    TestEquality_DifferentRand();

    TestDeserializeFromText_EmptyList();
    TestDeserializeFromText_NormalCase();
    TestDeserializeFromText_FileOpenError();

    TestBinaryRoundTrip_EmptyList();
    TestBinaryRoundTrip_NormalCase();
    TestDeserializeFromBin_FileOpenError();

    TestBinaryRoundTrip_LargeVolume();
}

int main() {
    RunAllTests();
    cout << "All tests passed" << endl;
    return 0;
}