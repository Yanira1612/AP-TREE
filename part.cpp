#ifdef PART
template<class kType>
double aptree<kType>::Kpart(const vector<typename map<int, query>::iterator> &Qset, int l, const vector<kType> &V, vector<vector<kType> > &re)
{
    int total=0;
    for(int i=0; i<int(V.size()); ++i)
    {
        total+=frequency.find(V[i])->second;
    }
    if(int(V.size())<=kf)
    {
        for(int i=0; i<int(V.size()); ++i)
        {
            re.push_back(vector<kType>());
            re[i].push_back(V[i]);
        }
        return calCost(re, Qset, total, l);
    }

    int ceil = V.size()-kf;   //����ÿ��cut�а�����key��Ŀ-1�ĺ͵����ޣ���ֹ֮����ֿ�cut)
    int each = total/kf;
    int freq, cFreq=0;

    int i, j;
    int overflow=0;
    //��Ȩֵ����
    j=0;
    freq = frequency.find(V[j])->second;
    cFreq+=freq;
    for(i=0; i<kf; ++i)
    {
        re.push_back(vector<kType>());  //Ϊre[i]����ռ�
        re[i].push_back(V[j++]);
        for(; j<int(V.size()); ++j)
        {
            freq = frequency.find(V[j])->second;    //V[i]��Ƶ��
            cFreq+=freq;
            if(cFreq < each*(i+1))
            {
                if(++overflow <= ceil)
                {
                    re[i].push_back(V[j]);
                }
                else break;
            }
            else break;
        }
    }
    for(; j<int(V.size()); ++j)
    {
        re.back().push_back(V[j]);
    }
    double minCost; //��i-1���i����֧��cost֮����Сֵ
    int bestj;//��¼���ŷ����Ӧ��j
    bool flag;//true:��re[i-1]�ֿ�,false:��re[i]�ֿ�
    double thisCost;
    vector<kType> rei, rei1;
    for(i=1; i<kf; ++i)
    {
        flag = true;
        bestj = -1; //�������
        rei = re[i];
        rei1 = re[i-1];
        minCost = calCost(re, Qset, total, l);
        for(j=re[i-1].size()-1; j>0; --j)    //��re[i-1][j]Ϊǰһ��node�����һ��cut
        {
            re[i].insert(re[i].begin(), re[i-1].back());
            re[i-1].pop_back();
            thisCost = calCost(re, Qset, total, l);
            if(thisCost<minCost){minCost = thisCost; bestj = j;}
        }
        re[i-1] = rei1; //��ԭ
        re[i] = rei; //��ԭ
        for(j=0; j<int(re[i].size()-1); ++j)    //��re[i][j]Ϊǰһ��node�����һ��cut
        {
            re[i-1].push_back(re[i][0]);
            re[i].erase(re[i].begin());
            thisCost = calCost(re, Qset, total, l);
            if(thisCost<minCost){minCost = thisCost; bestj = j; flag = false;}
        }
        re[i-1] = rei1; //��ԭ
        re[i] = rei; //��ԭ
        if(bestj!=-1)
        {
            if(flag)
            {
                for(j=re[i-1].size()-1; j>=bestj; --j)
                {
                    re[i].insert(re[i].begin(), re[i-1].back());
                    re[i-1].pop_back();
                }
            }
            else
            {
                for(j=0; j<=bestj; ++j)
                {
                    re[i-1].push_back(re[i][0]);
                    re[i].erase(re[i].begin());
                }
            }
        }
    }
    return minCost;
}

template<class kType>
double aptree<kType>::Spart(const vector<typename map<int, query>::iterator> &Qset, const space &reg, vector<space> &re)
{
    double dx = (reg.x2-reg.x1)/sfm, dy = (reg.y2-reg.y1)/sfn;
    vector<double> x;
    vector<double> y;
    for(int i=0; i<sfm; ++i)
    {
        x.push_back(reg.x1+dx*i);
    }
    x.push_back(reg.x2);
    for(int i=0; i<sfn; ++i)
    {
        y.push_back(reg.y1+dy*i);
    }
    y.push_back(reg.y2);
    space stmp;
    for(int i=0; i<sfm; ++i)
    {
        for(int j=0; j<sfn; ++j)
        {
            stmp.x1=x[i]; stmp.x2=x[i+1]; stmp.y1=y[j]; stmp.y2=y[j+1];
            re.push_back(stmp);
        }
    }

    double cost = 0;
    for(int i=0; i<int(Qset.size()); ++i)
    {
        for(int j=0; j<int(re.size()); ++j)
        {
            if(overlap(re[j], Qset[i]->second.reg))
            {
                cost+=re[j].area()/reg.area();
            }
        }
    }
    return cost;
}


#endif // PART
