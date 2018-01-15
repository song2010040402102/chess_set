#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <limits.h>
#include "search.h"

#define CURSTEPSIZE 128				
#define SUMSTEPSIZE (CURSTEPSIZE*100)

int LineTypeValue[16]=  {0,0,1,2,3,8,10,15,20,30,34,60,36,64,1000,10000};

INCREASEXY Increasement[4]={{1,0},{1,-1},{0,1},{1,1}};
vector<HashHistory> TempHashTable[3]; 
vector<HashHistory> HashTable[3];	
HashHistory HashList;  

inline bool ComStepValue(const Step &S1,const Step &S2)
{
	return S1.SValue >S2.SValue;
}
inline bool ComSLValue(const Step &S1,const Step &S2)
{		
	return S1.SValue <S2.SValue;
}
inline bool ComPointValue(const Point &P1,const Point &P2)
{		
	return P1.Value >P2.Value;
}
inline bool ComLineValue(const Step &S1,const Step &S2)
{
	return S1.LValue >S2.LValue;
}
inline bool CompPointPosition(const Point &p1, const Point &p2)
{
	if(p1.X>p2.X) 
		return true;
	else if(p1.X==p2.X)
	{
		if(p1.Y>p2.Y) 
			return true;
		else
			return false;
	}
	else
		return false;
}
inline bool CompStepPosition(const Step &s1, const Step &s2)
{	
	return s1.HashID>s2.HashID;
}
inline bool EqualPoint(const Point &P1,const Point &P2)
{
	return (P1.X==P2.X&&P1.Y==P2.Y);	
}
inline bool EqualStep(const Step &S1,const Step &S2)
{
	return S1.HashID==S2.HashID;		
}
inline void IncreaseDir(int &x,int &y,BYTE LineDir)
{
	switch(LineDir)
	{
		case ANGLE0:
			INCREASE0(x,y); break;
		case ANGLE45:
			INCREASE45(x,y); break;
		case ANGLE90:
			INCREASE90(x,y); break;
		case ANGLE135:
			INCREASE135(x,y); break;
	}	
}
inline void DecreaseDir(int &x,int &y,BYTE LineDir)
{
	switch(LineDir)
	{
		case ANGLE0:
			DECREASE0(x,y); break;
		case ANGLE45:
			DECREASE45(x,y); break;
		case ANGLE90:
			DECREASE90(x,y); break;
		case ANGLE135:
			DECREASE135(x,y); break;
	}
}
inline Point GetStartPoint(Point point,BYTE Angle)
{
	int x,y;
	Point Start;
	x=point.X;y=point.Y;
	switch(Angle)
	{
	case 0:
		Start.X=0,Start.Y=y;break;
	case 1:
		if(x+y<=18)
			Start.X=0,Start.Y=y+x;
		else
			Start.X=x+y-18,Start.Y=18;
		break;
	case 2:
		Start.X=x,Start.Y=0;break;
	case 3:
		if(x<=y)
			Start.X=0,Start.Y=y-x;
		else
			Start.X=x-y,Start.Y=0;
		break;
	}
	return Start;
}
void UniquePoint(vector<Point> &PointList)
{
	int size;
	vector<Point>::iterator iter,lab;
	sort(PointList.begin(),PointList.end(),CompPointPosition);
	lab=unique( PointList.begin(), PointList.end(),EqualPoint);
	for( iter=PointList.begin(),size=0; iter!=lab; iter++,size++);
	PointList.resize(size);
}
void UniqueStep(vector<Step> &StepList)	
{
	int size;
	vector<Step>::iterator iter,lab;
	sort(StepList.begin(),StepList.end(),CompStepPosition);	
	lab=unique( StepList.begin(), StepList.end(),EqualStep);
	for( iter=StepList.begin(),size=0; iter!=lab; iter++,size++);
	StepList.resize(size);
}
void LookForBestStep(vector<Step> &MoveList)
{	
	int Value=MoveList[0].LValue;
	vector<Step>StepList;
	vector<Step>::iterator iter;	
	for(iter=MoveList.begin();iter!=MoveList.end();iter++)
	{
		if(iter->LValue==Value)
			StepList.push_back(*iter);		
		else
			break;	
	}
	sort(StepList.begin(),StepList.end(),ComStepValue);
	MoveList.erase(MoveList.begin(),iter);
	MoveList.insert(MoveList.begin(),StepList.begin(),StepList.end());
}

CSearch::CSearch()
{	
	InitGame();
}

CSearch::~CSearch()
{	

}


void CSearch::Discount(Line Si1, Line Si2,int &Beta) 
{
	int i,j,x,y,size;
	int value1,value2,value3,value4;
	Point first,second;
	Line SiCom,SiPer;
	Line **LineTemp[2];
	vector<Point> ThreatPoint,PotenPoint;
	for(i=0;i<2;i++)
	{
		LineTemp[i]=new Line*[2];
		for( j=0;j<2;j++)LineTemp[i][j]=new Line[92];
	}	
	if(Si1.ThreatNum==2)
	{		
		ThreatPoint=Si1.ThreatPoint;
		size=Si1.ThreatPoint.size();
		for(i=0;i<size;i++)
		{
			first=ThreatPoint[i];
			Board[first.X][first.Y]=Si2.Side;

			BackupLine(first,LineTemp[0],Si1.Side,7);
			BackupLine(first,LineTemp[0],Si2.Side,7);

			UpdateLineForCross(first,Si1.Side,7);
			UpdateLineForCross(first,Si2.Side,7);
			for(j=i+1;j<size;j++)
			{				
				second=ThreatPoint[j];				
				Board[second.X][second.Y]=Si2.Side;

				BackupLine(second,LineTemp[1],Si1.Side,7);
				BackupLine(second,LineTemp[1],Si2.Side,7);

				UpdateLineForCross(second,Si1.Side,7);
				UpdateLineForCross(second,Si2.Side,7);
				SiCom=GetAllLineInfor(Si1.Side,7);
				SiPer=GetAllLineInfor(Si2.Side,7);
				if(SiCom.ThreatNum==0)
				{
					value1=(int)((LineTypeValue[11]-LineTypeValue[8])/2);
					value2=(int)((LineTypeValue[9]+LineTypeValue[10]+LineTypeValue[12])/3);
					value3=(int)((LineTypeValue[6]+LineTypeValue[7])/2);
					value4=(int)(value2-value3);
					if(SiPer.ThreatNum>2)
						Beta=-(THREAT-LargeDepth[ComputerColor]);
					else if(SiPer.ThreatNum==2)
						Beta-=(int)(((value1+value4)/2)*0.2);											
					else if(SiPer.ThreatNum==1)
						Beta-=(int)((value4/2)*0.2);					
				}
				Board[second.X][second.Y]=EMPTY;
				RecoverLine(second,LineTemp[1],Si1.Side,7);
				RecoverLine(second,LineTemp[1],Si2.Side,7);
			}
			Board[first.X][first.Y]=EMPTY;
			RecoverLine(first,LineTemp[0],Si1.Side,7);
			RecoverLine(first,LineTemp[0],Si2.Side,7);
		}			
	}
	if(Si1.ThreatNum==1)
	{
		ThreatPoint=Si1.ThreatPoint;
		size=Si1.ThreatPoint.size();
		for(i=0;i<size;i++)
		{
			x=ThreatPoint[i].X;
			y=ThreatPoint[i].Y;
			BackupLine(ThreatPoint[i],LineTemp[0],Si1.Side,2);
			BackupLine(ThreatPoint[i],LineTemp[0],Si2.Side,2);
			
			Board[x][y]=Si2.Side;
			UpdateLineForCross(ThreatPoint[i],Si1.Side,2);
			UpdateLineForCross(ThreatPoint[i],Si2.Side,2);

			SiCom=GetAllLineInfor(Si1.Side,2);
			SiPer=GetAllLineInfor(Si2.Side,2);
			if(SiCom.ThreatNum==0)
			{
				if(SiPer.BigPotenPoint.size()!=0)Beta-=(int)(((LineTypeValue[11]-LineTypeValue[8])/2)*0.2);
			}
			Board[x][y]=EMPTY;
			RecoverLine(ThreatPoint[i],LineTemp[0],Si1.Side,2);
			RecoverLine(ThreatPoint[i],LineTemp[0],Si2.Side,2);
		}
	}
	if(Si1.ThreatNum==0)
	{
		if(Si2.BigPotenPoint.size()!=0)Beta-=(int)((LineTypeValue[11]-LineTypeValue[8])*0.4);		
		if(Si2.BigPotenStep.size()!=0)
		{
			value1=(int)((LineTypeValue[11]-LineTypeValue[5])/2);
			value2=(int)((LineTypeValue[13]-LineTypeValue[7])/2);
			Beta-=(int)(((value1+value2)/2)*0.2);
		}
		if(Si2.SmaPotenPoint.size()>3)
		{
			value1=(int)((LineTypeValue[9]+LineTypeValue[10]+LineTypeValue[12])/3);
			value2=(int)((LineTypeValue[6]+LineTypeValue[7])/2);
			Beta-=(int)((value1-value2)*0.2);
		}
	}
	for(i=0;i<2;i++)
	{
		for(j=0;j<2;j++){Line *a=LineTemp[i][j];delete []a;}
		delete []LineTemp[i];LineTemp[i]=NULL; 
	}
}
int CSearch::MaxSearch(int Alpha,int Beta,int Depth)
{
	int i,k;
	int BestValue,Value;
	vector<int>SValue;
	bool ConSearch1=false;
	bool ConSearch2=false;
	bool pruning=false;	
	Line SiCom,SiPer;
	Line **LineTemp;  
	HashHistory hashhistory;
	vector<Step> MoveList;
	vector<Step>::iterator iter;

    SiCom=GetAllLineInfor(ComputerColor);
    SiPer=GetAllLineInfor(NEXT(ComputerColor));
	
	BestValue=THREAT;
	if(SiPer.ThreatNum>0)return -(THREAT-Depth+1);
	if(SiCom.ThreatNum>2)return (THREAT-Depth+1);	
	if(Depth>=MaxDepth[ComputerColor])
	{				
		ConSearch1=NeedConSearchForDoubleThreat(SiPer,SiCom);
		ConSearch2=NeedConSearchForSingleThreat(SiPer,SiCom);
		if(ConSearch1==true &&Depth<LargeDepth[ComputerColor])ConSearch2=false;	
		else if(ConSearch2==true &&Depth<LargeDepth[ComputerColor]);
		else 
		{
			Beta=SiCom.Value-SiPer.Value;
			if(Depth==LargeDepth[ComputerColor]) Discount(SiCom,SiPer,Beta);							
			return Beta;
		}									
	}

	bool Looked=LookUpHashTable(Depth);
	if(Looked==true)	
		MoveList=HashList.MoveList;		
	else if(SiCom.ThreatNum>=2)	
		MoveList=DefendForMultiThreat(SiCom);  
	else if(SiCom.ThreatNum==1)
		MoveList=DefendForSingleThreat(SiPer,SiCom,ConSearch1,ConSearch2);   
	else 
		MoveList=ConstructStepForNoThreat(SiPer,SiCom,ConSearch1);	

	LineTemp=new Line*[2];
	for( i=0;i<2;i++)LineTemp[i]=new Line[92];

	if(Looked==true &&HashList.pruning==false)
	{
		sort(MoveList.begin(),MoveList.end(),ComSLValue); 
		if(MoveList.size()>15) MoveList.resize(MoveList.size()*0.8);
	}				

	for(iter=MoveList.begin(),i=0;iter!=MoveList.end();iter++,i++) 
	{								
		MakeMove(*iter,NEXT(ComputerColor),LineTemp);

		Value=MinSearch(Alpha,Beta,Depth+1);
		
		iter->LValue=Value;
		SValue.push_back(Value);

		if(Value<Beta)
			Beta=Value;	

		if(Value<BestValue)
			BestValue=Value;

		BackMove(*iter,NEXT(ComputerColor),LineTemp);		

		if(Value<Alpha)
		{	
			pruning=true;
			break; 
		}

		if(Beta<=-(THREAT-LargeDepth[ComputerColor]+1))
		{
			pruning=true;
			break;
		}
	}	
	for(k=0;k<2;k++){Line *a=LineTemp[k];delete []a;}
	delete []LineTemp;LineTemp=NULL; 

	if(pruning==false)	
	{
		for(iter=MoveList.begin(),i=0;iter!=MoveList.end();iter++,i++)
		{
			iter->SValue=SValue[i];
		}		
	}
	if(Depth>=2 &&Depth<=MaxDepth[ComputerColor]-1)
	{
		hashhistory.pruning=pruning; 
		hashhistory.checksum=HashKey64;	
		hashhistory.MoveList=MoveList;  
		TempHashTable[Depth-2].push_back(hashhistory);	
	}

	return BestValue;
	
}

bool CSearch::NeedConSearchForDoubleThreat(Line Si1,Line Si2)
{
	if(Si2.ThreatNum==2)return true;	
	else if(Si1.BigPotenPoint.size()!=0)return true;  
	else if(Si2.ThreatNum==0 && Si1.BigPotenStep.size()!=0)return true; 
	else if(Si2.ThreatNum==0 && Si1.SmaPotenPoint.size()>3)return true; 
	else if(Si2.ThreatNum==1 &&Si1.BigPotenPoint.size()==0)
	{
		Line SiCom,SiPer;
		Line **LineTemp;
		vector<Point>::iterator iter;
		LineTemp=new Line*[2];
		for(int i=0;i<2;i++)LineTemp[i]=new Line[92];
		for(iter=Si2.ThreatPoint.begin();iter!=Si2.ThreatPoint.end();iter++)
		{			
			BackupLine(*iter,LineTemp,Si1.Side,2); 
			BackupLine(*iter,LineTemp,Si2.Side,1); 
			Board[iter->X][iter->Y]=Si1.Side;
 			UpdateLineForCross(*iter,Si1.Side,2);
			UpdateLineForCross(*iter,Si2.Side,1);
			SiCom=GetAllLineInfor(Si1.Side,2);	
			SiPer=GetAllLineInfor(Si2.Side,1);			
			if(SiPer.ThreatNum!=0)
			{
				Board[iter->X][iter->Y]=EMPTY;
				RecoverLine(*iter,LineTemp,Si1.Side,2);
				RecoverLine(*iter,LineTemp,Si2.Side,1);
				continue;
			}
			Board[iter->X][iter->Y]=EMPTY;
			RecoverLine(*iter,LineTemp,Si1.Side,2);
			RecoverLine(*iter,LineTemp,Si2.Side,1);
			if(SiCom.BigPotenPoint.size()!=0)
			{
				for(int k=0;k<2;k++){Line *a=LineTemp[k];delete []a;}
				delete []LineTemp;LineTemp=NULL; 
				return true;
			} 
		}
		for(int k=0;k<2;k++){Line *a=LineTemp[k];delete []a;}
		delete []LineTemp;LineTemp=NULL; 
	}
	return false; 
}
bool CSearch::NeedConSearchForSingleThreat(Line Si1,Line Si2)
{	
	if(Si2.ThreatNum==1 &&Si1.SmaPotenPoint.size()!=0)
		return true;
	else if(Si2.ThreatNum==1 &&Si1.SmaPotenPoint.size()==0) 
	{
		Line SiCom,SiPer;
		Line **LineTemp;
		vector<Point>::iterator iter;
		LineTemp=new Line*[2];
		for(int i=0;i<2;i++)LineTemp[i]=new Line[92];
		for(iter=Si2.ThreatPoint.begin();iter!=Si2.ThreatPoint.end();iter++)
		{			
			BackupLine(*iter,LineTemp,Si1.Side,3);
			BackupLine(*iter,LineTemp,Si2.Side,1);
			Board[iter->X][iter->Y]=Si1.Side;
 			UpdateLineForCross(*iter,Si1.Side,3);
			UpdateLineForCross(*iter,Si2.Side,1);
			SiCom=GetAllLineInfor(Si1.Side,3);
			SiPer=GetAllLineInfor(Si2.Side,1);			
			if(SiPer.ThreatNum!=0)
			{
				Board[iter->X][iter->Y]=EMPTY;
				RecoverLine(*iter,LineTemp,Si1.Side,3);
				RecoverLine(*iter,LineTemp,Si2.Side,1);
				continue;
			}
			Board[iter->X][iter->Y]=EMPTY;
			RecoverLine(*iter,LineTemp,Si1.Side,3);
			RecoverLine(*iter,LineTemp,Si2.Side,1);
			if(SiCom.SmaPotenPoint.size()!=0)
			{
				for(int k=0;k<2;k++){Line *a=LineTemp[k];delete []a;}
				delete []LineTemp;LineTemp=NULL; 
				return true;
			} 
		}
		for(int k=0;k<2;k++){Line *a=LineTemp[k];delete []a;}
		delete []LineTemp;LineTemp=NULL; 
	}		
	return false;
}
int CSearch::MinSearch(int Alpha,int Beta,int Depth)
{	
	int i,k;
	int BestValue,Value;
	vector<int>SValue;
	bool ConSearch1=false;
	bool ConSearch2=false;
	bool pruning=false;	
	Line SiCom,SiPer;
	Line **LineTemp;  

	HashHistory hashhistory;
	vector<Step> MoveList;
	vector<Step>::iterator iter;

	SiCom=GetAllLineInfor(ComputerColor);
    SiPer=GetAllLineInfor(NEXT(ComputerColor));
	
	BestValue=-THREAT;
	if(SiCom.ThreatNum>0)return (THREAT-Depth+1);
	if(SiPer.ThreatNum>2)return -(THREAT-Depth+1);
	if(Depth>=MaxDepth[ComputerColor])
	{		
		ConSearch1=NeedConSearchForDoubleThreat(SiCom,SiPer);
		ConSearch2=NeedConSearchForSingleThreat(SiCom,SiPer);
		if(ConSearch1==true &&Depth<LargeDepth[ComputerColor])ConSearch2=false;	
		else if(ConSearch2==true &&Depth<LargeDepth[ComputerColor]);
		else 			
		{
			Alpha=SiCom.Value-SiPer.Value;
			return Alpha;			
		}
	}		
	bool Looked=LookUpHashTable(Depth);
	if(Looked==true)	
		MoveList=HashList.MoveList;			
	else if(SiPer.ThreatNum>=2)
		MoveList=DefendForMultiThreat(SiPer);  
    else if(SiPer.ThreatNum==1)
		MoveList=DefendForSingleThreat(SiCom,SiPer,ConSearch1,ConSearch2);   
	else 
		MoveList=ConstructStepForNoThreat(SiCom,SiPer,ConSearch1);	
	
	LineTemp=new Line*[2];
	for( i=0;i<2;i++)LineTemp[i]=new Line[92];

	if(Looked==true &&HashList.pruning==false &&MoveList.size()>15)MoveList.resize(MoveList.size()*0.8);

	for(iter=MoveList.begin(),i=0;iter!=MoveList.end();iter++,i++) 
	{				
		MakeMove(*iter,ComputerColor,LineTemp);

		Value=MaxSearch(Alpha,Beta,Depth+1);	
		iter->LValue=Value;
		SValue.push_back(Value);

		if(Value>Alpha)
			Alpha=Value;

		if(Value>BestValue)
			BestValue=Value;

		BackMove(*iter,ComputerColor,LineTemp);		

		if(Value>Beta)
		{	
			pruning=true;
			break; 
		} 
		
		if(Alpha>=THREAT-LargeDepth[ComputerColor]+1)
		{
			pruning=true;
			break; 
		}
	}		
	for(k=0;k<2;k++){Line *a=LineTemp[k];delete []a;}
	delete []LineTemp;LineTemp=NULL; 

	if(pruning==false)	
	{
		for(iter=MoveList.begin(),i=0;iter!=MoveList.end();iter++,i++)
		{
			iter->SValue=SValue[i];
		}
	}
	if(Depth>=2 &&Depth<=MaxDepth[ComputerColor]-1)
	{
		hashhistory.pruning=pruning; 
		hashhistory.checksum=HashKey64;	
		hashhistory.MoveList=MoveList;  
		TempHashTable[Depth-2].push_back(hashhistory);	
	}	
	return BestValue;
	
}

bool CSearch::SearchCM(BYTE side)
{		
	for(int k=0;k<CMSIZE;k++) 
	{
		for(int i=0;i<8;i++)
		{
			if(HashKey64==HashCM[side][k].checksum[i])
			{
				HashStep=HashCM[side][k].step[i];	
				return true;
			}					
		}
	}			   
	return false;
}
void CSearch::ReadCM(BYTE side)
{
	int count=0;
	char str;
	char num[8];
	char filename[50];
	int x,y;
	int x1,y1;
	FILE *F1;
	Point point;
	for(x=0;x<CMSIZE;x++)	
		for(y=0;y<8;y++)
		{			
			HashCM[side][x].checksum[y]=0;
		}
	if(side==BLACK)	
	{
		for(int k=0;k<CMSIZE;k++)
		{
			sprintf(num,"%d", k+1);
			strcpy(filename,"chess_manual_black/");strcat(filename,num);strcat(filename,".sgf");
			if((F1=fopen(filename,"r"))==NULL)continue;	
			while(1)
			{		
				fscanf(F1,"%c,%d,%d\n",&str,&x,&y);
				if(str=='V')break;
				if(str=='J'||x<0)continue;
				for(int i=0;i<8;i++)
				{
					x1=x;y1=y;
					HashImage(i,x,y);	
					if(str=='B')
					{
						HashCM[BLACK][k].checksum[i]^=HashBoard[BLACK][x][y];
					}
					if(str=='W')
					{
						HashCM[BLACK][k].checksum[i]^=HashBoard[WHITE][x][y];
					}
					if(str=='P')	
					{
						if((count/8)%2==0)
						{
							point.X=x;point.Y=y;
							HashCM[BLACK][k].step[i].First=point;
							count++;
						}
						else if((count/8)%2==1)
						{
							point.X=x;point.Y=y;
							HashCM[BLACK][k].step[i].Second=point;	
							count++;
						}
					}
					x=x1;y=y1;
				}		
			}
			fclose(F1);
		}
	}
	if(side==WHITE)	
	{
		for(int k=0;k<CMSIZE;k++)
		{
			sprintf(num, "%d", k);
			strcpy(filename,"chess_manual_white/");strcat(filename,num);strcat(filename,".sgf");
			if((F1=fopen(filename,"r"))==NULL)continue;
			while(1)
			{		
				fscanf(F1,"%c,%d,%d\n",&str,&x,&y);
				if(str=='V')break;
				if(str=='J'||x<0)continue;
				for(int i=0;i<8;i++)
				{
					x1=x;y1=y;
					HashImage(i,x,y);
					if(str=='B')
					{
						HashCM[WHITE][k].checksum[i]^=HashBoard[BLACK][x][y];
					}
					if(str=='W')
					{
						HashCM[WHITE][k].checksum[i]^=HashBoard[WHITE][x][y];
					}
					if(str=='P')
					{
						if((count/8)%2==0)
						{
							point.X=x;point.Y=y;
							HashCM[WHITE][k].step[i].First=point;
							count++;
						}
						else if((count/8)%2==1)
						{
							point.X=x;point.Y=y;
							HashCM[WHITE][k].step[i].Second=point;	
							count++;
						}
					}
					x=x1;y=y1;
				}		
			}
			fclose(F1);
		}
	}
}
void CSearch::HashImage(int i,int &x,int &y)
{
	int t;
	if(i==1){x=18-x;}  
	if(i==2){y=18-y;}  
	if(i==3){x=18-x;y=18-y;}  
	if(i==4){t=x;x=y;y=t;}  
	if(i==5){t=x;x=y;y=18-t;}  
	if(i==6){t=x;x=18-y;y=t;}  
	if(i==7){t=x;x=18-y;y=18-t;}  
}

bool CSearch::LookUpHashTable(int Depth)
{	
	if(ComToCom)return false;
	if(Depth>=3)return false;
	int i=Depth;
	bool Looked=false;
	vector<HashHistory>::iterator iter;	
	if(HashTable[i].size()!=0)
	{				
		for(iter=HashTable[i].begin();iter!=HashTable[i].end();iter++)
		{
			if(HashKey64==iter->checksum)
			{			
				HashList.pruning=iter->pruning; 
				HashList.checksum=iter->checksum; 
				HashList.MoveList=iter->MoveList; 
				Looked=true;	
				break;
			}		
		}
	}	
	return Looked;
}
void CSearch::ReleaseHashTable()
{
	vector<HashHistory>::iterator iter;
	for(int i=0;i<3;i++)
	{
		/*if(HashTable[i].size()!=0)
		{
			for(iter=HashTable[i].begin();iter!=HashTable[i].end();iter++)
			{
				vector<Step>().swap(iter->MoveList);
			}
			vector<HashHistory>().swap(HashTable[i]);
		}*/		
		if(TempHashTable[i].size()!=0)
		{
			HashTable[i]=TempHashTable[i]; 
			for(iter=TempHashTable[i].begin();iter!=TempHashTable[i].end();iter++)
			{
				vector<Step>().swap(iter->MoveList);
			}
			vector<HashHistory>().swap(TempHashTable[i]); 
		}
	}
}

Step CSearch::ConStrangeStep()
{
	int i,j;
	bool Look=false;
	Step step;
	for(i=0;i<19;i++)
	{
		for(j=0;j<19;j++)
		{
			if(Board[i][j]!=EMPTY)
			{
				Look=true;
				break;	
			}
					
		}
		if(Look==true)break;
	}	
	if(i<=9 &&j<=9){step.First.X=i+1;step.First.Y=j;step.Second.X=i;step.Second.Y=j+1;}
	else if(i<=9 &&j>=9){step.First.X=i;step.First.Y=j-1;step.Second.X=i+1;step.Second.Y=j;}
	else if(i>=9 &&j<=9){step.First.X=i-1;step.First.Y=j;step.Second.X=i;step.Second.Y=j+1;}
	else if(i>=9 &&j>=9){step.First.X=i-1;step.First.Y=j;step.Second.X=i;step.Second.Y=j-1;}
	return step;
}

Step CSearch::ComputerMove()
{	
	bool look=false;
	int i,k;	
	int Depth,Alpha,Beta,Value;
	Line SiCom,SiPer;
	Line **LineTemp;  

	vector<Step> MoveList;	
	vector<Step>::iterator iter;	
	SiCom=GetAllLineInfor(ComputerColor);
    SiPer=GetAllLineInfor(NEXT(ComputerColor));	
	
	Alpha=-THREAT;	
	Beta=THREAT;
	Depth=0;
	WillWin=false;	
	
	if(StepID==1 &&Board[9][9]==EMPTY)
	{
		Step step=ConStrangeStep();
		return step;
	}	
	if(SiCom.ThreatNum>0)
		return ConstructWinStep(SiCom.ThreatPoint,ComputerColor);	
/*	look=SearchCM(ComputerColor);
	if(look==true)
		return HashStep;	*/
	if(StepID==2 &&look==false)
		LargeDepth[ComputerColor]=5;		
	ReleaseHashTable();	
	bool Looked=LookUpHashTable(Depth);
	if(Looked==true)	
		MoveList=HashList.MoveList;					
	else if(SiPer.ThreatNum>=2)
		MoveList=DefendForMultiThreat(SiPer);  
    else if(SiPer.ThreatNum==1)		
		MoveList=DefendForSingleThreat(SiCom,SiPer,false,false);   
	else 		
		MoveList=ConstructStepForNoThreat(SiCom,SiPer,false);	
	
	LineTemp=new Line*[2];
	for( i=0;i<2;i++)LineTemp[i]=new Line[92];

	if(Looked==true &&HashList.pruning==false &&MoveList.size()>15)MoveList.resize(MoveList.size()*0.8);

	for(iter=MoveList.begin(),i=0;iter!=MoveList.end();iter++,i++) 
	{
		MakeMove(*iter,ComputerColor,LineTemp);	

		Value=MaxSearch(Alpha,Beta,Depth+1);		
		iter->LValue=Value;				

		if(Value>Alpha)
			Alpha=Value;			

        BackMove(*iter,ComputerColor,LineTemp);	
		
		if(Alpha>=THREAT-LargeDepth[ComputerColor]+1)
		{
			while(++iter!=MoveList.end())	
			{				
				iter->LValue=0;						
			}
			if(LargeDepth[ComputerColor]<=MaxDepth[ComputerColor]){LargeDepth[ComputerColor]-=2;MaxDepth[ComputerColor]-=2;} 
			else LargeDepth[ComputerColor]-=2;	
			WillWin=true; 
			break;
		}	
	}	
	for(k=0;k<2;k++){Line *a=LineTemp[k];delete []a;}
	delete []LineTemp;LineTemp=NULL; 

	if(WillWin==false)
	{
		MaxDepth[ComputerColor]=MaxDepthBackUp;
		LargeDepth[ComputerColor]=LargeDepthBackUp;
	}
	if(StepID==2 &&look==false)
		LargeDepth[ComputerColor]=9;				
    sort(MoveList.begin(),MoveList.end(),ComLineValue);	
	LookForBestStep(MoveList); 
	
	if(MoveList[0].LValue<=-THREAT)MoveList[0].LValue=-WINLOSE;
	return MoveList[0];
}

int CSearch::CalculateStepValue(Step step,BYTE Side)  
{		
	int ComValue=0;int PerValue=0;
	int Sum1,Sum2,Sum;	Sum1=0;Sum2=0;
	Point first,second;
	first=step.First;second=step.Second;
	int x1,y1,x2,y2;
	x1=first.X;y1=first.Y;x2=second.X;y2=second.Y;
	for(int i=0;i<92;i++) 
	{
		if(i==y1||i==x1+19||i==38+x1+y1-5||i==38+27+13+y1-x1)continue;
		if(i==y2||i==x2+19||i==38+x2+y2-5||i==38+27+13+y2-x2)continue;
		ComValue+=LineInfo[Side][i].Value;
		PerValue+=LineInfo[NEXT(Side)][i].Value;
	}
	Board[first.X][first.Y]=Side;
	Board[second.X][second.Y]=Side;	
	for(int k=0;k<4;k++) 
	{
		x1=first.X;y1=first.Y;x2=second.X;y2=second.Y;
		while(x1!=0 &&x1!=18 &&y1!=0 &&y1!=18){DecreaseDir(x1,y1,k);}
		while(x2!=0 &&x2!=18 &&y2!=0 &&y2!=18){DecreaseDir(x2,y2,k);}
		if(x1!=x2 ||y1!=y2)
		{
			Sum1+=CalSingleLineValue(first,k,Side);
			Sum1+=CalSingleLineValue(second,k,Side);
			Sum2+=CalSingleLineValue(first,k,NEXT(Side));
			Sum2+=CalSingleLineValue(second,k,NEXT(Side));
		}
		else
		{
			Sum1+=CalSingleLineValue(first,k,Side);
			Sum2+=CalSingleLineValue(first,k,NEXT(Side));
		}
	}
	Board[first.X][first.Y]=EMPTY;
	Board[second.X][second.Y]=EMPTY;
	Sum=Sum1-Sum2+ComValue-PerValue; 
	return Sum;
}
int CSearch::CalSingleLineValue(Point point,BYTE Angle,BYTE Side) 
{
	int x,y;
	int Len,Style,ShapeIndex,Value;  
	bool OppSide=false;
	Point CurP,Start;
	Len=0;Style=0;ShapeIndex=0;Value=0;
	Start=GetStartPoint(point,Angle);
	CurP.X=Start.X;CurP.Y=Start.Y;   
	x=Start.X;y=Start.Y;     
	while(x!=-1&&x!=19&&y!=-1&&y!=19)
	{
	   if(OppSide==true){CurP.X=x;CurP.Y=y;OppSide=false;}
	   if(Board[x][y]==EMPTY)
	   {
		   Len++;
	   }
	   else if(Board[x][y]==Side)
	   {
		   Style=Style+(1<<Len);
		   Len++;		
	   }
	   else
	   {
		   if(Len>5)
		   {
				ShapeIndex=(1<<Len)+Style-64;
				Value+=LineTypeValue[PreTable[ShapeIndex]];
		   }
		   OppSide=true;
		   Len=0;
		   Style=0;			   
	   }
	   IncreaseDir(x,y,Angle);
	} 
	if(Len>5)
	{
		ShapeIndex=(1<<Len)+Style-64;
		Value+=LineTypeValue[PreTable[ShapeIndex]];
	}
	return Value;
}

vector<Step> CSearch::DefendForMultiThreat(Line Si)
{	
	int i,j,x,y,k,size;
	int ThreatNum=Si.ThreatNum;	
	Point first,second;
	Step step;	

	vector<Step> MoveList;	
	vector<Point> PointList;
	
	PointList=Si.ThreatPoint;	
		
	size = PointList.size();

	for( i=0; i<size; i++)	
	{
		for( j=i+1; j<size; j++)
		{
			bool Defend=true; 
			step.First = PointList[i];
			step.Second = PointList[j];		

			Board[step.First.X][step.First.Y]=NEXT(Si.Side);
			Board[step.Second.X][step.Second.Y]=NEXT(Si.Side); 
			
			for(x=0;x<size;x++)	
			{
				if(x==i||x==j)continue;
				first=PointList[x];
				Board[first.X][first.Y]=Si.Side; 
				for(k=0;k<4;k++)
				{
					if(AnalyzeLineSegID(first,k,Si.Side)==15) 
					{
						Defend=false; 
						break;
					}
				}
				Board[first.X][first.Y]=EMPTY;
				if(Defend==false)break;				
			}
			for(x=0;x<size;x++)	
			{
				if(x==i||x==j)continue;
				first=PointList[x];
				Board[first.X][first.Y]=Si.Side;
				for(y=x+1;y<size;y++)
				{
					if(y==i||y==j)continue;
					second=PointList[y];
					Board[second.X][second.Y]=Si.Side;
					for(k=0;k<4;k++)
					{
						if(AnalyzeLineSegID(first,k,Si.Side)==15||AnalyzeLineSegID(second,k,Si.Side)==15)
						{
							Defend=false;
							break;
						}
					}
					Board[second.X][second.Y]=EMPTY;
					if(Defend==false)break;	
				}
				Board[first.X][first.Y]=EMPTY;
				if(Defend==false)break;	
			}											
			Board[step.First.X][step.First.Y]=EMPTY;
			Board[step.Second.X][step.Second.Y]=EMPTY;			

			if(Defend==false &&ThreatNum<3)	
				continue;	

			step.SValue=CalculateStepValue(step,NEXT(Si.Side));
			step.HashID=HashBoard[NEXT(Si.Side)][step.First.X][step.First.Y]^HashBoard[NEXT(Si.Side)][step.Second.X][step.Second.Y];
			MoveList.push_back(step);			
		}
	}	
	sort(MoveList.begin(), MoveList.end(), ComStepValue);				
	return MoveList;
}
vector<Step> CSearch::DefendForSingleThreat(Line Si1,Line Si2,bool ConSearch1,bool ConSearch2)
{
	Step step;
	Line SiCom,SiPer;
	Line **LineTemp;
	vector<Point>::iterator iterP1,iterP2;
	vector<Step>::iterator iterS;
	vector<Point> PointList,ComList;
	vector<Step> MoveList;	
	
	if(Si2.ThreatPoint.size()>0)
	{
		LineTemp=new Line*[2];
		for(int i=0;i<2;i++)LineTemp[i]=new Line[92];
		for(iterP1=Si2.ThreatPoint.begin();iterP1!=Si2.ThreatPoint.end();iterP1++)
		{			
			BackupLine(*iterP1,LineTemp,Si1.Side);
			BackupLine(*iterP1,LineTemp,Si2.Side);

			Board[iterP1->X][iterP1->Y]=Si1.Side;

			UpdateLineForCross(*iterP1,Si1.Side);
			UpdateLineForCross(*iterP1,Si2.Side);

			SiCom=GetAllLineInfor(Si1.Side);
			SiPer=GetAllLineInfor(Si2.Side);
			
			if(SiPer.ThreatNum>0)	
			{
				Board[iterP1->X][iterP1->Y]=EMPTY;

				RecoverLine(*iterP1,LineTemp,Si1.Side);
				RecoverLine(*iterP1,LineTemp,Si2.Side);
				continue;
			}	
			if(ConSearch1==true)	 
			{
				if(SiCom.BigPotenPoint.size()!=0)	
				{
					for(iterP2=SiCom.BigPotenPoint.begin();iterP2!=SiCom.BigPotenPoint.end();iterP2++)
					{
						step.First=*iterP1;	
						step.Second=*iterP2;
						step.SValue=CalculateStepValue(step,Si1.Side);	
						step.HashID=HashBoard[Si1.Side][step.First.X][step.First.Y]^HashBoard[Si1.Side][step.Second.X][step.Second.Y];
						MoveList.push_back(step);
					}				
				}
				Board[iterP1->X][iterP1->Y]=EMPTY;

				RecoverLine(*iterP1,LineTemp,Si1.Side);
				RecoverLine(*iterP1,LineTemp,Si2.Side);
				continue;
			}
			if(ConSearch2==true)	
			{
				if(SiCom.SmaPotenPoint.size()!=0)
				{
					for(iterP2=SiCom.SmaPotenPoint.begin();iterP2!=SiCom.SmaPotenPoint.end();iterP2++)
					{
						step.First=*iterP1;	
						step.Second=*iterP2;
						step.SValue=CalculateStepValue(step,Si1.Side);	
						step.HashID=HashBoard[Si1.Side][step.First.X][step.First.Y]^HashBoard[Si1.Side][step.Second.X][step.Second.Y];
						MoveList.push_back(step);
					}				
				}
				Board[iterP1->X][iterP1->Y]=EMPTY;

				RecoverLine(*iterP1,LineTemp,Si1.Side);
				RecoverLine(*iterP1,LineTemp,Si2.Side);
				continue;
			}
			if(SiCom.BigPotenPoint.size()!=0)	
			{
				for(iterP2=SiCom.BigPotenPoint.begin();iterP2!=SiCom.BigPotenPoint.end();iterP2++)
				{
					PointList.push_back(*iterP2);
				}				
			}
			if(SiCom.SmaPotenPoint.size()!=0)
			{
				for(iterP2=SiCom.SmaPotenPoint.begin();iterP2!=SiCom.SmaPotenPoint.end();iterP2++)
				{
					PointList.push_back(*iterP2);
				}
			}
			if(SiCom.BigPotenStep.size()!=0)
			{
				for(iterS=SiCom.BigPotenStep.begin();iterS!=SiCom.BigPotenStep.end();iterS++)
				{
					PointList.push_back(iterS->First);
					PointList.push_back(iterS->Second);
				}
			}
			if(SiPer.BigPotenPoint.size()!=0)
			{
				for(iterP2=SiPer.BigPotenPoint.begin();iterP2!=SiPer.BigPotenPoint.end();iterP2++)
				{
					PointList.push_back(*iterP2);
				}
			}
			if(SiPer.BigPotenStep.size()!=0)
			{
				for(iterS=SiPer.BigPotenStep.begin();iterS!=SiPer.BigPotenStep.end();iterS++)
				{
					PointList.push_back(iterS->First);
					PointList.push_back(iterS->Second);
				}
			}
			if(SiPer.SmaPotenPoint.size()!=0)
			{
				for(iterP2=SiPer.SmaPotenPoint.begin();iterP2!=SiPer.SmaPotenPoint.end();iterP2++)
				{
					PointList.push_back(*iterP2);
				}
			}			

			UniquePoint(PointList);

			if(PointList.size()<10) 
			{				
				if(SiCom.SmaPotenStep.size()!=0)
				{
					for(iterS=SiCom.SmaPotenStep.begin();iterS!=SiCom.SmaPotenStep.end();iterS++)
					{
						PointList.push_back(iterS->First);
						PointList.push_back(iterS->Second);
					}
				}
				if(SiCom.ComPoint.size()!=0)
				{
					for(iterP2=SiCom.ComPoint.begin();iterP2!=SiCom.ComPoint.end();iterP2++)
					{
						PointList.push_back(*iterP2);
					}
				}
				if(SiPer.SmaPotenStep.size()!=0)
				{
					for(iterS=SiPer.SmaPotenStep.begin();iterS!=SiPer.SmaPotenStep.end();iterS++)
					{
						PointList.push_back(iterS->First);
						PointList.push_back(iterS->Second);
					}
				}
			}
			Board[iterP1->X][iterP1->Y]=EMPTY;

			RecoverLine(*iterP1,LineTemp,Si1.Side);	 
			RecoverLine(*iterP1,LineTemp,Si2.Side);			

			UniquePoint(PointList);
			for(iterP2=PointList.begin();iterP2!=PointList.end();iterP2++)
			{
				if(iterP1->X==iterP2->X&&iterP1->Y==iterP2->Y)continue; 
				step.First=*iterP1;	
				step.Second=*iterP2;
				step.SValue=CalculateStepValue(step,Si1.Side);	
				step.HashID=HashBoard[Si1.Side][step.First.X][step.First.Y]^HashBoard[Si1.Side][step.Second.X][step.Second.Y];
				MoveList.push_back(step);
			}
			PointList.resize(0);	
		}
		for(int k=0;k<2;k++){Line *a=LineTemp[k];delete []a;}
		delete []LineTemp;LineTemp=NULL; 
	}		
	if(MoveList.size()==0)  
	{	
		for(iterP1=Si2.ThreatPoint.begin();iterP1!=Si2.ThreatPoint.end();iterP1++)
		{
			ComList=ConstructComPoint(*iterP1,Si1,Si2);	
			if(ComList.size()>0)
			{
				for(iterP2=ComList.begin();iterP2!=ComList.end();iterP2++)
				{
					if(iterP1->X==iterP2->X&&iterP1->Y==iterP2->Y)continue;
					step.First=*iterP1;
					step.Second=*iterP2;
					step.SValue=CalculateStepValue(step,Si1.Side);
					step.HashID=HashBoard[Si1.Side][step.First.X][step.First.Y]^HashBoard[Si1.Side][step.Second.X][step.Second.Y];
					MoveList.push_back(step);
				}
			}
		}						
	}				 
	UniqueStep(MoveList);	 
	sort(MoveList.begin(),MoveList.end(),ComStepValue);	
	int size=MoveList.size();

	if(ConSearch1==true) 
	{
		if((int)(size*0.8)>3)MoveList.resize(3);
		else MoveList.resize((int)(size*0.8+1));
		return MoveList;
	}
	if(ConSearch2==true)
	{
		if((int)(size*0.8)>4)MoveList.resize(4);
		else MoveList.resize((int)(size*0.8+1));
		return MoveList;
	}

	if(size>30 &&size<60)MoveList.resize(30); 
	if(size>=60)MoveList.resize(size/2);   

	if(size>=30) AdjustStepValue(MoveList,Si1.Side);	
	sort(MoveList.begin(),MoveList.end(),ComStepValue);	
	if(size>20)MoveList.resize(20);
	return MoveList;
}

vector<Step> CSearch::ConstructStepForNoThreat(Line Si1,Line Si2,bool ConSearch1) 
{		
	Step step;
	Line SiCom,SiPer;
	Line **LineTemp;
	vector<Point>::iterator iterP1,iterP2;
	vector<Step>::iterator iterS;
	vector<Point>PointList1,PointList2;
	vector<Step>StepList;	
	vector<Step>MoveList;	
	if(ConSearch1==true)	
	{
		if(Si1.BigPotenPoint.size()!=0)	
		{
			LineTemp=new Line*[2];
			for(int i=0;i<2;i++)LineTemp[i]=new Line[92];
			for(iterP1=Si1.BigPotenPoint.begin();iterP1!=Si1.BigPotenPoint.end();iterP1++)
			{			 
				BackupLine(*iterP1,LineTemp,Si1.Side); 
				BackupLine(*iterP1,LineTemp,Si2.Side);

				Board[iterP1->X][iterP1->Y]=Si1.Side;

				UpdateLineForCross(*iterP1,Si1.Side);  
				UpdateLineForCross(*iterP1,Si2.Side);

				SiCom=GetAllLineInfor(Si1.Side);	
				SiPer=GetAllLineInfor(Si2.Side);				
				if(SiCom.BigPotenPoint.size()!=0)
				{
					for(iterP2=SiCom.BigPotenPoint.begin();iterP2!=SiCom.BigPotenPoint.end();iterP2++)
					{			 
						PointList2.push_back(*iterP2);
					}
				} 
				if(SiCom.ThreatPoint.size()!=0)	
				{
					for(iterP2=SiCom.ThreatPoint.begin();iterP2!=SiCom.ThreatPoint.end();iterP2++)
					{			 
						PointList2.push_back(*iterP2);
					}
				}
				if(SiCom.SmaPotenPoint.size()!=0)
				{
					for(iterP2=SiCom.SmaPotenPoint.begin();iterP2!=SiCom.SmaPotenPoint.end();iterP2++)
					{
						PointList2.push_back(*iterP2);
					}
				}
				if(SiCom.BigPotenStep.size()!=0)
				{
					for(iterS=SiCom.BigPotenStep.begin();iterS!=SiCom.BigPotenStep.end();iterS++)
					{			 
						PointList2.push_back(iterS->First);
						PointList2.push_back(iterS->Second);
					}
				}
				if(SiPer.BigPotenPoint.size()!=0)
				{
					for(iterP2=SiPer.BigPotenPoint.begin();iterP2!=SiPer.BigPotenPoint.end();iterP2++)
					{
						PointList2.push_back(*iterP2);
					}
				}
				else 
				{
					if(SiCom.ComPoint.size()!=0)
					{
						for(iterP2=SiCom.ComPoint.begin();iterP2!=SiCom.ComPoint.end();iterP2++)
						{
							PointList2.push_back(*iterP2);
						}
					}
					if(SiCom.SmaPotenStep.size()!=0)
					{
						for(iterS=SiCom.SmaPotenStep.begin();iterS!=SiCom.SmaPotenStep.end();iterS++)
						{			 
							PointList2.push_back(iterS->First);
							PointList2.push_back(iterS->Second);
						}
					}															
					if(SiPer.BigPotenStep.size()!=0)
					{
						for(iterS=SiPer.BigPotenStep.begin();iterS!=SiPer.BigPotenStep.end();iterS++)
						{
							PointList2.push_back(iterS->First);
							PointList2.push_back(iterS->Second);
						}
					}
					if(SiPer.SmaPotenPoint.size()!=0)
					{
						for(iterP2=SiPer.SmaPotenPoint.begin();iterP2!=SiPer.SmaPotenPoint.end();iterP2++)
						{
							PointList2.push_back(*iterP2);
						}
					}										
				}
				Board[iterP1->X][iterP1->Y]=EMPTY;

				RecoverLine(*iterP1,LineTemp,Si1.Side);	
				RecoverLine(*iterP1,LineTemp,Si2.Side);
				
				UniquePoint(PointList2); 
				for(iterP2=PointList2.begin();iterP2!=PointList2.end();iterP2++)
				{					
					step.First=*iterP1;	
					step.Second=*iterP2;
					step.SValue=CalculateStepValue(step,Si1.Side);
					step.HashID=HashBoard[Si1.Side][step.First.X][step.First.Y]^HashBoard[Si1.Side][step.Second.X][step.Second.Y];
					MoveList.push_back(step);
				}
				PointList2.resize(0);		
			}
			for(int k=0;k<2;k++){Line *a=LineTemp[k];delete []a;}
			delete []LineTemp;LineTemp=NULL; 
		}
		if(Si1.BigPotenStep.size()!=0)	
		{
			for(iterS=Si1.BigPotenStep.begin();iterS!=Si1.BigPotenStep.end();iterS++)
			{		
				step.First=iterS->First;
				step.Second=iterS->Second;
				step.SValue=CalculateStepValue(step,Si1.Side);
				step.HashID=HashBoard[Si1.Side][step.First.X][step.First.Y]^HashBoard[Si1.Side][step.Second.X][step.Second.Y];
				MoveList.push_back(step);
			}
		}
		if(Si1.SmaPotenPoint.size()>3)	
		{
			int size=Si1.SmaPotenPoint.size();
			for(int i=0;i<size;i++)
				for(int j=i+1;j<size;j++)
				{
					step.First=Si1.SmaPotenPoint[i];
					step.Second=Si1.SmaPotenPoint[j];
					step.SValue=CalculateStepValue(step,Si1.Side);
					step.HashID=HashBoard[Si1.Side][step.First.X][step.First.Y]^HashBoard[Si1.Side][step.Second.X][step.Second.Y];
					MoveList.push_back(step);
				}			
		}
		UniqueStep(MoveList); 
		sort(MoveList.begin(),MoveList.end(),ComStepValue);

		int size=MoveList.size();		
		if((int)(size*0.8)>5)MoveList.resize(5); 
		else MoveList.resize(size*0.8+1);
		return MoveList; 
	}
	
	if(Si1.BigPotenPoint.size()!=0)	
	{
		for(iterP1=Si1.BigPotenPoint.begin();iterP1!=Si1.BigPotenPoint.end();iterP1++)
		{			 
			PointList1.push_back(*iterP1);
		}
	}
	if(Si1.BigPotenStep.size()!=0)
	{
		for(iterS=Si1.BigPotenStep.begin();iterS!=Si1.BigPotenStep.end();iterS++)
		{
			PointList1.push_back(iterS->First);
			PointList1.push_back(iterS->Second);
		}
	}
	if(Si1.SmaPotenPoint.size()!=0)
	{
		for(iterP1=Si1.SmaPotenPoint.begin();iterP1!=Si1.SmaPotenPoint.end();iterP1++)
		{
			PointList1.push_back(*iterP1);
		}
	}			
	if(Si2.BigPotenPoint.size()!=0)
	{
		for(iterP1=Si2.BigPotenPoint.begin();iterP1!=Si2.BigPotenPoint.end();iterP1++)
		{
			PointList1.push_back(*iterP1);
		}
	}
	if(Si2.BigPotenStep.size()!=0)
	{
		for(iterS=Si2.BigPotenStep.begin();iterS!=Si2.BigPotenStep.end();iterS++)
		{
			PointList1.push_back(iterS->First);
			PointList1.push_back(iterS->Second);
		}
	}
	if(Si2.SmaPotenPoint.size()!=0)
	{
		for(iterP1=Si2.SmaPotenPoint.begin();iterP1!=Si2.SmaPotenPoint.end();iterP1++)
		{
			PointList1.push_back(*iterP1);
		}
	}	
	if(PointList1.size()<5)	
	{
		if(Si1.ComPoint.size()!=0)
		{
			for(iterP1=Si1.ComPoint.begin();iterP1!=Si1.ComPoint.end();iterP1++)
			{
				PointList1.push_back(*iterP1);
			}
		}		
	}	
	UniquePoint(PointList1);	
	if(PointList1.size()>0)
	{		
		LineTemp=new Line*[2];
		for(int i=0;i<2;i++)LineTemp[i]=new Line[92];
		for(iterP1=PointList1.begin();iterP1!=PointList1.end();iterP1++)	
		{			
			BackupLine(*iterP1,LineTemp,Si1.Side);
			BackupLine(*iterP1,LineTemp,Si2.Side);

			Board[iterP1->X][iterP1->Y]=Si1.Side;

			UpdateLineForCross(*iterP1,Si1.Side);
			UpdateLineForCross(*iterP1,Si2.Side);

			SiCom=GetAllLineInfor(Si1.Side);
			SiPer=GetAllLineInfor(Si2.Side);
			if(SiCom.ThreatPoint.size()!=0)	
			{
				for(iterP2=SiCom.ThreatPoint.begin();iterP2!=SiCom.ThreatPoint.end();iterP2++)
				{			 
					PointList2.push_back(*iterP2);
				}
			}
			if(SiCom.BigPotenPoint.size()!=0)
			{
				for(iterP2=SiCom.BigPotenPoint.begin();iterP2!=SiCom.BigPotenPoint.end();iterP2++)
				{			 
					PointList2.push_back(*iterP2);
				}
			}
			if(SiCom.SmaPotenPoint.size()!=0)
			{
				for(iterP2=SiCom.SmaPotenPoint.begin();iterP2!=SiCom.SmaPotenPoint.end();iterP2++)
				{
					PointList2.push_back(*iterP2);
				}
			}
			if(SiCom.BigPotenStep.size()!=0)
			{
				for(iterS=SiCom.BigPotenStep.begin();iterS!=SiCom.BigPotenStep.end();iterS++)
				{			 
					PointList2.push_back(iterS->First);
					PointList2.push_back(iterS->Second);
				}
			}			
			
			if(SiPer.BigPotenPoint.size()!=0)
			{
				for(iterP2=SiPer.BigPotenPoint.begin();iterP2!=SiPer.BigPotenPoint.end();iterP2++)
				{
					PointList2.push_back(*iterP2);
				}
			}
			if(SiPer.BigPotenStep.size()!=0)
			{
				for(iterS=SiPer.BigPotenStep.begin();iterS!=SiPer.BigPotenStep.end();iterS++)
				{
					PointList2.push_back(iterS->First);
					PointList2.push_back(iterS->Second);
				}
			}
			if(SiPer.SmaPotenPoint.size()!=0)
			{
				for(iterP2=SiPer.SmaPotenPoint.begin();iterP2!=SiPer.SmaPotenPoint.end();iterP2++)
				{
					PointList2.push_back(*iterP2);
				}
			}
			if(PointList2.size()<20) 
			{
				if(SiCom.ComPoint.size()!=0)
				{
					for(iterP2=SiCom.ComPoint.begin();iterP2!=SiCom.ComPoint.end();iterP2++)
					{
						PointList2.push_back(*iterP2);
					}
				}
				if(SiCom.SmaPotenStep.size()!=0)
				{
					for(iterS=SiCom.SmaPotenStep.begin();iterS!=SiCom.SmaPotenStep.end();iterS++)
					{			 
						PointList2.push_back(iterS->First);
						PointList2.push_back(iterS->Second);
					}
				}				
			}
			Board[iterP1->X][iterP1->Y]=EMPTY;

			RecoverLine(*iterP1,LineTemp,Si1.Side);	
			RecoverLine(*iterP1,LineTemp,Si2.Side);
			
			UniquePoint(PointList2); 
			for(iterP2=PointList2.begin();iterP2!=PointList2.end();iterP2++)
			{
				if(iterP1->X==iterP2->X&&iterP1->Y==iterP2->Y)continue;	
				step.First=*iterP1;	
				step.Second=*iterP2;
				step.SValue=CalculateStepValue(step,Si1.Side);
				step.HashID=HashBoard[Si1.Side][step.First.X][step.First.Y]^HashBoard[Si1.Side][step.Second.X][step.Second.Y];
				MoveList.push_back(step);
			}
			PointList2.resize(0);		
		}
		for(int k=0;k<2;k++){Line *a=LineTemp[k];delete []a;}
		delete []LineTemp;LineTemp=NULL; 
	}
	/*if(Si1.BigPotenStep.size()!=0)	
	{
		for(iterS=Si1.BigPotenStep.begin();iterS!=Si1.BigPotenStep.end();iterS++)
		{		
			step.First=iterS->First;
			step.Second=iterS->Second;
			step.SValue=CalculateStepValue(step,Si1.Side);
			step.HashID=HashBoard[Si1.Side][step.First.X][step.First.Y]^HashBoard[Si1.Side][step.Second.X][step.Second.Y];
			MoveList.push_back(step);
		}
	}*/
	if(Si1.SmaPotenStep.size()!=0)
	{
		for(iterS=Si1.SmaPotenStep.begin();iterS!=Si1.SmaPotenStep.end();iterS++)
		{		
			step.First=iterS->First;
			step.Second=iterS->Second;
			step.SValue=CalculateStepValue(step,Si1.Side);
			step.HashID=HashBoard[Si1.Side][step.First.X][step.First.Y]^HashBoard[Si1.Side][step.Second.X][step.Second.Y];
			MoveList.push_back(step);
		}
	}	
	if(MoveList.size()==0) 
	{
		int r=SearchRange();
		for(int i=9-r;i<=9+r;i++)  
			for(int j=9-r;j<=9+r;j++)
			{
				if(Board[i][j]==EMPTY)
				{
					Point point;
					point.X=i;point.Y=j;
					PointList1=ConstructComPoint(point,Si1,Si2);
					if(PointList1.size()!=0)
					{
						for(iterP1=PointList1.begin();iterP1!=PointList1.end();iterP1++)
						{
							if(iterP1->X==point.X&&iterP1->Y==point.Y)continue;
							step.First=point;
							step.Second=*iterP1;				
							step.SValue=CalculateStepValue(step,Si1.Side);
							step.HashID=HashBoard[Si1.Side][i][j]^HashBoard[Si1.Side][iterP1->X][iterP1->Y];
							MoveList.push_back(step);				
						}
					}
				}
			}
	}		
	UniqueStep(MoveList); 
	sort(MoveList.begin(),MoveList.end(),ComStepValue);	
	int size=MoveList.size();
	if(size>50 &&size<100)MoveList.resize(50); 
	if(size>=100)MoveList.resize(size/2);   

	if(size>=40)AdjustStepValue(MoveList,Si1.Side);

	sort(MoveList.begin(),MoveList.end(),ComStepValue);
	if(size>25)MoveList.resize(25);
	return MoveList;
}


void CSearch::AdjustStepValue(vector<Step> &MoveList,BYTE Side)
{
	int k,ThreatNum,size1,size2;
	Line SiCom,SiPer;
	Line **LineTemp;
	vector<Step>::iterator iter;

	LineTemp=new Line*[2];
	for( k=0;k<2;k++)LineTemp[k]=new Line[92];

	for(iter=MoveList.begin();iter!=MoveList.end();iter++)
	{
		BackupLine(iter->First,LineTemp,Side,2);
		BackupLine(iter->First,LineTemp,NEXT(Side),2);
		BackupLine(iter->Second,LineTemp,Side,2);
		BackupLine(iter->Second,LineTemp,NEXT(Side),2);

		Board[iter->First.X][iter->First.Y]=Side;
		Board[iter->Second.X][iter->Second.Y]=Side;

		UpdateLineForCross(iter->First,Side,2);
		UpdateLineForCross(iter->First,NEXT(Side),2);
		UpdateLineForCross(iter->Second,Side,2);
		UpdateLineForCross(iter->Second,NEXT(Side),2);

		SiCom=GetAllLineInfor(Side,2);
		SiPer=GetAllLineInfor(NEXT(Side),2);

		ThreatNum=SiCom.ThreatNum;
		size1=SiCom.BigPotenPoint.size();
		size2=SiPer.BigPotenPoint.size();
		if(ThreatNum>2)iter->SValue+=LineTypeValue[14];
		else if(ThreatNum==2)
		{
			iter->SValue+=(int)((LineTypeValue[13]*0.3+LineTypeValue[11]*0.7)*0.2);
			iter->SValue-=(int)((LineTypeValue[5]*0.2+LineTypeValue[7]*0.3+LineTypeValue[8]*0.5)*0.2);
			if(size1!=0)iter->SValue+=(int)(LineTypeValue[8]*0.2);
			else if(size1==0 &&size2!=0)iter->SValue-=(int)(LineTypeValue[8]*0.2);
		}
		else if(ThreatNum==1)
		{
			iter->SValue+=(int)((LineTypeValue[9]*0.3+LineTypeValue[10]*0.5+LineTypeValue[12]*0.2)*0.2);
			iter->SValue-=(int)((LineTypeValue[3]*0.2+LineTypeValue[4]*0.2+LineTypeValue[6]*0.3+LineTypeValue[7]*0.3)*0.2);
			if(size1!=0 && size2==0)iter->SValue+=(int)(LineTypeValue[8]*0.2);
			else if(size2!=0)iter->SValue-=(int)(LineTypeValue[8]*0.2);
		}
		else
		{			
			if(size2!=0)iter->SValue-=(int)(LineTypeValue[8]*0.2);	
			else if(size1!=0 &&size2==0)iter->SValue+=(int)(LineTypeValue[8]*0.2);	
		}

		Board[iter->First.X][iter->First.Y]=EMPTY;
		Board[iter->Second.X][iter->Second.Y]=EMPTY;

		RecoverLine(iter->First,LineTemp,Side,2);
		RecoverLine(iter->First,LineTemp,NEXT(Side),2);
		RecoverLine(iter->Second,LineTemp,Side,2);	
		RecoverLine(iter->Second,LineTemp,NEXT(Side),2);
	}

	for(k=0;k<2;k++){Line *a=LineTemp[k];delete []a;}
	delete []LineTemp;LineTemp=NULL;
}

/*long long CSearch::Rand64(void)
{	
	return ((long long)rand()) ^ ((long long)rand() << 15) ^ ((long long)rand() << 30) ^
	((long long)rand() << 45) ^ ((long long)rand() << 60);
   
	
}*/

long long CSearch::Rand64(int k,int i,int j)
{	
	long long hash[5]={0};
	hash[0]=(
		(long long)i ^ (long long)(31-i)<<5 ^ 
		(long long)j<<10 ^ (long long)(31-j)<<15 ^ 
		(long long)(i+j)<<20 ^ (long long)(63-(i+j))<<26 ^ 
		(long long)(i*j)<<32 ^ (long long)(511-i*j)<<41 ^ 
		(long long)(i*i+j*j)<<50 ^ 
		(long long)(i%2?1:0)<<60 ^ 
		(long long)(j%2?1:0)<<61 ^ 
		(long long)((i+j)%2?1:0)<<62 ^ 
		(long long)k<<63
		);
	hash[1]=hash[0]>>(k+i*(k+1)+j*k+5);
	hash[2]=hash[0]<<(k+i*(k+1)+j*k+5);
	hash[3]=hash[0]>>(k+i*k+j*(k+1)+5);
	hash[4]=hash[0]<<(k+i*k+j*(k+1)+5);
	return (hash[0]^hash[1]^hash[2]^hash[3]^hash[4]);
}

void CSearch::InitHashKey()
{	
	srand(0);
	int k,i,j;
	long long *p;
	p=&HashBoard[0][0][0];
	for(k=0;k<2;k++)
		for(i=0;i<19;i++)
			for(j=0;j<19;j++)
			{
				HashBoard[k][i][j]=Rand64(k,i,j);				
			}  
			for(i=0;i<2*19*19;i++)
				for(j=i+1;j<2*19*19;j++)
				{
					if((*(p+i))==(*(p+j)))
						printf("hask key same!\n");
				}
}

void CSearch::InitLineInfo()
{
	for(int i=0;i<2;i++)
		for(int j=0;j<92;j++)
		{
			LineInfo[i][j].Value=0;
			LineInfo[i][j].ThreatNum=0;	
			vector<Point>().swap(LineInfo[i][j].ThreatPoint);
			vector<Point>().swap(LineInfo[i][j].BigPotenPoint);
			vector<Point>().swap(LineInfo[i][j].SmaPotenPoint);
			vector<Step>().swap(LineInfo[i][j].BigPotenStep);
			vector<Step>().swap(LineInfo[i][j].SmaPotenStep);
			vector<Point>().swap(LineInfo[i][j].ComPoint);
		}		
}

int CSearch::WinOrLose(BYTE Side)
{
  int i,j;
  for(i=0;i<19;i++) 
	  for(j=0;j<14;j++)
	  {
		  if(Board[i][j]==Side &&Board[i][j+1]==Side &&Board[i][j+2]==Side &&Board[i][j+3]==Side &&Board[i][j+4]==Side &&Board[i][j+5]==Side )
			  return WINLOSE;
	  }
  for(i=0;i<19;i++) 
	  for(j=0;j<14;j++)
	  {
		  if(Board[j][i]==Side &&Board[j+1][i]==Side &&Board[j+2][i]==Side &&Board[j+3][i]==Side &&Board[j+4][i]==Side &&Board[j+5][i]==Side )
			  return WINLOSE;          
	  }
   for(i=0;i<14;i++) 
	   for(j=5;j<19;j++)
       {
		   if(Board[i][j]==Side &&Board[i+1][j-1]==Side &&Board[i+2][j-2]==Side &&Board[i+3][j-3]==Side &&Board[i+4][j-4]==Side &&Board[i+5][j-5]==Side )
			  return WINLOSE;           
	   }
    for(i=0;i<14;i++) 
	   for(j=13;j>-1;j--)
       {
		   if(Board[i][j]==Side &&Board[i+1][j+1]==Side &&Board[i+2][j+2]==Side &&Board[i+3][j+3]==Side &&Board[i+4][j+4]==Side &&Board[i+5][j+5]==Side )
			  return WINLOSE;          
	   }
	return 0;
       
}

void CSearch::DeleteHistory()
{
	if(PointHistory.size()!=0)	vector<Point>().swap(PointHistory);
	if(StepHistory.size()!=0)	vector<Step>().swap(StepHistory);
	vector<HashHistory>::iterator iter;
	for(int i=0;i<3;i++)
	{		
		if(HashTable[i].size()!=0)
		{			
			for(iter=HashTable[i].begin();iter!=HashTable[i].end();iter++)
			{
				vector<Step>().swap(iter->MoveList);
			}
			vector<HashHistory>().swap(HashTable[i]); 
		}
		if(TempHashTable[i].size()!=0)
		{			
			for(iter=TempHashTable[i].begin();iter!=TempHashTable[i].end();iter++)
			{
				vector<Step>().swap(iter->MoveList);
			}
			vector<HashHistory>().swap(TempHashTable[i]); 
		}
	}
}

void CSearch::InitBoard()
{		
	int i,j;
	for(i=0;i<19;i++)
	{		
		for(j=0;j<19;j++)
		{			
			Board[i][j]=EMPTY;
		}			
	}		
}

void CSearch::InitGame()
{			
    DeleteHistory();	 
	HashKey64=0;		
	Player=BLACK;
	PointID=0;
	StepID=0;
	GameStart=false;
	WillWin=false;
	ComToCom=false;

	FILE *file = fopen("ConnPreTbl.me", "rb");
	if(!file)
	{
		printf("Open ConnPreTbl.me file failed\n");
		return;
	}
	fread(PreTable, 1, sizeof(PreTable), file);
	fclose(file);

	InitBoard();  
	InitHashKey(); 
	InitLineInfo();	
}

void CSearch::UpdateBoard(Step step,BYTE Side)
{
	if(step.Second.X==-1)
	{
		HashKey64=HashKey64^HashBoard[Side][step.First.X][step.First.Y];
	}
	else
	{
		HashKey64=HashKey64^HashBoard[Side][step.First.X][step.First.Y];
		HashKey64=HashKey64^HashBoard[Side][step.Second.X][step.Second.Y];
	}		
	UpdateLineForCross(step.First,Side);
	UpdateLineForCross(step.First,NEXT(Side));
	UpdateLineForCross(step.Second,Side);
	UpdateLineForCross(step.Second,NEXT(Side));
}

void CSearch::UpdateLineForCross(Point point,BYTE Side,BYTE which)
{
	if(point.X==-1)return;
	int i,k;	
	Line **line=NULL;	
	
	line=new Line*[2];	
	for( i=0;i<2;i++)
	{
		line[i]=new Line[92];
	}	
					
	GetLineValueForCross(point,line,Side,which);		

	UpdateLine(line,point,Side,which); 

	for(k=0;k<2;k++)    
	{			
		delete []line[k];
	}
	delete []line;
	line=NULL; 
}

void CSearch::UpdateLine(Line **line,Point point,BYTE Side,BYTE which)
{	
	int x,y;
	bool All=true;
	int LineID;
	x=point.X;y=point.Y;			
	for(int k=0;k<4;k++)
	{			
		switch(k)
		{
		case 0:
			LineID=y;break;
		case 1:
			if(!(x+y>4&&x+y<32))continue;LineID=38+x+y-5;break;
		case 2:
			LineID=x+19;break;
		case 3:
			if(!(y-x>-14&&y-x<14))continue;LineID=38+27+13+y-x;break;
		default:
			break;
		}
		LineInfo[Side][LineID].Value=line[Side][LineID].Value;
		LineInfo[Side][LineID].ThreatNum=line[Side][LineID].ThreatNum;
		switch(which)
		{
		case 0: 
			All=true;
		case 1:
			{
				if(StepID>0){vector<Point>().swap(LineInfo[Side][LineID].ThreatPoint);}
				LineInfo[Side][LineID].ThreatPoint=line[Side][LineID].ThreatPoint;
				if(All==false)break;
			}
		case 2:
			{
				if(StepID>0){vector<Point>().swap(LineInfo[Side][LineID].BigPotenPoint);}
				LineInfo[Side][LineID].BigPotenPoint=line[Side][LineID].BigPotenPoint;
				if(All==false)break;
			}
		case 3:
			{
				if(StepID>0){vector<Point>().swap(LineInfo[Side][LineID].SmaPotenPoint);}
				LineInfo[Side][LineID].SmaPotenPoint=line[Side][LineID].SmaPotenPoint;
				if(All==false)break;
			}
		case 4:
			{
				if(StepID>0){vector<Step>().swap(LineInfo[Side][LineID].BigPotenStep);}
				LineInfo[Side][LineID].BigPotenStep=line[Side][LineID].BigPotenStep;
				if(All==false)break;
			}
		case 5:
			{
				if(StepID>0){vector<Step>().swap(LineInfo[Side][LineID].SmaPotenStep);}
				LineInfo[Side][LineID].SmaPotenStep=line[Side][LineID].SmaPotenStep;
				if(All==false)break;
			}
		case 6:
			{
				if(StepID>0){vector<Point>().swap(LineInfo[Side][LineID].ComPoint);}
				LineInfo[Side][LineID].ComPoint=line[Side][LineID].ComPoint;
				if(All==false)break;
			}
		default:
			break;
		}
	}							
}

Line CSearch::AnalyzeLine(Point Start,BYTE Angle,BYTE Side,BYTE which)
{
   int x,y;
   int Len,Style,ShapeIndex;   
   Len=0;Style=0;ShapeIndex=0;
   bool OppSide=false;
   Point CurP;
   CurP.X=Start.X;CurP.Y=Start.Y;
   Line VL1,VL2;
   VL2.ThreatNum=0;VL2.Value=0;
   vector<Point>::iterator iterP; 
   vector<Step>::iterator iterS; 
   x=Start.X;y=Start.Y;     
   while(x!=-1&&x!=19&&y!=-1&&y!=19)
   {
	   if(OppSide==true){CurP.X=x;CurP.Y=y;OppSide=false;}
	   if(Board[x][y]==EMPTY)
	   {
		   Len++;
	   }
	   else if(Board[x][y]==Side)
	   {
		   Style=Style+(1<<Len);
		   Len++;		
	   }
	   else
	   {
		   if(Len>5)
		   {
				ShapeIndex=(1<<Len)+Style-64;
				VL1=ValuateShape(CurP,ShapeIndex,Angle,which);			   
				if(VL1.Value==WINLOSE)return VL1;
				VL2.Value=VL2.Value+VL1.Value;
				VL2.ThreatNum=VL2.ThreatNum+VL1.ThreatNum;

				if(VL1.ThreatPoint.size()>0)
				   for(iterP=VL1.ThreatPoint.begin();iterP!=VL1.ThreatPoint.end();iterP++)
					   VL2.ThreatPoint.push_back(*iterP);
				if(VL1.BigPotenPoint.size()>0)
				   for(iterP=VL1.BigPotenPoint.begin();iterP!=VL1.BigPotenPoint.end();iterP++)
					   VL2.BigPotenPoint.push_back(*iterP);		
				if(VL1.SmaPotenPoint.size()>0)
				   for(iterP=VL1.SmaPotenPoint.begin();iterP!=VL1.SmaPotenPoint.end();iterP++)
					   VL2.SmaPotenPoint.push_back(*iterP);	
				if(VL1.BigPotenStep.size()>0)
					for(iterS=VL1.BigPotenStep.begin();iterS!=VL1.BigPotenStep.end();iterS++)
					   VL2.BigPotenStep.push_back(*iterS);	
				if(VL1.SmaPotenStep.size()>0)
					for(iterS=VL1.SmaPotenStep.begin();iterS!=VL1.SmaPotenStep.end();iterS++)
					   VL2.SmaPotenStep.push_back(*iterS);
				if(VL1.ComPoint.size()>0)
				   for(iterP=VL1.ComPoint.begin();iterP!=VL1.ComPoint.end();iterP++)
					   VL2.ComPoint.push_back(*iterP);		
		   }
		   OppSide=true;
		   Len=0;
		   Style=0;			   
	   }
       IncreaseDir(x,y,Angle);
   }   
  if(Len>5)
   {
		ShapeIndex=(1<<Len)+Style-64;
		VL1=ValuateShape(CurP,ShapeIndex,Angle,which);			   
		if(VL1.Value==WINLOSE)return VL1;
		VL2.Value=VL2.Value+VL1.Value;
		VL2.ThreatNum=VL2.ThreatNum+VL1.ThreatNum;

		if(VL1.ThreatPoint.size()>0)
		   for(iterP=VL1.ThreatPoint.begin();iterP!=VL1.ThreatPoint.end();iterP++)
			   VL2.ThreatPoint.push_back(*iterP);
		if(VL1.BigPotenPoint.size()>0)
		   for(iterP=VL1.BigPotenPoint.begin();iterP!=VL1.BigPotenPoint.end();iterP++)
			   VL2.BigPotenPoint.push_back(*iterP);		
		if(VL1.SmaPotenPoint.size()>0)
		   for(iterP=VL1.SmaPotenPoint.begin();iterP!=VL1.SmaPotenPoint.end();iterP++)
			   VL2.SmaPotenPoint.push_back(*iterP);	
		if(VL1.BigPotenStep.size()>0)
			for(iterS=VL1.BigPotenStep.begin();iterS!=VL1.BigPotenStep.end();iterS++)
				   VL2.BigPotenStep.push_back(*iterS);
		if(VL1.SmaPotenStep.size()>0)
			for(iterS=VL1.SmaPotenStep.begin();iterS!=VL1.SmaPotenStep.end();iterS++)
			   VL2.SmaPotenStep.push_back(*iterS);
		if(VL1.ComPoint.size()>0)
		   for(iterP=VL1.ComPoint.begin();iterP!=VL1.ComPoint.end();iterP++)
			   VL2.ComPoint.push_back(*iterP);
   }   
   return VL2;
}

Line CSearch::ValuateShape(Point CurP,int ShapeIndex,int Angle,BYTE which) 
{
	int i,j,t;	
	int Len,num;
	int Style=ShapeIndex+64;
	int m=63;
	int ShapeID;
	int First,Second;
	Line VL;
	VL.Value=0;VL.ThreatNum=0;
	Point EP,EP1,EP2;
	Step ES;
	for(i=1;i<32;i++)
		if((Style<<i)<0)break;
	Len=32-i-1;	
	
	ShapeID=PreTable[ShapeIndex];
	VL.Value=LineTypeValue[ShapeID];
	if(ShapeID==9||ShapeID==10||ShapeID==12)VL.ThreatNum=VL.ThreatNum+1;
	if(ShapeID==11||ShapeID==13)VL.ThreatNum=VL.ThreatNum+2;
	if(ShapeID==14)VL.ThreatNum=VL.ThreatNum+3;	

	if(which==7)return VL;

	if(ShapeID>=9 &&(which==1||which==0)) 
	{
		for(i=0;i<Len-5;i++)
		{	
			num=0;
			t=(Style>>i)&m;
			for(j=0;j<6;j++)
			{					
				if(((t>>j)&1)!=0)
                {
					num++;					
				}
			}
			if(num==5)
			{
				for(j=0;j<6;j++)
				{				
					if(((t>>j)&1)==0)
					{						
						EP.X=CurP.X+Increasement[Angle].ix*(i+j);
						EP.Y=CurP.Y+Increasement[Angle].iy*(i+j);						
						VL.ThreatPoint.push_back(EP);
						break;
					}
				}
			}
			if(num==4)
			{
				for(j=0;j<6;j++)
				{				
					if(((t>>j)&1)==0)
					{						
						EP.X=CurP.X+Increasement[Angle].ix*(i+j);
						EP.Y=CurP.Y+Increasement[Angle].iy*(i+j);						
						VL.ThreatPoint.push_back(EP);
					}
				}
			}
		}			
	}	
	else	
	{
		for(i=0;i<Len;i++)
		{				
			if(((Style>>i)&1)!=0)continue;
			First=(1<<i);			
			ShapeID=PreTable[ShapeIndex+First];
			if((ShapeID==11||ShapeID==13) &&(which==2||which==0))  
			{				
				EP1.X=CurP.X+Increasement[Angle].ix*i;
				EP1.Y=CurP.Y+Increasement[Angle].iy*i;				
				VL.BigPotenPoint.push_back(EP1);				
			}
		    if((ShapeID==9||ShapeID==10) &&(which==3||which==0))  
			{
				EP1.X=CurP.X+Increasement[Angle].ix*i;
				EP1.Y=CurP.Y+Increasement[Angle].iy*i;							
				VL.SmaPotenPoint.push_back(EP1);				
			}
			if(ShapeID==5 &&(PreTable[ShapeIndex]==2) &&(which==6||which==0))	
			{
				EP1.X=CurP.X+Increasement[Angle].ix*i;
				EP1.Y=CurP.Y+Increasement[Angle].iy*i;																						
				VL.ComPoint.push_back(EP1);											
			}
			if(which==2||which==3||which==6)continue;
			for(j=i+1;j<Len;j++)
			{					
				if(((Style>>j)&1)!=0)continue;
				Second=1<<j;
				ShapeID=PreTable[ShapeIndex+First+Second];
				if((ShapeID==11||ShapeID==13||ShapeID==14) &&(PreTable[ShapeIndex]==5||PreTable[ShapeIndex]==7)&&(which==4||which==0)) 
				{
					EP1.X=CurP.X+Increasement[Angle].ix*i;
					EP1.Y=CurP.Y+Increasement[Angle].iy*i;							

					EP2.X=CurP.X+Increasement[Angle].ix*j;
					EP2.Y=CurP.Y+Increasement[Angle].iy*j;						

					ES.First=EP1; ES.Second=EP2;						
					VL.BigPotenStep.push_back(ES);
				}
				if((ShapeID==9||ShapeID==10||ShapeID==12) &&(PreTable[ShapeIndex]==3||PreTable[ShapeIndex]==4)&&(which==5||which==0))	
				{
					EP1.X=CurP.X+Increasement[Angle].ix*i;
					EP1.Y=CurP.Y+Increasement[Angle].iy*i;							

					EP2.X=CurP.X+Increasement[Angle].ix*j;
					EP2.Y=CurP.Y+Increasement[Angle].iy*j;						

					ES.First=EP1; ES.Second=EP2;						
					VL.SmaPotenStep.push_back(ES);
				}				
			}			
		}				
	}	
	return VL;
}

Step CSearch::ConstructWinStep(vector<Point> PointList,BYTE Side)
{
	int i,j,k,size;
	Point First,Second;	
	Step step;
	vector<Step> ret;	
	
	size = PointList.size();
	step.First.X=-1;
	for( i=0; i<size; i++)
	{
		for( j=i+1; j<size; j++)
		{
			
			First = PointList[i];
			Second = PointList[j];	
			Board[First.X][First.Y]=Side;
			Board[Second.X][Second.Y]=Side;
			for(k=0;k<4;k++)
			{						
				if(AnalyzeLineSegID(First,k,Side)==15)	
				{
					Board[First.X][First.Y]=EMPTY;
					Board[Second.X][Second.Y]=EMPTY;
					step.First=First;step.Second=Second;
					step.LValue=WINLOSE;
					return step;
				}
				else if(AnalyzeLineSegID(Second,k,Side)==15)
				{
					Board[First.X][First.Y]=EMPTY;
					Board[Second.X][Second.Y]=EMPTY;
					step.First=First;step.Second=Second;
					step.LValue=WINLOSE;
					return step;
				}
			}
		}
	}	
	return step;
}

Line CSearch::GetAllLineInfor(BYTE Side,BYTE which) 
{
	bool All=false;
	Line L1;
	L1.ThreatNum=0;L1.Value=0;
	vector<int> threat;
	vector<Point>::iterator iterp;	
	vector<Step>::iterator iters;		
	L1.Side=Side;
	for(int i=0;i<92;i++)
	{
		L1.Value=L1.Value+LineInfo[Side][i].Value;
		L1.ThreatNum=L1.ThreatNum+LineInfo[Side][i].ThreatNum;	
		switch(which)
		{
		case 0:
			All=true;
		case 1:
			{
				for(iterp=LineInfo[Side][i].ThreatPoint.begin();iterp!=LineInfo[Side][i].ThreatPoint.end();iterp++)
				{
					L1.ThreatPoint.push_back(*iterp);  
					threat.push_back(i);
				}
				if(All==false)break;
			}
		case 2:
			{
				for(iterp=LineInfo[Side][i].BigPotenPoint.begin();iterp!=LineInfo[Side][i].BigPotenPoint.end();iterp++)
				{
					L1.BigPotenPoint.push_back(*iterp);  
				}
				if(All==false)break;
			}
		case 3:
			{
				for(iterp=LineInfo[Side][i].SmaPotenPoint.begin();iterp!=LineInfo[Side][i].SmaPotenPoint.end();iterp++)
				{
					L1.SmaPotenPoint.push_back(*iterp);  
				}
				if(All==false)break;
			}
		case 4:
			{
				for(iters=LineInfo[Side][i].BigPotenStep.begin();iters!=LineInfo[Side][i].BigPotenStep.end();iters++)
				{
					L1.BigPotenStep.push_back(*iters);  
				}
				if(All==false)break;
			}
		case 5:
			{
				for(iters=LineInfo[Side][i].SmaPotenStep.begin();iters!=LineInfo[Side][i].SmaPotenStep.end();iters++)
				{
					L1.SmaPotenStep.push_back(*iters);  
				}
				if(All==false)break;
			}
		case 6:
			{
				for(iterp=LineInfo[Side][i].ComPoint.begin();iterp!=LineInfo[Side][i].ComPoint.end();iterp++)
				{
					L1.ComPoint.push_back(*iterp);  
				}
				if(All==false)break;
			}
		default:
			break;
		}		
	}	
	EnsureThreatNum(L1,threat);	
	switch(which)	
	{
	case 0:
		All=true;
	case 2:
		{
			UniquePoint(L1.BigPotenPoint);
			if(All==false)break;
		}
	case 3:
		{
			UniquePoint(L1.SmaPotenPoint);
			if(All==false)break;
		}
	case 6:
		{
			UniquePoint(L1.ComPoint);
			if(All==false)break;
		}
	default:
		break;
	}
		
    return L1;
}

void CSearch::EnsureThreatNum(Line &Si,vector<int> threat)
{
	int i,j,x,y,k,size;
	bool Defend=true;
	Point first,second,third,fourth;	
	
	UniquePoint(Si.ThreatPoint);	

	if(Si.ThreatNum<2)return;
	bool Angle0=false;	bool Angle45=false;	bool Angle90=false;	bool Angle135=false;
	for(vector<int>::iterator iter=threat.begin();iter!=threat.end();iter++)  
	{		
		if((*iter)<19)Angle0=true;else if((*iter)<38)Angle90=true;
		else if((*iter)<65)Angle45=true;else Angle135=true;				
	}
	if(!((Angle0&&Angle45)||(Angle0&&Angle90)||(Angle0&&Angle135)||(Angle45&&Angle90)||(Angle45&&Angle135)||(Angle90&&Angle135)))return;	

	if(Si.ThreatNum>=2)
	{
		size=Si.ThreatPoint.size();		
		for(i=0;i<size;i++)
		{
			Defend=true;
			first=Si.ThreatPoint[i];
			Board[first.X][first.Y]=NEXT(Si.Side); 

			for(x=0;x<size;x++)	
			{
				if(x==i)continue;
				third=Si.ThreatPoint[x];
				Board[third.X][third.Y]=Si.Side;
				for(k=0;k<4;k++)
				{
					if(AnalyzeLineSegID(third,k,Si.Side)==15)
					{
						Defend=false;
						break;
					}
				}
				Board[third.X][third.Y]=EMPTY;
				if(Defend==false)break;				
			}
			for(x=0;x<size;x++)	
			{
				if(x==i)continue;
				third=Si.ThreatPoint[x];
				Board[third.X][third.Y]=Si.Side;
				for(y=x+1;y<size;y++)
				{
					if(y==i)continue;
					fourth=Si.ThreatPoint[y];
					Board[fourth.X][fourth.Y]=Si.Side;
					for(k=0;k<4;k++)
					{
						if(AnalyzeLineSegID(third,k,Si.Side)==15||AnalyzeLineSegID(fourth,k,Si.Side)==15)
						{
							Defend=false;
							break;
						}
					}
					Board[fourth.X][fourth.Y]=EMPTY;
					if(Defend==false)break;	
				}
				Board[third.X][third.Y]=EMPTY;
				if(Defend==false)break;	
			}
			if(Defend==true)	
			{
				Board[first.X][first.Y]=EMPTY;
				Si.ThreatNum=1;return;
			}
			Board[first.X][first.Y]=EMPTY;
		}

		for(i=0;i<size;i++)
		{			
			first=Si.ThreatPoint[i];
			Board[first.X][first.Y]=NEXT(Si.Side);
			for(j=i+1;j<size;j++)
			{	
				Defend=true;
				second=Si.ThreatPoint[j];				
				Board[second.X][second.Y]=NEXT(Si.Side); 

				for(x=0;x<size;x++)	
				{
					if(x==i||x==j)continue;
					third=Si.ThreatPoint[x];
					Board[third.X][third.Y]=Si.Side;
					for(k=0;k<4;k++)
					{
						if(AnalyzeLineSegID(third,k,Si.Side)==15)
						{
							Defend=false;
							break;
						}
					}
					Board[third.X][third.Y]=EMPTY;
					if(Defend==false)break;				
				}
				for(x=0;x<size;x++)	
				{
					if(x==i||x==j)continue;
					third=Si.ThreatPoint[x];
					Board[third.X][third.Y]=Si.Side;
					for(y=x+1;y<size;y++)
					{
						if(y==i||y==j)continue;
						fourth=Si.ThreatPoint[y];
						Board[fourth.X][fourth.Y]=Si.Side;
						for(k=0;k<4;k++)
						{
							if(AnalyzeLineSegID(third,k,Si.Side)==15||AnalyzeLineSegID(fourth,k,Si.Side)==15)
							{
								Defend=false;
								break;
							}
						}
						Board[fourth.X][fourth.Y]=EMPTY;
						if(Defend==false)break;	
					}
					Board[third.X][third.Y]=EMPTY;
					if(Defend==false)break;	
				}
				Board[second.X][second.Y]=EMPTY;
				if(Defend==true)	
				{
					Board[first.X][first.Y]=EMPTY;
					Si.ThreatNum=2;return;
				}				
			}
			Board[first.X][first.Y]=EMPTY;
		}			
	}
}


void CSearch::GetLineValueForCross(Point point,Line **line, BYTE Side,BYTE which)
{	
	int x=point.X;int y=point.Y;
	Point Start;
	Start.X=0;Start.Y=y;

	line[Side][y]=AnalyzeLine(Start,ANGLE0,Side,which);
	Start.X=x;Start.Y=0;
	line[Side][x+19]=AnalyzeLine(Start,ANGLE90,Side,which);
	while(1)
	{
		if(x==0||y==18)break;
		DecreaseDir(x,y,ANGLE45);
	}
	Start.X=x;Start.Y=y;
	x=point.X;y=point.Y;
	if(x+y>4&&x+y<32)line[Side][38+x+y-5]=AnalyzeLine(Start,ANGLE45,Side,which);
	while(1)
	{
		if(x==0||y==0)break;
		DecreaseDir(x,y,ANGLE135);
	}
	Start.X=x;Start.Y=y;	
	if(y-x>-14&&y-x<14)line[Side][38+27+13+y-x]=AnalyzeLine(Start,ANGLE135,Side,which);
}

void CSearch::BackupLine(Point point,Line **LineTemp,BYTE Side,BYTE which)
{	
	if(point.X==-1)return;
	int x,y;
	int LineID;
	bool All=true;
	x=point.X;y=point.Y;				
	for(int k=0;k<4;k++)
	{
		switch(k)
		{
		case 0:
			LineID=y;break;
		case 1:
			if(!(x+y>4&&x+y<32))continue;LineID=38+x+y-5;break;
		case 2:
			LineID=x+19;break;
		case 3:
			if(!(y-x>-14&&y-x<14))continue;LineID=38+27+13+y-x;break;
		default:
			break;
		}
		LineTemp[Side][LineID].Value=LineInfo[Side][LineID].Value;
		LineTemp[Side][LineID].ThreatNum=LineInfo[Side][LineID].ThreatNum;
		switch(which)
		{
		case 0: 
			All=true;
		case 1:
			{
				vector<Point>().swap(LineTemp[Side][LineID].ThreatPoint);
				LineTemp[Side][LineID].ThreatPoint=LineInfo[Side][LineID].ThreatPoint;
				if(All==false)break;
			}
		case 2:
			{
				vector<Point>().swap(LineTemp[Side][LineID].BigPotenPoint);
				LineTemp[Side][LineID].BigPotenPoint=LineInfo[Side][LineID].BigPotenPoint;
				if(All==false)break;
			}
		case 3:
			{
				vector<Point>().swap(LineTemp[Side][LineID].SmaPotenPoint);
				LineTemp[Side][LineID].SmaPotenPoint=LineInfo[Side][LineID].SmaPotenPoint;
				if(All==false)break;
			}
		case 4:
			{
				vector<Step>().swap(LineTemp[Side][LineID].BigPotenStep);
				LineTemp[Side][LineID].BigPotenStep=LineInfo[Side][LineID].BigPotenStep;
				if(All==false)break;
			}
		case 5:
			{
				vector<Step>().swap(LineTemp[Side][LineID].SmaPotenStep);
				LineTemp[Side][LineID].SmaPotenStep=LineInfo[Side][LineID].SmaPotenStep;
				if(All==false)break;
			}
		case 6:
			{
				vector<Point>().swap(LineTemp[Side][LineID].ComPoint);
				LineTemp[Side][LineID].ComPoint=LineInfo[Side][LineID].ComPoint;
				if(All==false)break;
			}
		default:
			break;
		}
	}									
}

void CSearch::MakeMove(Step step, BYTE Side,Line **LineTemp)
{
	BackupLine(step.First,LineTemp,Side);
	BackupLine(step.First,LineTemp,NEXT(Side));
	BackupLine(step.Second,LineTemp,Side);
	BackupLine(step.Second,LineTemp,NEXT(Side));

	Board[step.First.X][step.First.Y]=Side;
	Board[step.Second.X][step.Second.Y]=Side;	

	UpdateBoard(step,Side);
}

void CSearch::BackMove(Step step, BYTE Side,Line **LineTemp)
{
	Board[step.First.X][step.First.Y]=EMPTY;
	Board[step.Second.X][step.Second.Y]=EMPTY;	

	RecoverBoard(step,Side,LineTemp);	
}

void CSearch::RecoverBoard(Step step, BYTE Side,Line **LineTemp)
{
	if(step.Second.X==-1)
	{
		HashKey64=HashKey64^HashBoard[Side][step.First.X][step.First.Y];
	}
	else
	{
		HashKey64=HashKey64^HashBoard[Side][step.First.X][step.First.Y];
		HashKey64=HashKey64^HashBoard[Side][step.Second.X][step.Second.Y];
	}
	RecoverLine(step.First,LineTemp,Side);
	RecoverLine(step.First,LineTemp,NEXT(Side));
	RecoverLine(step.Second,LineTemp,Side);	
	RecoverLine(step.Second,LineTemp,NEXT(Side));	
}

void CSearch::RecoverLine(Point point,Line **LineTemp,BYTE Side,BYTE which)
{
	if(point.X==-1)return;
	int x,y;
	int LineID;
	bool All=true;
	x=point.X;y=point.Y;				
	for(int k=0;k<4;k++)
	{
		switch(k)
		{
		case 0:
			LineID=y;break;
		case 1:
			if(!(x+y>4&&x+y<32))continue;LineID=38+x+y-5;break;
		case 2:
			LineID=x+19;break;
		case 3:
			if(!(y-x>-14&&y-x<14))continue;LineID=38+27+13+y-x;break;
		default:
			break;
		}			
		LineInfo[Side][LineID].Value=LineTemp[Side][LineID].Value;
		LineInfo[Side][LineID].ThreatNum=LineTemp[Side][LineID].ThreatNum;
		switch(which)
		{
		case 0: 
			All=true;
		case 1:
			{
				vector<Point>().swap(LineInfo[Side][LineID].ThreatPoint);
				LineInfo[Side][LineID].ThreatPoint=LineTemp[Side][LineID].ThreatPoint;
				if(All==false)break;
			}
		case 2:
			{
				vector<Point>().swap(LineInfo[Side][LineID].BigPotenPoint);
				LineInfo[Side][LineID].BigPotenPoint=LineTemp[Side][LineID].BigPotenPoint;
				if(All==false)break;
			}
		case 3:
			{
				vector<Point>().swap(LineInfo[Side][LineID].SmaPotenPoint);
				LineInfo[Side][LineID].SmaPotenPoint=LineTemp[Side][LineID].SmaPotenPoint;
				if(All==false)break;
			}
		case 4:
			{
				vector<Step>().swap(LineInfo[Side][LineID].BigPotenStep);
				LineInfo[Side][LineID].BigPotenStep=LineTemp[Side][LineID].BigPotenStep;
				if(All==false)break;
			}
		case 5:
			{
				vector<Step>().swap(LineInfo[Side][LineID].SmaPotenStep);
				LineInfo[Side][LineID].SmaPotenStep=LineTemp[Side][LineID].SmaPotenStep;
				if(All==false)break;
			}
		case 6:
			{
				vector<Point>().swap(LineInfo[Side][LineID].ComPoint);
				LineInfo[Side][LineID].ComPoint=LineTemp[Side][LineID].ComPoint;
				if(All==false)break;
			}
		default:
			break;
		}
	}									
}

vector<Point> CSearch::ConstructComPoint(Point point,Line Si1,Line Si2) 
{
	int LineID1,LineID2,count;	
	int r;
	Point first,second;
	Step step;
	vector<Point> PointList;	
	first=point;	step.First=first;	
	r=SearchRange();
	for(int i=9-r;i<=9+r;i++)  
		for(int j=9-r;j<=9+r;j++)
		{
			if(Board[i][j]==EMPTY&&(i!=first.X||j!=first.Y))
			{
				second.X=i;
				second.Y=j;
				second.Value=0;
				step.Second=second;
				for(int s=0;s<2;s++)
				{
					count=0;
					Board[first.X][first.Y]=s;
					Board[second.X][second.Y]=s;
					for(int k=0;k<4;k++)
					{
						LineID1=AnalyzeLineSegID(first,k,s);
						LineID2=AnalyzeLineSegID(second,k,s);					
						if(LineID1>4||LineID2>4)
						{
							if(s==Si1.Side)
								second.Value+=CalculateStepValue(step,s); 
								
							if(s==Si2.Side)
								second.Value+=CalculateStepValue(step,s);	
								
							count++;
						}
						if(k==3 &&count>0)
						{
							PointList.push_back(second);		
						}
					}
					Board[first.X][first.Y]=EMPTY;
					Board[second.X][second.Y]=EMPTY;
				}				
			}			
		}			
	sort(PointList.begin(),PointList.end(),ComPointValue);
	if(PointList.size()>5)PointList.resize(5);
	return PointList;
}

int CSearch::AnalyzeLineSegID(Point point,int Dir,BYTE Side)
{
	int x,y;
	int Style,Len;
	int LineIndex;
	x=point.X;y=point.Y;
	Style=0;Len=0;			
	while(1)	
	{		
		if(Board[x][y]==NEXT(Side)){IncreaseDir(x,y,Dir);break;}
		if(x==0||x==18||y==0||y==18)break;
		DecreaseDir(x,y,Dir);
	}	
	while(x!=-1&&x!=19&&y!=-1&&y!=19)	
	{		
		if(Board[x][y]==Side){Style+=(1<<Len);Len++;}
		else if(Board[x][y]==EMPTY)Len++;			
		else break;		
		IncreaseDir(x,y,Dir);
	}
	LineIndex=Style+(1<<Len)-64;
	if(LineIndex>=0)
	{
		return PreTable[LineIndex];	
	}	
	else
	{
		return 0;
	}
}

int CSearch::SearchRange()
{
	int r;
	if(ComputerColor==BLACK)
	{
		if(StepID==2)r=2;
		else if(StepID==4)r=5;
		else r=6;
	}	
	if(ComputerColor==WHITE)
	{
		if(StepID==1)r=2;
		else if(StepID==3)r=5;
		else r=6;
	}
	return r;
}
