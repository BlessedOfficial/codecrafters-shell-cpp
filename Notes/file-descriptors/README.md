# File Descriptors, Standard Streams, and Redirection

Notes on how a process talks to the outside world through numbered handles, and why a shell can redirect output without the program knowing.

---

## 1. What is a file descriptor?

A **file descriptor (FD)** is simply an integer that represents an open resource from the program's point of view.

For example:

```text
0
1
2
3
4
...
```

The operating system maintains a table for each process that basically says:

```text
FD      →      What it refers to
------------------------------------
0       →      terminal input
1       →      terminal output
2       →      terminal output
3       →      some file
4       →      some socket
```

So when your program says:

```c
write(1, "hello\n", 6);
```

it's essentially saying:

> OS, write these 6 bytes to whatever resource FD 1 **currently** represents.

The important word is **currently**.

FD 1 does not permanently mean "screen."

---

## 2. Why do 0, 1 and 2 exist?

When you start a normal terminal program:

```bash
./program
```

the shell/OS normally gives the program three already-open descriptors:

| FD | Name | Typical target |
| :--- | :--- | :--- |
| 0 | stdin | terminal input (keyboard) |
| 1 | stdout | terminal output |
| 2 | stderr | terminal output |

These are called the **standard streams**.

Think of them as three pipes your program gets automatically:

```text
                 YOUR PROGRAM
              ┌───────────────┐
              │               │
keyboard ────►│ FD 0          │  stdin
              │               │
              │               │
              │ FD 1 ────────►│─── terminal
              │               │  stdout
              │               │
              │ FD 2 ────────►│─── terminal
              │               │  stderr
              └───────────────┘
```

---

## 3. FD 0 — stdin

FD 0 is **standard input**.

For example:

```bash
cat
```

`cat` needs to read something. If you type `hello`, the characters are made available through FD 0.

Conceptually:

```text
Keyboard
   │
   ▼
Terminal
   │
   ▼
FD 0
   │
   ▼
cat
```

Inside C/C++ you could read from FD 0:

```c
char buffer[100];

read(0, buffer, 100);
```

You're saying: read up to 100 bytes from whatever FD 0 represents. Normally that's your terminal.

---

## 4. FD 1 — stdout

FD 1 is **standard output**.

For example:

```c
write(1, "Hello\n", 6);
```

means: write `"Hello\n"` to FD 1.

Normally:

```text
program
   │
   │ write(..., 1, ...)
   ▼
 FD 1
   │
   ▼
terminal
   │
   ▼
Hello
```

That's why `cout << "Hello";` eventually results in output appearing on your terminal.

There are layers involved (`cout` → C/C++ runtime → system calls), but conceptually it's writing to stdout.

---

## 5. FD 2 — stderr

FD 2 is **standard error**. It's another output stream.

For example:

```cpp
cerr << "Something went wrong\n";
```

goes to stderr.

Conceptually:

```text
                   ┌── FD 1 → normal output
program ───────────┤
                   └── FD 2 → error output
```

Both normally appear on the terminal.

That's why you might see:

```text
Hello
Something went wrong
```

and think: "They're both going to the screen, so what's the difference?"

The difference is that they're **different streams**. That becomes extremely useful with redirection.

---

## 6. This is where redirection becomes interesting

Suppose you run:

```bash
./program
```

You have:

```text
FD 0 → terminal
FD 1 → terminal
FD 2 → terminal
```

Now run:

```bash
./program > output.txt
```

The shell changes **FD 1**.

Instead of `FD 1 → terminal` it becomes `FD 1 → output.txt`.

So now:

```text
program
   │
   ├── FD 0 → terminal
   │
   ├── FD 1 → output.txt
   │
   └── FD 2 → terminal
```

Therefore `cout << "Hello";` doesn't appear on the screen. It goes into `output.txt`.

---

## 7. The crucial idea

Your program doesn't need to know whether FD 1 is the terminal or a file.

Your program just says: **"Write to FD 1."**

The OS handles where FD 1 actually points.

So:

```bash
./program
```

might produce `FD 1 → terminal`, while:

```bash
./program > output.txt
```

produces `FD 1 → output.txt`.

The program itself can be exactly the same.

---

## 8. Think of an FD as a numbered door

A useful analogy:

- FD 0 = Door 0
- FD 1 = Door 1
- FD 2 = Door 2

Your program doesn't care what's behind the door. It just says:

- "Give me something through Door 0"
- "Send this through Door 1"
- "Send this error through Door 2"

Normally:

```text
Door 0 → keyboard
Door 1 → terminal
Door 2 → terminal
```

But the shell can rearrange the doors. For example `./program > output.txt` becomes:

```text
Door 0 → keyboard
Door 1 → output.txt
Door 2 → terminal
```

---

## 9. Redirect stdout only — stderr stays on the terminal

Suppose your program does:

```cpp
cout << "Normal message\n";
cerr << "Error message\n";
```

Run:

```bash
./program > output.txt
```

You get:

**terminal:**

```text
Error message
```

**output.txt:**

```text
Normal message
```

Why? Because:

```text
FD 1 → output.txt
FD 2 → terminal
```

---

## 10. Redirect stderr

You can specifically redirect FD 2:

```bash
./program 2> errors.txt
```

Now:

```text
FD 0 → terminal
FD 1 → terminal
FD 2 → errors.txt
```

So:

```cpp
cout << "Normal";
cerr << "Error";
```

results in:

**terminal:** `Normal`

**errors.txt:** `Error`

---

## 11. You can redirect both

```bash
./program > output.txt 2> errors.txt
```

giving:

```text
              PROGRAM
             /       \
            /         \
       FD 1             FD 2
        │                 │
        ▼                 ▼
   output.txt        errors.txt
```

---

## 12. Why does the shell do this?

This is particularly important for the shell you're building in Codecrafters.

When you type:

```bash
./program > output.txt
```

the shell sees:

```text
./program
>
output.txt
```

The `>` isn't actually something `./program` needs to understand. **The shell handles it.**

Conceptually, the shell does something like:

```cpp
pid = fork();

if (pid == 0)
{
    // child

    // Open output.txt
    int fd = open("output.txt", ...);

    // Make FD 1 point to that file
    dup2(fd, 1);

    // Run program
    exec(...);
}
```

The important operation here is:

```c
dup2(fd, 1);
```

This effectively says: **make FD 1 refer to the same open file as `fd`.**

Then the shell executes the program.

---

## 13. This explains why `exec()` is so powerful

Imagine before exec:

```text
Shell:
FD 0 → terminal
FD 1 → output.txt
FD 2 → terminal
```

The shell then calls `exec(...)` and the new program **inherits those descriptors**.

So your program starts with:

```text
FD 0 → terminal
FD 1 → output.txt
FD 2 → terminal
```

The program doesn't know that the shell redirected stdout. It just knows `cout << "Hello";` and therefore writes to FD 1. FD 1 happens to be a file.

See also: [exec/README.md](../exec/README.md) and [fork/README.md](../fork/README.md).

---

## 14. This is why Unix programs are so composable

Consider:

```bash
cat file.txt | grep hello
```

Now you're doing something even more interesting:

```text
          FD 1                 FD 0
cat ─────────────► PIPE ─────────────► grep
```

For `cat`: `FD 1 → pipe`

For `grep`: `FD 0 → pipe`

So `cat` doesn't know that its output is going into `grep`. And `grep` doesn't know that its input is coming from `cat`.

They just use:

- stdout = FD 1
- stdin = FD 0

The shell connects them.

---

## 15. The big picture

You can think of a Linux process like this:

```text
                    PROCESS
              ┌─────────────────┐
              │                 │
 keyboard ───►│ FD 0 (stdin)    │
              │                 │
              │ FD 1 (stdout) ──┼────► ?
              │                 │
              │ FD 2 (stderr) ──┼────► ?
              │                 │
              └─────────────────┘
```

The `?` could be:

- terminal
- file
- pipe
- socket
- another device

That's the fundamental abstraction.

So don't memorize:

> FD 1 = screen

Instead memorize:

> FD 1 = the process's **standard output**.

Normally that output is connected to the terminal, but it can be redirected to practically anything that behaves like an appropriate file descriptor.

And similarly:

| FD | Meaning |
| :--- | :--- |
| 0 | standard input |
| 1 | standard output |
| 2 | standard error |

Once this clicks, commands like:

```text
>
>>
2>
2>&1
|
<
```

and system calls like:

```text
open()
close()
read()
write()
dup()
dup2()
pipe()
exec()
```

start fitting together into one coherent system.
