const { deepStrictEqual } = require('assert');
const { getGroupsFromFile } = require('../util/util');

function betweenInclusive(num, min, max) {
    return num >= min && num <= max;
}

// for each rule, we need to figure out which index in the ticket had the most matching items.
// so add a scoreboard to each rule object
function createRuleScoringDataStructure(rules, numbersInTicket) {
    return rules.map(rule => ({
        ...rule,
        scoreByIndex: Array(numbersInTicket).fill(0),
    }));
}

function getDataFromFile(filename) {
    // three groups - rules, your ticket, nearby tickets
    const inputGroups = getGroupsFromFile(filename);
    return {
        myTicket: getTicketFromLine(inputGroups[1][1]),
        rules: getRulesFromLines(inputGroups[0]),
        tickets: getTicketsFromLines(inputGroups[2].slice(1)),
    };
}

function getRuleFromLine(line) {
    const [throwaway, name, r1, r2, r3, r4] = /([\w\s]+): (\d+)-(\d+) or (\d+)-(\d+)/.exec(line);
    return { name, r1, r2, r3, r4 };
}

function getRulesFromLines(lines) {
    return lines.map(getRuleFromLine);
}

function getTicketFromLine(line) {
    return line.split(",").map(s => parseInt(s));
}

function getTicketsFromLines(lines) {
    return lines.map(getTicketFromLine);
}

// find all ticket numbers that match no rules
// return the sum of these numbers
function getTicketInvalidScore(ticket, rules) {
    const invalidMysteryNumbers = [];
    for (let mysteriousNumber of ticket) {
        let matchedRule = rules.find(rule => matchRule(mysteriousNumber, rule));
        if (!matchedRule) invalidMysteryNumbers.push(mysteriousNumber);        
    }
    return sum(invalidMysteryNumbers);
}

function isTicketValid(ticket, rules) {
    for (let mysteriousNumber of ticket) {
        let matchedRule = rules.find(rule => matchRule(mysteriousNumber, rule));
        if (!matchedRule) return false;     
    }
    return true;
}

function matchRule(num, rule) {
    return betweenInclusive(num, rule.r1, rule.r2) || betweenInclusive(num, rule.r3, rule.r4);
}

function part1(filename) {
    const data = getDataFromFile(filename);
    const ticketInvalidScores = data.tickets.map(ticket => getTicketInvalidScore(ticket, data.rules));
    return sum(ticketInvalidScores);
}

// so basically we go through all the valid tickets and tally up how many ticket indices match each rule
// then we iteratively go through the rules and find rules where only one index matches the rule for all tickets
// repeat this process until we map every rule to an index
// then we score the departure related values because part 2
function part2(filename) {
    const {myTicket, rules, tickets} = getDataFromFile(filename);
    const validTickets = tickets.filter(ticket => isTicketValid(ticket, rules));
    const rulesScoreboard = createRuleScoringDataStructure(rules, validTickets[0].length);
    validTickets.forEach(ticket => scoreTicketAgainstRules(ticket, rulesScoreboard));
    mapRulesToIndices(rulesScoreboard, validTickets.length);
    const departureRelatedValues = rulesScoreboard
        .filter(rule => rule.name.startsWith('departure'))
        .map(rule => myTicket[rule.index]);
    return departureRelatedValues.reduce((total, value) => total * value, 1);
}

// iteratively go through scoreboard and figure out the "obvious" rules - rules where only one
// index matched the rule for every ticket
// as we find the obvious ones, others will become "obvious"
function mapRulesToIndices(rulesScoreboard, numberOfTickets) {
    let ambiguityDetected = false;
    while(rulesScoreboard.filter(scoreboard => scoreboard.index === undefined).length > 0 && !ambiguityDetected) {
        let foundAnObviousOne = false;
        rulesScoreboard.filter(s => !s.index).forEach(ruleScoreboard => {
            const magicIndices = ruleScoreboard.scoreByIndex.filter(score => score === numberOfTickets);
            if (magicIndices.length === 1) {
                // found an obvious rule to index match
                const indexMatchingRule = ruleScoreboard.scoreByIndex.findIndex(score => score === numberOfTickets);
                ruleScoreboard.index = indexMatchingRule;
                // now no other rule can claim this index
                rulesScoreboard.forEach(ruleScoreboard2 => ruleScoreboard2.scoreByIndex[indexMatchingRule] = 0);
                foundAnObviousOne = true;
            }
        });
        if (!foundAnObviousOne) {
            // if we got here, then no rule has laid obvious claim to an index
            // if that happened this method wouldn't work
            console.log('found no obvious one - ambiguity detected');
            ambiguityDetected = true;
        }
    }
}

// see which rules are matched by each ticket field
function scoreTicketAgainstRules(ticket, rulesScoreboard) {
    for(let rule of rulesScoreboard) {
        for(let mysteriousNumberIndex in ticket) {
            if (matchRule(ticket[mysteriousNumberIndex], rule)) {
                // this index matched the rule
                rule.scoreByIndex[mysteriousNumberIndex]++;
            }
        }
    }
}

// return sum of all elements in array
function sum(ar) {
    return ar.reduce((total, element) => total + element, 0);
}

function test() {
    deepStrictEqual(part1('example.txt'), 71);
}

function main() {
    console.log(`Part 1: ${part1('16.txt')}`);
    console.log(`Part 2: ${part2('16.txt')}`);
}

test();
main();
