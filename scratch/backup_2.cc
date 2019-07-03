//本次备份是为了测试双网卡情况下的路由包发送过程中路由构建的速度
//测试已经成功
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"
#include "ns3/netanim-module.h"
#include "ns3/olsr-helper.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/ipv4-list-routing-helper.h"
#include "ns3/spectrum-module.h"
#include "ns3/stats-module.h"
#include "ns3/pcap-file-wrapper.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/netanim-module.h"
using namespace ns3;
int
main( int argc, char *argv[] )
{
    //GlobalValue::Bind ("SimulatorImplementationType", StringValue ("ns3::RealtimeSimulatorImpl"));
    NodeContainer Satellite;
    Satellite.Create(15);
    //LogComponentEnable("Ipv4L3Protocol", LOG_LEVEL_LOGIC);
    LogComponentEnable("PropagationLossModel", LOG_LEVEL_DEBUG);
    //LogComponentEnable("OlsrRoutingProtocol", LOG_LEVEL_DEBUG);
    SpectrumWifiPhyHelper spectrumPhy = SpectrumWifiPhyHelper::Default();
    SpectrumWifiPhyHelper spectrumPhy_2 = SpectrumWifiPhyHelper::Default();
    Config::SetDefault( "ns3::WifiPhy::CcaMode1Threshold", DoubleValue( -62.0 ) );
    Ptr<MultiModelSpectrumChannel> spectrumChannel = CreateObject<MultiModelSpectrumChannel> ();
    Ptr<MultiModelSpectrumChannel> spectrumChannel_2 = CreateObject<MultiModelSpectrumChannel> ();
    Ptr<FriisPropagationLossModel> lossModel = CreateObject<FriisPropagationLossModel> ();
    Ptr<FriisPropagationLossModel> lossModel_2 = CreateObject<FriisPropagationLossModel> ();
    //std::vector<int> ilist = {2,14};
    std::vector<std::vector<int> > q={{1,14},{0,3},{0,3},{5,2},{3,6},{3,6},{8,5},{6,9},{6,9},{11,8},{9,12},{9,12},{14,11},{12,0},{12,0}};  
    lossModel->SetAssNode(q);
    lossModel_2->SetAssNode(q);
    lossModel->SetFrequency( 10.180e9 );
    lossModel_2->SetFrequency( 5.180e9 );
    spectrumChannel->AddPropagationLossModel( lossModel );
    spectrumChannel_2->AddPropagationLossModel( lossModel_2 );
    Ptr<ConstantSpeedPropagationDelayModel> delayModel = CreateObject<ConstantSpeedPropagationDelayModel> ();
    spectrumChannel->SetPropagationDelayModel( delayModel );
    spectrumChannel_2->SetPropagationDelayModel( delayModel );
    spectrumPhy.SetChannel( spectrumChannel );
    spectrumPhy.SetErrorRateModel( "ns3::NistErrorRateModel" );
    spectrumPhy.Set( "Frequency", UintegerValue( 10180 ) );
    spectrumPhy.Set( "TxPowerStart", DoubleValue( 5 ) );
    spectrumPhy.Set( "TxGain", DoubleValue( 45 ) );
    spectrumPhy.Set( "RxGain", DoubleValue( 45 ) );
    spectrumPhy.Set( "TxPowerEnd", DoubleValue( 5 ) );
    spectrumPhy_2.SetChannel( spectrumChannel_2 );
    spectrumPhy_2.SetErrorRateModel( "ns3::NistErrorRateModel" );
    spectrumPhy_2.Set( "Frequency", UintegerValue( 5180 ) );
    spectrumPhy_2.Set( "TxPowerStart", DoubleValue( 5 ) );
    spectrumPhy_2.Set( "TxGain", DoubleValue( 45) );
    spectrumPhy_2.Set( "RxGain", DoubleValue( 45 ) );
    spectrumPhy_2.Set( "TxPowerEnd", DoubleValue( 5 ) );
    WifiHelper wifi;
    WifiHelper wifi_2;
    wifi.SetStandard( WIFI_PHY_STANDARD_80211a );
    wifi.SetRemoteStationManager( "ns3::ConstantRateWifiManager", "DataMode", StringValue( "OfdmRate6Mbps" ) );
    wifi_2.SetStandard( WIFI_PHY_STANDARD_80211a );
    wifi_2.SetRemoteStationManager( "ns3::ConstantRateWifiManager", "DataMode", StringValue( "OfdmRate6Mbps" ) );
    NqosWifiMacHelper mac = NqosWifiMacHelper::Default();
    NetDeviceContainer Satellitedevice;
    Satellitedevice = wifi.Install( spectrumPhy, mac, Satellite );
    NetDeviceContainer Satellitedevice_2;
    Satellitedevice_2 = wifi_2.Install( spectrumPhy_2, mac, Satellite );
    MobilityHelper mobility;
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
    positionAlloc->Add (Vector (0, 0, 10));
    positionAlloc->Add (Vector (10, 0, 10));
    positionAlloc->Add (Vector (15, 4, 10));
    positionAlloc->Add (Vector (1, 1, 10));
    mobility.SetPositionAllocator (positionAlloc);
    mobility.SetMobilityModel( "ns3::GaussMarkovMobilityModel",
                  "Bounds", BoxValue( Box( -150000000, 150000000, -150000000, 150000000, -150000000, 150000000 ) ),
                  "TimeStep", TimeValue( Seconds( 5 ) ));
                     for ( uint32_t initialnumber = 0; initialnumber < 15; initialnumber++ )
    {
       Ptr<GaussMarkovMobilityModel> mob = Satellite.Get( initialnumber )->GetObject<GaussMarkovMobilityModel>();
       mob->Setnodenumber( 15 );
    }

    mobility.Install(Satellite);
    OlsrHelper      olsr;
    Ipv4StaticRoutingHelper staticRouting;
    Ipv4ListRoutingHelper   list;
    list.Add( staticRouting, 0 );
    list.Add( olsr, 10 );
    InternetStackHelper internet;
    internet.SetRoutingHelper( list );
    /* has effect on the next Install () */
    internet.Install( Satellite );
    Ipv4AddressHelper address;
    Ipv4InterfaceContainer Satip;
    address.SetBase( "195.1.1.0", "255.255.255.0" );
    Satip = address.Assign(Satellitedevice);
    address.SetBase( "195.1.2.0", "255.255.255.0" );
    Ipv4InterfaceContainer Satip_1 = address.Assign(Satellitedevice_2);
    AnimationInterface anim( "/home/limemnghui/ns-allinone-3.27/ns-3.27/doublenetdevice/doubledevices.xml" );
    spectrumPhy.EnablePcapAll("/home/limemnghui/ns-allinone-3.27/ns-3.27/doublenetdevice/try_doublenetdevices");
    
    Ipv4GlobalRoutingHelper g;
    Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper> ("/home/limemnghui/ns-allinone-3.27/ns-3.27/doublenetdevice/multisat@10.routes", std::ios::out);
    g.PrintRoutingTableAllAt (Seconds (10), routingStream);
    Ipv4GlobalRoutingHelper h;
    Ptr<OutputStreamWrapper> routingStream_1 = Create<OutputStreamWrapper> ("/home/limemnghui/ns-allinone-3.27/ns-3.27/doublenetdevice/multisat@5.routes", std::ios::out);
    h.PrintRoutingTableAllAt (Seconds (5), routingStream_1);
    Ipv4GlobalRoutingHelper l;
    Ptr<OutputStreamWrapper> routingStream_2 = Create<OutputStreamWrapper> ("/home/limemnghui/ns-allinone-3.27/ns-3.27/doublenetdevice/multisat@3.routes", std::ios::out);
    l.PrintRoutingTableAllAt (Seconds (3), routingStream_2);
    Ipv4GlobalRoutingHelper k;
    Ptr<OutputStreamWrapper> routingStream_4 = Create<OutputStreamWrapper> ("/home/limemnghui/ns-allinone-3.27/ns-3.27/doublenetdevice/multisat@15.routes", std::ios::out);
    k.PrintRoutingTableAllAt (Seconds (15), routingStream_4);
    routingStream_4 = Create<OutputStreamWrapper> ("/home/limemnghui/ns-allinone-3.27/ns-3.27/doublenetdevice/multisat@11.routes", std::ios::out);
    k.PrintRoutingTableAllAt (Seconds (11), routingStream_4);
    routingStream_4 = Create<OutputStreamWrapper> ("/home/limemnghui/ns-allinone-3.27/ns-3.27/doublenetdevice/multisat@13.routes", std::ios::out);
    k.PrintRoutingTableAllAt (Seconds (13), routingStream_4);
    routingStream_4 = Create<OutputStreamWrapper> ("/home/limemnghui/ns-allinone-3.27/ns-3.27/doublenetdevice/multisat@16.routes", std::ios::out);
    k.PrintRoutingTableAllAt (Seconds (16), routingStream_4);
    routingStream_4 = Create<OutputStreamWrapper> ("/home/limemnghui/ns-allinone-3.27/ns-3.27/doublenetdevice/multisat@17.routes", std::ios::out);
    k.PrintRoutingTableAllAt (Seconds (17), routingStream_4);
    routingStream_4 = Create<OutputStreamWrapper> ("/home/limemnghui/ns-allinone-3.27/ns-3.27/doublenetdevice/multisat@20.routes", std::ios::out);
    k.PrintRoutingTableAllAt (Seconds (20), routingStream_4);
    // spectrumPhy.EnablePcapAll("\\data\\tryforbackup");
    Simulator::Stop( Seconds( 50.0 ) );
    Simulator::Run();
    Simulator::Destroy();
    return(0);
}
