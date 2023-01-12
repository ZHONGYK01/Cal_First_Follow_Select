#include<cstring>
#include<cstdio>
#include<iostream>
#include<vector>
#include<map>
#include<set>
#include<stdlib.h>
const int maxn=20;
//std::vector<std::string> Vt;//终结符
//std::vector<std::string> Vn;//非终结符
std::set<std::string> SetVt;
std::set<std::string> SetVn;
//--------------------------------------------------------------------初始化表达式
class production
{
    public:
        production(std::string l,std::vector<std::string> r);
        void PrintProd();
        char getleft();//返回left
        char getright(int k);//返回right中的第k个
        int getrightlen();//返回right中vector的大小
        std::vector<char> SELECT;
        void AddFollow2Select(char s);//空产生式左部的FOLLOW集加入SELECT
        void AddFirst2Select(char s);//非空产生式右部第一个非终结符的FIRST集加入SELECT
        void AddVt2Select(char s);//非空产生式右部第一个终结符加入SELECT
    private:
        std::string left;
        std::vector<std::string> right;
        
};//class production

production::production(std::string l,std::vector<std::string> r)
{
    left=l;
    right=r;
}//production构造函数
void production::PrintProd()//production类的输出函数
{
    std::cout<<left<<"->";
    for(int i=0;i<(int)right.size();i++)
    {
        std::cout<<right[i];
    }
    std::cout<<std::endl;
}

char production::getleft()
{
    return left[0];
}
int production::getrightlen()
{
    return (int)right.size();
}
char production::getright(int k)
{
    return right[k][0];
}

std::vector<production> prod;//新建production类的vector

void checkSetVt(std::string str)//判断Vt是否在集合中
{
    if(SetVt.find(str)==SetVt.end())//Vt不在集合中
    {
        SetVt.insert(str);
    }
}

void checkSetVn(std::string str)//判断Vn是否在集合中
{
    if(SetVn.find(str)==SetVn.end())//Vn不在集合中
    {
        SetVn.insert(str);
    }
}


char sym[]={'+','-','*','/','!','?',',','.','(',')','[',']','{','}','^','#'};//终结符符号部分 ^是空; #是结束标记
int symnum=16;
bool checksym(char s)//判断是否为符号
{
    for(int i=0;i<symnum;i++)
    {
        if(s==sym[i])
        {
            std::string s1;
            checkSetVt(s1+s);
            return true;
        }
    }
    return false;
}

// std::string keywd[]={"id"};//终结符关键字部分
// int keywdnum=1;
// bool checkkeywd(std::string s)//判断是否为关键字
// {
//     for(int i=0;i<keywdnum;i++)
//     {
//         if(s==keywd[i])
//         {
//             checkSetVt(s);
//             return true;
//         }
//     }
//     return false;
// }

void Str2Prod(std::string lft,std::string rht);//预先声明
void FirstExe(std::string str)//预处理，生成产生式左部和右部
{
    std::string lft,rht;
    lft=str[0];//左部一定是首字母
    rht=str.substr(3);//左部+“->”之后的部分是右部
    Str2Prod(lft,rht);
}


void Str2Prod(std::string lft,std::string rht)//产生式左部和右部（经过预处理后）
{
    checkSetVn(lft);
    bool ExistOr=0;
    std::string s1;//空字符串
    std::vector<std::string> final;
    final.clear();
    for(int i=0;i<(int)rht.size();i++)
    {
        if(rht[i]=='|')
        {
            std::string newrht=rht.substr(i+1);
            ExistOr=1;
            production p1(s1+lft,final);
            prod.push_back(p1);//写入prod
            Str2Prod(lft,newrht);
            break;
        }
        if(rht[i]>='A' && rht[i]<='Z')//是非终结符
        {
            checkSetVn(s1+rht[i]);
            final.push_back(s1+rht[i]);
            continue;
        }
        if(checksym(rht[i]))//是否为符号
        {
            final.push_back(s1+rht[i]);
            continue;
        }
        if(rht[i]>='a' && rht[i]<='z')//是小写字母终结符(本代码未考虑关键字，认为小写字母均单独存在)
        {
            checkSetVt(s1+rht[i]);
            final.push_back(s1+rht[i]);
            continue;           
        }
    }//for
    if(!ExistOr)
    {
        production p1(s1+lft,final);
        prod.push_back(p1);//写入prod
    }

}
//----------------------------------------------------------------求FIRST集
class FIRST//First类
{
    public:
        void SetVn(std::string s);//插入Vn
        void SetVt(char s);//在Vt的set中插入Vt
        void PrintFirst();//打印
        bool FindEmpty();//查找Vt中是否有^
        char Vn;//Vn的First集
        std::set<char> Vt; //Vn的First集中元素
};
std::vector<FIRST> FirstSet;//First集

void FIRST::SetVn(std::string s)
{
    Vn=s[0];
}
void FIRST::SetVt(char s)
{
    Vt.insert(s);
}

void FIRST::PrintFirst()//打印FIRST集
{
    printf("FIRST( %c )={",Vn);
    for(std::set<char>::iterator it=Vt.begin();it!=Vt.end();it++)
    {
        printf(" %c ",*it);
    }
    printf("}\n");

}

bool FIRST::FindEmpty()
{
    for(std::set<char>::iterator it=Vt.begin();it!=Vt.end();it++)
    {
        if(*it=='^') return true;
    }
    return false;
}


int FindFirst(char S)//找到非终结符S所在FirstSet的下标
{
    int i;
    for(i=0;i<(int)FirstSet.size();i++)
    {
        if(FirstSet[i].Vn==S)
        {
            return i;
        }
    }
    return -1;
}
void CopyFirst(char D,char S)//将非终结符S的FIRST集 复制到 非终结符D的FIRST集
{
    int i,j;//i,j分别代表D和S所在FirstSet的下标
    i=FindFirst(D);
    j=FindFirst(S);
    for(std::set<char>::iterator it=FirstSet[j].Vt.begin();it!=FirstSet[j].Vt.end();it++)
    {
        FirstSet[i].SetVt(*it);
    }

}
void CalFirstSet()//求FIRST集
{
    for(int i=0;i<(int)prod.size();i++)//多次循环 确保所有FIRST集中的更新都完成
    {
        for(int j=0;j<(int)prod.size();j++)
        {
            int top=0;
            for(int t=0;t<=top;t++)
            {
                if(top>=prod[j].getrightlen()) break;
                char ThisVn=prod[j].getleft();//表达式左部
                char ThisRight=prod[j].getright(top);//表达式右部第top个字符
                if(ThisRight>='A' && ThisRight<='Z')//表达式右部第一个字符是非终结符
                {
                    CopyFirst(ThisVn,ThisRight);
                    int k=FindFirst(ThisRight);
                    if(FirstSet[k].FindEmpty())
                    {
                        top++;
                    }
                }
                else//表达式右部第一个字符是终结符
                {
                    int k=FindFirst(ThisVn);
                    FirstSet[k].SetVt(ThisRight);
                }
            }
    
        }
    }
} 
//----------------------------------------------------------------求FOLLOW集
class FOLLOW : public FIRST//FOLLOW类
{
    public:
        void PrintFollow();
};
std::vector<FOLLOW> FollowSet;//Follow集

void FOLLOW::PrintFollow()//打印FIRST集
{
    printf("FOLLOW( %c )={",Vn);
    for(std::set<char>::iterator it=Vt.begin();it!=Vt.end();it++)
    {
        printf(" %c ",*it);
    }
    printf("}\n");

}

int FindFollow(char S)//找到非终结符S所在FollowSet的下标
{
    int i;
    for(i=0;i<(int)FollowSet.size();i++)
    {
        if(FollowSet[i].Vn==S)
        {
            return i;
        }
    }
    return -1;
}

void CopyFollow2Follow(char D,char S)//将非终结符S的FOLLOW集 复制到 非终结符D的FOLLOW集
{
    int i,j;//i,j分别代表D和S所在FollowSet的下标
    i=FindFollow(D);
    j=FindFollow(S);
    for(std::set<char>::iterator it=FollowSet[j].Vt.begin();it!=FollowSet[j].Vt.end();it++)
    {
        FollowSet[i].SetVt(*it);
    }

}
void CopyFirst2Follow(char D,char S)//将非终结符S的First集 复制到 非终结符D的FOLLOW集
{
    int i,j;//i,j分别代表D和S所在FollowSet的下标
    i=FindFollow(D);
    j=FindFollow(S);
    for(std::set<char>::iterator it=FirstSet[j].Vt.begin();it!=FirstSet[j].Vt.end();it++)
    {
        if(*it!='^')
        FollowSet[i].SetVt(*it);
    }

}
void CalFollowSet()//求FOLLOW集
{
    //先将#插入FOLLOW(S)
    char S=prod[0].getleft();
    int S_pos=FindFollow(S);
    FollowSet[S_pos].SetVt('#');
    for(int i=0;i<(int)prod.size();i++)
    {
        for(int j=0;j<(int)prod.size();j++)
        {
            int j1,j2;
            j1=0;j2=0;
            for(j1=0;j1<prod[j].getrightlen();j1++)
            {
                for(j2=j1+1;j2<prod[j].getrightlen();)
                {
                    char R1=prod[j].getright(j1);
                    char R2=prod[j].getright(j2);
                    if(R1>='A' && R1<='Z')//R1是非终结符
                    {
                        if(R2>='A' && R2<='Z')
                        {
                             int k=FindFirst(R2);
                             CopyFirst2Follow(R1,R2);
                             if(FirstSet[k].FindEmpty()){j2++;}
                             else{break;}                            
                        }
                        else//若R2是终结符，把R2传入R1的FOLLOW集中
                        {
                            int k=FindFollow(R1);
                            FollowSet[k].SetVt(R2);
                            break;
                        }
                    }
                    else{break;}
                }
            }
            char l=prod[j].getleft();//左部符号
            j2=prod[j].getrightlen()-1;//右边的最后一个符号
            while(j2>=0)
            {
                char R=prod[j].getright(j2);
                if(R<'A' || R>'Z' || R=='^') break;
                CopyFollow2Follow(R,l);
                int k=FindFirst(R);
                if(FirstSet[k].FindEmpty()){j2--;continue;}
                else{break;}

            }
        }
    }
}
//-------------------------------------------------------------计算SELECT集
void production::AddFollow2Select(char s)
{
    int k=FindFollow(s);
    for(std::set<char>::iterator it=FollowSet[k].Vt.begin();it!=FollowSet[k].Vt.end();it++)
    {
        SELECT.push_back(*it);
    }
}
void production::AddFirst2Select(char s)
{
    int k=FindFirst(s);
    for(std::set<char>::iterator it=FirstSet[k].Vt.begin();it!=FirstSet[k].Vt.end();it++)
    {
        if(*it!='^')
        {
            SELECT.push_back(*it);
        }

    }
}
void production::AddVt2Select(char s)
{
    SELECT.push_back(s);
}

void CalSelect()
{
    for(int i=0;i<(int)prod.size();i++)
    {
        for(int j=0;j<prod[i].getrightlen();j++)
        {
            char L=prod[i].getleft();
            char R=prod[i].getright(j);//右部第j个
            if(R=='^')
            {
                prod[i].AddFollow2Select(L);
                break;
            }
            else if(R>='A' && R<='Z')
            {
                prod[i].AddFirst2Select(R);
                int k=FindFirst(R);
                if(!FirstSet[k].FindEmpty())
                {
                    break;
                }
                if(j==prod[i].getrightlen()-1)//此产生式右部均可推出空
                {
                    prod[i].AddFollow2Select(L);
                }
            }
            else//R是终结符
            {
                prod[i].AddVt2Select(R);
                break;
            }
        }
    }
}

void PrintSelect()
{
    for(int i=0;i<(int)prod.size();i++)
    {
        printf("SELECT( %d )={",i);
        for(int j=0;j<(int)prod[i].SELECT.size();j++)
        {
            printf(" %c ",prod[i].SELECT[j]);
        }
        printf("}\n");
    }
}

//-------------------------------------------------------
void initSet()//初始化FirstSet FollowSet
{
    for(std::set<std::string>::iterator it=SetVn.begin();it!=SetVn.end();it++)//依次访问SetVn中的元素
    {
        FIRST t1;
        FOLLOW t2;
        t1.SetVn(*it);
        t2.SetVn(*it);
        FirstSet.push_back(t1);
        FollowSet.push_back(t2);
    }
}

int main()
{
    std::string str1;
    while(1)
    {
        std::cin>>str1;
        if(str1=="END")
        {
            break;//输入END结束输入
        }
        FirstExe(str1);
    }
    initSet();
    CalFirstSet();
    printf("--------------------------------------\n");
    for(int i=0;i<(int)FirstSet.size();i++)
    {
        FirstSet[i].PrintFirst();
    }
    CalFollowSet();
    printf("--------------------------------------\n");
    for(int i=0;i<(int)FollowSet.size();i++)
    {
        FollowSet[i].PrintFollow();
    }
    printf("--------------------------------------\n");
    for(int i=0;i<(int)prod.size();i++)
    {
        printf("(%d) ",i);
        prod[i].PrintProd();
    }
    printf("--------------------------------------\n");   
    CalSelect();
    PrintSelect();
    system("pause");
    return 0;
}