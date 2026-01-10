#import "@local/sysu-templates:0.3.0": exercise, solution

#show: exercise.with(
  title: "Exercise 17",
  subtitle: "Database System Concepts (Theory)",
  student: (name: "Langxi Yuan", id: "23336294"),
)

= 17.15

Consider the following two transactions:

#figure(enum(
  numbering: n => if n == 1 { [$T_13$:] } else { [$T_14$:] },
  [
    #set align(start)

    read($A$);

    read($B$);

    *if* $A = 0$ *then* $B := B + 1$;

    write($B$);

    #v(1em)
  ],
  [
    #set align(start)

    read($B$);

    read($A$);

    *if* $B = 0$ *then* $A := A + 1$;

    write($A$);
  ],
))

Let the consistency requirement be $A = 0 or B = 0$, with $A = B = 0$ as the initial values.

+ Show that every serial execution involving these two transactions preserves the consistency of the database.

+ Show a concurrent execution of $T_13$ and $T_14$ that produces a nonserializable schedule.

+ Is there a concurrent execution of $T_13$ and $T_14$ that produces a serializable schedule?

#solution[
  + There are only two serial orders:

    - Case 1: $T_13$ followed by $T_14$

      - Initial values: $A = B = 0$.

      - $T_13$: Reads $A = 0, B = 0$, condition true, sets $B := 1$.

      - $T_14$: Reads $A = 0, B = 1$, condition false, $A$ remains $0$.

      - Final values: $A = 0, B = 1$, consistent.

    - Case 2: $T_14$ followed by $T_13$
    
      - Initial values: $A = B = 0$.

      - $T_14$: Reads $B = 0, A = 0$, condition true, sets $A := 1$.

      - $T_13$: Reads $A = 1, B = 0$, condition false, $B$ remains $0$.

      - Final values: $A = 1, B = 0$, consistent.

  + Consider the following interleaved schedule:

    - $T_13$: read($A$), sees $0$.

    - $T_14$: read($B$), sees $0$.

    - $T_13$: read($B$), sees $0$.

    - $T_14$: read($A$), sees $0$.

    - $T_13$: $B := B + 1$; write($B$).

    - $T_14$: $A := A + 1$; write($A$).

    - Final values: $A = 1, B = 1$, inconsistent.

    This schedule is not serializable
    because it cannot be transformed into either of the serial orders without changing the final state.

  + Yes. Any interleaving that preserves the effect of one full transaction before the other is serializable.
]
