#import "@local/sysu-templates:0.3.0": exercise, solution

#show: exercise.with(
  title: "Exercise 18",
  subtitle: "Database System Concepts (Theory)",
  student: (name: "Langxi Yuan", id: "23336294"),
)

= 18.2

Consider the following two transactions:

$
  T_13: & "read"(A); \
        & "read"(B); \
        & bold("if") A = 0 bold("then") B := B + 1; \
        & "write"(B). \
  T_14: & "read"(B); \
        & "read"(A); \
        & bold("if") B = 0 bold("then") A := A + 1; \
        & "write"(A). \
$

Add lock and unlock instructions to transactions $T_31$ and $T_32$ so that they observe the two-phase locking protocol.
Can the execution of these transactions result in a deadlock?
