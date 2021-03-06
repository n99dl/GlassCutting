#include <bits/stdc++.h>

using namespace std;

int bigGlassSizeN,bigGlassSizeM;
int n;
pair<int,int> glassPieces[16];
vector< pair<int,int> > possibleCut;
bool selectedPieces[16];
int tempGlassPiecesPlacing[16];
int tempCost[16];
int resultTable[1001][1001];
int tempcut = 0;

class Config
{
public:
    int cost = 0;
    int estimatingValue = 0;
    int remainingSize = 0;
    bool setofSelectedGlass[16]= {0};
    int numberOfSelectedGlass = 0;
    int glassPiecesPlacing[16] = {0};
    bool ori = 0;
    int predecessor = 0;
    int index = 0;
    //int maxsize = 0;
    Config(int _remainingSize, bool _setofSelectedGlass[],int _cost, int _glassPiecesPlacing[],int _predecessor,int _index)
    {
        int remainingPieceArea = 0;
        remainingSize = _remainingSize;
        for (int i=1; i<=n; i++)
        {
            setofSelectedGlass[i] = _setofSelectedGlass[i];
            glassPiecesPlacing[i] = _glassPiecesPlacing[i];
            remainingPieceArea += (setofSelectedGlass[i]) ? 0 : glassPieces[i].first*glassPieces[i].second ;
            //maxsize = (setofSelectedGlass[i]) ? maxsize : max(max(maxsize,glassPieces[i].first),glassPieces[i].second);
            numberOfSelectedGlass += setofSelectedGlass[i];
        }
        index = _index;
        predecessor = _predecessor;
        cost = _cost;
        //estimatingValue = remainingPieceArea % bigGlassSizeM;
        //if (bigGlassSizeM*remainingSize<remainingPieceArea)
        if (remainingPieceArea==0 || remainingSize ==0) estimatingValue = 0;
        else
        {
            estimatingValue = 1e9+1;
            int high = min(bigGlassSizeM,remainingSize);
            int low = remainingPieceArea/ max(bigGlassSizeM,remainingSize) + ((remainingPieceArea% max(bigGlassSizeM,remainingSize))>0);
            for (int i=low; i<=min(high,(int)sqrt(remainingPieceArea)); i++)
            {
                int j = remainingPieceArea / i + ((remainingPieceArea % i)>0);
                estimatingValue = min(estimatingValue,i*j -remainingPieceArea);
            }
        }
    }
};
bool operator < (const Config& a, const Config& b)
{
    return a.cost + a.estimatingValue > b.cost + b.estimatingValue;
}

vector<Config> configList;
priority_queue<Config> gameTree;

void Try(int i,int remainingSizeM,int cut,int remainingSizeN,int predecessor)
{
    if (i>n)
    {
        Config temp(remainingSizeN - cut,selectedPieces,tempCost[n],tempGlassPiecesPlacing,predecessor,configList.size());
        configList.push_back(temp);
        gameTree.push(temp);
        return;
    }
    if (!selectedPieces[i])
    {
        selectedPieces[i]=1;
        if (glassPieces[i].first<=cut && glassPieces[i].second<=remainingSizeM)
        {
            tempGlassPiecesPlacing[i] = 1;
            tempCost[i] = tempCost[i-1] + (cut * glassPieces[i].second - glassPieces[i].first * glassPieces[i].second);
            Try(i+1,remainingSizeM - glassPieces[i].second,cut,remainingSizeN,predecessor);
        }
        if (glassPieces[i].second<=cut && glassPieces[i].first<=remainingSizeM)
        {
            tempGlassPiecesPlacing[i] = 2;
            tempCost[i] = tempCost[i-1] + (cut * glassPieces[i].first - glassPieces[i].first * glassPieces[i].second);
            Try(i+1,remainingSizeM - glassPieces[i].first,cut,remainingSizeN,predecessor);
        }
        selectedPieces[i]=0;
    }
    tempCost[i] = tempCost[i-1];
    tempGlassPiecesPlacing[i] = 0;
    Try(i+1,remainingSizeM,cut,remainingSizeN,predecessor);
}
void Cut(int x,Config base)
{
    tempCost[0]=base.cost;
    for (int i=1; i<=n; i++)
    {
        tempGlassPiecesPlacing[i] = 0;
        selectedPieces[i] = base.setofSelectedGlass[i];
    }
    Try(1,bigGlassSizeM,x,base.remainingSize,base.index);
}
void buildResultTable(Config temp)
{
    if (temp.ori) return;
    buildResultTable(configList[temp.predecessor]);
    int place = 0;
    int cut = 0;
    for (int i=1; i<=n; i++)
        if (temp.glassPiecesPlacing[i] != 0)
        {
            if (temp.glassPiecesPlacing[i] == 2)
                swap(glassPieces[i].first,glassPieces[i].second);
            for (int r=place + 1; r <=place + glassPieces[i].second; r++)
                for (int c=tempcut + 1; c<=tempcut+glassPieces[i].first; c++)
                {
                    resultTable[r][c]=i;
                }
            place+=glassPieces[i].second;
            cut = max(cut,glassPieces[i].first);
        }
    tempcut += cut;
}
int main()
{
    freopen("glasscutting.txt","r",stdin);
    freopen("rs_glasscutting1.txt","w",stdout);
    cin>>bigGlassSizeN>>bigGlassSizeM;
    cin>>n;
    vector< pair<int,int> > uniqueErasing;
    for (int i=1; i<=n; i++)
    {
        cin>>glassPieces[i].first>>glassPieces[i].second;
        selectedPieces[i] = false;
        uniqueErasing.push_back(make_pair(glassPieces[i].first,i));
        uniqueErasing.push_back(make_pair(glassPieces[i].second,i));
    }
    sort(uniqueErasing.begin(),uniqueErasing.end());
    possibleCut.push_back(uniqueErasing[0]);
    for (int i=1; i<uniqueErasing.size(); i++)
    {
        if (uniqueErasing[i].first != possibleCut[possibleCut.size()-1].first)
            possibleCut.push_back(uniqueErasing[i]);
    }
    Config temp(bigGlassSizeN,selectedPieces,0,tempGlassPiecesPlacing,-1,0);
    temp.ori = 1;
    configList.push_back(temp);
    gameTree.push(temp);
    while (gameTree.size())
    {
        Config temp = gameTree.top();
        gameTree.pop();
        if (temp.numberOfSelectedGlass == n)
        {
            cout<<configList.size()<<" "<<temp.cost<<"\n";
            buildResultTable(temp);
            for (int i=1; i<=bigGlassSizeM; i++)
            {
                for (int j=1; j<=bigGlassSizeN; j++)
                {
                    cout<<resultTable[i][j]<<"  ";
                    if (resultTable[i][j]<10) cout<<" ";
                }
                cout<<"\n\n";
            }
            return 0;
        }
        for (int i=0; i<possibleCut.size(); i++)
            if (!temp.setofSelectedGlass[possibleCut[i].second]&&temp.remainingSize - possibleCut[i].first>=0)
            {
                Cut(possibleCut[i].first,temp);
            }
    }
}
