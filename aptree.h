#ifndef APTREE_H_INCLUDED
#define APTREE_H_INCLUDED

#include <vector>
#include <map>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <queue>
#include <assert.h>

using namespace std;

void functionTest();
void timeTest();

template<class kType>
class aptree
{
friend void functionTest();
public:
    struct location
    {
        double x, y;   //Coordenadas Horzontales y Verticales
    };
    struct space
    {
        double x1, x2;  //Rango de abscisas
        double y1, y2;  //Rango de ordenadas y1<y2
        friend ostream &operator<<(ostream &os, const space &o){os<<o.x1<<'\t'<<o.x2<<'\t'<<o.y1<<'\t'<<o.y2; return os;}
        double area()const{return (x2-x1)*(y2-y1);}
    };
    struct object
    {
        vector<kType> key;
        location loc;
        friend ifstream &operator>>(ifstream &fin, object &o)
        {
            o.key.clear();
            fin >> o.loc.x >> o.loc.y;
            kType kt; int kn;
            fin>>kn;
            for(int i=0; i<kn; ++i){fin>>kt; o.key.push_back(kt);}
            return fin;
        }
        friend ostream &operator<<(ostream &os, const object &o)
        {
            os <<"location:"<<'\t'<< o.loc.x << '\t' << o.loc.y << endl;
            os<<"keys:"<<'\t';
            for(int i=0; i<int(o.key.size()); ++i) os<<'\t'<<o.key[i]<<'\t';
            os<<endl;
            return os;
        }
    };
private:
    static const int K=1, S=2, Q=3; //Clase utilizada para representar el Nodo
    static const int Inf=2147483647;

    bool LinS(const location &l, const space &s)const   //Determinar si es Punto 
    //está dentro de espacio S
    {
        if(l.x>=s.x1&&l.x<=s.x2 && l.y<=s.y2&&l.y>=s.y1) return true;
        else return false;
    }
    bool overlap(const space &s1, const space &s2)const //Determine si s1 y s2 
    //tienen intersección
    {
        if(s1.x1>=s2.x2 || s1.x2<=s2.x1) return false;
        if(s1.y1>=s2.y2 || s1.y2<=s2.y1) return false;
        return true;
    }

    bool cover(const space &s1,const space &s2)const //Determine si s1 está 
    //completamente cubierto por s2
    {
        if (s1.x1>=s2.x1&&s1.x2<=s2.x2&&s1.y1>=s2.y1&&s1.y2<=s2.y2) return true;
        else return false;
    }
////////////////////
    struct node
    {
        int property;   //Los valores son K, S y Q,
        // que representan respectivamente el nodo k, el nodo s y el nodo q
    };
    struct kNode:public node
    {
        int offset;                 //El nodo coincide con las primeras palabras clave
        vector<vector<kType> > cut;
        //Cada elemento corresponde a un corte.
        //Cada elemento es un puntero a una lista lineal
        //organizada por todas las palabras clave que le corresponden.
        vector<node*> N;   //Apunte a cada nodo secundario, el procesamiento 
        //de corte ficticio es el mismo que Node
    };
    struct sNode:public node
    {
        space s;                    //espacio asignado actualmente
        vector<space> cell;         //Cada elemento corresponde a una región espacial
        vector<node*> N;
        //Apunte a cada nodo secundario 
        //celda ficticia al final
        //Use N.size()>cell.size() para juzgar si hay una celda ficticia
        //Llamar a una celda ficticia con N.back()
    };
public:
    struct query
    {
        friend ifstream &operator>>(ifstream &fin, query &o)
        {
            o.key.clear();
            fin >> o.reg.x1 >> o.reg.x2 >> o.reg.y1 >> o.reg.y2;
            kType kt; int kn;
            fin>>kn;
            for(int i=0; i<kn; ++i)
            {
                fin>>kt;
                o.key.push_back(kt);
            }
            return fin;
        }
        friend ostream &operator<<(ostream &os, const query &q)
        {
            os <<"space:"<<'\t'<< q.reg.x1 << '\t' << q.reg.x2 << '\t' << q.reg.y1 << '\t' << q.reg.y2 << endl;
            os <<"keys:"<<'\t';
            for(int i=0; i<int(q.key.size()); ++i) os<<q.key[i]<<'\t';
            os<<endl;
            return os;
        }
        friend class aptree;
        vector<kType> key;  //almacenar todas las palabras clave
        space reg;      //Rango de espacio correspondiente
    private:
        vector<node*> N; //Todos los q-nodos que contienen la consulta
    };
private:
    struct qNode:public node
    {
        vector<typename map<int, query>::iterator> q; //ָApunte a cada nodo contenido, 
        //el elemento es un iterador
    };

    int thre;   //El número máximo de consultas contenidas en cada q-nodo
    double KL;  //El límite superior de KL-divergencia de la carga de consultas, 
    //que se realiza como el límite superior del número de registros y cancelaciones
    int kf;     //El número de ramas (corte) correspondiente a cada k-nodo
    int sfm, sfn;     //Cada nodo s divide el área correspondiente 
    //en un área de matriz de sfm*sfn
    int currentId;
    int timesChange;    //Número de Registro + Cancelación //cambios de tiempo

//    int total;          //frecuencia total de la palabra clave
    space SpaceOfTree;
    vector<kType> KeyOfTree;
    map<kType, int> frequency;   //frecuencia de cada palabra clave

    node* root; //raiz
    map<int, query> Qmap;  //Organice todas las consultas en la tabla de 
    //búsqueda dinámica Qmap en STL, y el miembro correspondiente a int
    // puede considerarse como la identificación del usuario

    void ObjectMatching(const object &o, int ita, node *n, vector<typename map<int, query>::iterator> &R)const;
    //n: comienza desde el nodo n 
    //ita: actualmente coincide con la palabra clave ita
    int cFind(const vector<vector<kType> > &key, const kType &k, int s, int e)const;
    //Búsqueda binaria del corte que coincide con k en kN y 
    //devuelve el subíndice correspondiente

    //El subíndice inicial s, el subíndice final e, ambos incluyen

    //no encontrado devuelve -1
    int sFind(const vector<space> &reg, const location &s)const;    //�������
    int sFind(const vector<space> &reg, const location &s, int st, int e)const;
    int kFind(const vector<kType> &key, const kType &k, int s, int e)const;
    void reconstruct();
    void buildIndex(node *&N, const vector<typename map<int, query>::iterator> Qset,
                     int l, const space &s, bool kP, bool sP);
    //N: nodo actual
    //Q: conjunto de consultas
    //l: qué palabra clave coincide actualmente, 
    //kP: se puede seguir dividiendo por palabra clave
    //sP: se puede seguir dividiendo por espacio
    double Kpart(const vector<typename map<int, query>::iterator> &Qset,
               int l, const vector<kType> &V, vector<vector<kType> > &re);
    //Devuelve la unidad de consumo de tiempo correspondiente 
    //al esquema de división de palabras clave

    //Parámetro entrante Qset: conjunto de consultas a dividir

    //Pase el parámetro re: el resultado de la división se almacena en re, 
    //y cada elemento de re representa un conjunto de palabras clave

    //Cada clave de rama correspondiente a cada kNode se ordena (incrementando)
    //l: Para dividir la l-ésima palabra clave de la consulta

    double Spart(const vector<typename map<int, query>::iterator> &Qset,
               const space &reg, vector<space> &re);
    //Devuelve la unidad de consumo de tiempo 
    //correspondiente al esquema de división espacial

    //Parámetro entrante Qset: conjunto de consultas a dividir
    //Pasa el parámetro re: el resultado de la división se almacena en re,
    // y cada elemento de re representa un área

    //Forma de división: matriz de sfm*sfn

     //Las ramas correspondientes a cada sNode están en orden (la abscisa 
     //es creciente y la ordenada correspondiente a cada abscisa es creciente)
    //Si se divide en 2*2 áreas: [(1,2)(1,2)] [(1,2)(2,3)] [(2,3)(1,2)] [(2, 3 )(2,3)]
    double calCost(const vector<vector<kType> > &re,
                   const vector<typename map<int, query>::iterator> &Qset,
                   int total, int l)const;
    void Clear()
    {
        frequency.clear();
        KeyOfTree.clear();
        for(typename map<int, query>::iterator pos = Qmap.begin(); pos!=Qmap.end(); ++pos)
        {
            pos->second.N.clear();
        }
        Clear(root);
    }
    void Clear(node *n);
    void regis(query &q,node *N);
//    void deregis(query &q,node *N)
public:
    int ObjectMatching(const object &o, vector<typename map<int, query>::iterator> &R)const
    {ObjectMatching(o, 0, root, R); return int(R.size());}
    //Devolver el número de consultas coincidentes
    //La dirección de la consulta coincidente se almacena en el parámetro R entrante
    //o: objeto con coincidencia
    aptree(int thre0, double KL0, int kf0, int sfm0, int sfn0)
        :thre(thre0), KL(KL0), kf(kf0), sfm(sfm0), sfn(sfn0), currentId(0), timesChange(0)
    {
        assert(kf0>1);
        assert(thre0>0);
        assert(KL0>0);
    }
    ~aptree();
    void buildIndex(const vector<query> &Q);
    int regis(const query &q);     //registrar usuario q
    void deregis(int ID);           //cerrar sesión usuario q cuyo id es ID
    void print(const char name[])const;    //para depurar
};

template<class kType>
aptree<kType>::~aptree()
{
    Clear(reinterpret_cast<node*>(root));
}

#define BUILDINDEX
#define OBJECTMATCHING
#define PRINT
#define TOOL
#define PART
#define REGIS
#include "buildIndex.cpp"
#include "objectMatching.cpp" //
#include "print.cpp"//
#include "tool.cpp"
#include "part.cpp"//
#include "regis.cpp" //

#endif // APTREE_H_INCLUDED
