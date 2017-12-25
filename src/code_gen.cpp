/*
@msg - Message
@ent - Entity component
@ent_local -  Entity component without serialisation

*/

#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "core\common.h"

#define ERROR(...) (printf("ERROR: "), printf(__VA_ARGS__), printf("\n"), assert(0), getchar() , exit(-1))
#define TOKEN_EXPECT_STR(token, str2) if (!token->str || strlen(str2) != token->len || strncmp(token->str, str2,  token->len)) ERROR("Expected '%s', but got '%.*s'", str2, token->len, token->str)

static char* read_file(const char* path) {
    FILE* file = fopen(path, "r");
    if (!file) return NULL;
    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    void* data = malloc(file_size);
    rewind(file);
    fread(data, 1, file_size, file);
    fclose(file);
    //if (size) size = file_size;
    return (char*)data;
}

enum Datatype {
    DATATYPE_8 = 1,
    DATATYPE_16 = 2,
    DATATYPE_32 = 4,
    DATATYPE_64 = 8,
    DATATYPE_SIGNED = 16,
    DATATYPE_FLOAT = 32,
    DATATYPE_ARRAY = 64
};

typedef enum {
    TOKEN__NULL = 0,
    TOKEN_IDENTIFIER,
    TOKEN_OPERATOR,
    TOKEN_INT,
    TOKEN_FLOAT,
    TOKEN_CHAR,
    TOKEN_STRING,
    TOKEN__END
} Token_Type;

struct Token {
    Token_Type type;
    int line_num;
    int column_num;
    char* str;
    int len;
};

enum Ast_Type {
    AST__NULL = 0,
    AST_RAW,
    AST_MSG,
    AST__END
};

struct Ast {
    Ast_Type type;
    char* str;
    int len;
    union {
        struct {
            char* str;
            int len;
        } raw;
        struct {
            int id;
            char* name;
            int name_len;
            Array<int> var_types;
            Array<char*> var_names;
            Array<int> var_name_lens;
        } msg;
    };
};

Array<Ast> g_msgs;

static bool token_equals(const Token token, const char* str) {
    for (int i = 0; i < token.len; i++)
        if (token.str[i] != str[i]) return false;
    return (str[token.len] == '\0');
}

bool is_space(char c) {
    return (c == ' ' || c == '\t' || c == '\n' || c == '\r');
}

bool is_alpha(char c) {
    return (c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z');
}

bool is_alnum(char c) {
    return (c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z' || c >= '0' && c <= '9');
}

bool is_digit(char c) {
    return (c >= '0' && c <= '9');
}

bool is_punct(char c) {
    return (c >= '!' && c <= '/' || c >= ':' && c <= '@' || c >= '[' && c <= '`' || c >= '{' && c <= '~');
}

static Array<Token> tokenize(char* src) {
    Array<Token> tokens = {};
    int line_num = 0;
    int column_num = 0;
    char* c = src;
    for (;;) {//while (i + 3 <= buffer_size) {
        Token token;
        token.type = TOKEN__NULL;
        size_t len = 0;

        // Skip whitespace
        while (is_space(c[0])) {
            if (c[0] == '\n') {
                line_num++;
                column_num = 0;
            }
            else if (c[0] != '\r') {
                column_num++;
            }
            c++;
        }

        // Skip line comment
        if (c[0] == '/' && c[1] == '/') {
            c += 2;
            while (c[0] != '\n' && c[0] != '\0' && c[0] != EOF) c++;
            continue;
        }

        // Skip block comment
        if (c[0] == '/' && c[1] == '*') {
            c += 2;
            while (c[0] != '\n' && c[0] != '\0' && c[0] != EOF) {
                if (c[0] == '*' && c[1] == '/') {
                    c += 2;
                    break;
                }
                c++;
            }
            continue;
        }

        // End of file
        if (c[0] == EOF || c[0] == NULL)
            break;

        // Identifier
        if (is_alpha(c[0]) || c[0] == '_') {
            token.type = TOKEN_IDENTIFIER;
            do len++; while (is_alnum(c[len]) || c[len] == '_');
        }
        else if (c[0] == '\'') {
            token.type = TOKEN_CHAR;
            bool backslash = false;
            do {
                backslash = (!backslash && c[len] == '\\');
                len++;
                if (c[len] == NULL || c[len] == EOF || c[len] == '\n') break;
            } while (c[len] != '\'' || backslash);
            len++;
        }
        else if (c[0] == '\"') {
            token.type = TOKEN_STRING;
            bool backslash = false;
            do {
                backslash = (!backslash && c[len] == '\\');
                len++;
                if (c[len] == NULL || c[len] == EOF) break;
            } while (c[len] != '\"' || backslash);
            len++;
        }
        else {
            // Int, Float
            if (is_digit(c[0]) || (c[0] == '.')) {
                len = 0;
                while (is_digit(c[len])) len++;

                if (c[len] == '.') {
                    do len++; while (is_digit(c[len]) || c[len] == '.' || tolower(c[len]) == 'f' || tolower(c[len]) == 'e');
                    if (len != 1) token.type = TOKEN_FLOAT;
                    else len = 0;
                }
                else
                    token.type = TOKEN_INT;
            }
            // Char:
            // Operator:
            if (token.type == TOKEN__NULL && (is_punct(c[0]))) {
                token.type = TOKEN_OPERATOR;
                // +=  -=  *=  /=  %=  ^=  !=  <=  >=  ==
                if (c[1] == '=' && (c[0] == '+' || c[0] == '-' || c[0] == '*' || c[0] == '/' || c[0] == '%' || c[0] == '^' || c[0] == '!' || c[0] == '<' || c[0] == '>' || c[0] == '='))
                    len = 2;
                // ++  --  &&  ||
                else if (c[0] == c[1] && (c[0] == '+' || c[0] == '-' || c[0] == '|' || c[0] == '&'))
                    len = 2;
                // ->
                else if (c[0] == '-' && c[1] == '>')
                    len = 2;
                // >>  <<  >>=  <<=
                else if (c[0] == c[1] && (c[0] == '<' || c[0] == '>'))
                    len = (c[2] == '=') ? 3 : 2;
                // Single character operators:
                else
                    len = 1;
            }
        }
        if (token.type == TOKEN__NULL) {
            ERROR("Unexpected character '%c'\n", c[0]);
            len = 1;
        }

        token.line_num = line_num;
        token.column_num = column_num;
        token.str = c;
        token.len = len;
        tokens.push(token);

        c += len;
        column_num += len;
    }
    tokens.push({}); // Null token
    return tokens;
}


Ast parse_msg_struct(Array<Token> tokens, Token** token_ptr) {
    Ast ast = {};
    ast.type = AST_MSG;
    Token* token = *token_ptr;
    if (token->type != TOKEN_IDENTIFIER) ERROR("expected identifier, but got '%.*s'", token->len, token->str);
    ast.msg.name = token->str;
    ast.msg.name_len = token->len;
    token++;
    TOKEN_EXPECT_STR(token, "(");
    token++;
    while (true) {
        int datatype;
        if (!token->type) ERROR("Missing expected token");
        if (token->str[0] == ')') break;
        char* str = token->str;
        if      (str[0] == 'u')  datatype = 0;
        else if (str[0] == 's')  datatype = DATATYPE_SIGNED;
        else if (str[0] == 'f')  datatype = DATATYPE_FLOAT;
        else ERROR("Unexpected token '%.*s'", token->len, token->str);
        str++;
        if      (strncmp(str, "8" , 1) == 0) { datatype |= DATATYPE_8;  str += 1; }
        else if (strncmp(str, "16", 2) == 0) { datatype |= DATATYPE_16; str += 2; }
        else if (strncmp(str, "32", 2) == 0) { datatype |= DATATYPE_32; str += 2; }
        else if (strncmp(str, "64", 2) == 0) { datatype |= DATATYPE_64; str += 2; }
        else ERROR("Unexpected token '%.*s'", token->len, token->str);
        if (str - token->str != token->len) ERROR("Unkown datatype '%.*s'", token->len, token->str);
        token++;
        if (token->type != TOKEN_IDENTIFIER) ERROR("Unexpected token '%.*s'", token->len, token->str);
        ast.msg.var_types.push(datatype);
        ast.msg.var_names.push(token->str);
        ast.msg.var_name_lens.push(token->len);
        token++;
        if (token->str && token->str[0] == ')') break;
        TOKEN_EXPECT_STR(token, ",");
        token++;
    }
    token++;
    TOKEN_EXPECT_STR(token, "{");
    ast.str = token->str+1;
    int scope_count = 1;
    while (true) {
        token++;
        if (!token->type) ERROR("Missing expected token");
        if (token->str[0] == '{') {
            scope_count++; 
            continue;
        }
        else if (token->str[0] == '}') {
            scope_count--;
            if (scope_count == 0) break;
            continue;
        }
    }
    ast.len = (int)(token->str - ast.str);
    token++;
    *token_ptr = token;
    ast.msg.id = g_msgs.length();
    g_msgs.push(ast);
    return ast;
}

Array<Ast> parse(Array<Token> tokens, char* str_code) {
    Token* token = &tokens[0];
    Array<Ast> array = {};
    char* raw_begin = str_code;
    char* raw_end = NULL;
    while (token->type) {
        raw_end = token->str;
        if (token->str[0] == '@') {
            token++;
            if (token->str && strncmp(token->str, "msg", token->len) == 0) {
                token++;
                Ast ast_raw;
                ast_raw.type = AST_RAW;
                ast_raw.str = raw_begin;
                ast_raw.len = (int)(raw_end - raw_begin);
                array.push(ast_raw);
                char* str = token->str;
                Ast ast_msg_struct = parse_msg_struct(tokens, &token);
                //ast_msg_struct.str = str;
                //ast_msg_struct.len = token->str - str + 1;
                array.push(ast_msg_struct);
                raw_begin = token->str + token->len;
                continue;
            } else {
                token--;
            }
        }
        raw_end = token->str + token->len;
        token++;
    }
    if (raw_begin && raw_begin < raw_end) {
        Ast ast_raw;
        ast_raw.type = AST_RAW;
        ast_raw.str = raw_begin;
        ast_raw.len = (int)(raw_end - raw_begin);
        array.push(ast_raw);
    }
    return array;
}


char* get_type_str(int datatype) {
    switch (datatype) {
    case DATATYPE_8:  return "u8";
    case DATATYPE_16: return "u16";
    case DATATYPE_32: return "u32";
    case DATATYPE_64: return "u64";
    case DATATYPE_8 | DATATYPE_SIGNED: return "s8";
    case DATATYPE_16 | DATATYPE_SIGNED: return "s16";
    case DATATYPE_32 | DATATYPE_SIGNED: return "s32";
    case DATATYPE_64 | DATATYPE_SIGNED: return "s64";
    case DATATYPE_32 | DATATYPE_FLOAT: return "f32";
    case DATATYPE_64 | DATATYPE_FLOAT: return "f64";
    }
    /*if      (datatype & DATATYPE_FLOAT ) fprintf(stream, "f");
    else if (datatype & DATATYPE_SIGNED) fprintf(stream, "s");
    else                                 fprintf(stream, "u");
    if      (datatype & DATATYPE_8 ) fprintf(stream, "8" );
    else if (datatype & DATATYPE_16) fprintf(stream, "16");
    else if (datatype & DATATYPE_32) fprintf(stream, "32");
    else if (datatype & DATATYPE_64) fprintf(stream, "64");
    else assert(0);*/
}

void gen_code_serialize(FILE* stream, Ast ast, bool deserialize) {
    /*if (deserialize)
        fprintf(stream, "\nvoid deserialize(%.*s* msg) {\n", ast.msg.name_len, ast.msg.name);
    else
        fprintf(stream, "\nvoid serialize(%.*s* msg) {\n", ast.msg.name_len, ast.msg.name);*/
    if (!deserialize)
        fprintf(stream, "    *(u16*)(_data+0) = htons(%i);\n", ast.msg.id);
    int byte_index = 2;
    for (int i = 0; i < ast.msg.var_names.length(); i++) {
        char* var_name = ast.msg.var_names[i];
        int var_name_len = ast.msg.var_name_lens[i];
        int datatype = ast.msg.var_types[i];
        int data_size = (datatype & 15);
        char* base_function = (deserialize) ? "ntoh" : "hton";
        char* function = NULL;
        if      ((datatype & DATATYPE_FLOAT) && (datatype & DATATYPE_32)) function = "f";
        else if ((datatype & DATATYPE_FLOAT) && (datatype & DATATYPE_64)) function = "d";
        else if (datatype & DATATYPE_8)  function = "c";
        else if (datatype & DATATYPE_16) function = "s";
        else if (datatype & DATATYPE_32) function = "l";
        else if (datatype & DATATYPE_64) function = "ll";
        else ERROR("Unkown datatype");
        if (deserialize)
            fprintf(stream, "    %s %.*s = ntoh%s(*(%s*)(_data+%i));\n", get_type_str(datatype), var_name_len, var_name, function, get_type_str(datatype), byte_index);
        else
            fprintf(stream, "    *(%s*)(_data+%i) = hton%s(%.*s);\n", get_type_str(datatype), byte_index, function, var_name_len, var_name);
        byte_index += data_size;
        //printf("    %.*s = %s%s(%.*s);\n", var_name_len, var_name, base_function, function, var_name_len, var_name);
    }
   // fprintf(stream, "}\n");
}

void gen_code(FILE* stream, Ast ast) {
    switch (ast.type) {
    case AST_MSG: {

        fprintf(stream, "void _%.*s_receive(u8* _data) {\n", ast.msg.name_len, ast.msg.name);
        gen_code_serialize(stream, ast, true);
        fprintf(stream, "%.*s", ast.len, ast.str);
        fprintf(stream, "}\n");

        fprintf(stream, "void %.*s(", ast.msg.name_len, ast.msg.name);
        int total_size = 2;
        for (int i = 0; i < ast.msg.var_names.length(); i++) {
            char* var_name = ast.msg.var_names[i];
            int var_name_len = ast.msg.var_name_lens[i];
            int datatype = ast.msg.var_types[i];
            int data_size = (datatype & 15);
            total_size += data_size;
            if (i != 0) fprintf(stream, ", ");
            fprintf(stream, get_type_str(datatype));
            fprintf(stream, " %.*s", var_name_len, var_name);
        }
        fprintf(stream, ") {\n");
        //gen_code_serialize(stream, ast, false);
        fprintf(stream, "    u8* _data = malloc(%i);\n", total_size);
        gen_code_serialize(stream, ast, false);
        fprintf(stream, "\n    if (g_is_server) _%.*s_receive(_data);\n", ast.msg.name_len, ast.msg.name);
        fprintf(stream, "}\n");

        //gen_code_serialize(stream, ast, true);
        break;
    }
    default:
        fprintf(stream, "%.*s", ast.len, ast.str);
        break;
    }
}

int main() {
    Array<char*> input_files = {};
    Array<char> output = {};
    input_files.push("src/msg.meta.h");
    input_files.push("src/ent.meta.h");
    for (int i = 0; i < input_files.length(); i++) {
        char* src = "@msg msg_sten(u8 a, u16 b, u32 c, u64 d, f32 x, f64 y) {\n    printf(\"msg_sten:%i\\n\", a); \n}\n\n";// read_file(input_files[i]);
        Array<Token> tokens = tokenize(src);
        Array<Ast> ast_array = parse(tokens, src);
        for (int i = 0; i < ast_array.length(); i++) {
            Ast ast = ast_array[i];
            gen_code(stdout, ast);
        }
        fprintf(stdout, "void receive(void* data, size_t len) {\n");
        fprintf(stdout, "    size_t index = 0;\n");
        fprintf(stdout, "    while(index < len) {\n");
        fprintf(stdout, "        u16 id = ntohs(*(u16*)(data+index));\n");
        fprintf(stdout, "        switch(id) {\n");
        for (int i = 0; i < g_msgs.length(); i++) {
            fprintf(stdout, "            case %i: _%.*s_receive(data+index); break;\n", i, g_msgs[i].msg.name_len, g_msgs[i].msg.name);
        }
        fprintf(stdout, "            default: assert(0);\n");
        fprintf(stdout, "        }\n");
        fprintf(stdout, "    }\n");
        fprintf(stdout, "    assert(index == len);\n");
        fprintf(stdout, "}\n");
        getchar();
        printf("...");
    }
    return 0;
}