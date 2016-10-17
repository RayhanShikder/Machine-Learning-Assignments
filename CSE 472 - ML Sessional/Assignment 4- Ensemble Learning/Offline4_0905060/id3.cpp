#include<iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include<cmath>
#include<algorithm>

using namespace std;

int g_id;
int t_pos,f_pos,f_neg;

struct sample{
    int a,b,c,d,e,f,g,h,i,j;
};

struct node{
    int id;
    int name;
    int cancer;
    string type;

    string parent;
    vector< pair<string,int>>childs;
};

vector<node>tree;
vector< vector<string>>att_vals;

int to_int(string str)  /// converts a string into integer
{
    int y;
    int i;
    for(i=0;i<str.size();i++)
    {
        if(i==0)y=str[i]-'0';
        else
        {
            y*=10;
            y+=str[i]-'0';
        }
    }

    return y;
}


int get_max(vector<double>gains,vector<int>atts)
{
    int id;
    double max=-1.0;
    for(int i=0;i<gains.size()-1;i++)
    {
        if(gains[i]>max && atts[i]!=0){max=gains[i];id=i;}
    }
    return id;
}
void calculate_att_vals(vector< vector<string>>subset, vector<int>atts)//calculate possible values of the attribute
{
    map<string,bool>flag;

    for(int i=0;i<atts.size();i++)
    {
        vector<string>temp;
        for(int j=0;j<subset.size();j++)
        {
            if(!flag[subset[j][i]])
            {
                temp.push_back(subset[j][i]);
                flag[subset[j][i]]=true;
            }
        }
        att_vals.push_back(temp);
        temp.clear();
        flag.clear();
    }

}
map<string,vector< vector<string>>>get_childs(vector< vector<string>>subset,int x,vector<int>atts)
{
    map<string,vector< vector<string>>>maps;

    for(int i=0;i<subset.size();i++)
    {
        maps[subset[i][x]].push_back(subset[i]);
    }

    return maps;

}

vector<double> calculate_gain(vector< vector<string>>subset, vector<int>atts)
{
    vector<double>res;

    vector<string>vals;
    map<string,int> mapping;

    res.resize(10);
    for(int i=9;i>=0;i--)
    {
        for(int j=0;j<subset.size();j++)
        {
            if(mapping[subset[j][i]]==0)vals.push_back(subset[j][i]);
            mapping[subset[j][i]]++;
        }


        if(i==9)
        {
            double temp=0.0;
            for(int k=0;k<vals.size();k++)
            {
                double x= (double)mapping[vals[k]]/(double)subset.size();
                if(x>0.0)
                temp+= (-x * log(x))/log(2);
            }

            res[i]=temp;
        }
        else if(atts[i]!=0)//check if attribute is in current attribute list
        {
            double temps=0.0;
            for(int k=0;k<vals.size();k++)
            {
                double x= (double)mapping[vals[k]]/(double)subset.size();
                if(x>0.0)
                temps+= ((double)mapping[vals[k]]/(double)subset.size())*(-x * log(x))/log(2);
            }

            res[i] = res[9] - temps;
        }
        else if(atts[i]==0)
        {
            res[i]=0.0;
        }

        mapping.clear();
        vals.clear();

    }


    return res;
}

int is_all_positive(vector< vector<string>>subset)
{
    int positive=0;
    int negative=0;
    for(int i=0;i<subset.size();i++)
    {
        if(subset[i][9]=="1")positive++;
        else if(subset[i][9]=="0")negative++;
    }
    if(positive==subset.size())return 1;
    if(negative==subset.size())return 0;
    return positive<negative?3:4;
}

bool is_att_empty(vector<int>atts)
{
    for(int i=0;i<atts.size()-1;i++)
    {
        if(atts[i]==1)return false;
    }
    return true;
}
void id3(vector< vector<string>>subset, vector<int>atts, string target_att,int cur)
{
    node x;
    if(is_all_positive(subset)==1)
    {
        //cout<<"%";
        x.id=g_id;
        x.type="leaf";
        x.cancer=1;
        tree.push_back(x);
        g_id++;
        return;

    }
    else if(is_all_positive(subset)==0)
    {

        //cout<<"+";
        x.id=g_id;
        x.type="leaf";
        x.cancer=0;

        tree.push_back(x);
        g_id++;
        return;
    }

    else if(is_att_empty(atts))
    {

        //cout<<"-";
        node x;
        x.id=g_id;
        x.type="leaf";
        x.cancer=is_all_positive(subset)-3;

        tree.push_back(x);
        g_id++;
        return;
    }

    else if(subset.size()==0)
    {

        //cout<<"-";
        node x;
        x.id=g_id;
        x.type="leaf";
        x.cancer=1;

        tree.push_back(x);
        g_id++;
        return;
    }
    x.cancer=2;

    vector<double> gains;
    gains = calculate_gain(subset,atts);//calculate gain for every attributes
    map<string, vector< vector<string>>>childs;

    int best = get_max(gains,atts);



        if(g_id==0)x.type="root";
        else x.type="mid";

        x.id=g_id;
        x.name=best;
        g_id++;


        childs=get_childs(subset,best,atts);//find child subsets

        vector<int>temp_atts;
        for(int i=0;i<atts.size();i++)
        {
            if(i==best)temp_atts.push_back(0); //the new attributes
            else temp_atts.push_back(atts[i]);
            //temp_atts.push_back(0);
        }

        for(int i=0;i<att_vals[best].size();i++)
        {
            //cout<<"#"<<endl;
            //cout<<att_vals[best][i]<<" ";

          //  if(childs[att_vals[best][i]].size()>0){
            id3(childs[att_vals[best][i]],temp_atts,target_att,cur+1);
            x.childs.push_back(make_pair(att_vals[best][i],tree.size()-1));
            //}
        }
        tree.push_back(x);

}

void print_tree(int id)
{

    if(tree[id].type=="leaf"){cout<<"leaf "<<tree[id].name<<endl;return;}
    else
    {
        cout<<tree[id].type<<" "<<tree[id].name<<endl;
        for(int i=0;i<tree[id].childs.size();i++)
        {
            print_tree(tree[id].childs[i].second);
        }
    }
}

bool predicts(vector<string>ex,int id,int type)
{
    if(tree[id].type=="leaf")
    {
        if(tree[id].cancer==to_int(ex[9]))
        {
            if(tree[id].cancer==1 && type==0)
                t_pos++;
            return true;
        }
        else
        {
            if(tree[id].cancer==1&& type==0)f_pos++;
            else if(type==0) f_neg++;

            return false;
        }
     //cout<<tree[id].cancer<<" "<<to_int(ex[9])<<endl;
    //return tree[id].cancer==to_int(ex[9])?true:false;

    }
    string child;
    for(int i=0;i<tree[id].childs.size();i++)
    {
        if(tree[id].childs[i].first == ex[tree[id].name])
        {
            return predicts(ex,tree[id].childs[i].second,type);
            break;
        }
    }
    return false;
}

bool predict(vector<string>example,int type)
{
    int id;
    for(int i=0;i<tree.size();i++)
    {
        if(tree[i].id==0)
        {
            //cout<<i<<"- ";
           // print_tree(i);
           id=i;
            break;
        }
    }
    return predicts(example,id,type);
    //return true;

}

int test(vector< vector<string>>test_set,int type)
{
    int count=0;

    for(int i=0;i<test_set.size();i++)
    {
        if(predict(test_set[i],type))count++;
    }

    return count;
}

int main()
{
    std::ifstream  data("data.csv");
    ofstream myfile;
    myfile.open ("example.txt");
    std::string line;
    vector< vector<string>> samples;//total samples
    vector< vector<string>> train_samples;
    vector< vector<string>> test_samples;


    vector<string>temp;
    vector<int>atts;

    double total_ok=0.0,total=0.0,train_total_ok=0.0,train_total=0.0;



    while(std::getline(data,line))
    {
        stringstream lineStream(line);
        string cell;
        for(int i=0;std::getline(lineStream,cell,',');i++)
        {

           temp.push_back(cell);
            // You have a cell!!!!
        }
        samples.push_back(temp);
        temp.clear();
    }

    for(int i=0;i<10;i++)atts.push_back(1);

    calculate_att_vals(samples,atts);


for(int k=0;k<100;k++){

    random_shuffle(samples.begin(),samples.end());//shuffle the sample data

    for(int i=0;i<samples.size();i++)//choose 80% data for training and others for testing
    {
        if(i<(samples.size()*80)/100)
            train_samples.push_back(samples[i]);
        else test_samples.push_back(samples[i]);
    }

    for(int i=0;i<10;i++)atts.push_back(1);

//    calculate_att_vals(test_samples,atts);

    id3(train_samples,atts,"9",0);

    total_ok+=(double)test(test_samples,0);

    train_total_ok+=(double)test(train_samples,1);
    train_total+=(double)train_samples.size();

    total+=(double)test_samples.size();

    //cout<<(double)((double)pos*100)/(double)test_samples.size();



    for(int i=0;i<tree.size();i++)
    {
        if(tree[i].id==0)
        {
            //cout<<i<<"- ";
            //print_tree(i);
            break;
        }
    }

//cout<<tree.size()<<" ";
    tree.clear();
    train_samples.clear();
    test_samples.clear();

    random_shuffle(samples.begin(),samples.end());
    g_id=0;
    atts.clear();

}

double precision = ((double)t_pos/(double(t_pos)+(double)f_pos));
double recall = ((double)t_pos/((double(t_pos)+(double)f_neg)));
double f_measure = (2*precision*recall)/(precision + recall);


cout<<"accuracy: "<<(total_ok/total)*(double)100.0<<endl;


cout<<"precision: "<<precision*(double)100.0<<endl;

cout<<"recall: "<<recall*(double)100.0<<endl;

cout<<"f-measure "<<f_measure<<endl;

cout<<"g-measure "<<sqrt(precision*recall)<<endl;

cout<<"training error: "<<(1-((double)train_total_ok/(train_total)))*100<<endl;
cout<<"test error "<<(1-((double)total_ok/(total)))*100<<endl;



    return 0;
}
