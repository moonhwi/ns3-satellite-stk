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
    LogComponentEnable("RandomWalk2d", LOG_LEVEL_ALL);
    LogComponentEnable("OlsrRoutingProtocol", LOG_LEVEL_ALL);
    SpectrumWifiPhyHelper spectrumPhy = SpectrumWifiPhyHelper::Default();
    Config::SetDefault( "ns3::WifiPhy::CcaMode1Threshold", DoubleValue( -62.0 ) );
    Ptr<MultiModelSpectrumChannel> spectrumChannel = CreateObject<MultiModelSpectrumChannel> ();
    Ptr<FriisPropagationLossModel> lossModel = CreateObject<FriisPropagationLossModel> ();
    //std::vector<int> ilist = {2,14};
    std::vector<std::vector<int> > q={{1,14},{0,3},{0,3},{2,5},{3,6},{3,6},{5,8},{6,9},{6,9},{8,11},{9,12},{9,12},{11,14},{12,0},{12,0}};  
    lossModel->SetAssNode(q);
    lossModel->SetFrequency( 10.180e9 );
    spectrumChannel->AddPropagationLossModel( lossModel );
    Ptr<ConstantSpeedPropagationDelayModel> delayModel = CreateObject<ConstantSpeedPropagationDelayModel> ();
    spectrumChannel->SetPropagationDelayModel( delayModel );
    spectrumPhy.SetChannel( spectrumChannel );
    spectrumPhy.SetErrorRateModel( "ns3::NistErrorRateModel" );
    spectrumPhy.Set( "Frequency", UintegerValue( 10180 ) );
    spectrumPhy.Set( "TxPowerStart", DoubleValue( 5 ) );
    spectrumPhy.Set( "TxGain", DoubleValue( 45 ) );
    spectrumPhy.Set( "RxGain", DoubleValue( 45 ) );
    spectrumPhy.Set( "TxPowerEnd", DoubleValue( 5 ) );
    WifiHelper wifi;
    wifi.SetStandard( WIFI_PHY_STANDARD_80211a );
    wifi.SetRemoteStationManager( "ns3::ConstantRateWifiManager", "DataMode", StringValue( "OfdmRate6Mbps" ) );
    NqosWifiMacHelper mac = NqosWifiMacHelper::Default();
    //mac.SetType( "ns3::AdhocWifiMac",
             //"Slot", StringValue( "16us" ) );
    NetDeviceContainer Satellitedevice;
    Satellitedevice = wifi.Install( spectrumPhy, mac, Satellite );

    MobilityHelper mobility;
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
    positionAlloc->Add (Vector (0, 0, 10));
    positionAlloc->Add (Vector (10, 0, 10));
    positionAlloc->Add (Vector (15, 4, 10));
    positionAlloc->Add (Vector (1, 1, 10));
    mobility.SetPositionAllocator (positionAlloc);
  mobility.SetMobilityModel( "ns3::GaussMarkovMobilityModel",
                  "Bounds", BoxValue( Box( -150000000, 150000000, -150000000, 150000000, -150000000, 150000000 ) ),
                  "TimeStep", TimeValue( Seconds( 1 ) ));
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
    address.SetBase( "195.1.1.0", "255.255.255.0" );
    Ipv4InterfaceContainer Satip;
    Satip = address.Assign(Satellitedevice);
    
    OnOffHelper onOff1( "ns3::UdpSocketFactory", Address( InetSocketAddress( Satip.GetAddress( 0 ), 9 ) ) );
    Config::SetDefault ("ns3::OnOffApplication::PacketSize", UintegerValue (210));
    Config::SetDefault ("ns3::OnOffApplication::DataRate", StringValue ("448kb/s"));


    onOff1.SetAttribute( "OnTime", StringValue( "ns3::ConstantRandomVariable[Constant=0.1]" ) );
    onOff1.SetAttribute( "OffTime", StringValue( "ns3::ConstantRandomVariable[Constant=0.9]" ) );
    ApplicationContainer apps1 = onOff1.Install( Satellite.Get(7) );
    apps1.Start( Seconds( 1.0 ) );
    apps1.Stop( Seconds( 20.0 ) );


    ApplicationContainer apps2=onOff1.Install(Satellite.Get(1));
    apps2.Start( Seconds( 1.0 ) );
    apps2.Stop( Seconds( 20.0 ) );

    PacketSinkHelper sink ("ns3::UdpSocketFactory",
    Address( InetSocketAddress( Satip.GetAddress( 0 ), 9)));
    ApplicationContainer apps= sink.Install (Satellite);
    apps.Start (Seconds (0));
    apps.Stop (Seconds (25.0));

    AnimationInterface anim( "first.xml" );
    spectrumPhy.EnablePcapAll("try");
    Simulator::Stop( Seconds( 25.0 ) );
    Simulator::Run();
    Simulator::Destroy();
    return(0);
}
