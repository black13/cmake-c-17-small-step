#include <iostream>
#include <memory>

// Base Expression Template
struct Expression {
    virtual ~Expression() = default;
    virtual std::unique_ptr<Expression> step() const = 0;
    virtual std::unique_ptr<Expression> clone() const = 0;
    virtual bool isValue() const { return false; }
};

// Integer Value (final state)
struct Value : public Expression {
    int value;
    Value(int v) : value(v) {}
    std::unique_ptr<Expression> step() const override { return nullptr; }
    std::unique_ptr<Expression> clone() const override { return std::make_unique<Value>(value); }
    bool isValue() const override { return true; }
};

// Boolean Value
struct BoolValue : public Expression {
    bool value;
    BoolValue(bool v) : value(v) {}
    std::unique_ptr<Expression> step() const override { return nullptr; }
    std::unique_ptr<Expression> clone() const override { return std::make_unique<BoolValue>(value); }
    bool isValue() const override { return true; }
};

// Addition Expression
struct Add : public Expression {
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;
    Add(std::unique_ptr<Expression> l, std::unique_ptr<Expression> r) : left(std::move(l)), right(std::move(r)) {}
    std::unique_ptr<Expression> step() const override {
        if (!left->isValue()) {
            return std::make_unique<Add>(left->step(), right->clone());
        } else if (!right->isValue()) {
            return std::make_unique<Add>(left->clone(), right->step());
        } else {
            return std::make_unique<Value>(static_cast<Value*>(left.get())->value + static_cast<Value*>(right.get())->value);
        }
    }
    std::unique_ptr<Expression> clone() const override { return std::make_unique<Add>(left->clone(), right->clone()); }
};

// Conditional (Branching) Expression
struct IfElse : public Expression {
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Expression> trueBranch;
    std::unique_ptr<Expression> falseBranch;

    IfElse(std::unique_ptr<Expression> cond, std::unique_ptr<Expression> tBranch, std::unique_ptr<Expression> fBranch)
        : condition(std::move(cond)), trueBranch(std::move(tBranch)), falseBranch(std::move(fBranch)) {}

    std::unique_ptr<Expression> step() const override {
        if (!condition->isValue()) {
            return std::make_unique<IfElse>(condition->step(), trueBranch->clone(), falseBranch->clone());
        } else {
            if (static_cast<BoolValue*>(condition.get())->value) {
                return trueBranch->clone();
            } else {
                return falseBranch->clone();
            }
        }
    }

    std::unique_ptr<Expression> clone() const override {
        return std::make_unique<IfElse>(condition->clone(), trueBranch->clone(), falseBranch->clone());
    }
};

// Function to perform the small-step evaluation
std::unique_ptr<Expression> evaluate(std::unique_ptr<Expression> expr) {
    while (!expr->isValue()) {
        expr = expr->step();
    }
    return expr;
}

int main() {
    // Example: if (true) then 5 else (3 + 4)
    auto expr = std::make_unique<IfElse>(
        std::make_unique<BoolValue>(true),
        std::make_unique<Value>(5),
        std::make_unique<Add>(std::make_unique<Value>(3), std::make_unique<Value>(4))
    );

    // Evaluate the expression
    auto result = evaluate(std::move(expr));

    // Print the result
    std::cout << "Result: " << static_cast<Value*>(result.get())->value << std::endl;

    return 0;
}
