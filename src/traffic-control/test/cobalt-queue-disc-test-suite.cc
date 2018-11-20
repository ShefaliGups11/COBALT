#include "ns3/test.h"
#include "ns3/cobalt-queue-disc.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "ns3/string.h"
#include "ns3/double.h"
#include "ns3/log.h"
#include "ns3/simulator.h"

using namespace ns3;

// The following code borrowed from Linux cobalt.h, for unit testing
#define REC_INV_SQRT_BITS_ns3 (8 * sizeof(uint16_t))
/* or sizeof_in_bits(rec_inv_sqrt) */
/* needed shift to get a Q0.32 number from rec_inv_sqrt */
#define REC_INV_SQRT_SHIFT_ns3 (32 - REC_INV_SQRT_BITS_ns3)


static uint16_t _codel_Newton_step (uint32_t count, uint16_t rec_inv_sqrt)
{
  uint32_t invsqrt = ((uint32_t)rec_inv_sqrt) << REC_INV_SQRT_SHIFT_ns3;
  uint32_t invsqrt2 = ((uint64_t)invsqrt * invsqrt) >> 32;
  uint64_t val = (3LL << 32) - ((uint64_t)count * invsqrt2);

  val >>= 2;
  val = (val * invsqrt) >> (32 - 2 + 1);
  return (val >> REC_INV_SQRT_SHIFT_ns3);
}

static uint32_t _reciprocal_scale (uint32_t val, uint32_t ep_ro)
{
  return (uint32_t)(((uint64_t)val * ep_ro) >> 32);
}


// End Linux borrow

/**
 * \ingroup traffic-control-test
 * \ingroup tests
 *
 * \brief Cobalt Queue Disc Test Item
 */
class CobaltQueueDiscTestItem : public QueueDiscItem {
public:
  /**
   * Constructor
   *
   * \param p packet
   * \param addr address
   * \param protocol
   */
  
  CobaltQueueDiscTestItem (Ptr<Packet> p, const Address & addr,uint16_t protocol, bool ecnCapable);
  virtual ~CobaltQueueDiscTestItem ();
  virtual void AddHeader (void);
  virtual bool Mark(void);

private:
  CobaltQueueDiscTestItem ();
  /**
   * \brief Copy constructor
   * Disable default implementation to avoid misuse
   */
  CobaltQueueDiscTestItem (const CobaltQueueDiscTestItem &);
  /**
   * \brief Assignment operator
   * \return this object
   * Disable default implementation to avoid misuse
   */
  CobaltQueueDiscTestItem &operator = (const CobaltQueueDiscTestItem &);
  bool m_ecnCapablePacket; ///< ECN capable packet?
};



CobaltQueueDiscTestItem::CobaltQueueDiscTestItem (Ptr<Packet> p, const Address & addr,uint16_t protocol, bool ecnCapable)
  : QueueDiscItem (p, addr, ecnCapable),
  m_ecnCapablePacket (ecnCapable)
{
}

CobaltQueueDiscTestItem::~CobaltQueueDiscTestItem ()
{
}

void
CobaltQueueDiscTestItem::AddHeader (void)
{
}

bool
CobaltQueueDiscTestItem::Mark (void)
{
  if (m_ecnCapablePacket)
    {
      return true;
    }
  return false;
}

/**
 * \ingroup traffic-control-test
 * \ingroup tests
 *
 * \brief Test 1: simple enqueue/dequeue with no drops
 */
class CobaltQueueDiscBasicEnqueueDequeue : public TestCase
{
public:
  /**
   * Constructor
   *
   * \param mode the mode
   */
  CobaltQueueDiscBasicEnqueueDequeue (QueueSizeUnit mode);
  virtual void DoRun (void);

  /**
   * Queue test size function
   * \param queue the queue disc
   * \param size the size
   * \param error the error string
   * 
   */
  
  private:
  QueueSizeUnit m_mode; ///< mode
};



CobaltQueueDiscBasicEnqueueDequeue::CobaltQueueDiscBasicEnqueueDequeue (QueueSizeUnit mode)
  : TestCase ("Basic enqueue and dequeue operations, and attribute setting"+ mode)
{
  m_mode = mode;
}


void
CobaltQueueDiscBasicEnqueueDequeue::DoRun (void)
{
  Ptr<CobaltQueueDisc> queue = CreateObject<CobaltQueueDisc> ();

  uint32_t pktSize = 1000;
  uint32_t modeSize = 0;
  
  Address dest;

  NS_TEST_EXPECT_MSG_EQ (queue->SetAttributeFailSafe ("MinBytes", UintegerValue (pktSize)), true,
                         "Verify that we can actually set the attribute MinBytes");
  NS_TEST_EXPECT_MSG_EQ (queue->SetAttributeFailSafe ("Interval", StringValue ("50ms")), true,
                         "Verify that we can actually set the attribute Interval");
  NS_TEST_EXPECT_MSG_EQ (queue->SetAttributeFailSafe ("Target", StringValue ("4ms")), true,
                         "Verify that we can actually set the attribute Target");

  if (m_mode == QueueSizeUnit::BYTES)
    {
      modeSize = pktSize;
    }
  else if (m_mode == QueueSizeUnit::PACKETS)
    {
      modeSize = 1;
    }
  NS_TEST_EXPECT_MSG_EQ (queue->SetAttributeFailSafe ("MaxSize", QueueSizeValue (QueueSize (m_mode, modeSize * 1500))),
                         true, "Verify that we can actually set the attribute MaxSize");
  queue->Initialize ();

  Ptr<Packet> p1, p2, p3, p4, p5, p6;
  p1 = Create<Packet> (pktSize);
  p2 = Create<Packet> (pktSize);
  p3 = Create<Packet> (pktSize);
  p4 = Create<Packet> (pktSize);
  p5 = Create<Packet> (pktSize);
  p6 = Create<Packet> (pktSize);

  NS_TEST_EXPECT_MSG_EQ (queue->GetCurrentSize().GetValue (), 0 * modeSize, "There should be no packets in queue");
  queue->Enqueue (Create<CobaltQueueDiscTestItem> (p1, dest,0, false));
  NS_TEST_EXPECT_MSG_EQ (queue->GetCurrentSize().GetValue (), 1 * modeSize, "There should be one packet in queue");
  queue->Enqueue (Create<CobaltQueueDiscTestItem> (p2, dest,0, false));
  NS_TEST_EXPECT_MSG_EQ (queue->GetCurrentSize().GetValue (), 2 * modeSize, "There should be two packets in queue");
  queue->Enqueue (Create<CobaltQueueDiscTestItem> (p3, dest,0, false));
  NS_TEST_EXPECT_MSG_EQ (queue->GetCurrentSize().GetValue (), 3 * modeSize, "There should be three packets in queue");
  queue->Enqueue (Create<CobaltQueueDiscTestItem> (p4, dest,0, false));
  NS_TEST_EXPECT_MSG_EQ (queue->GetCurrentSize().GetValue (), 4 * modeSize, "There should be four packets in queue");
  queue->Enqueue (Create<CobaltQueueDiscTestItem> (p5, dest,0, false));
  NS_TEST_EXPECT_MSG_EQ (queue->GetCurrentSize().GetValue (), 5 * modeSize, "There should be five packets in queue");
  queue->Enqueue (Create<CobaltQueueDiscTestItem> (p6, dest,0, false));
  NS_TEST_EXPECT_MSG_EQ (queue->GetCurrentSize().GetValue (), 6 * modeSize, "There should be six packets in queue");

  
  NS_TEST_EXPECT_MSG_EQ (queue->GetDropOverLimit (), 0, "There should be no packets being dropped due to full queue");

  Ptr<QueueDiscItem> item;

  item = queue->Dequeue ();
  NS_TEST_EXPECT_MSG_EQ ((item != 0), true, "I want to remove the first packet");
  NS_TEST_EXPECT_MSG_EQ (queue->GetCurrentSize().GetValue (), 5 * modeSize, "There should be five packets in queue");
  NS_TEST_EXPECT_MSG_EQ (item->GetPacket ()->GetUid (), p1->GetUid (), "was this the first packet ?");

  item = queue->Dequeue ();
  NS_TEST_EXPECT_MSG_EQ ((item != 0), true, "I want to remove the second packet");
  NS_TEST_EXPECT_MSG_EQ (queue->GetCurrentSize().GetValue (), 4 * modeSize, "There should be four packets in queue");
  NS_TEST_EXPECT_MSG_EQ (item->GetPacket ()->GetUid (), p2->GetUid (), "Was this the second packet ?");

  item = queue->Dequeue ();
  NS_TEST_EXPECT_MSG_EQ ((item != 0), true, "I want to remove the third packet");
  NS_TEST_EXPECT_MSG_EQ (queue->GetCurrentSize().GetValue (), 3 * modeSize, "There should be three packets in queue");
  NS_TEST_EXPECT_MSG_EQ (item->GetPacket ()->GetUid (), p3->GetUid (), "Was this the third packet ?");

  item = queue->Dequeue ();
  NS_TEST_EXPECT_MSG_EQ ((item != 0), true, "I want to remove the forth packet");
  NS_TEST_EXPECT_MSG_EQ (queue->GetCurrentSize().GetValue (), 2 * modeSize, "There should be two packets in queue");
  NS_TEST_EXPECT_MSG_EQ (item->GetPacket ()->GetUid (), p4->GetUid (), "Was this the fourth packet ?");

  item = queue->Dequeue ();
  NS_TEST_EXPECT_MSG_EQ ((item != 0), true, "I want to remove the fifth packet");
  NS_TEST_EXPECT_MSG_EQ (queue->GetCurrentSize().GetValue (), 1 * modeSize, "There should be one packet in queue");
  NS_TEST_EXPECT_MSG_EQ (item->GetPacket ()->GetUid (), p5->GetUid (), "Was this the fifth packet ?");

  item = queue->Dequeue ();
  NS_TEST_EXPECT_MSG_EQ ((item != 0), true, "I want to remove the last packet");
  NS_TEST_EXPECT_MSG_EQ (queue->GetCurrentSize().GetValue (), 0 * modeSize, "There should be zero packet in queue");
  NS_TEST_EXPECT_MSG_EQ (item->GetPacket ()->GetUid (), p6->GetUid (), "Was this the sixth packet ?");

  item = queue->Dequeue ();
  NS_TEST_EXPECT_MSG_EQ ((item == 0), true, "There are really no packets in queue");

 
  NS_TEST_EXPECT_MSG_EQ (queue->GetDropCount (), 0, "There should be no packet drops according to Cobalt algorithm");
}


/**
 * \ingroup traffic-control-test
 * \ingroup tests
 *
 * \brief Test 2: NewtonStep unit test - test against explicit port of Linux implementation
 */
class CobaltQueueDiscNewtonStepTest : public TestCase
{
public:
  CobaltQueueDiscNewtonStepTest ();
  virtual void DoRun (void);
};

CobaltQueueDiscNewtonStepTest::CobaltQueueDiscNewtonStepTest ()
  : TestCase ("NewtonStep arithmetic unit test")
{
}

void
CobaltQueueDiscNewtonStepTest::DoRun (void)
{
  Ptr<CobaltQueueDisc> queue = CreateObject<CobaltQueueDisc> ();

  // Spot check a few points in the expected operational range of
  // CobaltQueueDisc's m_count and m_recInvSqrt variables
  uint32_t count = 2;
  uint16_t recInvSqrt = 65535;
  queue->m_count = count;
  queue->m_recInvSqrt = recInvSqrt;
  queue->NewtonStep ();
  // Test that ns-3 value is exactly the same as the Linux value
  NS_TEST_ASSERT_MSG_EQ (_codel_Newton_step (count, recInvSqrt), queue->m_recInvSqrt,
                         "ns-3 NewtonStep() fails to match Linux equivalent");

  count = 4;
  recInvSqrt = 36864;
  queue->m_count = count;
  queue->m_recInvSqrt = recInvSqrt;
  queue->NewtonStep ();
  // Test that ns-3 value is exactly the same as the Linux value
  NS_TEST_ASSERT_MSG_EQ (_codel_Newton_step (count, recInvSqrt), queue->m_recInvSqrt,
                         "ns-3 NewtonStep() fails to match Linux equivalent");
}

/**
 * \ingroup traffic-control-test
 * \ingroup tests
 *
 * \brief Test 3: ControlLaw unit test - test against explicit port of Linux implementation
 */
class CobaltQueueDiscControlLawTest : public TestCase
{
public:
  CobaltQueueDiscControlLawTest ();
  virtual void DoRun (void);
  /**
   * Codel control law function
   * \param queue the queue disc
   * \param t
   * \returns the codel control law
   */
  uint32_t _cobalt_control_law (Ptr<CobaltQueueDisc> queue, uint32_t t);
};

CobaltQueueDiscControlLawTest::CobaltQueueDiscControlLawTest ()
  : TestCase ("ControlLaw arithmetic unit test")
{
}

// The following code borrowed from Linux codel.h,
// except the addition of queue parameter
uint32_t
CobaltQueueDiscControlLawTest::_cobalt_control_law (Ptr<CobaltQueueDisc> queue, uint32_t t)
{
  return t + _reciprocal_scale (queue->Time2CoDel (queue->m_interval), queue->m_recInvSqrt << REC_INV_SQRT_SHIFT_ns3);
}
// End Linux borrrow

void
CobaltQueueDiscControlLawTest::DoRun (void)
{
  Ptr<CobaltQueueDisc> queue = CreateObject<CobaltQueueDisc> ();

  /* Spot check a few points of m_dropNext
   The integer approximations in Linux should be within
   2% of the true floating point value obtained in ns-3
   */
  uint32_t dropNextTestVals [4] = {292299, 341128, 9804717, 55885007};

  for (int i = 0; i < 4; ++i)
    {
      uint32_t ns3Result = queue->ControlLaw (dropNextTestVals[i]);
      uint32_t upperBound = ns3Result + 0.02 * ns3Result;
      uint32_t lowerBound = ns3Result - 0.02 * ns3Result;
      uint32_t linuxResult = _cobalt_control_law (queue, dropNextTestVals[i]);
      NS_TEST_EXPECT_MSG_EQ ((lowerBound < linuxResult || linuxResult < upperBound), true,
                             "Linux result should stay within 2% of ns-3 result");
    }
}

/**
 * \ingroup traffic-control-test
 * \ingroup tests
 *
 * \brief Test 4,5,6: ECN tests
 */
class CobaltQueueDiscECNtest : public TestCase
{
public:
  CobaltQueueDiscECNtest ();
  virtual void DoRun (void);
  /**
   * Enqueue function
   * \param queue the queue disc
   * \param size the size
   * \param nPkt the number of packets
   * \param ecnCapable ECN capable flag
   */
  void Enqueue (Ptr<CobaltQueueDisc> queue, uint32_t size, uint32_t nPkt, bool ecnCapable);
  /**
   * Run Cobalt test function
   * \param mode the mode
   */

  void RunECNtest (QueueSizeUnit mode);
  void Dequeue (Ptr<CobaltQueueDisc> queue, uint32_t nPkt);
  void DequeueWithDelay (Ptr<CobaltQueueDisc> queue, double delay, uint32_t nPkt);
};

CobaltQueueDiscECNtest::CobaltQueueDiscECNtest ()
  : TestCase ("ECN verification tests, for both packets and bytes mode")
{
}

void
CobaltQueueDiscECNtest::RunECNtest (QueueSizeUnit mode)
{
  uint32_t pktSize = 1500;
  uint32_t modeSize = 0;
  Ptr<CobaltQueueDisc> queue = CreateObject<CobaltQueueDisc> ();
  CobaltQueueDisc::Stats st;

  // Test 4: Cobalt queue disc is ECN enabled, but packets are not ECN capable
  queue = CreateObject<CobaltQueueDisc> ();

  if (mode == QueueSizeUnit::BYTES)
    {
      modeSize = pktSize;
    }
  else if (mode == QueueSizeUnit::PACKETS)
    {
      modeSize = 1;
    }


  NS_TEST_EXPECT_MSG_EQ (queue->SetAttributeFailSafe ("MaxSize", QueueSizeValue (QueueSize (mode,modeSize*1500))),
                         true, "Verify that we can actually set the attribute MaxSize");
  NS_TEST_EXPECT_MSG_EQ (queue->SetAttributeFailSafe ("UseEcn", BooleanValue (true)), true,
                         "Verify that we can actually set the attribute UseECN");

  queue->Initialize ();
  if (mode == QueueSizeUnit::BYTES)
      Enqueue (queue, pktSize, 100, false);
  else
      Enqueue (queue, 1, 100, false);
  DequeueWithDelay (queue, 0.005, 100);
  Simulator::Stop (Seconds (8.0));
  Simulator::Run ();
  st = StaticCast<CobaltQueueDisc> (queue)->GetStats ();
  NS_TEST_EXPECT_MSG_NE (st.forcedDrop, 0, "There should be some forced drops");
  NS_TEST_EXPECT_MSG_EQ (st.forcedMark, 0, "There should no forced marks");


  // Test 5: Packets are ECN capable, but Cobalt queue disc is not ECN enabled
  queue = CreateObject<CobaltQueueDisc> ();
  NS_TEST_EXPECT_MSG_EQ (queue->SetAttributeFailSafe ("MaxSize", QueueSizeValue (QueueSize (mode,modeSize*1500))),
                         true, "Verify that we can actually set the attribute MaxSize");
  NS_TEST_EXPECT_MSG_EQ (queue->SetAttributeFailSafe ("UseEcn", BooleanValue (false)), true,
                         "Verify that we can actually set the attribute UseECN");

  queue->Initialize ();
  if (mode == QueueSizeUnit::BYTES)
      Enqueue (queue, pktSize, 100, true);
  else
      Enqueue (queue, 1, 100, true);
  DequeueWithDelay (queue, 0.005, 100);
  Simulator::Stop (Seconds (8.0));
  Simulator::Run ();
  st = StaticCast<CobaltQueueDisc> (queue)->GetStats ();
  NS_TEST_EXPECT_MSG_NE (st.forcedDrop, 0, "There should be some forced drops");
  NS_TEST_EXPECT_MSG_EQ (st.forcedMark, 0, "There should no forced marks");


  // Test 6: Packets are ECN capable and Cobalt queue disc is ECN enabled
  queue = CreateObject<CobaltQueueDisc> ();
   NS_TEST_EXPECT_MSG_EQ (queue->SetAttributeFailSafe ("MaxSize", QueueSizeValue (QueueSize (mode,modeSize*1500))),
                         true, "Verify that we can actually set the attribute MaxSize");
   NS_TEST_EXPECT_MSG_EQ (queue->SetAttributeFailSafe ("UseEcn", BooleanValue (true)), true,
                         "Verify that we can actually set the attribute UseECN");

  queue->Initialize ();
   if (mode == QueueSizeUnit::BYTES)
      Enqueue (queue, pktSize, 100, true);
  else
      Enqueue (queue, 1, 100, true);
  DequeueWithDelay (queue, 0.005, 100);
  Simulator::Stop (Seconds (8.0));
  Simulator::Run ();
  st = StaticCast<CobaltQueueDisc> (queue)->GetStats ();
  NS_TEST_EXPECT_MSG_NE (st.forcedMark, 0, "There should be some forced marks");
}


void
CobaltQueueDiscECNtest::Enqueue (Ptr<CobaltQueueDisc> queue, uint32_t size, uint32_t nPkt, bool ecnCapable)
{
  Address dest;
  for (uint32_t i = 0; i < nPkt; i++)
    {
      queue->Enqueue (Create<CobaltQueueDiscTestItem> (Create<Packet> (size), dest, 0, ecnCapable));
    }
}

void
CobaltQueueDiscECNtest::Dequeue (Ptr<CobaltQueueDisc> queue, uint32_t nPkt)
{
  for (uint32_t i = 0; i < nPkt; i++)
    {
      Ptr<QueueDiscItem> item = queue->Dequeue ();
    }
}

void
CobaltQueueDiscECNtest::DequeueWithDelay (Ptr<CobaltQueueDisc> queue, double delay, uint32_t nPkt)
{
  for (uint32_t i = 0; i < nPkt; i++)
    {
      Simulator::Schedule (Time (Seconds ((i + 1) * delay)), &CobaltQueueDiscECNtest::Dequeue, this, queue, 1);
    }
}

void
CobaltQueueDiscECNtest::DoRun (void)
{
  RunECNtest (QueueSizeUnit::PACKETS);
  RunECNtest (QueueSizeUnit::BYTES);
  Simulator::Destroy ();
}



/**
 * \ingroup traffic-control-test
 * \ingroup tests
 *
 * \brief Test 7: Cobalt Queue Disc Drop Test Item
 */
class CobaltQueueDiscDropTest : public TestCase
{
public:
  CobaltQueueDiscDropTest ();
  virtual void DoRun (void);
  /**
   * Enqueue function
   * \param queue the queue disc
   * \param size the size
   * \param nPkt the number of packets
   */
  void Enqueue (Ptr<CobaltQueueDisc> queue, uint32_t size, uint32_t nPkt);
  /**
   * Run Cobalt test function
   * \param mode the mode
   */
  void RunDropTest (QueueSizeUnit mode);
  
  void EnqueueWithDelay (Ptr<CobaltQueueDisc> queue, uint32_t size, uint32_t nPkt);


};

CobaltQueueDiscDropTest::CobaltQueueDiscDropTest ()
  : TestCase ("Drop tests verification for both packets and bytes mode")
{
}

void
CobaltQueueDiscDropTest::RunDropTest (QueueSizeUnit mode)

{
  uint32_t pktSize = 1500;
  uint32_t modeSize = 0;
  CobaltQueueDisc::Stats st;
  Ptr<CobaltQueueDisc> queue = CreateObject<CobaltQueueDisc> ();

  if (mode == QueueSizeUnit::BYTES)
    {
      modeSize = pktSize;
    }
  else if (mode == QueueSizeUnit::PACKETS)
    {
      modeSize = 1;
    }

queue = CreateObject<CobaltQueueDisc> ();
 NS_TEST_EXPECT_MSG_EQ (queue->SetAttributeFailSafe ("MaxSize", QueueSizeValue (QueueSize (mode, modeSize*100))),
                         true, "Verify that we can actually set the attribute MaxSize");
 NS_TEST_EXPECT_MSG_EQ (queue->SetAttributeFailSafe ("UseEcn", BooleanValue (true)), true,
                       "Verify that we can actually set the attribute UseECN");

queue->Initialize ();

 if (mode == QueueSizeUnit::BYTES)
    EnqueueWithDelay (queue, pktSize, 200);
else
    EnqueueWithDelay (queue, 1, 200);

Simulator::Stop (Seconds (8.0));
Simulator::Run ();

st = StaticCast<CobaltQueueDisc> (queue)->GetStats ();

// The Pdrop value should increase, from it's default value of zero
NS_TEST_EXPECT_MSG_NE (queue->GetPdrop(), 0, "Pdrop should be non-zero");
NS_TEST_EXPECT_MSG_NE (st.qLimDrop, 0, "Drops due to queue overflow should be non-zero");
}

void
CobaltQueueDiscDropTest::EnqueueWithDelay (Ptr<CobaltQueueDisc> queue, uint32_t size, uint32_t nPkt)
{
  Address dest;
  double delay = 0.01;  // enqueue packets with delay
  for (uint32_t i = 0; i < nPkt; i++)
    {
      Simulator::Schedule (Time (Seconds ((i + 1) * delay)), &CobaltQueueDiscDropTest::Enqueue, this, queue, size, 1);
    }
}


void
CobaltQueueDiscDropTest::Enqueue (Ptr<CobaltQueueDisc> queue, uint32_t size, uint32_t nPkt)
{
  Address dest;
  for (uint32_t i = 0; i < nPkt; i++)
    {
      queue->Enqueue (Create<CobaltQueueDiscTestItem> (Create<Packet> (size), dest, 0, true));
    }
}

void
CobaltQueueDiscDropTest::DoRun (void)
{
  RunDropTest (QueueSizeUnit::PACKETS);
  RunDropTest (QueueSizeUnit::BYTES);
  Simulator::Destroy ();
}

static class CobaltQueueDiscTestSuite : public TestSuite
{
public:
  CobaltQueueDiscTestSuite ()
    : TestSuite ("cobalt-queue-disc", UNIT)
  {
    // Test 1: simple enqueue/dequeue with no drops
    AddTestCase (new CobaltQueueDiscBasicEnqueueDequeue (PACKETS), TestCase::QUICK);
    AddTestCase (new CobaltQueueDiscBasicEnqueueDequeue (BYTES), TestCase::QUICK);
    // Test 2: test NewtonStep() against explicit port of Linux implementation
    AddTestCase (new CobaltQueueDiscNewtonStepTest (), TestCase::QUICK);
    // Test 3: test ControlLaw() against explicit port of Linux implementation
    AddTestCase (new CobaltQueueDiscControlLawTest (), TestCase::QUICK);
    // Tests 4,5,6: test ECN capability
    AddTestCase (new CobaltQueueDiscECNtest (), TestCase::QUICK);
    // Test 7: Drop test
    AddTestCase (new CobaltQueueDiscDropTest (), TestCase::QUICK);

  }
} g_cobaltQueueTestSuite; ///< the test suite
