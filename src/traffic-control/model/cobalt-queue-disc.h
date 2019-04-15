/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2017 NITK Surathkal
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
 *
 */

#ifndef COBALT_H
#define COBALT_H

#include "ns3/queue-disc.h"
#include "ns3/nstime.h"
#include "ns3/boolean.h"
#include "ns3/data-rate.h"
#include "ns3/simulator.h"
#include "ns3/string.h"
#include "ns3/traced-value.h"
#include "ns3/random-variable-stream.h"
#include "ns3/trace-source-accessor.h"

class CobaltQueueDiscNewtonStepTest;  // Forward declaration for unit test
class CobaltQueueDiscControlLawTest;  // Forward declaration for unit test

namespace ns3 {


#define REC_INV_SQRT_CACHE (16)
#define DEFAULT_COBALT_LIMIT 1000

class TraceContainer;

/**
 * \ingroup traffic-control
 *
 * \brief COBALT packet queue disc
 */
class CobaltQueueDisc : public QueueDisc
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  /**
   * \brief CobaltQueueDisc Constructor
   *
   * Create a Cobalt queue disc
   */
  CobaltQueueDisc ();

  /**
   * \brief Destructor
   *
   * Destructor
   */
  virtual ~CobaltQueueDisc ();

  /**
   * \brief Stats
   */
  typedef struct
  {
    uint32_t unforcedDrop;  //!< Probability drops by Blue
    uint32_t forcedDrop;    //!< Forced drops by Codel or Blue on Non-ECN packets in the dropping state
    uint32_t qLimDrop;      //!< Queue overflow
    uint32_t forcedMark;    //!< Forced marks by Codel on ECN-enabled packets in the dropping state
  } Stats;

  uint32_t GetDropCount (void);
  uint32_t GetDropOverLimit (void);

  /**
   * \brief Drop types
   */
  enum
  {
    DTYPE_NONE,        //!< (0) Ok, no drop
    DTYPE_FORCED,      //!< (1) A "forced" drop
    DTYPE_UNFORCED,    //!< (2) An "unforced" (random) drop
  };

  /**
   * \brief Enumeration of the modes supported in the class.
   *
   */
  enum QueueDiscMode
  {
    PACKETS,     /**< (0) Use number of packets for maximum queue disc size*/
    BYTES,       /**< (1) Use number of bytes for maximum queue disc size*/
  };

  /**
   * \brief Set the operating mode of this queue disc.
   *
   * \param mode The operating mode of this queue disc.
   */
  void SetMode (QueueDiscMode mode);

  /**
   * \brief Get the operating mode of this queue disc.
   *
   * \returns The operating mode of this queue disc.
   */
  QueueDiscMode GetMode (void);

  /**
   * \brief Get the current value of the queue in bytes or packets.
   *
   * \returns The queue size in bytes or packets.
   */
  uint32_t GetQueueSize (void);

  Time GetQueueDelay (void);

  /**
   * \brief Get the Cobalt statistics after running.
   *
   * \returns The drop statistics.
   */
  Stats GetStats ();


  /**
   * \brief Get the target queue delay
   *
   * \returns The target queue delay
   */
  Time GetTarget (void);

  /**
   * \brief Get the interval
   *
   * \returns The interval
   */
  Time GetInterval (void);

  /**
   * \brief Get the time for next packet drop while in the dropping state
   *
   * \returns The time for next packet drop
   */
  int64_t GetDropNext (void);

  static constexpr const char* TARGET_EXCEEDED_DROP = "Target exceeded drop";  //!< Sojourn time above target
  static constexpr const char* OVERLIMIT_DROP = "Overlimit drop";  //!< Overlimit dropped packet
  static constexpr const char* unforcedDrop = "unforcedDrop";  //!< probability drops by blue
  static constexpr const char* forcedDrop = "forcedDrop";  //!< Forced drops by Codel or blue on Non-ECN packets in the dropping state
  static constexpr const char* qLimDrop = "qlimit drop";  //!< Queue overflow
  static constexpr const char* forcedMark = "forcedMark";  //!< forced maeks by Codel on ECN-enabled

  /**
   * \brief Get the drop probability of Blue
   *
   * \returns The current value of Blue's drop probability
   */
  double GetPdrop ();

  /**
   * Assign a fixed random variable stream number to the random variables
   * used by this model.  Return the number of streams (possibly zero) that
   * have been assigned.
   *
   * \param stream first stream index to use
   * \return the number of stream indices assigned by this model
   */
  int64_t AssignStreams (int64_t stream);

  /**
   * Return the unsigned 32-bit integer representation of the input Time
   * object. Units are microseconds
   * @param t the input Time Object
   * @return the unsigned 32-bit integer representation
   */
  int64_t Time2CoDel (Time t);

protected:
  /**
   * \brief Dispose of the object
   */
  virtual void DoDispose (void);

private:
  friend class ::CobaltQueueDiscNewtonStepTest;  // Test code
  friend class ::CobaltQueueDiscControlLawTest;  // Test code

  virtual bool DoEnqueue (Ptr<QueueDiscItem> item);
  virtual Ptr<QueueDiscItem> DoDequeue (void);
  virtual Ptr<const QueueDiscItem> DoPeek (void) const;
  virtual bool CheckConfig (void);

  /**
    * \brief Initialize the queue parameters.
    */
  virtual void InitializeParams (void);

  /**
   * \brief Calculate the reciprocal square root of m_count by using Newton's method
   *  http://en.wikipedia.org/wiki/Methods_of_computing_square_roots#Iterative_methods_for_reciprocal_square_roots
   * m_recInvSqrt (new) = (m_recInvSqrt (old) / 2) * (3 - m_count * m_recInvSqrt^2)
   */
  void NewtonStep (void);

  /**
   * \brief Determine the time for next drop
   * CoDel control law is t + m_interval/sqrt(m_count).
   * Here, we use m_recInvSqrt calculated by Newton's method in NewtonStep() to avoid
   * both sqrt() and divide operations
   *
   * \param t Current next drop time
   * \returns The new next drop time:
   */
  int64_t ControlLaw (int64_t t);

  void InvSqrt (void);

  void CacheInit (void);
  /**
   * Check if CoDel time a is successive to b
   * @param a left operand
   * @param b right operand
   * @return true if a is greater than b
   */

  bool CoDelTimeAfter (int64_t a, int64_t b);

  /**
   * Check if CoDel time a is successive or equal to b
   * @param a left operand
   * @param b right operand
   * @return true if a is greater than or equal to b
   */
  bool CoDelTimeAfterEq (int64_t a, int64_t b);

  /**
   * Check if CoDel time a is preceding b
   * @param a left operand
   * @param b right operand
   * @return true if a is less than to b
   */
  bool CoDelTimeBefore (int64_t a, int64_t b);

  /**
   * Check if CoDel time a is preceding or equal to b
   * @param a left operand
   * @param b right operand
   * @return true if a is less than or equal to b
   */
  bool CoDelTimeBeforeEq (int64_t a, int64_t b);

  /**
   * Called when the queue becomes full to alter the drop probabilities of Blue
   */
  void CobaltQueueFull (int64_t now);

  /**
   * Called when the queue becomes empty to alter the drop probabilities of Blue
   */
  void CobaltQueueEmpty (int64_t now);

  /**
   * Called to decide whether the current packet should be dropped based on decisions taken by Blue and Codel working parallely
   * Returns true if the packet should be dropped, false otherwise
   */
  bool CobaltShouldDrop (Ptr<QueueDiscItem> item, int64_t now);

  // Common to Codel and Blue
  // Maintained by Cobalt
  Stats m_stats;                          //!< Cobalt statistics
  // Supplied by user
  uint32_t m_minBytes;                    //!< Minimum bytes in queue to allow a packet drop

  // Codel parameters
  // Maintained by Cobalt
  TracedValue<uint32_t> m_count;          //!< Number of packets dropped since entering drop state
  TracedValue<int64_t> m_dropNext;       //!< Time to drop next packet
  TracedValue<Time> m_sojourn;            //!< Time in queue
  TracedValue<bool> m_dropping;           //!< True if in dropping state
  uint32_t m_recInvSqrt;                  //!< Reciprocal inverse square root

  // Supplied by user
  Time m_interval;                        //!< 100 ms sliding minimum time window width
  Time m_target;                          //!< 5 ms target queue delay
  bool m_useEcn;                          //!< True if ECN is used (packets are marked instead of being dropped)

  // Blue parameters
  // Maintained by Cobalt
  Ptr<UniformRandomVariable> m_uv;        //!< Rng stream
  uint32_t m_lastUpdateTimeBlue;          //!< Blue's last update time for drop probability

  // Supplied by user
  double m_increment;                     //!< increment value for marking probability
  double m_decrement;                     //!< decrement value for marking probability
  double m_Pdrop;                         //!< Drop Probability
  uint32_t m_recInvSqrtCache[REC_INV_SQRT_CACHE] = {0};

};

} // namespace ns3

#endif /* COBALT_H */
