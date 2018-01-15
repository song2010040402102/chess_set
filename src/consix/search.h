#ifndef CSEARCH_CONSIX
#define CSEARCH_CONSIX

#include <vector>
#include <string.h>
using namespace std;

#define BLACK 0 
#define WHITE 1 
#define EMPTY 2 

#define ANGLE0 0
#define ANGLE45 1
#define ANGLE90 2
#define ANGLE135 3

#define INCREASE0(x,y){(x)++;}
#define INCREASE45(x,y){(x)++;(y)--;}
#define INCREASE90(x,y){(y)++;}
#define INCREASE135(x,y){(x)++;(y)++;}

#define DECREASE0(x,y){(x)--;}
#define DECREASE45(x,y){(x)--;(y)++;}
#define DECREASE90(x,y){(y)--;}
#define DECREASE135(x,y){(x)--;(y)--;}

#define NEXT(s) (s^1) 
#define WINLOSE 10000 
#define THREAT (WINLOSE-1) 
#define TABLESIZE  1048512 
#define CMSIZE 1000 

typedef unsigned char BYTE;

struct INCREASEXY
{
	int ix;
	int iy;
};


typedef struct Point
{
	BYTE side; 
	short X; 
	short Y; 
	int Value;	
	int label; 
} *PPoint;


typedef struct Step
{ 
	long long HashID; 
	Point First; 
	Point Second; 
	int SValue;  
	int LValue;  
} *PStep;


typedef struct Line 
{	
	BYTE Side;  
	int Value; 
	int ThreatNum; 
	vector<Point> ThreatPoint;  
	vector<Point> BigPotenPoint;  
	vector<Point> SmaPotenPoint;  
	vector<Step> BigPotenStep; 
	vector<Step> SmaPotenStep; 
	vector<Point> ComPoint;  
} *PLine;


struct HashJoseki
{	
	long long checksum[8]; 
	Step step[8];  
};


struct HashHistory
{
	bool pruning; 
	long long checksum; 
	vector<Step> MoveList;	
};


class CSearch
{	
public:	 
	int PointID; 
    int StepID;  

	bool GameStart;  
	bool WillWin; 
	bool ComToCom;

	BYTE ComputerColor; 
	BYTE Board[19][19]; 
	BYTE PreTable[TABLESIZE]; 
	BYTE Player; 
	BYTE MaxDepth[2];  
	BYTE LargeDepth[2];  
	BYTE MaxDepthBackUp; 
	BYTE LargeDepthBackUp; 

	Step HashStep; 

	Line LineInfo[2][92];
	vector<Point>  PointHistory; 
    vector<Step>  StepHistory;	

	long long HashKey64; 
	long long HashBoard[2][19][19]; 
	HashJoseki HashCM[2][CMSIZE]; 

	CSearch();
	~CSearch();		

	void BackMove(Step step, BYTE Side,Line **LineTemp); 
	void MakeMove(Step step,BYTE Side,Line **LineTemp);	 

	void InitGame(); 
	void InitBoard(); 
	void InitHashKey(); 
	void InitLineInfo(); 

	void DeleteHistory(); 

	
	void BackupLine(Point point,Line **LineTemp,BYTE Side,BYTE which=0); 
	void UpdateLineForCross(Point point,BYTE Side,BYTE which=0); 
	void RecoverLine(Point point,Line **LineTemp,BYTE Side,BYTE which=0); 
	void GetLineValueForCross(Point point,Line **line,BYTE Side,BYTE which=0); 
	void UpdateLine(Line **line,Point point,BYTE Side,BYTE which=0); 

	Line AnalyzeLine(Point Start,BYTE Angle,BYTE Side,BYTE which=0); 
	Line GetAllLineInfor(BYTE Side,BYTE which=0); 
	Line ValuateShape(Point CurP,int ShapeIndex,int Angle,BYTE which=0); 
	
	void UpdateBoard(Step step,BYTE Side);
	void RecoverBoard(Step step,BYTE Side,Line **LineTemp);

	void HashImage(int i,int &x,int &y); 

	void ReadCM(BYTE side);	

	void ReleaseHashTable(); 

	void EnsureThreatNum(Line &Si,vector<int> threat); 

	void AdjustStepValue(vector<Step> &MoveList,BYTE Side); 

	void Discount(Line Si1,Line Si2,int &Beta); 

	int SearchRange(); 

	int WinOrLose(BYTE Side); 

	
	int MaxSearch(int Alpha,int Beta,int Depth);
	int MinSearch(int Alpha,int Beta,int Depth);	

	bool SearchCM(BYTE side); 

	bool LookUpHashTable(int Depth); 

	
	bool NeedConSearchForDoubleThreat(Line Si1,Line Si2);
	bool NeedConSearchForSingleThreat(Line Si1,Line Si2);	
	
	
	long long Rand64(int k,int i,int j); 

	Step ComputerMove(); 

	Step ConstructWinStep(vector<Point> point,BYTE Side); 
	Step ConStrangeStep();	

	
	vector<Step> DefendForMultiThreat(Line Si);
	vector<Step> DefendForSingleThreat(Line Si1,Line Si2,bool ConSearch1,bool ConSearch2);
	vector<Step> ConstructStepForNoThreat(Line Si1,Line Si2,bool ConSearch1);
	
	vector<Point> ConstructComPoint(Point point,Line Si1,Line Si2); 

	
	int CalculateStepValue(Step step,BYTE Side); 
	int CalSingleLineValue(Point point ,BYTE Angle,BYTE Side);	
	int AnalyzeLineSegID(Point point,int Dir,BYTE Side); 
};
#endif
