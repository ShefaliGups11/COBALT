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

std::string dir = "PlotsMix/";

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
  std::ofstream fPlotQueue (dir + "cwndTraces/A.plotme", std::ios::out | std::ios::app);
  fPlotQueue << Simulator::Now ().GetSeconds () << " " << newval / 1446.0 << std::endl;
  fPlotQueue.close ();
}

static void
tracer1 (uint32_t oldval, uint32_t newval)
{
  std::ofstream fPlotQueue (dir + "cwndTraces/B.plotme", std::ios::out | std::ios::app);
  fPlotQueue << Simulator::Now ().GetSeconds () << " " << newval / 1446.0 << std::endl;
  fPlotQueue.close ();
}

static void
tracer2 (uint32_t oldval, uint32_t newval)
{
  std::ofstream fPlotQueue (dir + "cwndTraces/C.plotme", std::ios::out | std::ios::app);
  fPlotQueue << Simulator::Now ().GetSeconds () << " " << newval / 1446.0 << std::endl;
  fPlotQueue.close ();

}

static void
tracer3 (uint32_t oldval, uint32_t newval)
{
  std::ofstream fPlotQueue (dir + "cwndTraces/D.plotme", std::ios::out | std::ios::app);
  fPlotQueue << Simulator::Now ().GetSeconds () << " " << newval / 1446.0 << std::endl;
  fPlotQueue.close ();
}

static void
tracer4 (uint32_t oldval, uint32_t newval)
{
  std::ofstream fPlotQueue (dir + "cwndTraces/E.plotme", std::ios::out | std::ios::app);
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
  float stopTime = startTime + simDuration;
  std::string queue_disc_type = "CobaltQueueDisc";

  CommandLine cmd;
  cmd.AddValue ("queue_disc_type", "Queue disc type for gateway by defalut is COBALT (e.g. ns3::CobaltQueueDisc)", queue_disc_type);
  cmd.Parse (argc,argv);

  queue_disc_type = std::string ("ns3::") + queue_disc_type;

  TypeId qdTid;
  NS_ABORT_MSG_UNLESS (TypeId::LookupByNameFailSafe (queue_disc_type, &qdTid), "TypeId " << queue_disc_type << " not found");


  std::string bottleneckBandwidth = "10Mbps";
  std::string bottleneckDelay = "50ms";

  std::string accessBandwidth = "10Mbps";
  std::string accessDelay = "5ms";


  NodeContainer source;
  source.Create (5);

  NodeContainer udpsource;
  udpsource.Create (2);

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

  NetDeviceContainer devices[5];
  for (i = 0; i < 5; i++)
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
  Ipv4InterfaceContainer interfaces[5];
  Ipv4InterfaceContainer interfaces_sink;
  Ipv4InterfaceContainer interfaces_gateway;
  Ipv4InterfaceContainer udpinterfaces[2];

  NetDeviceContainer udpdevices[2];

  for (i = 0; i < 5; i++)
    {
      address.NewNetwork ();
      interfaces[i] = address.Assign (devices[i]);
    }

  for (i = 0; i < 2; i++)
    {
      udpdevices[i] = accessLink.Install (udpsource.Get (i), gateway.Get (0));
      address.NewNetwork ();
      udpinterfaces[i] = address.Assign (udpdevices[i]);
    }

  address.NewNetwork ();
  interfaces_gateway = address.Assign (devices_gateway);

  address.NewNetwork ();
  interfaces_sink = address.Assign (devices_sink);


  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();


  uint16_t port = 50000;
  uint16_t port1 = 50001;
  Address sinkLocalAddress (InetSocketAddress (Ipv4Address::GetAny (), port));
  Address sinkLocalAddress1 (InetSocketAddress (Ipv4Address::GetAny (), port1));
  PacketSinkHelper sinkHelper ("ns3::TcpSocketFactory", sinkLocalAddress);
  PacketSinkHelper sinkHelper1 ("ns3::UdpSocketFactory", sinkLocalAddress1);


  // Configure application
  AddressValue remoteAddress (InetSocketAddress (interfaces_sink.GetAddress (1), port));
  AddressValue remoteAddress1 (InetSocketAddress (interfaces_sink.GetAddress (1), port1));

  Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (1000));
  BulkSendHelper ftp ("ns3::TcpSocketFactory", Address ());
  ftp.SetAttribute ("Remote", remoteAddress);
  ftp.SetAttribute ("SendSize", UintegerValue (1000));


  ApplicationContainer sourceApp = ftp.Install (source
                                                );
  sourceApp.Start (Seconds (0));
  sourceApp.Stop (Seconds (stopTime - 1));

  sinkHelper.SetAttribute ("Protocol", TypeIdValue (TcpSocketFactory::GetTypeId ()));
  ApplicationContainer sinkApp = sinkHelper.Install (sink);
  sinkApp.Start (Seconds (0));
  sinkApp.Stop (Seconds (stopTime));


  OnOffHelper clientHelper6 ("ns3::UdpSocketFactory", Address ());
  clientHelper6.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  clientHelper6.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  clientHelper6.SetAttribute ("DataRate", DataRateValue (DataRate ("10Mb/s")));
  clientHelper6.SetAttribute ("PacketSize", UintegerValue (1000));

  ApplicationContainer clientApps6;

  clientHelper6.SetAttribute ("Remote", remoteAddress1);
  clientApps6.Add (clientHelper6.Install (udpsource.Get (0)));
  clientApps6.Start (Seconds (0));
  clientApps6.Stop (Seconds (stopTime - 1));

  OnOffHelper clientHelper7 ("ns3::UdpSocketFactory", Address ());
  clientHelper7.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  clientHelper7.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  clientHelper7.SetAttribute ("DataRate", DataRateValue (DataRate ("10Mb/s")));
  clientHelper7.SetAttribute ("PacketSize", UintegerValue (1000));

  ApplicationContainer clientApps7;
  clientHelper7.SetAttribute ("Remote", remoteAddress1);
  clientApps7.Add (clientHelper7.Install (udpsource.Get (1)));
  clientApps7.Start (Seconds (0));
  clientApps7.Stop (Seconds (stopTime - 1));

  sinkHelper1.SetAttribute ("Protocol", TypeIdValue (UdpSocketFactory::GetTypeId ()));
  ApplicationContainer sinkApp1 = sinkHelper1.Install (sink);
  sinkApp1.Start (Seconds (0));
  sinkApp1.Stop (Seconds (stopTime));

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

