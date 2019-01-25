/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2018 NITK Surathkal
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Cobalt, the CODEL - BLUE - Alternate Queueing discipline
 * Based on linux code.
 *
 * Ported to ns-3 by: Vignesh Kannan <vignesh2496@gmail.com>
 *                    Harsh Lara <harshapplefan@gmail.com>
 *                    Jendaipou Palmei <jendaipoupalmei@gmail.com>
 *                    Shefali Gupta <shefaligups11@gmail.com>
 *                    Mohit P.Tahiliani <tahiliani@nitk.edu.in>
 */

#include "ns3/log.h"
#include "ns3/enum.h"
#include "ns3/uinteger.h"
#include "ns3/abort.h"
#include "cobalt-queue-disc.h"
#include "ns3/object-factory.h"
#include "ns3/drop-tail-queue.h"
#include "ns3/net-device-queue-interface.h"
#include <climits>

/*
 * COBALT operates the Codel and BLUE algorithms in parallel, in order
 * to obtain the best features of each.  Codel is excellent on flows
 * which respond to congestion signals in a TCP-like way.  BLUE is far
 * more effective on unresponsive flows.
 */

namespace ns3 {


NS_LOG_COMPONENT_DEFINE ("CobaltQueueDisc");

NS_OBJECT_ENSURE_REGISTERED (CobaltQueueDisc);

/**
 * CoDel time stamp, used to carry CoDel time informations.
 */
class CobaltTimestampTag : public Tag
{
public:
  CobaltTimestampTag ();
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;

  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (TagBuffer i) const;
  virtual void Deserialize (TagBuffer i);
  virtual void Print (std::ostream &os) const;

  /**
   * Gets the Tag creation time
   * @return the time object stored in the tag
   */
  Time GetTxTime (void) const;
private:
  int64_t m_creationTime; //!< Tag creation time
};

CobaltTimestampTag::CobaltTimestampTag ()
  : m_creationTime (Simulator::Now ().GetTimeStep ())
{
}

TypeId
CobaltTimestampTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::CobaltTimestampTag")
    .SetParent<Tag> ()
    .AddConstructor<CobaltTimestampTag> ()
    .AddAttribute ("CreationTime",
                   "The time at which the timestamp was created",
                   StringValue ("0.0s"),
                   MakeTimeAccessor (&CobaltTimestampTag::GetTxTime),
                   MakeTimeChecker ())
  ;
  return tid;
}

TypeId
CobaltTimestampTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
CobaltTimestampTag::GetSerializedSize (void) const
{
  return 8;
}

void
CobaltTimestampTag::Serialize (TagBuffer i) const
{
  i.WriteU64 (m_creationTime);
}

void
CobaltTimestampTag::Deserialize (TagBuffer i)
{
  m_creationTime = i.ReadU64 ();
}

void
CobaltTimestampTag::Print (std::ostream &os) const
{
  os << "CreationTime=" << m_creationTime;
}

Time
CobaltTimestampTag::GetTxTime (void) const
{
  return TimeStep (m_creationTime);
}

TypeId CobaltQueueDisc::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::CobaltQueueDisc")
    .SetParent<QueueDisc> ()
    .SetGroupName ("TrafficControl")
    .AddConstructor<CobaltQueueDisc> ()
    .AddAttribute ("MaxSize",
                   "The maximum number of packets/bytes accepted by this queue disc.",
                   QueueSizeValue (QueueSize (QueueSizeUnit::BYTES, 1500 * DEFAULT_COBALT_LIMIT)),
                   MakeQueueSizeAccessor (&QueueDisc::SetMaxSize,
                                          &QueueDisc::GetMaxSize),
                   MakeQueueSizeChecker ())
    .AddAttribute ("MinBytes",
                   "The Cobalt algorithm minbytes parameter.",
                   UintegerValue (1500),
                   MakeUintegerAccessor (&CobaltQueueDisc::m_minBytes),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("Interval",
                   "The Cobalt algorithm interval",
                   StringValue ("100ms"),
                   MakeTimeAccessor (&CobaltQueueDisc::m_interval),
                   MakeTimeChecker ())
    .AddAttribute ("Target",
                   "The Cobalt algorithm target queue delay",
                   StringValue ("5ms"),
                   MakeTimeAccessor (&CobaltQueueDisc::m_target),
                   MakeTimeChecker ())
    .AddAttribute ("UseEcn",
                   "True to use ECN (packets are marked instead of being dropped)",
                   BooleanValue (false),
                   MakeBooleanAccessor (&CobaltQueueDisc::m_useEcn),
                   MakeBooleanChecker ())
    .AddAttribute ("Pdrop",
                   "Marking Probabilty",
                   DoubleValue (0),
                   MakeDoubleAccessor (&CobaltQueueDisc::m_Pdrop),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("Increment",
                   "Pdrop increment value",
                   DoubleValue (1. / 256),
                   MakeDoubleAccessor (&CobaltQueueDisc::m_increment),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("Decrement",
                   "Pdrop decrement Value",
                   DoubleValue (1. / 4096),
                   MakeDoubleAccessor (&CobaltQueueDisc::m_decrement),
                   MakeDoubleChecker<double> ())
    .AddTraceSource ("Count",
                     "Cobalt count",
                     MakeTraceSourceAccessor (&CobaltQueueDisc::m_count),
                     "ns3::TracedValueCallback::Uint32")
    .AddTraceSource ("DropState",
                     "Dropping state",
                     MakeTraceSourceAccessor (&CobaltQueueDisc::m_dropping),
                     "ns3::TracedValueCallback::Bool")
    .AddTraceSource ("Sojourn",
                     "Time in the queue",
                     MakeTraceSourceAccessor (&CobaltQueueDisc::m_sojourn),
                     "ns3::Time::TracedValueCallback")
    .AddTraceSource ("DropNext",
                     "Time until next packet drop",
                     MakeTraceSourceAccessor (&CobaltQueueDisc::m_dropNext),
                     "ns3::TracedValueCallback::Uint32")
  ;

  return tid;
}

/**
 * Performs a reciprocal divide, similar to the
 * Linux kernel reciprocal_divide function
 * \param A numerator
 * \param R reciprocal of the denominator B
 * \return the value of A/B
 */
/* borrowed from the linux kernel */
static inline uint32_t ReciprocalDivide (uint32_t A, uint32_t R)
{
  return (uint32_t)(((uint64_t)A * R) >> 32);
}

double min (double x, double y)
{
  return (x < y) ? x : y;
}

double max (double x, double y)
{
  return (x > y) ? x : y;
}

/**
 * Returns the current time translated in CoDel time representation
 * \return the current time
 */
static int64_t CoDelGetTime (void)
{
  Time time = Simulator::Now ();
  int64_t ns = time.GetNanoSeconds ();

  return ns;
}

CobaltQueueDisc::CobaltQueueDisc ()
  : QueueDisc ()
{
  NS_LOG_FUNCTION (this);
  InitializeParams ();
  m_uv = CreateObject<UniformRandomVariable> ();
}

double CobaltQueueDisc::GetPdrop ()
{
  return m_Pdrop;
}

CobaltQueueDisc::~CobaltQueueDisc ()
{
  NS_LOG_FUNCTION (this);
}

int64_t
CobaltQueueDisc::AssignStreams (int64_t stream)
{
  NS_LOG_FUNCTION (this << stream);
  m_uv->SetStream (stream);
  return 1;
}

void
CobaltQueueDisc::InitializeParams (void)
{
  // Cobalt parameters
  NS_LOG_FUNCTION (this);
  m_recInvSqrtCache[0] = ~0;
  CacheInit ();
  m_count = 0;
  m_dropping = false;
  m_recInvSqrt = ~0U;
  m_lastUpdateTimeBlue = 0;
  m_dropNext = 0;
  m_sojourn = 0;

  // Stats
  m_stats.forcedDrop = 0;
  m_stats.unforcedDrop = 0;
  m_stats.qLimDrop = 0;
  m_stats.forcedMark = 0;
}

CobaltQueueDisc::Stats
CobaltQueueDisc::GetStats ()
{
  NS_LOG_FUNCTION (this);
  return m_stats;
}

bool
CobaltQueueDisc::CoDelTimeAfter (int64_t a, int64_t b)
{
  return  ((int)(a) - (int)(b) > 0);
}

bool
CobaltQueueDisc::CoDelTimeAfterEq (int64_t a, int64_t b)
{
  return ((int)(a) - (int)(b) >= 0);
}

bool
CobaltQueueDisc::CoDelTimeBefore (int64_t a, int64_t b)
{
  return  ((int)(a) - (int)(b) < 0);
}

bool
CobaltQueueDisc::CoDelTimeBeforeEq (int64_t a, int64_t b)
{
  return ((int)(a) - (int)(b) <= 0);
}

uint32_t
CobaltQueueDisc::Time2CoDel (Time t)
{
  return (t.GetNanoSeconds ());
}

Time
CobaltQueueDisc::GetTarget (void)
{
  return m_target;
}

Time
CobaltQueueDisc::GetInterval (void)
{
  return m_interval;
}

int64_t
CobaltQueueDisc::GetDropNext (void)
{
  return m_dropNext;
}

uint32_t
CobaltQueueDisc::GetDropOverLimit (void)
{
  return m_stats.qLimDrop;
}

uint32_t
CobaltQueueDisc::GetDropCount (void)
{
  return m_stats.forcedDrop;
}

void
CobaltQueueDisc::NewtonStep (void)
{
  NS_LOG_FUNCTION (this);
  uint32_t invsqrt = ((uint32_t) m_recInvSqrt);
  uint32_t invsqrt2 = ((uint64_t) invsqrt * invsqrt) >> 32;
  uint64_t val = (3ll << 32) - ((uint64_t) m_count * invsqrt2);

  val >>= 2; /* avoid overflow */
  val = (val * invsqrt) >> (32 - 2 + 1);
  m_recInvSqrt = val;
}

/* There is a big difference in timing between the accurate values placed in
 * the cache and the approximations given by a single Newton step for small
 * count values, particularly when stepping from count 1 to 2 or vice versa.
 * Above 16, a single Newton step gives sufficient accuracy in either
 * direction, given the precision stored.
 *
 * The magnitude of the error when stepping up to count 2 is such as to give
 * the value that *should* have been produced at count 4.
 */

void
CobaltQueueDisc::CacheInit (void)
{
  m_recInvSqrt = ~0U;
  m_recInvSqrtCache[0] = m_recInvSqrt;

  for (m_count = 1; m_count < (uint32_t)(REC_INV_SQRT_CACHE); m_count++)
    {
      NewtonStep ();
      NewtonStep ();
      NewtonStep ();
      NewtonStep ();
      m_recInvSqrtCache[m_count] = m_recInvSqrt;
    }
}

void
CobaltQueueDisc::InvSqrt (void)
{
  if (m_count < (uint32_t)REC_INV_SQRT_CACHE)
    {
      m_recInvSqrt = m_recInvSqrtCache[m_count];
    }
  else
    {
      NewtonStep ();
    }
}

int64_t
CobaltQueueDisc::ControlLaw (int64_t t)
{
  NS_LOG_FUNCTION (this);
  return t + ReciprocalDivide (Time2CoDel (m_interval), m_recInvSqrt);
}

Time
CobaltQueueDisc::GetQueueDelay (void)
{
  NS_LOG_FUNCTION (this);
  return m_sojourn;
}

void
CobaltQueueDisc::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  m_uv = 0;
  QueueDisc::DoDispose ();
}

Ptr<const QueueDiscItem>
CobaltQueueDisc::DoPeek (void) const
{
  NS_LOG_FUNCTION (this);
  if (GetInternalQueue (0)->IsEmpty ())
    {
      NS_LOG_LOGIC ("Queue empty");
      return 0;
    }

  Ptr<const QueueDiscItem> item = GetInternalQueue (0)->Peek ();

  NS_LOG_LOGIC ("Number packets " << GetInternalQueue (0)->GetNPackets ());
  NS_LOG_LOGIC ("Number bytes " << GetInternalQueue (0)->GetNBytes ());

  return item;
}

bool
CobaltQueueDisc::CheckConfig (void)
{
  NS_LOG_FUNCTION (this);
  if (GetNQueueDiscClasses () > 0)
    {
      NS_LOG_ERROR ("CobaltQueueDisc cannot have classes");
      return false;
    }

  if (GetNPacketFilters () > 0)
    {
      NS_LOG_ERROR ("CobaltQueueDisc cannot have packet filters");
      return false;
    }

  if (GetNInternalQueues () == 0)
    {

      AddInternalQueue (CreateObjectWithAttributes<DropTailQueue<QueueDiscItem> >
                          ("MaxSize", QueueSizeValue (GetMaxSize ())));
    }


  if (GetNInternalQueues () != 1)
    {
      NS_LOG_ERROR ("CobaltQueueDisc needs 1 internal queue");
      return false;
    }
  return true;
}

bool
CobaltQueueDisc::DoEnqueue (Ptr<QueueDiscItem> item)
{
  NS_LOG_FUNCTION (this << item);
  Ptr<Packet> p = item->GetPacket ();
  if (GetCurrentSize () + item > GetMaxSize ())
    {
      NS_LOG_LOGIC ("Queue full -- dropping pkt");
      int64_t now = CoDelGetTime ();
      // Call this to update Blue's drop probability
      CobaltQueueFull (now);
      DropBeforeEnqueue (item, OVERLIMIT_DROP);
      m_stats.qLimDrop++;
      return false;
    }

  // Tag packet with current time for DoDequeue() to compute sojourn time
  CobaltTimestampTag tag;
  p->AddPacketTag (tag);

  bool retval = GetInternalQueue (0)->Enqueue (item);

  // If Queue::Enqueue fails, QueueDisc::Drop is called by the internal queue
  // because QueueDisc::AddInternalQueue sets the drop callback

  NS_LOG_LOGIC ("Number packets " << GetInternalQueue (0)->GetNPackets ());
  NS_LOG_LOGIC ("Number bytes " << GetInternalQueue (0)->GetNBytes ());

  return retval;
}

Ptr<QueueDiscItem>
CobaltQueueDisc::DoDequeue (void)
{
  NS_LOG_FUNCTION (this);

  while (1)
    {
      Ptr<QueueDiscItem> item = GetInternalQueue (0)->Dequeue ();
      if (!item)
        {
          // Leave dropping state when queue is empty (derived from Codel)
          m_dropping = false;
          NS_LOG_LOGIC ("Queue empty");
          int64_t now = CoDelGetTime ();
          // Call this to update Blue's drop probability
          CobaltQueueEmpty (now);
          return 0;
        }

      int64_t now = CoDelGetTime ();

      NS_LOG_LOGIC ("Popped " << item);
      NS_LOG_LOGIC ("Number packets remaining " << GetInternalQueue (0)->GetNPackets ());
      NS_LOG_LOGIC ("Number bytes remaining " << GetInternalQueue (0)->GetNBytes ());

      // Determine if item should be dropped
      // ECN marking happens inside this function, so it need not be done here
      bool drop = CobaltShouldDrop (item, now);

      if (drop)
        {
          DropAfterDequeue (item, TARGET_EXCEEDED_DROP);
        }
      else
        {
          return item;
        }
    }
}

// Call this when a packet had to be dropped due to queue overflow.
void CobaltQueueDisc::CobaltQueueFull (int64_t now)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_LOGIC ("Outside IF block");
  if (CoDelTimeAfter ((now - m_lastUpdateTimeBlue), Time2CoDel (m_target)))
    {
      NS_LOG_LOGIC ("inside IF block");
      m_Pdrop = min (m_Pdrop + m_increment, (double)1.0);
      m_lastUpdateTimeBlue = now;
    }
  m_dropping = true;
  m_dropNext = now;
  if (!m_count)
    {
      m_count = 1;
    }
}

// Call this when the queue was serviced but turned out to be empty.
void CobaltQueueDisc::CobaltQueueEmpty (int64_t now)
{
  NS_LOG_FUNCTION (this);
  if (m_Pdrop && CoDelTimeAfter ((now - m_lastUpdateTimeBlue), Time2CoDel (m_target)))
    {
      m_Pdrop = max (m_Pdrop - m_decrement, (double)0.0);
      m_lastUpdateTimeBlue = now;
    }
  m_dropping = false;

  if (m_count && CoDelTimeAfterEq ((now - m_dropNext), 0))
    {
      m_count--;
      InvSqrt ();
      m_dropNext = ControlLaw (m_dropNext);
    }
}

// Determines if Cobalt should drop the packet
bool CobaltQueueDisc::CobaltShouldDrop (Ptr<QueueDiscItem> item, int64_t now)
{
  NS_LOG_FUNCTION (this);
  bool drop = false, codelForcedDrop = false;


  /* Simplified Codel implementation */
  CobaltTimestampTag tag;
  bool found = item->GetPacket ()->RemovePacketTag (tag);
  NS_ASSERT_MSG (found, "found a packet without an input timestamp tag");
  NS_UNUSED (found);          //silence compiler warning
  Time delta = Simulator::Now () - tag.GetTxTime ();
  NS_LOG_INFO ("Sojourn time " << delta.GetSeconds ());
  m_sojourn = delta;
  int64_t sojournTime = Time2CoDel (delta);
  int64_t schedule = now - m_dropNext;
  bool over_target = CoDelTimeAfter (sojournTime, Time2CoDel (m_target));
  bool next_due = m_count && schedule >= 0;

  if (over_target)
    {
      if (!m_dropping)
        {
          m_dropping = true;
          m_dropNext = ControlLaw (now);
        }
      if (!m_count)
        {
          m_count = 1;
        }
    }
  else if (m_dropping)
    {
      m_dropping = false;
    }

  if (next_due && m_dropping)
    {
      /* Check for marking possibility only if BLUE decides NOT to drop. */
      /* Check if router and packet, both have ECN enabled. Only if this is true, mark the packet. */
      if (!drop)
        {
          drop = !(m_useEcn && item->Mark ());
          if (!drop)
            {
              m_stats.forcedMark++;
            }
          else
            {
              codelForcedDrop = true;
            }
        }

      m_count = max (m_count, m_count + 1);

      InvSqrt ();
      m_dropNext = ControlLaw (m_dropNext);
      schedule = now - m_dropNext;
    }
  else
    {
      while (next_due)
        {
          m_count--;
          InvSqrt ();
          m_dropNext = ControlLaw (m_dropNext);
          schedule = now - m_dropNext;
          next_due = m_count && schedule >= 0;
        }
    }
  /* Simple BLUE implementation. Lack of ECN is deliberate. */
  if (m_Pdrop)
    {
      double u = m_uv->GetValue ();
      drop = drop | (u < m_Pdrop);
    }

  /* Overload the drop_next field as an activity timeout */
  if (!m_count)
    {
      m_dropNext = now + Time2CoDel (m_interval);
    }
  else if (schedule > 0 && !drop)
    {
      m_dropNext = now;
    }

  // Updating stats
  if (drop)
    {
      if (codelForcedDrop || m_Pdrop == 1.0)
        {
          m_stats.forcedDrop++;
        }
      else
        {
          m_stats.unforcedDrop++;
        }
    }

  return drop;
}

} // namespace ns3
