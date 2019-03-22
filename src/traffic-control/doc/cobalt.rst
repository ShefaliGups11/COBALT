.. include:: replace.txt


Cobalt queue disc
-----------------

This chapter describes the Cobalt ([Kath17]_, [Cake16]_, [Buf14]_ ) queue disc 
implementation in |ns3|.

Developed by  Kathleen Nichols, Van Jacobson, Eric Dumazet, Michael D. Taht,
and Jonathan Morton as a solution to the bufferbloat [Buf14]_ problem. 
Cobalt (CoDel-Blue-Alternate) is a queuing discipline which uses a 
combination of the Codel and Blue active queue management algorithms proposed
earlier. It was mainly developed to handle the unresponsive flows which fqCoDel
did not do so well.


Model Description
*****************

The source code for the Cobalt model is located in the directory
``src/traffic-control/model`` and consists of 2 files `cobalt-queue-disc.h` and
`cobalt-queue-disc.cc` defining a CobaltQueueDisc class and a helper
CobaltTimestampTag class. The code was ported to |ns3| by Vignesh Kanan,
Harsh Lara, Shefali Gupta, Jendaipou Palmei and Mohit P. Tahiliani based on 
Linux kernel code implemented by Kathleen Nichols, Van Jacobson, Eric Dumazet, 
Michael D. Taht, and Jonathan Morton.  

Stefano Avallone and Pasquale Imputato extend a great deal of help by providing 
the results obtained from the Linux model which is used to test its correctness 
by comparing with the results obtained from |ns3| model.

* class :cpp:class:`CobaltQueueDisc`: This class implements the main Cobalt
algorithm:

  * ``CobaltQueueDisc::DoEnqueue ()``: This routine tags a packet with the
current time before pushing it into the queue.  The timestamp tag is used by
``CobaltQueue::DoDequeue()`` to compute the packet's sojourn time.  If the
queue is full upon the packet arrival, this routine will drop the packet and
record the number of drops due to queue overflow, which is stored in
`m_stats.qLimDrop`.

  * ``CobaltQueueDisc::ShouldDrop ()``: This routine is
``CobaltQueueDisc::DoDequeue()``'s helper routine that determines whether a
packet should be dropped or not based on its sojourn time.  If the sojourn time
goes above `m_target` and remains above continuously for at least `m_interval`,
the routine returns ``true`` indicating that it is OK to drop the packet.
``Otherwise, it returns ``false``. This routine decides if a packet should be
dropped based on the dropping state of CoDel and drop probability of BLUE.
The idea is to have both algorithms running in parallel and their effectiveness
is decided by their respective parameters (Pdrop of BLUE and dropping state of
CoDel). If either of them decide to drop the packet, the packet is dropped.

  * ``CobaltQueueDisc::DoDequeue ()``: This routine performs the actual packet
``drop based on ``CobaltQueueDisc::ShouldDrop ()``'s return value and schedules
the next drop. Cobalt will decrease BLUE's drop probability if the queue is
empty. This will ensure that the queue does not underflow. Otherwise Cobalt will
take the next packet from the queue and calculate its drop state by running
CoDel and BLUE in parallel till there are none left to drop.

* class :cpp:class:`CobaltTimestampTag`: This class implements the timestamp
tagging for a packet.  This tag is used to compute the packet's sojourn time
(the difference between the time the packet is dequeued and the time it is
pushed into the queue). 



References
==========

[Kath17] Controlled Delay Active Queue Management (draft-ietf-aqm-fq-codel-07)
Available online at `<https://tools.ietf.org/html/draft-ietf-aqm-codel-07>`_.

[Cake16] Linux implementation of Cobalt as a part of the cake framework.
Available online at `<https://github.com/dtaht/sch_cake/blob/master/cobalt.c>`_.

[Buf14] Bufferbloat.net.  Available online at `<http://www.bufferbloat.net/>`_.


Attributes
==========

The key attributes that the CobaltQueue Disc class holds include the following:

* ``MaxSize:`` The maximum number of packets/bytes accepted by this queue disc.
* ``MinBytes:`` The Cobalt algorithm minbytes parameter. The default value is
1500 bytes.
* ``Interval:`` The sliding-minimum window. The default value is 100 ms.
* ``Target:`` The Cobalt algorithm target queue delay. The default value is
5 ms.
* ``Pdrop:`` Value of drop probability.
* ``Increment:`` Increment value of drop probability. Default value is 1./256 .
* ``Decrement:`` Decrement value of drop probability. Default value is 1./4096 .
* ``Count:`` Cobalt count.
* ``DropState:`` Dropping state of Cobalt. Default value is false.
* ``Sojourn:`` Per packet time spent in the queue.
* ``DropNext:`` Time until next packet drop.

Examples
========

The first example is `cobalt-example.cc` located in
``src/traffic-control/examples``.  To run the file (the first invocation below
shows the available command-line options):

::

   $ ./waf --run cobalt-example


Validation
**********

The Cobalt model is tested using :cpp:class:`CobaltQueueDiscTestSuite` class
defined in `src/traffic-control/test/Cobalt-queue-test-suite.cc`.
The suite includes 5 test cases:

* Test 1: Simple enqueue/dequeue with no drops.
* Test 2: Change of Blue's drop probability upon queue full
(Activation of Blue).

The test suite can be run using the following commands:

::

  $ ./waf configure --enable-examples --enable-tests
  $ ./waf build
  $ ./test.py -s cobalt-queue-disc

or

::

  $ NS_LOG="CobaltQueueDisc" ./waf --run "test-runner --suite=cobalt-queue-disc"
