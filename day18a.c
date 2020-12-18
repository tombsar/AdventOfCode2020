#include "common.h"
#include "stringview.h"

enum Operator {
    OP_ADD,
    OP_MULT
};

enum ExpressionType {
    EXPR_LITERAL,
    EXPR_COMPOUND
};

struct ExpressionCompound {
    struct Expression * first;
    struct Expression * second;
    enum Operator op;
};

struct Expression {
    enum ExpressionType type;
    union {
        long int value;
        struct ExpressionCompound compound;
    };
};

long int evaluateExpression (struct Expression const * expr) {
    ASSERT(expr);
    long int result = 0;
    if (expr->type == EXPR_LITERAL) {
        return expr->value;
    } else {
        long int firstval = evaluateExpression(expr->compound.first);
        long int secondval = evaluateExpression(expr->compound.second);
        switch (expr->compound.op) {
            case OP_ADD: {
                result = firstval + secondval;
            } break;
            case OP_MULT: {
                result = firstval * secondval;
            } break;
            default: {
                ASSERT(0);
            }
        }
    }
    return result;
}

enum TokenType {
    TOKEN_NONE,
    TOKEN_INTEGER,
    TOKEN_ADD,
    TOKEN_MULT,
    TOKEN_OPEN_PAREN,
    TOKEN_CLOSE_PAREN
};

struct Token {
    enum TokenType type;
    long int value;
};

struct Expression * newLiteralExpression (long int value) {
    struct Expression * expr = malloc(sizeof(struct Expression));
    expr->type = EXPR_LITERAL;
    expr->value = value;
    return expr;
}

struct Expression * newCompoundExpression (struct Expression * first, struct Expression * second, enum Operator op) {
    struct Expression * expr = malloc(sizeof(struct Expression));
    expr->type = EXPR_COMPOUND;
    expr->compound.first = first;
    expr->compound.second = second;
    expr->compound.op = op;
    return expr;
}

void freeExpression (struct Expression * expr) {
    if (expr->type == EXPR_COMPOUND) {
        freeExpression(expr->compound.first);
        freeExpression(expr->compound.second);
        free(expr->compound.first);
        free(expr->compound.second);
        expr->compound.first = NULL;
        expr->compound.second = NULL;
    }
}

struct Expression * parseExpression (struct Token * start, struct Token * end) {
    size_t operator_stack_count = 0;
    struct Token operator_stack [128];

    struct Token * op = start;
    for (struct Token * it = start; it != end; ++it) {
        struct Token t = *it;
        switch (t.type) {
            case TOKEN_INTEGER: {
                ASSERT(op != end);
                *(op++) = t;
            } break;
            case TOKEN_ADD:
            case TOKEN_MULT: {
                while (operator_stack_count && operator_stack[operator_stack_count-1].type != TOKEN_OPEN_PAREN) {
                    ASSERT(op != end);
                    *(op++) = operator_stack[--operator_stack_count];
                }
                ASSERT(operator_stack_count < ARRAYCOUNT(operator_stack));
                operator_stack[operator_stack_count++] = t;
            } break;
            case TOKEN_OPEN_PAREN: {
                ASSERT(operator_stack_count < ARRAYCOUNT(operator_stack));
                operator_stack[operator_stack_count++] = t;
            } break;
            case TOKEN_CLOSE_PAREN: {
                ASSERT(operator_stack_count);
                while (operator_stack[operator_stack_count-1].type != TOKEN_OPEN_PAREN) {
                    ASSERT(op != end);
                    *(op++) = operator_stack[--operator_stack_count];
                    ASSERT(operator_stack_count);
                }
                if (operator_stack[operator_stack_count-1].type == TOKEN_OPEN_PAREN) {
                    operator_stack_count -= 1;
                }
            } break;
            default: {
                ASSERT(0);
            }
        }
    }
    while (operator_stack_count) {
        ASSERT(operator_stack[operator_stack_count-1].type != TOKEN_OPEN_PAREN);
        ASSERT(op != end);
        *(op++) = operator_stack[--operator_stack_count];
    }
    end = op;

#if 0
    for (struct Token * it = start; it != end; ++it) {
        switch (it->type) {
            case TOKEN_INTEGER: {
                if (it != start) {
                    putchar(' ');
                }
                printf("%li", it->value);
            } break;
            case TOKEN_ADD: {
                fputs(" +", stdout);
            } break;
            case TOKEN_MULT: {
                fputs(" *", stdout);
            } break;
            default: {
                ASSERT(0);
            }
        }
    }
    printf("\n");
#endif

    size_t expr_stack_count = 0;
    struct Expression * expr_stack [128];
    for (struct Token * it = start; it != end; ++it) {
        switch (it->type) {
            case TOKEN_INTEGER: {
                ASSERT(expr_stack_count < ARRAYCOUNT(expr_stack));
                expr_stack[expr_stack_count++] = newLiteralExpression(it->value);
            } break;
            case TOKEN_ADD: {
                ASSERT(expr_stack_count >= 2);
                struct Expression * second = expr_stack[expr_stack_count-1];
                struct Expression * first = expr_stack[expr_stack_count-2];
                expr_stack[expr_stack_count-2] = newCompoundExpression(first, second, OP_ADD);
                expr_stack_count = expr_stack_count-1;
            } break;
            case TOKEN_MULT: {
                ASSERT(expr_stack_count >= 2);
                struct Expression * second = expr_stack[expr_stack_count-1];
                struct Expression * first = expr_stack[expr_stack_count-2];
                expr_stack[expr_stack_count-2] = newCompoundExpression(first, second, OP_MULT);
                expr_stack_count = expr_stack_count-1;
            } break;
            default: {
                ASSERT(0);
            }
        }
    }

    ASSERT(expr_stack_count == 1);
    return expr_stack[0];
}

int main (int argc, char ** argv) {
    long int sum = 0;
    do {
        char buf [BUFSIZ];
        char * s = fgets(&(buf[0]), sizeof(buf), stdin);
        if (!s) {
            break;
        }

        size_t tokens_capacity = 1024;
        size_t tokens_count = 0;
        struct Token tokens [tokens_capacity];

        StringView_t line = sv_view_c_string(s);
        sv_eat_spaces(&line);
        while (!sv_is_empty(&line)) {
            char c = *line.start; // TODO: sv_peek
            if (isdigit(c)) {
                struct Token token = {
                    .type = TOKEN_INTEGER,
                    .value = strtol(line.start, (char **)&line.start, 10)
                };
                ASSERT(tokens_count < tokens_capacity);
                tokens[tokens_count++] = token;
            } else if (c == '+') {
                sv_eat_char(&line);
                struct Token token = {
                    .type = TOKEN_ADD
                };
                ASSERT(tokens_count < tokens_capacity);
                tokens[tokens_count++] = token;
            } else if (c == '*') {
                sv_eat_char(&line);
                struct Token token = {
                    .type = TOKEN_MULT
                };
                ASSERT(tokens_count < tokens_capacity);
                tokens[tokens_count++] = token;
            } else if (c == '(') {
                sv_eat_char(&line);
                struct Token token = {
                    .type = TOKEN_OPEN_PAREN
                };
                ASSERT(tokens_count < tokens_capacity);
                tokens[tokens_count++] = token;
            } else if (c == ')') {
                sv_eat_char(&line);
                struct Token token = {
                    .type = TOKEN_CLOSE_PAREN
                };
                ASSERT(tokens_count < tokens_capacity);
                tokens[tokens_count++] = token;
            } else {
                ERROR("Invalid token");
            }
            sv_eat_spaces(&line);
        }

        struct Expression * expr = parseExpression(&(tokens[0]), &(tokens[tokens_count]));
        long int result = evaluateExpression(expr);
        //printf("%li\n", result);
        sum += result;
        freeExpression(expr);
    } while (1);
    DISP(sum);
}
