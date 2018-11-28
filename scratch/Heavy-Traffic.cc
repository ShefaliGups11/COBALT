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
 * Authors: Shefali Gupta <shefaligups11@gmail.com>
 *          Jendaipou Palmei <jendaipoupalmei@gmail.com>
 *          Mohit P. Tahiliani <tahiliani@nitk.edu.in>
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include <fstream>
#include "ns3/ipv6-static-routing-helper.h"
#include "ns3/ipv6-routing-table-entry.h"
#include "ns3/internet-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/tcp-header.h"
#include "ns3/traffic-control-module.h"
#include  <string>

using namespace ns3;

std::string dir = "PlotsHeavy/";

void
CheckQueueSize (Ptr<QueueDisc> queue)
{
  double qSize = queue->GetCurrentSize ().GetValue ();
  // check queue size every 1/100 of a second
  Simulator::Schedule (Seconds (0.1), &CheckQueueSize, queue);

  std::ofstream fPlotQueue (dir + "queueTraces/queue0.plotme", std::ios::out | std::ios::app);
  fPlotQueue << Simulator::Now ().GetSeconds () << " " << qSize << std::endl;
  fPlotQueue.close ();
}

static void
tracer (uint32_t oldval, uint32_t newval)
{
  std::ofstream fPlotQueue (dir + "cwndTraces/S_1.plotme", std::ios::out | std::ios::app);
  fPlotQueue << Simulator::Now ().GetSeconds () << " " << newval / 1446.0 << std::endl;
  fPlotQueue.close ();
}

static void
tracer1 (uint32_t oldval, uint32_t newval)
{
  std::ofstream fPlotQueue (dir + "cwndTraces/S_2.plotme", std::ios::out | std::ios::app);
  fPlotQueue << Simulator::Now ().GetSeconds () << " " << newval / 1446.0 << std::endl;
  fPlotQueue.close ();
}

static void
tracer2 (uint32_t oldval, uint32_t newval)
{
  std::ofstream fPlotQueue (dir + "cwndTraces/S_3.plotme", std::ios::out | std::ios::app);
  fPlotQueue << Simulator::Now ().GetSeconds () << " " << newval / 1446.0 << std::endl;
  fPlotQueue.close ();

}

static void
tracer3 (uint32_t oldval, uint32_t newval)
{
  std::ofstream fPlotQueue (dir + "cwndTraces/S_4.plotme", std::ios::out | std::ios::app);
  fPlotQueue << Simulator::Now ().GetSeconds () << " " << newval / 1446.0 << std::endl;
  fPlotQueue.close ();
}

static void
tracer4 (uint32_t oldval, uint32_t newval)
{
  std::ofstream fPlotQueue (dir + "cwndTraces/S_5.plotme", std::ios::out | std::ios::app);
  fPlotQueue << Simulator::Now ().GetSeconds () << " " << newval / 1446.0 << std::endl;
  fPlotQueue.close ();
}

static void
tracer5 (uint32_t oldval, uint32_t newval)
{
  std::ofstream fPlotQueue (dir + "cwndTraces/S_6.plotme", std::ios::out | std::ios::app);
  fPlotQueue << Simulator::Now ().GetSeconds () << " " << newval / 1446.0 << std::endl;
  fPlotQueue.close ();
}

static void
tracer6 (uint32_t oldval, uint32_t newval)
{
  std::ofstream fPlotQueue (dir + "cwndTraces/S_7.plotme", std::ios::out | std::ios::app);
  fPlotQueue << Simulator::Now ().GetSeconds () << " " << newval / 1446.0 << std::endl;
  fPlotQueue.close ();
}

static void
tracer7 (uint32_t oldval, uint32_t newval)
{
  std::ofstream fPlotQueue (dir + "cwndTraces/S_8.plotme", std::ios::out | std::ios::app);
  fPlotQueue << Simulator::Now ().GetSeconds () << " " << newval / 1446.0 << std::endl;
  fPlotQueue.close ();
}

static void
tracer8 (uint32_t oldval, uint32_t newval)
{
  std::ofstream fPlotQueue (dir + "cwndTraces/S_9.plotme", std::ios::out | std::ios::app);
  fPlotQueue << Simulator::Now ().GetSeconds () << " " << newval / 1446.0 << std::endl;
  fPlotQueue.close ();
}

static void
tracer9 (uint32_t oldval, uint32_t newval)
{
  std::ofstream fPlotQueue (dir + "cwndTraces/S_10.plotme", std::ios::out | std::ios::app);
  fPlotQueue << Simulator::Now ().GetSeconds () << " " << newval / 1446.0 << std::endl;
  fPlotQueue.close ();
}

static void
tracer10 (uint32_t oldval, uint32_t newval)
{
  std::ofstream fPlotQueue (dir + "cwndTraces/S_11.plotme", std::ios::out | std::ios::app);
  fPlotQueue << Simulator::Now ().GetSeconds () << " " << newval / 1446.0 << std::endl;
  fPlotQueue.close ();
}

static void
tracer11 (uint32_t oldval, uint32_t newval)
{
  std::ofstream fPlotQueue (dir + "cwndTraces/S_12.plotme", std::ios::out | std::ios::app);
  fPlotQueue << Simulator::Now ().GetSeconds () << " " << newval / 1446.0 << std::endl;
  fPlotQueue.close ();
}

static void
tracer12 (uint32_t oldval, uint32_t newval)
{
  std::ofstream fPlotQueue (dir + "cwndTraces/S_13.plotme", std::ios::out | std::ios::app);
  fPlotQueue << Simulator::Now ().GetSeconds () << " " << newval / 1446.0 << std::endl;
  fPlotQueue.close ();
}

static void
tracer13 (uint32_t oldval, uint32_t newval)
{
  std::ofstream fPlotQueue (dir + "cwndTraces/S_14.plotme", std::ios::out | std::ios::app);
  fPlotQueue << Simulator::Now ().GetSeconds () << " " << newval / 1446.0 << std::endl;
  fPlotQueue.close ();
}

static void
tracer14 (uint32_t oldval, uint32_t newval)
{
  std::ofstream fPlotQueue (dir + "cwndTraces/S_15.plotme", std::ios::out | std::ios::app);
  fPlotQueue << Simulator::Now ().GetSeconds () << " " << newval / 1446.0 << std::endl;
  fPlotQueue.close ();
}

static void
tracer15 (uint32_t oldval, uint32_t newval)
{
  std::ofstream fPlotQueue (dir + "cwndTraces/S_16.plotme", std::ios::out | std::ios::app);
  fPlotQueue << Simulator::Now ().GetSeconds () << " " << newval / 1446.0 << std::endl;
  fPlotQueue.close ();
}

static void
tracer16 (uint32_t oldval, uint32_t newval)
{
  std::ofstream fPlotQueue (dir + "cwndTraces/S_18.plotme", std::ios::out | std::ios::app);
  fPlotQueue << Simulator::Now ().GetSeconds () << " " << newval / 1446.0 << std::endl;
  fPlotQueue.close ();
}

static void
tracer17 (uint32_t oldval, uint32_t newval)
{
  std::ofstream fPlotQueue (dir + "cwndTraces/S_19.plotme", std::ios::out | std::ios::app);
  fPlotQueue << Simulator::Now ().GetSeconds () << " " << newval / 1446.0 << std::endl;
  fPlotQueue.close ();
}

static void
tracer18 (uint32_t oldval, uint32_t newval)
{
  std::ofstream fPlotQueue (dir + "cwndTraces/S_20.plotme", std::ios::out | std::ios::app);
  fPlotQueue << Simulator::Now ().GetSeconds () << " " << newval / 1446.0 << std::endl;
  fPlotQueue.close ();
}

static void
tracer19 (uint32_t oldval, uint32_t newval)
{
  std::ofstream fPlotQueue (dir + "cwndTraces/S_21.plotme", std::ios::out | std::ios::app);
  fPlotQueue << Simulator::Now ().GetSeconds () << " " << newval / 1446.0 << std::endl;
  fPlotQueue.close ();
}

static void
tracer20 (uint32_t oldval, uint32_t newval)
{
  std::ofstream fPlotQueue (dir + "cwndTraces/S_22.plotme", std::ios::out | std::ios::app);
  fPlotQueue << Simulator::Now ().GetSeconds () << " " << newval / 1446.0 << std::endl;
  fPlotQueue.close ();
}

static void
tracer21 (uint32_t oldval, uint32_t newval)
{
  std::ofstream fPlotQueue (dir + "cwndTraces/S_23.plotme", std::ios::out | std::ios::app);
  fPlotQueue << Simulator::Now ().GetSeconds () << " " << newval / 1446.0 << std::endl;
  fPlotQueue.close ();
}

static void
tracer22 (uint32_t oldval, uint32_t newval)
{
  std::ofstream fPlotQueue (dir + "cwndTraces/S_24.plotme", std::ios::out | std::ios::app);
  fPlotQueue << Simulator::Now ().GetSeconds () << " " << newval / 1446.0 << std::endl;
  fPlotQueue.close ();
}

static void
tracer23 (uint32_t oldval, uint32_t newval)
{
  std::ofstream fPlotQueue (dir + "cwndTraces/S_25.plotme", std::ios::out | std::ios::app);
  fPlotQueue << Simulator::Now ().GetSeconds () << " " << newval / 1446.0 << std::endl;
  fPlotQueue.close ();
}

static void
tracer24 (uint32_t oldval, uint32_t newval)
{
  std::ofstream fPlotQueue (dir + "cwndTraces/S_26.plotme", std::ios::out | std::ios::app);
  fPlotQueue << Simulator::Now ().GetSeconds () << " " << newval / 1446.0 << std::endl;
  fPlotQueue.close ();
}

static void
tracer25 (uint32_t oldval, uint32_t newval)
{
  std::ofstream fPlotQueue (dir + "cwndTraces/S_27.plotme", std::ios::out | std::ios::app);
  fPlotQueue << Simulator::Now ().GetSeconds () << " " << newval / 1446.0 << std::endl;
  fPlotQueue.close ();
}

static void
tracer26 (uint32_t oldval, uint32_t newval)
{
  std::ofstream fPlotQueue (dir + "cwndTraces/S_28.plotme", std::ios::out | std::ios::app);
  fPlotQueue << Simulator::Now ().GetSeconds () << " " << newval / 1446.0 << std::endl;
  fPlotQueue.close ();
}

static void
tracer27 (uint32_t oldval, uint32_t newval)
{
  std::ofstream fPlotQueue (dir + "cwndTraces/S_29.plotme", std::ios::out | std::ios::app);
  fPlotQueue << Simulator::Now ().GetSeconds () << " " << newval / 1446.0 << std::endl;
  fPlotQueue.close ();
}

static void
tracer28 (uint32_t oldval, uint32_t newval)
{
  std::ofstream fPlotQueue (dir + "cwndTraces/S_30.plotme", std::ios::out | std::ios::app);
  fPlotQueue << Simulator::Now ().GetSeconds () << " " << newval / 1446.0 << std::endl;
  fPlotQueue.close ();
}

static void
tracer29 (uint32_t oldval, uint32_t newval)
{
  std::ofstream fPlotQueue (dir + "cwndTraces/S_31.plotme", std::ios::out | std::ios::app);
  fPlotQueue << Simulator::Now ().GetSeconds () << " " << newval / 1446.0 << std::endl;
  fPlotQueue.close ();
}

static void
tracer30 (uint32_t oldval, uint32_t newval)
{
  std::ofstream fPlotQueue (dir + "cwndTraces/S_32.plotme", std::ios::out | std::ios::app);
  fPlotQueue << Simulator::Now ().GetSeconds () << " " << newval / 1446.0 << std::endl;
  fPlotQueue.close ();
}

static void
tracer31 (uint32_t oldval, uint32_t newval)
{
  std::ofstream fPlotQueue (dir + "cwndTraces/S_33.plotme", std::ios::out | std::ios::app);
  fPlotQueue << Simulator::Now ().GetSeconds () << " " << newval / 1446.0 << std::endl;
  fPlotQueue.close ();
}

static void
tracer32 (uint32_t oldval, uint32_t newval)
{
  std::ofstream fPlotQueue (dir + "cwndTraces/S_34.plotme", std::ios::out | std::ios::app);
  fPlotQueue << Simulator::Now ().GetSeconds () << " " << newval / 1446.0 << std::endl;
  fPlotQueue.close ();
}

static void
tracer33 (uint32_t oldval, uint32_t newval)
{
  std::ofstream fPlotQueue (dir + "cwndTraces/S_35.plotme", std::ios::out | std::ios::app);
  fPlotQueue << Simulator::Now ().GetSeconds () << " " << newval / 1446.0 << std::endl;
  fPlotQueue.close ();
}

static void
tracer34 (uint32_t oldval, uint32_t newval)
{
  std::ofstream fPlotQueue (dir + "cwndTraces/S_36.plotme", std::ios::out | std::ios::app);
  fPlotQueue << Simulator::Now ().GetSeconds () << " " << newval / 1446.0 << std::endl;
  fPlotQueue.close ();
}

static void
tracer35 (uint32_t oldval, uint32_t newval)
{
  std::ofstream fPlotQueue (dir + "cwndTraces/S_37.plotme", std::ios::out | std::ios::app);
  fPlotQueue << Simulator::Now ().GetSeconds () << " " << newval / 1446.0 << std::endl;
  fPlotQueue.close ();
}

static void
tracer36 (uint32_t oldval, uint32_t newval)
{
  std::ofstream fPlotQueue (dir + "cwndTraces/S_38.plotme", std::ios::out | std::ios::app);
  fPlotQueue << Simulator::Now ().GetSeconds () << " " << newval / 1446.0 << std::endl;
  fPlotQueue.close ();
}

static void
tracer37 (uint32_t oldval, uint32_t newval)
{
  std::ofstream fPlotQueue (dir + "cwndTraces/S_39.plotme", std::ios::out | std::ios::app);
  fPlotQueue << Simulator::Now ().GetSeconds () << " " << newval / 1446.0 << std::endl;
  fPlotQueue.close ();
}

static void
tracer38 (uint32_t oldval, uint32_t newval)
{
  std::ofstream fPlotQueue (dir + "cwndTraces/S_40.plotme", std::ios::out | std::ios::app);
  fPlotQueue << Simulator::Now ().GetSeconds () << " " << newval / 1446.0 << std::endl;
  fPlotQueue.close ();
}

static void
tracer39 (uint32_t oldval, uint32_t newval)
{
  std::ofstream fPlotQueue (dir + "cwndTraces/S_41.plotme", std::ios::out | std::ios::app);
  fPlotQueue << Simulator::Now ().GetSeconds () << " " << newval / 1446.0 << std::endl;
  fPlotQueue.close ();
}

static void
tracer40 (uint32_t oldval, uint32_t newval)
{
  std::ofstream fPlotQueue (dir + "cwndTraces/S_42.plotme", std::ios::out | std::ios::app);
  fPlotQueue << Simulator::Now ().GetSeconds () << " " << newval / 1446.0 << std::endl;
  fPlotQueue.close ();
}

static void
tracer41 (uint32_t oldval, uint32_t newval)
{
  std::ofstream fPlotQueue (dir + "cwndTraces/S_43.plotme", std::ios::out | std::ios::app);
  fPlotQueue << Simulator::Now ().GetSeconds () << " " << newval / 1446.0 << std::endl;
  fPlotQueue.close ();
}

static void
tracer42 (uint32_t oldval, uint32_t newval)
{
  std::ofstream fPlotQueue (dir + "cwndTraces/S_44.plotme", std::ios::out | std::ios::app);
  fPlotQueue << Simulator::Now ().GetSeconds () << " " << newval / 1446.0 << std::endl;
  fPlotQueue.close ();
}

static void
tracer43 (uint32_t oldval, uint32_t newval)
{
  std::ofstream fPlotQueue (dir + "cwndTraces/S_45.plotme", std::ios::out | std::ios::app);
  fPlotQueue << Simulator::Now ().GetSeconds () << " " << newval / 1446.0 << std::endl;
  fPlotQueue.close ();
}

static void
tracer44 (uint32_t oldval, uint32_t newval)
{
  std::ofstream fPlotQueue (dir + "cwndTraces/S_46.plotme", std::ios::out | std::ios::app);
  fPlotQueue << Simulator::Now ().GetSeconds () << " " << newval / 1446.0 << std::endl;
  fPlotQueue.close ();
}

static void
tracer45 (uint32_t oldval, uint32_t newval)
{
  std::ofstream fPlotQueue (dir + "cwndTraces/S_47.plotme", std::ios::out | std::ios::app);
  fPlotQueue << Simulator::Now ().GetSeconds () << " " << newval / 1446.0 << std::endl;
  fPlotQueue.close ();
}

static void
tracer46 (uint32_t oldval, uint32_t newval)
{
  std::ofstream fPlotQueue (dir + "cwndTraces/S_48.plotme", std::ios::out | std::ios::app);
  fPlotQueue << Simulator::Now ().GetSeconds () << " " << newval / 1446.0 << std::endl;
  fPlotQueue.close ();
}

static void
tracer47 (uint32_t oldval, uint32_t newval)
{
  std::ofstream fPlotQueue (dir + "cwndTraces/S_49.plotme", std::ios::out | std::ios::app);
  fPlotQueue << Simulator::Now ().GetSeconds () << " " << newval / 1446.0 << std::endl;
  fPlotQueue.close ();
}

static void
tracer48 (uint32_t oldval, uint32_t newval)
{
  std::ofstream fPlotQueue (dir + "cwndTraces/S_50.plotme", std::ios::out | std::ios::app);
  fPlotQueue << Simulator::Now ().GetSeconds () << " " << newval / 1446.0 << std::endl;
  fPlotQueue.close ();
}

static void
tracer49 (uint32_t oldval, uint32_t newval)
{
  std::ofstream fPlotQueue (dir + "cwndTraces/x.plotme", std::ios::out | std::ios::app);
  fPlotQueue << Simulator::Now ().GetSeconds () << " " << newval / 1446.0 << std::endl;
  fPlotQueue.close ();
}

static void
cwnd ()
{
  Config::ConnectWithoutContext ("/NodeList/0/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&tracer));
  Config::ConnectWithoutContext ("/NodeList/1/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&tracer1));
  Config::ConnectWithoutContext ("/NodeList/2/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&tracer2));
  Config::ConnectWithoutContext ("/NodeList/3/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&tracer3));
  Config::ConnectWithoutContext ("/NodeList/4/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&tracer4));
  Config::ConnectWithoutContext ("/NodeList/5/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&tracer5));
  Config::ConnectWithoutContext ("/NodeList/6/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&tracer6));
  Config::ConnectWithoutContext ("/NodeList/7/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&tracer7));
  Config::ConnectWithoutContext ("/NodeList/8/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&tracer8));
  Config::ConnectWithoutContext ("/NodeList/9/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&tracer9));
  Config::ConnectWithoutContext ("/NodeList/10/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&tracer10));
  Config::ConnectWithoutContext ("/NodeList/11/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&tracer11));
  Config::ConnectWithoutContext ("/NodeList/12/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&tracer12));
  Config::ConnectWithoutContext ("/NodeList/13/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&tracer13));
  Config::ConnectWithoutContext ("/NodeList/14/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&tracer14));
  Config::ConnectWithoutContext ("/NodeList/15/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&tracer15));
  Config::ConnectWithoutContext ("/NodeList/16/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&tracer16));
  Config::ConnectWithoutContext ("/NodeList/17/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&tracer17));
  Config::ConnectWithoutContext ("/NodeList/18/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&tracer18));
  Config::ConnectWithoutContext ("/NodeList/19/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&tracer19));
  Config::ConnectWithoutContext ("/NodeList/20/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&tracer20));
  Config::ConnectWithoutContext ("/NodeList/21/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&tracer21));
  Config::ConnectWithoutContext ("/NodeList/22/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&tracer22));
  Config::ConnectWithoutContext ("/NodeList/23/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&tracer23));
  Config::ConnectWithoutContext ("/NodeList/24/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&tracer24));
  Config::ConnectWithoutContext ("/NodeList/25/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&tracer25));
  Config::ConnectWithoutContext ("/NodeList/26/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&tracer26));
  Config::ConnectWithoutContext ("/NodeList/27/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&tracer27));
  Config::ConnectWithoutContext ("/NodeList/28/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&tracer28));
  Config::ConnectWithoutContext ("/NodeList/29/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&tracer29));
  Config::ConnectWithoutContext ("/NodeList/30/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&tracer30));
  Config::ConnectWithoutContext ("/NodeList/31/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&tracer31));
  Config::ConnectWithoutContext ("/NodeList/32/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&tracer32));
  Config::ConnectWithoutContext ("/NodeList/33/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&tracer33));
  Config::ConnectWithoutContext ("/NodeList/34/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&tracer34));
  Config::ConnectWithoutContext ("/NodeList/35/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&tracer35));
  Config::ConnectWithoutContext ("/NodeList/36/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&tracer36));
  Config::ConnectWithoutContext ("/NodeList/37/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&tracer37));
  Config::ConnectWithoutContext ("/NodeList/38/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&tracer38));
  Config::ConnectWithoutContext ("/NodeList/39/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&tracer39));
  Config::ConnectWithoutContext ("/NodeList/40/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&tracer40));
  Config::ConnectWithoutContext ("/NodeList/41/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&tracer41));
  Config::ConnectWithoutContext ("/NodeList/42/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&tracer42));
  Config::ConnectWithoutContext ("/NodeList/43/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&tracer43));
  Config::ConnectWithoutContext ("/NodeList/44/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&tracer44));
  Config::ConnectWithoutContext ("/NodeList/45/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&tracer45));
  Config::ConnectWithoutContext ("/NodeList/46/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&tracer46));
  Config::ConnectWithoutContext ("/NodeList/47/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&tracer47));
  Config::ConnectWithoutContext ("/NodeList/48/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&tracer48));
  Config::ConnectWithoutContext ("/NodeList/48/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&tracer49));
}




int main (int argc, char *argv[])
{
  int i = 0;
  float startTime = 0.0;
  float simDuration = 101;      // in seconds
  std::string  pathOut = ".";
  bool writeForPlot = true;
  std::string EcnMode = "NoEcn";
  bool useEcn = false;
  std::string queue_disc_type = "CobaltQueueDisc";
  float stopTime = startTime + simDuration;


  CommandLine cmd;
  cmd.AddValue ("queue_disc_type", "Queue disc type for gateway (e.g. ns3::CoDelQueueDisc)", queue_disc_type);
  cmd.Parse (argc,argv);

  queue_disc_type = std::string ("ns3::") + queue_disc_type;

  TypeId qdTid;
  NS_ABORT_MSG_UNLESS (TypeId::LookupByNameFailSafe (queue_disc_type, &qdTid), "TypeId " << queue_disc_type << " not found");

  std::string bottleneckBandwidth = "10Mbps";
  std::string bottleneckDelay = "50ms";

  std::string accessBandwidth = "10Mbps";
  std::string accessDelay = "5ms";

  NodeContainer source;
  source.Create (50);

  NodeContainer gateway;
  gateway.Create (2);

  NodeContainer sink;
  sink.Create (1);

  Config::SetDefault ("ns3::TcpSocket::SndBufSize", UintegerValue (1 << 20));
  Config::SetDefault ("ns3::TcpSocket::RcvBufSize", UintegerValue (1 << 20));
  Config::SetDefault ("ns3::TcpSocket::DelAckTimeout", TimeValue (Seconds (0)));
  Config::SetDefault ("ns3::TcpSocket::InitialCwnd", UintegerValue (1));
  Config::SetDefault ("ns3::TcpSocketBase::LimitedTransmit", BooleanValue (false));
  Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (1446));
  Config::SetDefault ("ns3::TcpSocketBase::WindowScaling", BooleanValue (true));
  Config::SetDefault ("ns3::CobaltQueueDisc::UseEcn", BooleanValue (useEcn));
  Config::SetDefault ("ns3::TcpSocketBase::EcnMode", StringValue (EcnMode));
  Config::SetDefault (queue_disc_type + "::MaxSize", QueueSizeValue (QueueSize ("200p")));

  InternetStackHelper internet;
  internet.InstallAll ();

  TrafficControlHelper tchPfifo;
  uint16_t handle = tchPfifo.SetRootQueueDisc ("ns3::PfifoFastQueueDisc");
  tchPfifo.AddInternalQueues (handle, 3, "ns3::DropTailQueue", "MaxSize", StringValue ("1000p"));


  TrafficControlHelper tch;
  tch.SetRootQueueDisc (queue_disc_type);

// Create and configure access link and bottleneck link
  PointToPointHelper accessLink;
  accessLink.SetDeviceAttribute ("DataRate", StringValue (accessBandwidth));
  accessLink.SetChannelAttribute ("Delay", StringValue (accessDelay));

  NetDeviceContainer devices[50];
  for (i = 0; i < 50; i++)
    {
      devices[i] = accessLink.Install (source.Get (i), gateway.Get (0));
      tchPfifo.Install (devices[i]);
    }

  NetDeviceContainer devices_sink;
  devices_sink = accessLink.Install (gateway.Get (1), sink.Get (0));
  tchPfifo.Install (devices_sink);

  PointToPointHelper bottleneckLink;
  bottleneckLink.SetDeviceAttribute ("DataRate", StringValue (bottleneckBandwidth));
  bottleneckLink.SetChannelAttribute ("Delay", StringValue (bottleneckDelay));

  NetDeviceContainer devices_gateway;
  devices_gateway = bottleneckLink.Install (gateway.Get (0), gateway.Get (1));
  QueueDiscContainer queueDiscs = tch.Install (devices_gateway);

  Ipv4AddressHelper address;
  address.SetBase ("10.0.0.0", "255.255.255.0");

  // Configure the source and sink net devices
  // and the channels between the source/sink and the gateway
  //Ipv4InterfaceContainer sink_Interfaces;
  Ipv4InterfaceContainer interfaces[50];
  Ipv4InterfaceContainer interfaces_sink;
  Ipv4InterfaceContainer interfaces_gateway;

  for (i = 0; i < 50; i++)
    {
      address.NewNetwork ();
      interfaces[i] = address.Assign (devices[i]);
    }

  address.NewNetwork ();
  interfaces_gateway = address.Assign (devices_gateway);

  address.NewNetwork ();
  interfaces_sink = address.Assign (devices_sink);

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  uint16_t port = 50000;
  Address sinkLocalAddress (InetSocketAddress (Ipv4Address::GetAny (), port));
  PacketSinkHelper sinkHelper ("ns3::TcpSocketFactory", sinkLocalAddress);

  // Configure application
  AddressValue remoteAddress (InetSocketAddress (interfaces_sink.GetAddress (1), port));
  BulkSendHelper ftp ("ns3::TcpSocketFactory", Address ());
  ftp.SetAttribute ("Remote", remoteAddress);
  ftp.SetAttribute ("SendSize", UintegerValue (1000));

  ApplicationContainer sourceApp = ftp.Install (source);
  sourceApp.Start (Seconds (0));
  sourceApp.Stop (Seconds (stopTime - 1));

  sinkHelper.SetAttribute ("Protocol", TypeIdValue (TcpSocketFactory::GetTypeId ()));
  ApplicationContainer sinkApp = sinkHelper.Install (sink);
  sinkApp.Start (Seconds (0));
  sinkApp.Stop (Seconds (stopTime));

  if (writeForPlot)
    {
      Ptr<QueueDisc> queue = queueDiscs.Get (0);
      Simulator::ScheduleNow (&CheckQueueSize, queue);
    }

  std::string dirToSave = "mkdir -p " + dir;
  system (dirToSave.c_str ());
  system ((dirToSave + "/pcap/").c_str ());
  system ((dirToSave + "/cwndTraces/").c_str ());
  system ((dirToSave + "/queueTraces/").c_str ());
  bottleneckLink.EnablePcapAll (dir + "pcap/N", true);

  Simulator::Schedule (Seconds (0.1), &cwnd);

  Simulator::Stop (Seconds (stopTime));
  Simulator::Run ();

  Simulator::Destroy ();
  return 0;
}

