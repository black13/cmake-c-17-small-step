#include <iostream>
#include <memory>

// Base Expression Template
struct Expression {
    virtual ~Expression() = default;
    virtual std::unique_ptr<Expression> step() const = 0; // Define the small-step transition
    virtual std::unique_ptr<Expression> clone() const = 0; // Define the clone method
    virtual bool isValue() const { return false; } // Check if it's a final value
};

// Value (final state)
struct Value : public Expression {
    int value;
    Value(int v) : value(v) {}
    std::unique_ptr<Expression> step() const override { return nullptr; }
    std::unique_ptr<Expression> clone() const override { return std::make_unique<Value>(value); }
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
            // Assume left and right are of Value type, perform addition
            return std::make_unique<Value>(static_cast<Value*>(left.get())->value + static_cast<Value*>(right.get())->value);
        }
    }
    std::unique_ptr<Expression> clone() const override { return std::make_unique<Add>(left->clone(), right->clone()); }
};

// Function to perform the small-step evaluation
std::unique_ptr<Expression> evaluate(std::unique_ptr<Expression> expr) {
    while (!expr->isValue()) {
        expr = expr->step();
    }
    return expr;
}

int main() {
    // Build an expression for (1 + (2 + 3))
    auto expr = std::make_unique<Add>(
        std::make_unique<Value>(1),
        std::make_unique<Add>(std::make_unique<Value>(2), std::make_unique<Value>(3))
    );

    // Evaluate the expression
    auto result = evaluate(std::move(expr));

    // Print the result
    std::cout << "Result: " << static_cast<Value*>(result.get())->value << std::endl;

    return 0;
}
