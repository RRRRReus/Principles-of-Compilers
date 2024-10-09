#include <iostream>
#include <map>
#include <string>

// 符号表项结构
struct SymbolTableEntry {
    std::string type; // 标识符的类型，例如 int, float 等
    int lineNumber;   // 标识符出现的行号
    // 你可以根据需要添加更多字段
};

// 符号表类
class SymbolTable {
public:
    // 插入符号表项
    void insert(const std::string& identifier, const SymbolTableEntry& entry) {
        table[identifier] = entry;
    }

    // 查询符号表项
    SymbolTableEntry* lookup(const std::string& identifier) {
        auto it = table.find(identifier);
        if (it != table.end()) {
            return &it->second;
        } else {
            return nullptr;
        }
    }

private:
    std::map<std::string, SymbolTableEntry> table;
};

// 示例使用
// int main() {
//     SymbolTable symbolTable;

//     // 插入符号表项
//     SymbolTableEntry entry1 = {"int", 1};
//     symbolTable.insert("x", entry1);

//     // 查询符号表项
//     SymbolTableEntry* result = symbolTable.lookup("x");
//     if (result) {
//         std::cout << "Identifier: x, Type: " << result->type << ", Line: " << result->lineNumber << std::endl;
//     } else {
//         std::cout << "Identifier not found" << std::endl;
//     }

//     return 0;
// }