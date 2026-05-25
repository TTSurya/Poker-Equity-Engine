# Poker Equity Engine ♠️♥️♦️♣️

A high-performance multiway Texas Hold'em equity calculator designed to estimate player win probabilities across different stages of the game. The engine supports preflop, flop, turn, and river scenarios using a combination of probabilistic simulation and exact computation techniques.

---

## Features

- Multiway equity calculation for multiple players
- Supports all game stages:
  - Preflop
  - Flop
  - Turn
  - River
- Optimized hand-strength evaluation
- Monte Carlo simulation for large preflop state spaces
- Exact combinatorial enumeration for postflop scenarios
- Correct handling of ties and split pots
- Efficient bit-level card representation for faster computation

---

## Methodology

### Preflop Evaluation

Before community cards are revealed, the number of possible game outcomes grows rapidly with player count. Evaluating every possible state becomes computationally expensive.

To address this, the engine uses:

- Monte Carlo simulation
- Random sampling of valid board states
- Equity estimation based on simulation outcomes

This provides accurate approximations while maintaining practical execution times.

---

### Postflop Evaluation

After community cards are revealed, the number of remaining possibilities reduces significantly.

For these scenarios, the engine switches to:

- Exact combinatorial enumeration

This enables precise equity computation by evaluating every valid remaining game state.

---

## Hand Rankings Supported

The evaluator detects and ranks all standard Texas Hold'em hands:

1. High Card  
2. Pair  
3. Two Pair  
4. Three of a Kind  
5. Straight  
6. Flush  
7. Full House  
8. Four of a Kind  
9. Straight Flush  
10. Royal Flush  

---

## Input Format

```text
t
n m

[player hole cards]

[community cards]
```

Where:

- `t` = number of test cases
- `n` = number of players
- `m` = number of revealed community cards

Card representation:

```text
RankSuit
```

Examples:

```text
Ah
Ks
Td
7c
```

Ranks:

```text
A K Q J T 9 8 7 6 5 4 3 2
```

Suits:

```text
c = Clubs
d = Diamonds
h = Hearts
s = Spades
```

---

## Example

Input:

```text
1
3 3
Ah As
Kh Kd
Qh Qd
2c 7h Js
```

Output:

```text
82.060 9.192 8.749 
```

Interpretation:

- Player 1 → 82.060% equity
- Player 2 → 9.192% equity
- Player 3 → 8.749% equity

---

## Tech Stack

- C++
- STL
- Monte Carlo Simulation
- Bit Manipulation
- Combinatorial Search
- Probability and Statistics

---

## Future Improvements

- Opponent range-based equity calculation
- Bayesian hand distribution modeling
- Parallelized simulation
- Lookup-table optimization
- Visualization tools and interface support

---

The project focuses on efficient probability computation and algorithmic optimization for evaluating decision-making under uncertainty in Texas Hold'em.
