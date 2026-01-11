#import "@local/sysu-templates:0.3.0": exercise, solution

#show: exercise.with(
  title: "Assignment 7: Wireless and Mobile Networks",
  subtitle: "Computer Networks (Theory)",
  student: (name: "Langxi Yuan", id: "23336294"),
)

= P1

Consider the single-sender CDMA example in @fig-1.
What would be the sender's output (for the 2 data bits shown)
if the sender's CDMA code were $(1, 1, -1, 1, 1, -1, -1, 1)$?

#figure(
  caption: [Figure 7.5 from textbook],
  image("assets/images/20260111-031227.png"),
) <fig-1>

#solution[
  Note that $"output" = "data" times "code"$. The sender's output is:

  - Block 1: $(-1, -1, 1, -1, -1, 1, 1, -1)$.

  - Block 2: $(1, 1, -1, 1, 1, -1, -1, 1)$.
]

= P5

Suppose there are two ISPs providing WiFi access in a particular caf\u{00e9},
with each ISP operating its own AP and having its own IP address block.

+ Further suppose that by accident, each ISP has configured its AP to operate over channel 11.
  Will the 802.11 protocol completely break down in this situation?
  Discuss what happens when two stations, each associated with a different ISP, attempt to transmit at the same time.

+ Now suppose that one AP operates over channel 1 and the other over channel 11.
  How do your answers change?

#solution[
  + No, the protocol will not break down.
    802.11 is specifically designed to handle shared physical mediums.
    Instead of breaking, the two networks will "coexist" by sharing the available airtime on Channel 11.
    However, the aggregate throughput (total speed) available to each ISP will be roughly halved
    because they are now competing for the same frequency resources.
    If they attempt to transmit at the same time, there will be a collision,
    and the 802.11 Protocol's collision avoidance mechanisms (like CSMA/CA) will kick in.


  + In this scenario, the two networks operate on non-overlapping channels.
    In the 2.4 GHz band, Channel 1 and Channel 11 are separated by enough frequency (50 MHz)
    that they do not interfere with each other.
]

= P8

Consider the scenario shown in @fig-2, in which there are four wireless nodes, A, B, C, and D.
The radio coverage of the four nodes is shown via the shaded ovals; all nodes share the same frequency.
When A transmits, it can only be heard/received by B;
when B transmits, both A and C can hear/receive from B;
when C transmits, both B and D can hear/receive from C;
when D transmits, only C can hear/receive from D.
Suppose now that each node has an infinite supply of messages that it wants to send to each of the other nodes.
If a message’s destination is not an immediate neighbor, then the message must be relayed.
For example, if A wants to send to D, a message from A must first be sent to B,
which then sends the message to C, which then sends the message to D.
Time is slotted, with a message transmission time taking exactly one time slot, e.g., as in slotted Aloha.
During a slot, a node can do one of the following:
(i) send a message,
(ii) receive a message (if exactly one message is being sent to it),
(iii) remain silent.
As always, if a node hears two or more simultaneous transmissions,
a collision occurs and none of the transmitted messages are received successfully.
You can assume here that there are no bit-level errors,
and thus if exactly one message is sent,
it will be received correctly by those within the transmission radius of the sender.

+ Suppose now that an omniscient controller (i.e., a controller that knows the state of every node in the network)
  can command each node to do whatever it (the omniscient controller) wishes,
  i.e., to send a message, to receive a message, or to remain silent.
  Given this omniscient controller, what is the maximum rate at which a data message can be transferred from C to A,
  given that there are no other messages between any other source/destination pairs?

+ Suppose now that A sends messages to B, and D sends messages to C.
  What is the combined maximum rate at which data messages can flow from A to B and from D to C?

+ Suppose now that A sends messages to B, and C sends messages to D.
  What is the combined maximum rate at which data messages can flow from A to B and from C to D?

+ Suppose now that the wireless links are replaced by wired links.
  Repeat questions (1) through (3) again in this wired scenario.
  Now suppose we are again in the wireless scenario,
  and that for every data message sent from source to destination,
  the destination will send an ACK message back to the source (e.g., as in TCP).
  Also suppose that each ACK message takes up one slot.
  Repeat questions (1)–(3) above for this scenario.

#figure(
  caption: [Figure 7.34 from textbook],
  image("assets/images/20260111-031405.png"),
) <fig-2>

#solution[
  Let $"mps"$ denote message(s) per slot.

  + Each message needs two transmissions: from C to B and from B to A.
    Because of interference, one message's C to B trip cannot overlap with another's B to A trip,
    so the combined maximum rate is $1 / 2 "mps"$.

  + Since the two transmissions do not overlap, the combined maximum rate is $2 "mps"$.

  + A and C cannot transmit simultaneously because of interference at B.
    Hence the combined maximum rate is $1 "mps"$.

  + For wired scenario:

    - For question (1), when $n$ messages are transmitted, the total slots used is $n + 1$,
      so the combined maximum rate is $n / (n + 1) "mps"$, which approaches $1 "mps"$ as $n -> oo$.

    - For question (2), the combined maximum rate remains the same.

    - For question (3), the combined maximum rate becomes $2 "mps"$.

    For wireless scenario with ACKs:

    - For question (1), the combined maximum rate is $1 / 4 "mps"$.

    - For question (2), the combined maximum rate is $1 "mps"$.

    - For question (3), let: #h(1fr)

      #figure(table(
        columns: 2,
        inset: 0.8em,
        table.header([*slot*], [*transmission*]),
        [1], $A -> B$,
        [2], $B stretch(->)^"ACK" A, C -> D$,
        [3], $A -> B, D stretch(->)^"ACK" C$,
        $dots.v$, $dots.v$,
        $n$, $A -> B, D stretch(->)^"ACK" C$,
        $n + 1$, $B stretch(->)^"ACK" A, C -> D$,
        $n + 2$, $D stretch(->)^"ACK" C$,
      ))

      The combined maximum rate is $(2 n) / (n + 2) "mps"$, which approaches $2 "mps"$ as $n -> oo$.
]
