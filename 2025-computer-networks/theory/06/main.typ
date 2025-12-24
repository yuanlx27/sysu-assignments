#import "@local/sysu-templates:0.3.0": exercise

#show: exercise.with(
  title: "Assignment 6",
  subtitle: "Computer Networks (Theory)",
  student: (name: "Langxi Yuan", id: "23336294"),
)

= P2

Show (give an example other than the one in @fig-1) that
two-dimensional parity checks can correct and detect a single bit error.
Show (give an example of) a double-bit error that can be detected but not corrected.

#figure(
  caption: [Figure 6.5 from textbook],
  image(width: 70%, "assets/images/20251221-142057.png"),
) <fig-1>

= P5

Consider the generator, $G = 1001$, and suppose that D has the value 11000111010.
What is the value of R?

= P11

Suppose four active nodes---nodes A, B, C and D---are competing for access to a channel using slotted ALOHA.
Assume each node has an infinite number of packets to send.
Each node attempts to transmit in each slot with probability $p$.
The first slot is numbered slot 1, the second slot is numbered slot 2, and so on.

+ What is the probability that node A succeeds for the first time in slot 5?

+ What is the probability that some node (either A, B, C or D) succeeds in slot 4?

+ What is the probability that the first success occurs in slot 3?

+ What is the efficiency of this four-node system?

= P14

Consider three LANs interconnected by two routers, as shown in @fig-2.

+ Assign IP addresses to all of the interfaces.
  For Subnet 1 use addresses of the form 192.168.1.xxx;
  for Subnet 2 use addresses of the form 192.168.2.xxx;
  and for Subnet 3 use addresses of the form 192.168.3.xxx.

+ Assign MAC addresses to all of the adapters.

+ Consider sending an IP datagram from Host E to Host B.
  Suppose all of the ARP tables are up to date.
  Enumerate all the steps, as done for the single-router example in Section 6.4.1.

+ Repeat (3), now assuming that the ARP table in the sending host is empty (and the other tables are up to date).

#figure(
  caption: [Figure 6.33 from textbook],
  image(width: 80%, "assets/images/20251221-144614.png"),
) <fig-2>

= P15

Consider @fig-2.
Now we replace the router between subnets 1 and 2 with a switch S1, and label the router between subnets 2 and 3 as R1.

+ Consider sending an IP datagram from Host E to Host F.
  Will Host E ask router R1 to help forward the datagram? Why?
  In the Ethernet frame containing the IP datagram, what are the source and destination IP and MAC addresses?

+ Suppose E would like to send an IP datagram to B, and assume that E's ARP cache does not contain B's MAC address.
  Will E perform an ARP query to find B's MAC address? Why?
  In the Ethernet frame (containing the IP datagram destined to B) that is delivered to router R1,
  what are the source and destination IP and MAC addresses?

+ Suppose Host A would like to send an IP datagram to Host B, and neither A's ARP cache contains B's MAC address nor does B's ARP cache contain A's MAC address.
  Further suppose that the switch S1's forwarding table contains entries for Host B and router R1 only.
  Thus, A will broadcast an ARP request message.
  What actions will switch S1 perform once it receives the ARP request message?
  Will router R1 also receive this ARP request message?
  If so, will R1 forward the message to Subnet 3?
  Once Host B receives this ARP request message, it will send back to Host A an ARP response message.
  But will it send an ARP query message to ask for A's MAC address? Why?
  What will switch S1 do once it receives an ARP response message from Host B?

= P21

Consider @fig-2 in problem P14.
Provide MAC addresses and IP addresses for the interfaces at Host A, both routers, and Host F.
Suppose Host A sends a datagram to Host F.
Give the source and destination MAC addresses in the frame encapsulating this IP datagram as the frame is transmitted
_(i)_ from A to the left router,
_(ii)_ from the left router to the right router,
_(iii)_ from the right router to F.
Also give the source and destination IP addresses in the IP datagram
encapsulated within the frame at each of these points in time.

= P26

Let's consider the operation of a learning switch in the context of a network
in which 6 nodes labeled A through F are star connected into an Ethernet switch.
Suppose that
_(i)_ B sends a frame to E,
_(ii)_ E replies with a frame to B,
_(iii)_ A sends a frame to B,
_(iv)_ B replies with a frame to A.
The switch table is initially empty.
Show the state of the switch table before and after each of these events.
For each of these events, identify the link(s) on which the transmitted frame will be forwarded,
and briefly justify your answers.

= P29

Consider the MPLS network shown in @fig-3, and suppose that routers R5 and R6 are now MPLS enabled.
Suppose that we want to perform traffic engineering
so that packets from R6 destined for A are switched to A via R6-R4-R3-R1,
and packets from R5 destined for A are switched via R5-R4-R2-R1.
Show the MPLS tables in R5 and R6, as well as the modified table in R4, that would make this possible.

#figure(
  caption: [Figure 6.29 from textbook],
  image(width: 80%, "assets/images/20251221-151252.png"),
) <fig-3>

= P32

Consider the data center network with hierarchical topology in @fig-4.
Suppose now there are 80 pairs of flows,
with ten flows between the first and ninth rack, ten flows between the second and tenth rack, and so on.
Further suppose that all links in the network are 10 Gbps,
except for the links between hosts and TOR switches, which are 1 Gbps.

+ Each flow has the same data rate;
  determine the maximum rate of a flow.

+ For the same traffic pattern, determine the maximum rate of a flow for the highly interconnected topology in @fig-5.

+ Now suppose there is a similar traffic pattern, but involving 20 hosts on each rack and 160 pairs of flows.
  Determine the maximum flow rates for the two topologies.

#figure(
  caption: [Figure 6.30 from textbook],
  image("assets/images/20251221-151851.png"),
) <fig-4>

#figure(
  caption: [Figure 6.31 from textbook],
  image("assets/images/20251221-151953.png"),
) <fig-5>
