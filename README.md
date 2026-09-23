# BST Log Analyzer

A C program that reads a macOS system log file and stores each process in a binary search tree (BST). For every process it keeps how many log lines it wrote, plus its latest timestamp and message. I built it for a Data Structures course.

## Features

- **Insert:** reads the log file line by line and adds each process to the tree. If the process is already there, its counter goes up and its latest entry is updated. Insertion is iterative, because the recursive version had trouble with the large input file.
- **Search:** finds a process by name and prints its log count, last timestamp and last message.
- **Delete:** removes a process from the tree. The two-children case uses the in-order successor.
- **List:** prints every process in alphabetical order (in-order traversal).
- **Statistics:** total logs, number of unique processes, the most active and the quietest process, and the average number of logs per process.
- **Tree depth:** measures how deep the tree is, to see how balanced it is.

## Results

On a macOS log with about 169,000 entries:

| Metric | Value |
|---|---|
| Unique processes | 301 |
| Tree depth | 15 |
| Theoretical minimum depth | ~9 (log2(301) ≈ 8.2) |

The tree is not self-balancing, so the order of the input affects its shape. A depth of 15 against a minimum of about 9 means some branches are longer than they need to be.

## Log format

The program expects the default output of the macOS `log show` command:

```
Timestamp                       Thread     Type        Activity   PID    TTL
2026-05-09 16:07:48.620752+0300 0x5aabc    Default     0x0        341    8    powerd: [com.apple.powerd:displayState] ...
```

The process name is the text before the first `:` in the message. Lines that don't have this shape are skipped.

## Limitations

- The tree is not balanced (see Results).
- Memory is not freed when the program ends.