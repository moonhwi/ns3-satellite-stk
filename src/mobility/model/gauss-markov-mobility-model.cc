/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009 Dan Broyles
 *
 * This pogram is free software; you can redistribute it and/or modify
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
 * Author: Dan Broyles <dbroyl01@ku.edu>
 */
#include <cmath>
#include "ns3/simulator.h"
#include "ns3/double.h"
#include "ns3/pointer.h"
#include "ns3/string.h"
#include "gauss-markov-mobility-model.h"
#include "position-allocator.h"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
int rankforme=0;
char originposition[256];
int node_number;
std::vector<int> lawnode;
namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (GaussMarkovMobilityModel);

TypeId
GaussMarkovMobilityModel::GetTypeId (void)
{

  static TypeId tid = TypeId ("ns3::GaussMarkovMobilityModel")
    .SetParent<MobilityModel> ()
    .SetGroupName ("Mobility")
    .AddConstructor<GaussMarkovMobilityModel> ()
    .AddAttribute ("Bounds",
                   "Bounds of the area to cruise.",
                   BoxValue (Box (-100.0, 100.0, -100.0, 100.0, 0.0, 100.0)),
                   MakeBoxAccessor (&GaussMarkovMobilityModel::m_bounds),
                   MakeBoxChecker ())
    .AddAttribute ("TimeStep",
                   "Change current direction and speed after moving for this time.",
                   TimeValue (Seconds (1.0)),
                   MakeTimeAccessor (&GaussMarkovMobilityModel::m_timeStep),
                   MakeTimeChecker ());
  return tid;
}

GaussMarkovMobilityModel::GaussMarkovMobilityModel ()
{

  m_event = Simulator::ScheduleNow (&GaussMarkovMobilityModel::Start, this);
  m_helper.Unpause ();
}

void
GaussMarkovMobilityModel::Setoriginpos(char *positionorvector)
{
  for (unsigned int i=0;i<256;i++)
  {

    originposition[i]=positionorvector[i];
 
  }

}
void 
GaussMarkovMobilityModel::Setnodenumber(int n)
{
  node_number=n;
}

void
GaussMarkovMobilityModel::Start (void)
{

  /*if (m_meanVelocity == 0.0)
    {
      //Initialize the mean velocity, direction, and pitch variables
      m_meanVelocity = m_rndMeanVelocity->GetValue ();
      m_meanDirection = m_rndMeanDirection->GetValue ();
      m_meanPitch = m_rndMeanPitch->GetValue ();
      double cosD = std::cos (m_meanDirection);
      double cosP = std::cos (m_meanPitch);
      double sinD = std::sin (m_meanDirection);
      double sinP = std::sin (m_meanPitch);
      //Initialize the starting velocity, direction, and pitch to be identical to the mean ones
      m_Velocity = m_meanVelocity;
      m_Direction = m_meanDirection;
      m_Pitch = m_meanPitch;
      //Set the velocity vector to give to the constant velocity helper
      //m_helper.SetVelocity (Vector (m_Velocity*cosD*cosP, m_Velocity*sinD*cosP, m_Velocity*sinP));
    }*/
//=====================这里是为了获得自己的节点所处的队列位置，这样方便后续的位置更新
  /*std::fstream infile;
  infile.open("/mnt/hgfs/share/erw.txt",std::ios::in);
  char buff[256];int rownumber=0;
  //for (unsigned int a=0;a<sizeof(originposition);a++)
  //  std::cout<<"originposition:"<<originposition[a];
  while(!infile.eof())
  {
    infile.getline(buff,256,'\n');
    rownumber+=1;
    std::string str=buff;
    std::string str1=originposition;
    //std::cout<<"str:"<<str<<" str1:"<<str1<<std::endl;
    if(str==str1)
    {
      rankforme=rownumber;
      break;
    }
  }

  
  infile.close();*/

//============================================
//  m_helper.SetPosition (Vector(-3748836.954130,4856904.662021,2637084.068845));
//  m_helper.SetVelocity (Vector (-6076.564494, 1672.881634, 3686.270584));
  //m_helper.Update ();



  //Get the next values from the gaussian distributions for velocity, direction, and pitch
  /*double rv = m_normalVelocity->GetValue ();
  double rd = m_normalDirection->GetValue ();
  double rp = m_normalPitch->GetValue ();
*/
  //Calculate the NEW velocity, direction, and pitch values using the Gauss-Markov formula:
  //newVal = alpha*oldVal + (1-alpha)*meanVal + sqrt(1-alpha^2)*rv
  //where rv is a random number from a normal (gaussian) distribution
//  double one_minus_alpha = 1 - m_alpha;
//  double sqrt_alpha = std::sqrt (1 - m_alpha*m_alpha);
/*  m_Velocity.x  = m_alpha * m_Velocity  + one_minus_alpha * m_meanVelocity  + sqrt_alpha * rv;
  m_Direction = m_alpha * m_Direction + one_minus_alpha * m_meanDirection + sqrt_alpha * rd;
  m_Pitch     = m_alpha * m_Pitch     + one_minus_alpha * m_meanPitch     + sqrt_alpha * rp;

  //Calculate the linear velocity vector to give to the constant velocity helper
  double cosDir = std::cos (m_Direction);
  double cosPit = std::cos (m_Pitch);
  double sinDir = std::sin (m_Direction);
  double sinPit = std::sin (m_Pitch);
  double vx = m_Velocity * cosDir * cosPit;
  double vy = m_Velocity * sinDir * cosPit;
  double vz = m_Velocity * sinPit;
  m_helper.SetVelocity (Vector (vx, vy, vz));
*/
  //rankforme=rankforme+1;
  //td::cout<<"rankforme value: "<<rankforme<<std::endl;
  m_helper.Unpause ();
  DoWalk (m_timeStep);
}

void
GaussMarkovMobilityModel::DoWalk (Time delayLeft)
{
 // ====================================bad code 
/*   std::fstream fin;
  char datapathfortest[64];
  std::sprintf(datapathfortest,"/mnt/hgfs/share/erw%d.txt",rankforme-1);
  fin.open(datapathfortest);
  fin.seekg(0, std::ios::end); //将文件指针指向文件末端 
  std::streampos fp = fin.tellg(); //fp为文件指针的偏移量 

  if (int(fp) == 0)
   {fin.close() ;
    std::cout<<"error ";
    NotifyCourseChange ();
  }
    else{
      fin.close();
   // 偏移量为0，证明文件为空，为首次进入系统
  //m_helper.UpdateWithBounds (m_bounds);
  std::cout<<"rankforme"<<rankforme<<std::endl;
  std::fstream infile_position;
  char datapath[64];
  std::sprintf(datapath,"/mnt/hgfs/share/erw%d.txt",rankforme-1);
  infile_position.open(datapath);
  char initialposition[100];
  std::vector<std::string> update_data_s;
  infile_position.getline(initialposition,100,'\n');
  infile_position.close();
  char* token = std::strtok(initialposition," ");
  while( token != NULL ) 
      { 
        printf( "%s \n", token ); 
        update_data_s.insert(update_data_s.end(),token);
        token = strtok( NULL, " "); 
      }
  std::vector<float> update_data;
  for (unsigned int o=0;o<update_data_s.size();o++)
      {
        double resultfor1;
        std::stringstream ss;//用于转化string 到float
        ss<<update_data_s[o];
        ss>>resultfor1;
        update_data.push_back(resultfor1);
      }
  Vector nextPosition  = m_helper.GetCurrentPosition ();
  nextPosition.x=update_data[0];
  nextPosition.y=update_data[1];
  nextPosition.z=update_data[2];
  Vector nextspeed = m_helper.GetVelocity ();
  nextspeed.x=update_data[3];
  nextspeed.y=update_data[4];
  nextspeed.z=update_data[5];
  m_helper.SetPosition (Vector(nextPosition.x, nextPosition.y, nextPosition.z));
  m_helper.SetVelocity (Vector (nextspeed.x,nextspeed.y, nextspeed.z));
  
  NotifyCourseChange ();}*/
if(rankforme+1>node_number)
  rankforme=1;
else
  rankforme=rankforme+1;
//std::cout<<"rankforme"<<rankforme<<std::endl;

/*================5-22,为了同步注释
std::fstream infile_position;
  char datapath[64];
  std::sprintf(datapath,"/mnt/hgfs/share/erw%d.txt",rankforme-1);
  infile_position.open(datapath,std::ios::in);
  char initialposition[100];
  std::vector<std::string> update_data_s;
  infile_position.getline(initialposition,100,'\n');
  if(initialposition[0]=='\0')
    {std::cout<<"fuck its wrong!!let's keep on going!!"<<std::endl;
  infile_position.close();
  Vector nextPosition  = m_helper.GetCurrentPosition ();
  Vector nextspeed = m_helper.GetVelocity ();
  nextPosition.x=nextPosition.x+nextspeed.x*delayLeft.GetSeconds();
  nextPosition.y=nextPosition.y+nextspeed.y*delayLeft.GetSeconds();
  nextPosition.z=nextPosition.z+nextspeed.z*delayLeft.GetSeconds();
  m_helper.SetPosition (Vector(nextPosition.x, nextPosition.y, nextPosition.z));
  m_helper.SetVelocity (Vector (nextspeed.x,nextspeed.y, nextspeed.z));
m_event = Simulator::Schedule (delayLeft, &GaussMarkovMobilityModel::Start, this);
NotifyCourseChange ();
}else{
  char* token = std::strtok(initialposition," ");
  while( token != NULL ) 
      { 
        printf( "%s ", token ); 
        update_data_s.insert(update_data_s.end(),token);
        token = strtok( NULL, " "); 
      }
  std::vector<float> update_data;
  for (unsigned int o=0;o<update_data_s.size();o++)
      {
        double resultfor1;
        std::stringstream ss;//用于转化string 到float
        ss<<update_data_s[o];
        ss>>resultfor1;
        update_data.push_back(resultfor1);
      }
  Vector nextPosition  = m_helper.GetCurrentPosition ();
  nextPosition.x=update_data[0];
  nextPosition.y=update_data[1];
  nextPosition.z=update_data[2];
  Vector nextspeed = m_helper.GetVelocity ();
  nextspeed.x=update_data[3];
  nextspeed.y=update_data[4];
  nextspeed.z=update_data[5];
  m_helper.SetPosition (Vector(nextPosition.x, nextPosition.y, nextPosition.z));
  m_helper.SetVelocity (Vector (nextspeed.x,nextspeed.y, nextspeed.z));
m_event = Simulator::Schedule (delayLeft, &GaussMarkovMobilityModel::Start, this);
NotifyCourseChange ();}*/
std::fstream infile_position;
  char datapath[64];
  std::sprintf(datapath,"/mnt/hgfs/share/erw%d.txt",rankforme-1);
  infile_position.open(datapath);
  char initialposition[100];
  std::vector<std::string> update_data_s;
  infile_position.getline(initialposition,100,'\n');

if(initialposition[0]=='\0')
  {std::cout<<"fuck its wrong!!let's keep on going!!"<<std::endl;
  infile_position.close();
  Vector nextPosition  = m_helper.GetCurrentPosition ();
  Vector nextspeed = m_helper.GetVelocity ();
  nextPosition.x=nextPosition.x+nextspeed.x*delayLeft.GetSeconds();
  nextPosition.y=nextPosition.y+nextspeed.y*delayLeft.GetSeconds();
  nextPosition.z=nextPosition.z+nextspeed.z*delayLeft.GetSeconds();
  m_helper.SetPosition (Vector(nextPosition.x, nextPosition.y, nextPosition.z));
  m_helper.SetVelocity (Vector (nextspeed.x,nextspeed.y, nextspeed.z));
m_event = Simulator::Schedule (delayLeft, &GaussMarkovMobilityModel::Start, this);
NotifyCourseChange ();}

else{
  //std::cout<<"let's keep on going!!"<<std::endl;
  std::vector<std::string> vecContent;
  std::string strLine;
  while (infile_position)
  {
    getline(infile_position, strLine);
    vecContent.push_back(strLine);
  }
    infile_position.close();
   
    std::ofstream outFile2(datapath);
    std::vector<std::string>::const_iterator iter = vecContent.begin();
  for (; vecContent.end() != iter; ++iter)
  {
    outFile2.write((*iter).c_str(), (*iter).size());
    outFile2 << '\n';
  }
  outFile2.close();



  char* token = std::strtok(initialposition," ");
  while( token != NULL ) 
      { 
        //std::cout<<token;
        update_data_s.insert(update_data_s.end(),token);
        token = strtok( NULL, " "); 
      }
  std::vector<float> update_data;
  for (unsigned int o=0;o<update_data_s.size();o++)
      {
        double resultfor1;
        std::stringstream ss;//用于转化string 到float
        ss<<update_data_s[o];
        ss>>resultfor1;
        update_data.push_back(resultfor1);
        //std::cout<<resultfor1<<" ";
      }
  Vector nextPosition  = m_helper.GetCurrentPosition ();
  nextPosition.x=update_data[0];
  nextPosition.y=update_data[1];
  nextPosition.z=update_data[2];
  Vector nextspeed = m_helper.GetVelocity ();
  nextspeed.x=update_data[3];
  nextspeed.y=update_data[4];
  nextspeed.z=update_data[5];

  m_helper.SetPosition (Vector(nextPosition.x, nextPosition.y, nextPosition.z));
  m_helper.SetVelocity (Vector (nextspeed.x,nextspeed.y, nextspeed.z));
  m_event = Simulator::Schedule (delayLeft, &GaussMarkovMobilityModel::Start, this);
NotifyCourseChange ();}
}

//-------------------下面的是已经测试正确的代码。初代版本误差较大。
/*
  Vector position = m_helper.GetCurrentPosition ();
  Vector speed = m_helper.GetVelocity ();
  double r=sqrt(pow(position.x,2)+ pow(position.y,2)+pow(position.z,2));
//this is a and @t
  double j2=(-1.08263/1000);//j2 argument
  double j3=2.540*pow(10,-6);//j3 argument
  double re=6.371*pow(10,6);//re is mean radius of earth
  double miu=39860044*pow(10,7);//miu is the gravity argument
  //std::cout<<"position .x"<<position.x<<"position y "<<position.y<<"position z "<<position.z<<"r:"<<r<<"\n";
  double rou=2.28*pow(10,-112);//rou is air density
  double s_m=0.0054;//s_m is the area toward wind rate weight
  double cd =2.2;//cd is the stoping argument
  double vr=sqrt(speed.x*speed.x+speed.y*speed.y+speed.z*speed.z);//vr is the velocity of one time


  double ax = -miu/pow(10,10)*position.x/pow(r,3)*pow(10,10)*(1+3/2*j2*pow((re/r),2)*(1-5*pow(position.z/r,2))+5/2*j3*pow((re/r),3)*(3*position.z/r-7*pow(position.z/r,3)))-1/2*cd*s_m*rou*vr*speed.x;
  double ay = ax*position.y/position.x-1/2*cd*s_m*rou*vr*speed.y;
  double az = -miu*position.z/pow(r,3)*(1+3/2*j2*pow((re/r),2)*(3-5*pow((position.z/r),2)+5/2*j3*pow((re/r),3)*(6*position.z/r-7*pow(position.z/r,3)-3/5*r/position.z)))-1/2*cd*s_m*rou*vr*speed.z;
  //std::cout<<"ax"<<ax<<"ay "<<ay<<"az "<<az<<"\n";
  Vector nextPosition = position;
  nextPosition.x += (speed.x+1/2*ax*delayLeft.GetSeconds ()) * delayLeft.GetSeconds ();
  //std::cout<<delayLeft.GetSeconds ();
  nextPosition.y += (speed.y+1/2*ay*delayLeft.GetSeconds ())* delayLeft.GetSeconds ();
  nextPosition.z += (speed.z+1/2*az*delayLeft.GetSeconds ()) * delayLeft.GetSeconds ();

  //std::cout<<"speedx"<<speed.x<<" speedy"<<speed.y<< " speedz"<<speed.z<<"\n";
  if (delayLeft.GetSeconds () < 0.0) delayLeft = Seconds (0.0001);

  // Make sure that the position by the next time step is still within the boundary.
  // If out of bounds, then alter the velocity vector and average direction to keep the position in bounds
  if (m_bounds.IsInside (nextPosition))
    {
      m_helper.SetPosition (Vector(nextPosition.x, nextPosition.y, nextPosition.z));
      m_helper.SetVelocity (Vector (speed.x+ax*delayLeft.GetSeconds (),speed.y+ay*delayLeft.GetSeconds (), speed.z+az*delayLeft.GetSeconds ()));
      m_event = Simulator::Schedule (delayLeft, &GaussMarkovMobilityModel::Start, this);
    }
  else
    {
      if (nextPosition.x > m_bounds.xMax || nextPosition.x < m_bounds.xMin)
        {
          speed.x = -speed.x;
          m_meanDirection = M_PI - m_meanDirection;
        }

      if (nextPosition.y > m_bounds.yMax || nextPosition.y < m_bounds.yMin)
        {
          speed.y = -speed.y;
          m_meanDirection = -m_meanDirection;
        }

      if (nextPosition.z > m_bounds.zMax || nextPosition.z < m_bounds.zMin)
        {
          speed.z = -speed.z;
          m_meanPitch = -m_meanPitch;
        }

      m_Direction = m_meanDirection;
      m_Pitch = m_meanPitch;
      m_helper.SetVelocity (Vector (speed.x+ax*delayLeft.GetSeconds (),speed.y+ay*delayLeft.GetSeconds (), speed.z+az*delayLeft.GetSeconds ()));
      m_helper.Unpause ();
      m_event = Simulator::Schedule (delayLeft, &GaussMarkovMobilityModel::Start, this);
    }
  NotifyCourseChange ();
}
*/
//-------------------------------------------------------------截止到这里是上一代代码，下面为新增代码，功能为从stk读取数据之后发送给ns3中的所有节点。--------2019-5-15




void
GaussMarkovMobilityModel::DoDispose (void)
{
  // chain up
  MobilityModel::DoDispose ();
}

Vector
GaussMarkovMobilityModel::DoGetPosition (void) const
{
  //m_helper.Update ();
  return m_helper.GetCurrentPosition ();
}
void
GaussMarkovMobilityModel::DoSetPosition (const Vector &position)
{
  m_helper.SetPosition (position);
  Simulator::Remove (m_event);
  m_event = Simulator::ScheduleNow (&GaussMarkovMobilityModel::Start, this);
}
void
GaussMarkovMobilityModel::DoSetPositionForLMH (const Vector &position)
{
  m_helper.SetPosition (position);
}
void
GaussMarkovMobilityModel::DoSetVelocity(const Vector &vel)
{
  m_helper.SetVelocity (vel);
}
Vector
GaussMarkovMobilityModel::DoGetVelocity (void) const
{
  return m_helper.GetVelocity ();
}

int64_t
GaussMarkovMobilityModel::DoAssignStreams (int64_t stream)
{

  return 6;
}

} // namespace ns3
