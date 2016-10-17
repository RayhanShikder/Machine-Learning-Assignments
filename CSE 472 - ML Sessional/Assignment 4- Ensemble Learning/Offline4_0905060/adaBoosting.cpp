#include<iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include<cmath>
#include<algorithm>
#include<ctime>
#include<cstdlib>

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
    vector< pair<string,int>>results;
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

double sumOfWeights(vector<double>w)
{
    double sum=0.0;
    for(int i=0;i<w.size();i++)sum+=w[i];
    return sum;
}

vector<double> calculate_gain(vector< vector<string>>subset, vector<int>atts,vector<double>w)
{
    vector<double>res;

    vector<string>vals;
    map<string,double> mapping;

    res.resize(10);
    for(int i=9;i>=0;i--)
    {
        for(int j=0;j<subset.size();j++)
        {
            if(mapping[subset[j][i]]==0)vals.push_back(subset[j][i]);
            ///mapping[subset[j][i]]++;
            mapping[subset[j][i]]+=w[j];
        }


        if(i==9)
        {
            double temp=0.0;
            for(int k=0;k<vals.size();k++)
            {
                double x= (double)mapping[vals[k]]/(double)sumOfWeights(w);
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
                double x= (double)mapping[vals[k]]/(double)sumOfWeights(w);
                if(x>0.0)
                temps+= ((double)mapping[vals[k]]/(double)sumOfWeights(w))*(-x * log(x))/log(2);
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

int getClass(vector< vector<string>>subset,int att_id,string cur_val)
{
    int pos=0,neg=0;
    for(int i=0;i<subset.size();i++)
    {
        if(subset[i][att_id]==cur_val)
        {
            if(to_int(subset[i][9])==1)pos++;
            else neg++;
        }
    }
    if(pos>neg)return 1;
    else return 0;

}

vector< pair<string,int>>getClassifier(vector< vector<string>>subset,int att_id)
{
    vector< pair<string,int>>res;

    for(int i=0;i<att_vals[att_id].size();i++)
    {
        res.push_back(make_pair(att_vals[att_id][i],getClass(subset,att_id,att_vals[att_id][i])));
    }

    return res;
}

node decisionStamp(vector< vector<string>>subset, vector<int>atts, string target_att,int cur,vector<double>w)
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
        return x;

    }
    else if(is_all_positive(subset)==0)
    {

        //cout<<"+";
        x.id=g_id;
        x.type="leaf";
        x.cancer=0;

        tree.push_back(x);
        g_id++;
        return x;
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
        return x;
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
        return x;
    }
    ///x.cancer=2;

x.cancer=is_all_positive(subset)-3;//addded for decision stamp



    vector<double> gains;
    gains = calculate_gain(subset,atts,w);//calculate gain for every attributes
    map<string, vector< vector<string>>>childs;

    int best = get_max(gains,atts);




        if(g_id==0)x.type="root";
        else x.type="mid";

        x.id=g_id;
        x.name=best;
        g_id++;


        x.results = getClassifier(subset,best);



        return x;

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

bool predicts(vector<string>ex,node x,int type)
{
    string val;
    int res;
    val = ex[x.name];
    for(int i=0;i<x.results.size();i++)
    {
        if(x.results[i].first==val)
        {
            res = x.results[i].second;
        }
    }

    if(to_int(ex[9])==res)return true;
    return false;
}



void test_adaBoost(vector< vector<string> >test_sample,vector<node>hr,vector<double>beta)
{
    //cout<<" attss "<<hr.size()<<endl;
    //for(int i=0;i<hr.size();i++)cout<<hr[i].name<<" "<<beta[i]<<" ";
    //cout<<endl;

    int cor=0;

    for(int i=0;i<test_sample.size();i++)
    {
        double pos=0.0;
        double neg=0.0;

            for(int j=0;j<hr.size();j++)
            {
               for(int l=0;l<hr[j].results.size();l++)
               {
                   if(hr[j].results[l].first==test_sample[i][hr[j].name])
                   {
                       if(hr[j].results[l].second==1)pos+=log(1/beta[j]);
                       else neg+=log(1/beta[j]);
                   }
               }
            }
           // cout<<pos<<" "<<neg<<endl;
            if(pos>=neg && to_int(test_sample[i][9])==1)cor++;
            if(neg>pos && to_int(test_sample[i][9])==0)cor++;


    }
    cout<<"correct "<<cor<<" "<<test_sample.size()<<endl;
    cout<<"AdaBoost Accuracy: "<<((double)cor/(double)test_sample.size())*100<<endl;

}

double getMax(vector<double>val)
{
    double max = -1.0;
    for(int i=0;i<val.size();i++)
    {
        if(val[i]>max)max=val[i];
    }
    return max;
}
int main()
{
    srand(time(NULL));
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


int round;
int rounds[4];

vector<node>hr;//stores the result of individual classifiers
vector<double>weights;//stores the weights of training samples
vector<double>beta;

cout<<"enter the round of adaBoost: "<<endl;
cin>>round;
hr.resize(round);
beta.resize(round);




vector<double>pr;

    random_shuffle(samples.begin(),samples.end());//shuffle the sample data

    for(int i=0;i<samples.size();i++)//choose 80% data for training and others for testing
    {
        if(i<(samples.size()*80)/100)
            train_samples.push_back(samples[i]);
        else test_samples.push_back(samples[i]);
    }


vector<double>cum_pr;
vector< vector<string>>temp_train;

for(int k=0;k<round;k++){ //the adaBooster




        if(k==0)//initialize the weights
        {
            weights.resize(train_samples.size());
            for(int l=0;l<train_samples.size();l++)
            {
                weights[l]=(double)1/(double)train_samples.size();
                //weights[l]=1;
            }


       // pr.resize(weights.size());
        }
        pr.resize(weights.size());
        cum_pr.resize(weights.size());

    for(int i=0;i<10;i++)atts.push_back(1);

//    calculate_att_vals(test_samples,atts);

double total_weight=sumOfWeights(weights);
for(int l=0;l<pr.size();l++)//calculate pr
{
    pr[l]=weights[l]/total_weight;
    if(l==0)cum_pr[l]=pr[l];
    else cum_pr[l]=pr[l]+cum_pr[l-1];
}


vector< pair<double,int>>temp_pr;
for(int i=0;i<pr.size();i++)
{
    temp_pr.push_back(make_pair(pr[i],i));
}
sort(temp_pr.begin(),temp_pr.end());

for(int q=temp_pr.size()-1;q>=(train_samples.size()*20)/100;q--)
{
    temp_train.push_back(train_samples[temp_pr[q].second]);
}

/*for(int q=0;q<train_samples.size();q++)
{
    double random = (double)rand()/(1.0+RAND_MAX);
    if(random<cum_pr[q])temp_train.push_back(train_samples[q]);
}
*/
//cout<<"max pr "<<getMax(cum_pr)<<endl;

//cout<<"rand "<<random<<endl;
    hr[k]=decisionStamp(temp_train,atts,"9",0,pr);

double er=0.0;
int cor=0;
for(int l=0;l<train_samples.size();l++)//calculate the total error
{
    if(!predicts(train_samples[l],hr[k],0))er+=pr[l];
    else cor++;
}
//cout<<"cor "<<cor<<" total "<<train_samples.size()<<endl;

if(er>0.5)//if performance decreases then stop
{
    round=k-1;
    cout<<"breaking..."<<endl;
    break;
}

beta[k]=er/(1-er);
vector<double>temp_w;

for(int l=0;l<train_samples.size();l++)//calculate the new weights
{
    temp_w.push_back(predicts(train_samples[l],hr[k],0)==true?weights[l]:weights[l]*beta[k]);
}

for(int l=0;l<temp_w.size();l++)weights[l]=temp_w[l];
temp_w.clear();
pr.clear();
cum_pr.clear();
temp_train.clear();






//cout<<tree.size()<<" ";
    tree.clear();

   //if(k<round-1) {
       //train_samples.clear();
     //  test_samples.clear();
   //}

   // random_shuffle(samples.begin(),samples.end());
    g_id=0;
    atts.clear();

}


//for(int k=0;k<hr.size();k++)cout<<hr[k]<<" ";
cout<<endl;
test_adaBoost(train_samples,hr,beta);




    return 0;
}

