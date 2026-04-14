# random-pointer-list-serialization
Тестовое задание: сериализация и десериализация двусвязного списка с произвольными указателями на C++.

Binary format:
    [uint32_t node_count]
    repeated node_count times:
    [uint16_t data_size]
    [char[data_size] data_bytes]
    repeated node_count times:
    [int32_t rand_index]