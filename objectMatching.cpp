#ifdef OBJECTMATCHING

template<class kType>
void aptree<kType>::ObjectMatching(const object &o, int ita, node *n, vector<typename map<int, query>::iterator> &R)const
{
    bool flag;
    if(n->property == Q) //q-node
    {
        qNode *qN=reinterpret_cast<qNode*>(n);
        for(int i=0; i<int(qN->q.size()); ++i)
        {
            if(!LinS(o.loc, qN->q[i]->second.reg)) continue; //verifica el espacio
            flag = true;
            for(int j=0; j<int(qN->q[i]->second.key.size()); ++j) //verificar palabra clave
            {
                if(kFind(o.key, qN->q[i]->second.key[j], 0, o.key.size()-1) == -1)
                {flag=false; break;}
            }
            if(!flag) continue;
            R.push_back(qN->q[i]);
        }
        return;
    }
    int clast=-1, ccurrent=-1; // ¿Cuántos cortes coincidió la última vez con esta vez?
    int sIndex;
    if(n->property == K) //k-node
    {
        kNode *kN=reinterpret_cast<kNode*>(n);
        for(int i=ita; i<=int(o.key.size()); ++i){ //Filtrar según palabras clave, 
        //el número de nodos que quedan después de filtrar puede ser mayor que 1
            clast = ccurrent;
            ccurrent = cFind(kN->cut, o.key[i], 0, int(kN->cut.size())-1);
            if(ccurrent==-1) continue;
            if(ccurrent != clast) ObjectMatching(o, i+1, kN->N[ccurrent], R);
        }
        if(kN->N.size()>kN->cut.size()) //Hay un corte ficticio
            ObjectMatching(o, ita, kN->N.back(), R);
    }
    else    //s-node
    {
        sNode *sN=reinterpret_cast<sNode*>(n);
        sIndex = sFind(sN->cell, o.loc);
        if(sIndex!=-1) ObjectMatching(o, ita, sN->N[sIndex], R);
        if(sN->N.size()>sN->cell.size())    //Hay un corte ficticio
            ObjectMatching(o, ita, sN->N.back(), R);
    }
}

#endif // OBJECTMATCHING
