%option noyywrap
%option nounput
%option noinput
%top{
    #include <stdarg.h>
    #include "common.h"     //引入common.h头文件,含一个枚举类型dump_type_t
    #include <map>
    #include <string>
    #include <vector>
    #include <iostream>


}
%{

    extern dump_type_t dump_type;

    inline void dump_tokens(const char* format, ...){
        va_list args;
        va_start(args, format);
        if (dump_type == TOKENS)
            vfprintf(yyout, format, args);
        va_end(args);
    }
             // 符号表项结构
        class SymbolTableEntry {
            public:
            std::string type; // 标识符的类型，例如 int, float 等
            int lineNumber;   // 标识符出现的行号
            // 你可以根据需要添加更多字段
        };

    // 符号表类
    class SymbolTable {
    private:
    std::vector<std::map<std::string, SymbolTableEntry*>> tables; // 符号表栈

    public:
        SymbolTable() {
            tables.push_back(std::map<std::string, SymbolTableEntry*>());
        }
        // 进入新的作用域
        void enterScope() {
            tables.push_back(std::map<std::string, SymbolTableEntry*>());
        }

        // 退出当前作用域
        void exitScope() {
            if (!tables.empty()) {
                // 释放当前作用域中的所有符号表项
                for (auto& entry : tables.back()) {
                    delete entry.second;
                }
                tables.pop_back();
            } else {
                std::cerr << "Error: No scope to exit" << std::endl;
            }
        }

        // 插入符号表项
        void insert(const std::string& identifier, SymbolTableEntry* entry) {
            if (!tables.empty()) {
                tables.back()[identifier] = entry;
            } else {
                std::cerr << "Error: No scope to insert into" << std::endl;
            }
        }


        // 查询符号表项
        SymbolTableEntry* lookup(const std::string& identifier) {
            for (auto it = tables.rbegin(); it != tables.rend(); ++it) {
                auto entryIt = it->find(identifier);
                if (entryIt != it->end()) {
                    return entryIt->second;
                }
            }
            return nullptr;
        }

    };

    // 创建一个全局符号表对象
    SymbolTable symbolTable;

    // 全局变量，用于区分当前是否在声明标识符
    bool isDeclaration = false;
    /* Your code here, if desired (lab3). */
%}

/* definitions section */
DECIMIAL ([1-9][0-9]*|0)
OCTAL 0[0-7]+               
HEX 0[xX][0-9a-fA-F]+        
ID [[:alpha:]_][[:alpha:][:digit:]_]*          
EOL (\r\n|\n|\r)        
WHITE [\t ]    
/*  Your code here (lab3). */

%%  
    /* rules section */     //规则部分
"int" {
    isDeclaration=true;
    dump_tokens("INT\t%s\n", yytext);//如果匹配到关键字 "int"，调用 dump_tokens() 输出 "INT"，并打印匹配的文本 yytext（当前的匹配字符串）。
}

"void" {
    isDeclaration=true;

    dump_tokens("VOID\t%s\n", yytext);
}

"if" {
    dump_tokens("IF\t%s\n", yytext);
}

"else" {
    dump_tokens("ELSE\t%s\n", yytext);
}

"return" {
    dump_tokens("RETURN\t%s\n", yytext);
}

"=" {
    dump_tokens("ASSIGN\t%s\n", yytext);
}

"<" {
    dump_tokens("LESS\t%s\n", yytext);
}

"+" {
    dump_tokens("ADD\t%s\n", yytext);
}

";" {
    dump_tokens("SEMICOLON\t%s\n", yytext);
}

"(" {
    dump_tokens("LPAREN\t%s\n", yytext);
}

")" {
    dump_tokens("RPAREN\t%s\n", yytext);
}

"{" {
    dump_tokens("LBRACE\t%s\n", yytext);
    symbolTable.enterScope(); // 进入新的作用域
}

"}" {
    dump_tokens("RBRACE\t%s\n", yytext);
    symbolTable.exitScope(); // 退出当前作用域
}

{DECIMIAL} {
    int decimal;
    decimal = atoi(yytext); //将字符串转换为整数
    dump_tokens("DECIMAL\t%s\t%d\n", yytext, decimal);
}

{OCTAL} {
        int decimal;
        decimal = strtol(yytext, NULL, 8);  // 将八进制字符串转换为十进制
        dump_tokens("OCTAL\t%s\t%d\n", yytext, decimal);    // 输出十进制数
    }

    
{HEX} {
        int decimal;
        decimal = strtol(yytext, NULL, 16);  // 将十六进制字符串转换为十进制
        dump_tokens("HEX\t%s\t%d\n", yytext, decimal);      // 输出十进制数
    }

{ID} {
    SymbolTableEntry* entry = nullptr;
    if (isDeclaration) {
        // 插入符号表项
        entry = new SymbolTableEntry{"identifier", yylineno};
        symbolTable.insert(yytext, entry);
        isDeclaration = false; // 重置声明标识符标志
    } else {
        // 查找符号表项
        entry = symbolTable.lookup(yytext);
        if (entry == nullptr) {
            fprintf(stderr, "Error: Undeclared identifier '%s' at line %d\n", yytext, yylineno);
        }
    }
 
    dump_tokens("ID\t%s\t%p\n", yytext ,entry);
}

{EOL} yylineno++;   

{WHITE}

    /*  Your code here (lab3). */
%%

 /* user code section */

    
       