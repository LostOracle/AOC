
const { deepStrictEqual } = require('assert');
const { getLinesFromFile } = require('../util/util');

// recursively convert expression into a tree we can use to evaluate said expression
// tree is a nested array with each element either being an operation like ['operator', operand]
// or ['operator', [nested operations]]
function expressionTree(expression) {
    const tree = [['+']];
    let index = 0;
    while(index < expression.length) {
        const char = expression.charAt(index);
        switch(char) {
            case '+':
            case '*':
                tree.push([char]);
                break;
            case '(':
                // start a new sub expression
                const subexpressionResult = expressionTree(expression.substring(index + 1));
                tree[tree.length - 1].push(subexpressionResult.tree);
                index += subexpressionResult.charactersProcessed + 1;
                break;
            case ')':
                // we have finished a sub expression; return results
                return {
                    charactersProcessed: index,
                    tree,
                };
            case ' ':
                // space has no power here. it's a ground war.
                break;
            default:
                // number
                const number = parseInt(char);
                tree[tree.length - 1].push(number);
        }
        index++;
    }
    return tree;
}

// evaluate a math expression from a string
function evaluateExpressionPart1(expression) {
    const tree = expressionTree(expression);
    return evaluateExpressionTree(tree);
}

// like part 1 but addition happens before multiplication
function evaluateExpressionPart2(expression) {
    const tree = expressionTree(expression);
    const operationsOrdered = imposeOrderOfOperations(tree);
    return evaluateExpressionTree(operationsOrdered);
}

// evaluate expression tree
function evaluateExpressionTree(tree) {
    return tree.reduce((total, expression) => {
        // operation on subexpression
        if (Array.isArray(expression[1]))
            return evaluateOperation(total, evaluateExpressionTree(expression[1]), expression[0]);

        // plain operation on number
        else
            return evaluateOperation(total, expression[1], expression[0]);
    }, 0);
}

function evaluateOperation(operand1, operand2, operator) {
    if (operator === '+') return operand1 + operand2;
    else if (operator === '*') return operand1 * operand2;
    else throw new Error(`unknown operator ${operator}`);
}

// rearrange expression tree such that all addition operations are solved before multiplication
function imposeOrderOfOperations(tree) {
    let i = 0;
    while(i < tree.length) {
        if (Array.isArray(tree[i][1])) tree[i][1] = imposeOrderOfOperations(tree[i][1]);
        if (i > 0 && tree[i][0] === '+' && tree[i-1][0] === '*') {
            // here we have an addition after a multiplication. solve the addition first
            tree[i-1][1] = [
                [
                    '+',
                    tree[i-1][1],
                ],[
                    '+',
                    tree[i][1],
                ]
            ];
            tree.splice(i, 1);
            continue;
        }
        i++;
    }
    return tree.filter(operation => operation[0] !== 'removed');
}

function part1(filename) {
    return sumHomeworkResults(filename, evaluateExpressionPart1);
}

function part2(filename) {
    return sumHomeworkResults(filename, evaluateExpressionPart2);
}

function sumHomeworkResults(filename, expressionEvaluator) {
    const expressions = getLinesFromFile(filename);
    const results = expressions.map(expression => expressionEvaluator(expression));
    return results.reduce((total, result) => total + result, 0);

}

function test() {
    deepStrictEqual(
        expressionTree('1 + 2 * 3 + 4 * 5 + 6'),
        [ [ '+', 1 ], [ '+', 2 ], [ '*', 3 ], [ '+', 4 ], [ '*', 5 ], [ '+', 6 ] ]
    );
    deepStrictEqual(evaluateExpressionPart1('1 + 2 * 3 + 4 * 5 + 6'), 71);
    deepStrictEqual(evaluateExpressionPart1('2 * 3 + (4 * 5)'), 26);
    deepStrictEqual(evaluateExpressionPart1('5 + (8 * 3 + 9 + 3 * 4 * 3)'), 437);
    deepStrictEqual(evaluateExpressionPart1('5 * 9 * (7 * 3 * 3 + 9 * 3 + (8 + 6 * 4))'), 12240);
    deepStrictEqual(evaluateExpressionPart1('((2 + 4 * 9) * (6 + 9 * 8 + 6) + 6) + 2 + 4 * 2'), 13632);
    deepStrictEqual(evaluateExpressionPart1('(1 + 2) * (3 + 4) + 5'), 26);

    deepStrictEqual(evaluateExpressionPart2('1 + (2 * 3) + (4 * (5 + 6))'), 51);
    deepStrictEqual(evaluateExpressionPart2('2 * 3 + (4 * 5)'), 46);
    deepStrictEqual(evaluateExpressionPart2('5 + (8 * 3 + 9 + 3 * 4 * 3)'), 1445);
    deepStrictEqual(evaluateExpressionPart2('5 * 9 * (7 * 3 * 3 + 9 * 3 + (8 + 6 * 4))'), 669060);
    deepStrictEqual(evaluateExpressionPart2('((2 + 4 * 9) * (6 + 9 * 8 + 6) + 6) + 2 + 4 * 2'), 23340);
}

function main() {
    console.log(`Part 1: Sum of answers is ${part1('18.txt')}`);
    console.log(`Part 2: Sum of answers with order of operations is ${part2('18.txt')}`);
}

test();
main();
