#ifndef _PMS_H_
#define _PMS_H_

#include "basis_pms.h"
#include "deci.h"
#include <fstream>
#include <queue>
#include <vector>
#include <iostream>
using namespace std;
ofstream fout("out1.txt",ios::app);

Satlike::Satlike()
{
	problem_weighted=1;
	partial=1; //1 if the instance has hard clauses, and 0 otherwise.

	max_clause_length=0;
	min_clause_length=100000000;

	//size of the instance
	num_vars=0;		//var index from 1 to num_vars
	num_clauses=0;		//clause index from 0 to num_clauses-1
	num_hclauses=0;
	num_sclauses=0;

	print_time=240;
	cutoff_time=301;

	print1=0;
	print2=0;
	print_time1=50;
	print_time2=260;
	cutoff_time1=60;
	cutoff_time2=300;
}

void Satlike::settings()
{
	//steps
	weight_multiplier=100;
	tries=0;
	max_tries = 100000000;
	max_flips = 200000000;
	max_non_improve_flip = 10000000;
	max_flips = max_non_improve_flip;
	step=1;

	large_clause_count_threshold=0;
	soft_large_clause_count_threshold=0;

	rdprob=0.01;
	hd_count_threshold=42;
	rwprob=0.091;
	smooth_probability=0.000003;
	h_inc=1;
	softclause_weight_threshold=400;

	if(top_clause_weight==555 || top_clause_weight==3345){max_non_improve_flip = 100000000;max_flips = max_non_improve_flip;return;}//for tpr-multiple-path
	if(num_vars==21368 && max_clause_length==8)//for aes
	{
		//weight_multiplier=1;
		max_non_improve_flip = 100000000;
		max_flips = max_non_improve_flip;
		h_inc=2;
		softclause_weight_threshold=0;
		rdprob=0.01;
		hd_count_threshold=15;
		rwprob=0;
		smooth_probability=0.0001;
		return ;
	}
	if(num_vars>12000 && num_vars<17000 && top_clause_weight==49771 && max_clause_length>310 && max_clause_length<351)//for fault
	{
		//weight_multiplier=1;
		h_inc=2;
		softclause_weight_threshold=0;
		rdprob=0.01;
		hd_count_threshold=15;
		rwprob=0;
		smooth_probability=0.0001;
		return ;
	}

	if(num_vars<1100) //level 2
	{
		weight_multiplier=1;
		h_inc=2;
		softclause_weight_threshold=0;
		rdprob=0.01;
		hd_count_threshold=15;
		rwprob=0;
		smooth_probability=0.01;
		return ;
	}

}

void Satlike::save()
{

    for(int c=0;c<num_clauses;c++)
    {
        clause_lit_count_saved[c]=clause_lit_count[c];
        clause_delete_saved[c] = clause_delete[c];

        clause_lit_saved[c]=new lit[clause_lit_count[c]+1];
        for(int v=0;v<clause_lit_count[c];v++)
        {
            clause_lit_saved[c][v]=clause_lit[c][v];
        }
    }
//    cout<<"save:"<<endl;
//    for(int c=0;c<num_clauses;c++)
//    {
//        if(clause_delete_saved[c]!=1){
//            for(int i=0;i<clause_lit_count_saved[c];i++)
//            {
//                if(clause_lit_saved[c][i].sense==false)
//                {
//                    cout<<"-"<<clause_lit_saved[c][i].var_num<<" ";
//                }
//                else cout<<clause_lit_saved[c][i].var_num<<" ";
//            }
//            cout<<endl;
//        }
//    }


}
void Satlike::allocate_memory()
{
	int malloc_var_length = num_vars+10;
	int malloc_clause_length = num_clauses+10;

	unit_clause = new lit[malloc_clause_length];
	unit_clause_saved=new lit[malloc_clause_length];
    unit_gate=new lit[gate_num];
    assigned_B=new int[malloc_var_length];
    confict_0=new int[malloc_var_length];
    confict_1=new int[malloc_var_length];
    B1_value=new int[malloc_var_length];
    B2_value=new int[malloc_var_length];
    B1_num=new int[malloc_var_length];
    B2_num=new int[malloc_var_length];
    F_value=new int[malloc_var_length];

    gate=new Gate[malloc_var_length];
	var_lit = new lit* [malloc_var_length];
	var_lit_count = new int [malloc_var_length];
	clause_lit = new lit* [malloc_clause_length];
	clause_lit_count = new int [malloc_clause_length];
	clause_delete=new int[malloc_clause_length];

	clause_lit_saved = new lit* [malloc_clause_length];
	clause_lit_count_saved = new int [malloc_clause_length];
	clause_delete_saved=new int[malloc_clause_length];

	score = new long long [malloc_var_length];
	var_neighbor = new int* [malloc_var_length];
	var_neighbor_count = new int [malloc_var_length];
	time_stamp = new long long [malloc_var_length];
	neighbor_flag = new int [malloc_var_length];
	temp_neighbor = new int [malloc_var_length];
	var_gate_neighbor=new int*[malloc_var_length];
	var_gate_neighbor_count=new int [malloc_var_length];
	gate_neighbor_flag=new int[malloc_var_length];
	temp_gate_neighbor=new int [malloc_var_length];
	in_gate=new int [malloc_var_length];


    //vector< int > Dominated;
    //vector< int > Filtered_Node;
    Filtered_Node=new int[malloc_var_length];

    Dominated=new int [malloc_var_length];


	org_clause_weight = new long long [malloc_clause_length];
	clause_weight = new long long [malloc_clause_length];
	sat_count = new int [malloc_clause_length];
	sat_var = new int [malloc_clause_length];
	clause_selected_count = new long long [malloc_clause_length];
	best_soft_clause = new int [malloc_clause_length];

	hardunsat_stack = new int [malloc_clause_length];
	index_in_hardunsat_stack = new int [malloc_clause_length];
	softunsat_stack = new int [malloc_clause_length];
	index_in_softunsat_stack = new int [malloc_clause_length];

	unsatvar_stack = new int [malloc_var_length];
	index_in_unsatvar_stack = new int [malloc_var_length];
	unsat_app_count = new int [malloc_var_length];

	goodvar_stack = new int [malloc_var_length];
	already_in_goodvar_stack = new int[malloc_var_length];

	cur_soln = new int [malloc_var_length];
	best_soln = new int [malloc_var_length];
	local_opt_soln = new int[malloc_var_length];

	large_weight_clauses = new int [malloc_clause_length];
	soft_large_weight_clauses = new int [malloc_clause_length];
	already_in_soft_large_weight_stack = new int [malloc_clause_length];

	best_array = new int [malloc_var_length];
	temp_lit = new int [malloc_var_length];

    for(int i=0; i<malloc_var_length; ++i)
    {
        gate[i].dominat_attr = 0;
        gate[i].out_var = 0;
        gate[i].first_clauses = 0;
        gate[i].last_clauses = -1;
        var_neighbor_count[i] = 0;
        var_gate_neighbor_count[i] = 0;
        in_gate[i] = 0;
    }
}

void Satlike::free_memory()
{
	int i;
	for (i = 0; i < num_clauses; i++)
		delete[] clause_lit[i];

	for(i=1; i<=num_vars; ++i)
	{
		delete[] var_lit[i];
		delete[] var_neighbor[i];
	}

	delete [] var_lit;
	delete [] var_lit_count;
	delete [] clause_lit;
	delete [] clause_lit_count;

	delete [] var_neighbor;
	delete [] var_neighbor_count;




	//delete [] fix;

}

void Satlike::build_neighbor_relation()
{
	int	i,j,count;
	int v,c,n;
    int temp_neighbor_count;
    int temp_gate_neighbor_count;

    for(i=1;i<=num_vars;i++)
    {
        in_gate[i]=0;
    }
	for(v=1; v<=num_vars; ++v)
	{
		neighbor_flag[v] = 1;
		gate_neighbor_flag[v]=1;
		temp_neighbor_count = 0;
		temp_gate_neighbor_count=0;

		for(i=0; i<var_lit_count[v]; ++i)
		{
			c = var_lit[v][i].clause_num;
			for(j=0; j<clause_lit_count[c]; ++j)
			{
                n=clause_lit[c][j].var_num;
				if(neighbor_flag[n]!=1)
				{
                    if(clause_lit_count[c]-2 >=0)
                    {
                        if(j==clause_lit_count[c]-1 && v!=clause_lit[c][clause_lit_count[c]-2].var_num)
                        {
                            gate_neighbor_flag[n]=1;
                            temp_gate_neighbor[temp_gate_neighbor_count++]=n;
                        }
                        if(j==clause_lit_count[c]-1  && v==clause_lit[c][clause_lit_count[c]-2].var_num)
                        {
                            in_gate[v]=n;
                        }
                    }

					neighbor_flag[n] = 1;
					temp_neighbor[temp_neighbor_count++] = n;
				}
			}
		}

		neighbor_flag[v] = 0;
		var_neighbor[v] = new int[temp_neighbor_count];
		var_neighbor_count[v]=temp_neighbor_count;

		gate_neighbor_flag[v]=0;
		var_gate_neighbor[v]=new int[temp_gate_neighbor_count];
		var_gate_neighbor_count[v]=temp_gate_neighbor_count;

		count = 0;
		for(i=0; i<temp_neighbor_count; i++)
		{
			var_neighbor[v][count++] = temp_neighbor[i];
			neighbor_flag[temp_neighbor[i]] = 0;
		}
		count=0;
		for(i=0;i<temp_gate_neighbor_count;i++)
        {
            var_gate_neighbor[v][count++]=temp_gate_neighbor[i];
            neighbor_flag[temp_gate_neighbor[i]]=0;
        }
	}
	//�������ÿ���������ھ���Ϣ
//	for(int i=1;i<=num_vars;i++)
//    {
//        cout<<i<<endl;
//        for(int j=0;j<var_neighbor_count[i];j++)
//        {
//            cout<<var_neighbor[i][j]<<" ";
//        }
//        cout<<endl;
//    }
//    for(int i=1;i<=num_vars;i++)
//    {
//        cout<<i<<endl;
//        for(int j=0;j<var_gate_neighbor_count[i];j++)
//        {
//            cout<<var_gate_neighbor[i][j]<<" ";
//        }
//        cout<<endl;
//    }
//    for(int i=1;i<=num_vars;i++)
//    {
//        cout<<i<<": "<<in_gate[i]<<endl;
//    }
}

void Satlike::check_lit()
{
    cout<<num_vars<<" "<<num_clauses<<endl;
}

vector<string> Satlike::split(const char *s, const char *delim)
{
    vector<string> result;
    if (s && strlen(s))
    {
        int len = strlen(s);
        char *src = new char[len + 1];
        strcpy(src, s);
        src[len] = '\0';
        char *tokenptr = strtok(src, delim);
        while (tokenptr != NULL)
        {
            string tk = tokenptr;
            result.push_back(tk);
            tokenptr = strtok(NULL, delim);
        }
        delete[] src;
    }
    return result;
}

string Satlike::split(string str){  //ȥ�����ҵ�[]�;�Ŷ���
    string res;
    int left = 0, right = str.size()-1;
    int len = str.size();
    while(str[left]=='[' || str[left]=='('){
        left++;
        len--;
    }
    while(str[left]=='{')
    {
        left++;
        len--;
    }
    while(!isdigit(str[right])){
        right--;
        len--;
    }
    return str.substr(left,len);
}

string Satlike::split_syspp(string str)
{
    string res;
    int left = 0, right = str.size()-1;
    int len = str.size();
    while(str[left]=='(')
    {
        left++;
        len--;
    }
    while(str[right]==',' || str[right]==')')
    {
        right--;
        len--;
    }
    return str.substr(left,len);
}

string Satlike::findBegin(string str){
    return "";
}
void Satlike::seq(char* filename)
{
string str;
//    freopen("systems/c432.sys","r",stdin);

    z_count.resize(500000);
    dominator_str.resize(500000);
    for(int i=0;i<500000;i++)
    {
        z_count[i]=0;
    }
    ifstream fin(filename);
    int flag = 0;
    bool flag1=true;

    string first_gate;
    bool first = true;
    int idx = -1;
    int m_cur_idx = 1;
    while(fin>>str){
        if(first){
            first = false;
            continue;
        }
        str = split(str);
        vector<string> res = split(str.c_str(),",");
        if(res[0][0]=='i' && res[0][1]!='n'){
            flag = 1;
        }
        else if(res[0][0]=='o' && res[0][1]!='r')
        {
            flag=2;
        }
        else{
            flag=3;
        }
        if(flag==1){
            in_len=in_len+res.size();
        }
        else if(flag==2){
            int out_res_size=res.size()-1;
            for(int i=out_res_size;i>=0;i--)
            {
                if(m.find(res[i]) == m.end())
                {
                    m.insert(pair<string,int>(res[i], m_cur_idx++));
                    out_len++;
                }
            }
        }
        else{
            gate_num++;
            if(m.find(res[1]) == m.end()){    //not exist
//                idx = m[res[1]];
//                idx = cur_idx;
                m.insert(pair<string,int>(res[1], m_cur_idx++));   //key->value
                //cout<<res[res_count]<<" "<<m[res[res_count]]<<endl;
            }

        }
    }
//    cout<<"in_num:"<<in_len<<"   out_len:"<<out_len<<"  gate_num:"<<gate_num<<endl;
    string s="i";
    for(int i=1;i<=in_len;i++){
        stringstream ss;
        ss<<i;
        string tmp;
        ss>>tmp;
        s=s+tmp;
        if(m.find(s) == m.end()){    //not exist
//                idx = m[res[1]];
//                idx = cur_idx;
            m.insert(pair<string,int>(s,m_cur_idx++));   //key->value
        }
        s="i";
    }
    s="o";
    for(int i=1;i<=out_len;i++){
        stringstream ss;
        ss<<i;
        string tmp;
        ss>>tmp;
        s=s+tmp;
//        s=s+string(i);
        if(m.find(s) == m.end()){
            m.insert(pair<string,int>(s,m_cur_idx++));   //key->value
        }
        s="o";
    }
    s="z";
    for(int i=1;i<=gate_num;i++){
        stringstream ss;
        ss<<i;
        string tmp;
        ss>>tmp;
        s=s+tmp;
//        s=s+string(i);
        if(m.find(s) == m.end()){
            m.insert(pair<string,int>(s,m_cur_idx++));   //key->value
        }
        s="z";
    }

}
void Satlike::seq_dominate(char* filename,char* outfile){//����sys�ļ�

    string str;
//    freopen("systems/c432.sys","r",stdin);
    ofstream fout(outfile);
    map<string,string> out_gate_map;
    map<string,int> out_inpunum_map;
    map<string,vector<string> > gate_input_map;
    z_count.resize(500000);
    dominator_str.resize(500000);
    for(int i=0;i<500000;i++)
    {
        z_count[i]=0;
    }
    ifstream fin(filename);
    int flag = 0;
    bool flag1=true;

    string first_gate;
    bool first = true;
    int idx = -1;
    int m_cur_idx = 1;
    while(fin>>str){
        if(first){
            first = false;
            continue;
        }
        str = split(str);
        vector<string> res = split(str.c_str(),",");
        if(res[0][0]=='i' && res[0][1]!='n'){
            flag = 1;
        }
        else if(res[0][0]=='o' && res[0][1]!='r')
        {
            flag=2;
        }
        else{
            flag=3;
        }
        if(flag==1){
            in_len=in_len+res.size();
        }
        else if(flag==2){
            out_len=out_len+res.size();
        }
        else{
            gate_num++;
            int res_count=res.size()-1;
            vector<string> tmp_input;
            int tmp_input_cnt=0;
            out_gate_map.insert(pair<string,string>(res[2], res[1]));   //key->value

            while(res_count!=1)
            {
                if(m.find(res[res_count]) == m.end()){    //not exist
//                idx = m[res[1]];
//                idx = cur_idx;
                m.insert(pair<string,int>(res[res_count], m_cur_idx++));   //key->value
                //cout<<res[res_count]<<" "<<m[res[res_count]]<<endl;
                //cout<<res[res_count];

                }
                else
                {
                    //cout<<res[res_count]<<" "<<z_count[m[res[res_count]]]<<endl;
                    z_count[m[res[res_count]]]++; //��ʾ����������ֵĴ���
                }

                if(res_count>2)
                {
                    //tmp_input[tmp_input_cnt]=res[res_count];
                    tmp_input.push_back(res[res_count]);
                    tmp_input_cnt++;
                }
                res_count--;
            }
            out_inpunum_map.insert(pair<string,int>(res[2],tmp_input_cnt));
            gate_input_map.insert(pair<string,vector<string> >(res[1],tmp_input));
        }
    }
//    cout<<"in_num:"<<in_len<<"   out_len:"<<out_len<<"  gate_num:"<<gate_num<<endl;
    string s="i";
    for(int i=1;i<=in_len;i++){
        stringstream ss;
        ss<<i;
        string tmp;
        ss>>tmp;
        s=s+tmp;
        if(m.find(s) == m.end()){    //not exist
//                idx = m[res[1]];
//                idx = cur_idx;
            m.insert(pair<string,int>(s,m_cur_idx++));   //key->value
        }
        s="i";
    }
    s="o";
    for(int i=1;i<=out_len;i++){
        stringstream ss;
        ss<<i;
        string tmp;
        ss>>tmp;
        s=s+tmp;
//        s=s+string(i);
        if(m.find(s) == m.end()){
            m.insert(pair<string,int>(s,m_cur_idx++));   //key->value
        }
        s="o";
    }
    s="z";
    for(int i=1;i<=gate_num;i++){
        stringstream ss;
        ss<<i;
        string tmp;
        ss>>tmp;
        s=s+tmp;
//        s=s+string(i);
        if(m.find(s) == m.end()){
            m.insert(pair<string,int>(s,m_cur_idx++));   //key->value
//            cout<<"????!!!!!!��??"<<endl;
         }
        s="z";
    }
    fout<<"% System Gates"<<endl<<"["<<endl;
    //���±���
    ifstream fin1(filename);
    flag = 0;
    flag1=true;

    first_gate;
    first = true;
    idx = -1;
    m_cur_idx = 1;
    int res_out;
    while(fin1>>str){
        if(first){
            first = false;
            continue;
        }
        str = split(str);
        vector<string> res = split(str.c_str(),",");
        if(res[0][0]=='i' && res[0][1]!='n'){
            continue;
        }
        else if(res[0][0]=='o' && res[0][1]!='r')
        {
            continue;
        }
        else if(z_count[m[res[2]]]==1){
                fout<<"("<<res[1]<<","<<res[2]<<",[";
                for(res_out=3;res_out<res.size()-1;res_out++)
                {
                    fout<<res[res_out]<<",";
                }
                fout<<res[res_out]<<"],"<<res[0]<<",dominated),"<<endl;
        }
        else{
            bool hasDominator=false;
            for(int res_count=res.size()-1;res_count>2;res_count--)
            {//�������z_count��ֵΪ1,��Ϊdominator
                if(z_count[m[res[res_count]]]==1)
                {
                    fout<<"("<<res[1]<<","<<res[2]<<",[";
                    for(res_out=3;res_out<res.size()-1;res_out++)
                    {
                        fout<<res[res_out]<<",";
                    }
                    fout<<res[res_out]<<"],"<<res[0]<<",dominator),"<<endl;
                    hasDominator=true;
                    break;
                }
            }
            if(hasDominator==false)
            {
                fout<<"("<<res[1]<<","<<res[2]<<",[";
                for(res_out=3;res_out<res.size()-1;res_out++)
                {
                    fout<<res[res_out]<<",";
                }
                fout<<res[res_out]<<"],"<<res[0]<<",free),"<<endl;
            }
        }
    }

    fout<<"]."<<endl<<"% System Cones"<<endl<<"["<<endl;


    //���±���
    ifstream fin2(filename);
    vector<string> dominated_result;
    int dominated_cnt=0;
    flag = 0;
    flag1=true;

    first_gate;
    first = true;
    idx = -1;
    m_cur_idx = 1;
    queue<string> gate_q;
    while(fin2>>str){
        if(first){
            first = false;
            continue;
        }
        str = split(str);
        vector<string> res = split(str.c_str(),",");
//        for(int j=0;j<res.size();j++)
//        {
//            cout<<res[j]<<" ";
//        }
//        cout<<endl;
        if(res[0][0]=='i' && res[0][1]!='n'){
            continue;
        }
        else if(res[0][0]=='o' && res[0][1]!='r')
        {
            continue;
        }
        else if(z_count[m[res[2]]]==1){
            continue;
        }
        else{
            bool hasDominator=false;
            int dominated_count=0;
            for(int res_count=res.size()-1;res_count>2;res_count--)
            {
                if(z_count[m[res[res_count]]]==1 && out_gate_map.find(res[res_count])!= out_gate_map.end())
                {
                    dominated_count++;
                    hasDominator=true;

                }
            }
            if(hasDominator==true)
            {
                 fout<<"("<<res[1]<<",[";
                 //cout<<"("<<res[1]<<",[";
            }


            for(int res_count=res.size()-1;res_count>2;res_count--)
            {
                //if(z_count[m[res[res_count]]]==1 && dominated_count>1)
                if(z_count[m[res[res_count]]]==1 && out_gate_map.find(res[res_count])!= out_gate_map.end())
                {
                    dominated_count--;
                    //vector<int> inputnum_tmp=gate_input_map[res[res_count]];
                    //int input_num=gate_input_map[res[res_count]].size()-1;

                    gate_q.push(out_gate_map[res[res_count]]);
                    //cout<<"push queue: "<<out_gate_map[res[res_count]]<<endl;
                    while(!gate_q.empty())
                    {
                        string gate_tmp=gate_q.front();
                        dominated_result.push_back(gate_q.front());
                        dominated_cnt++;
                        //dominated_result[dominated_cnt++]=gate_q.front();
                        //cout<<gate_q.front()<<",";//�����ͳ����ֱ��ͳ�ε����
                        gate_q.pop();
                        vector<string> ipt_tmp;
                        int ipt_tmp_count=0;
                        vector<string> ::iterator it;//����������
                        if(gate_input_map.find(gate_tmp) != gate_input_map.end() && gate_input_map[gate_tmp].size() != 0) {

                            for(it = gate_input_map[gate_tmp].begin();it!=gate_input_map[gate_tmp].end();++it)
                            {

                                ipt_tmp.push_back(*it);
                                //cout<<"ipt_tmp.push_backed(): "<<(*it)<<endl;
                                ipt_tmp_count++;
                                //cout<<"gate_input_map :"<<*it<<endl;
                            }
//                            for (int i=0;i<10;i++)
//                        {
//                            cout<<"ipt_tmp[i] :"<<ipt_tmp[i]<<endl;
//                        }
                        //cout<<ipt_tmp.size()<<" ::"<<ipt_tmp_count<<endl;
                        }
                        //ipt_tmp.swap(gate_input_map);
                        //cout<<"ipt_tmp.size() "<<ipt_tmp_count<<endl;
                        int ipt_count=ipt_tmp.size()-1;
                        //cout<<ipt_count<<endl;
                        while(ipt_count>=0)
                        {
                            if(z_count[m[ipt_tmp[ipt_count]]]==1  && out_gate_map[ipt_tmp[ipt_count]].size()>0)
                            {
                                //cout<<ipt_tmp[ipt_count]<<"  : res[res_count] : "<<res[res_count]<<endl;
                                gate_tmp=out_gate_map[ipt_tmp[ipt_count]];
                                //cout<<"gate_tmp: "<<gate_tmp<<"size() "<<gate_tmp.size()<<endl;
                                gate_q.push(gate_tmp);

                            }
                            ipt_count--;


                        }

                    }
                }
                //else if(z_count[m[res[res_count]]]==1 && dominated_count==1)

                    //fout<<gate_out_map[res[res_count]]<<"]),"<<endl;

            }

        }
//         while(dominated_cnt!=0)
//        {
//            fout<<dominated_result[dominated_cnt--]<<",";
//        }
//        if(dominated_cnt==0)
//        {
//            fout<<dominated_result[0]<<"]),"<<endl;
//        }
        while(dominated_result.size()!=1 && dominated_result.size()>0)
        {
            fout<<dominated_result[dominated_result.size()-1]<<",";
            dominated_result.pop_back();
        }
        if(dominated_result.size()==1)
        {
            fout<<dominated_result[dominated_result.size()-1]<<"]),"<<endl;
            dominated_result.pop_back();
        }
    }



}
void Satlike::unit_propagation()
{
    while(gate_num>0)
    {
        gate_num--;
    }
}
void Satlike::assign1()
{
    while(unit_clause_count>0)
    {
        unit_clause_count--;
//        cout<<unit_clause[unit_clause_count].var_num<<endl;
        int v=unit_clause[unit_clause_count].var_num;
        int sense=unit_clause[unit_clause_count].sense;
        int c,l;
        for(int i=0;i<var_lit_count[v];++i)
        {
            c=var_lit[v][i].clause_num;
            if(clause_delete[c]==1) continue;

            if(sense==var_lit[v][i].sense)
            {
                clause_delete[c]=1;
                continue;
            }
            int swap_lit_var;
            bool swap_lit_sense;
            for(int j=0;j<clause_lit_count[c];j++)
            {
                if(clause_lit[c][j].var_num==v)
                {
                    for(int ii=j+1;ii<clause_lit_count[c];ii++)
                    {
                        lit tmp = clause_lit[c][ii-1];
                        clause_lit[c][ii-1] = clause_lit[c][ii];
                        clause_lit[c][ii] = tmp;
                        //swap(clause_lit[c][i-1],clause_lit[c][i]);
//                        swap_lit_var=clause_lit[c][i].var_num;
//                        swap_lit_sense=clause_lit[c][i].sense;
//                        clause_lit[c][i].var_num=clause_lit[c][i-1].var_num;
//                        clause_lit[c][i].sense=clause_lit[c][i-1].sense;
//                        clause_lit[c][i-1].sense=swap_lit_sense;
//                        clause_lit[c][i-1].var_num=swap_lit_var;
                    }
                    clause_lit_count[c]--;
                    break;
                }
            }
            if(clause_lit_count[c]==1)
            {
                unit_clause[unit_clause_count++]=clause_lit[c][0];
            }
        }
    }
    for(int c=0;c<num_clauses;c++)
    {
        for(int i=0;i<clause_lit_count[c];i++)
        {
            if(clause_delete[c]!=1){
                if(clause_lit[c][i].sense==false)
                {
                    //cout<<"-"<<clause_lit[c][i].var_num<<" ";
                }
                //else cout<<clause_lit[c][i].var_num<<" ";
            }
        }
       // cout<<endl;
//        if(clause_delete[c]!=1)
//        {
//            cout<<endl;
//        }
    }
}

void Satlike::reduce_clauses()
{
    for(int i=0;i<num_clauses;i++)
    {
        clause_delete[i]=0;
    }

    //assign1();
    int filtered_gate=0;
    int dominated_gate=0;
    int last_clauses;

    //vector<int> gate_in;
    int cur_gate;
    int first_clauses;



    int tmp_out=0;
    map<int,int> blocked_edge;
    bool added_tmp_out=false;
    bool count3=false;
    bool re_traverse=false;
    int in_var=0;

    if(unit_clause_count>0)
    {
        assign1();
        for(int i=0;i<=num_clauses;i++)
        {
            if(clause_delete[i]==1)
                continue;
            else{
                cur_gate=clause_lit[i][clause_lit_count[i]-1].var_num;
                first_clauses=i;
                last_clauses=i;
                break;
            }
        }
        for(int c=0;c<num_clauses;c++)
        {
            if(clause_delete[c]==1)
            {
                continue;
            }
            if(cur_gate==clause_lit[c][clause_lit_count[c]-1].var_num && re_traverse==false  && clause_delete[c]!=1)
            {
                if(clause_lit_count[c]==2 && added_tmp_out==false)
                {
                    added_tmp_out=true;
                    tmp_out=clause_lit[c][clause_lit_count[c]-2].var_num;
                }
                else if(clause_lit_count[c]>2)
                {
                    count3=true;
                }
            }
            else if(cur_gate!=clause_lit[c][clause_lit_count[c]-1].var_num && re_traverse==false && clause_delete[c]!=1)
            {
                c=first_clauses;
                gate[clause_lit[c][clause_lit_count[c]-1].var_num].first_clauses=first_clauses;
                re_traverse=true;
                cur_gate=clause_lit[c][clause_lit_count[c]-1].var_num;
                c--;//��ȥfor ѭ��c++ ��Ӱ��
            }
            else if(cur_gate==clause_lit[c][clause_lit_count[c]-1].var_num && re_traverse==true && clause_delete[c]!=1)
            {
                if(count3==true && added_tmp_out==true)
                {
                    if(clause_lit_count[c]>2)
                    {
                        int clause_var_tmp_index=clause_lit_count[c]-3;
                        while(clause_var_tmp_index>=0)
                        {
                            clause_lit[c][clause_var_tmp_index].B_Node_attr=2;
                            blocked_edge[clause_lit[c][clause_var_tmp_index].var_num] = clause_lit[c][clause_lit_count[c]-1].var_num;
                            clause_var_tmp_index--;
                        }
                    }
                }
            }
            else if(clause_delete[c]!=1)
            {
                if(blocked_edge.size()>0)
                {
                    Blocked_Edge.push_back(blocked_edge);
                    //cout<<"blocked_size: "<<Blocked_Edge.size()<<endl;
                }
                tmp_out=0;
                added_tmp_out=false;
                re_traverse=false;
                first_clauses=c;
                gate[cur_gate].last_clauses=c-1;
                blocked_edge.clear();
                cur_gate=clause_lit[c][clause_lit_count[c]-1].var_num ;
                c--;
            }
        }
    }
//    for(int c=0;c<num_clauses;c++)
//    {
//        if(clause_lit_count[c]<3)
//        {
//            tmp_unit_var=clause_lit[c][clause_lit_count[c]-1].var_num;
//            if(gate[tmp_unit_var].dominat_attr==1 ||gate[tmp_unit_var].dominat_attr==4 )
//            {
//                unit_clause[unit_clause_count++]=clause_lit[c][clause_lit_count[c]-1];
//                unit_clause[unit_clause_count++]=clause_lit[c][clause_lit_count[c]-2];
//                //assign1();
//            }
//        }
//    }
//    int tmp_lit_var=0;
//    for(int c=0;c<num_clauses;c++)
//    {
//        if(clause_delete[c]==1)
//        {
//            continue;
//        }
//        if(tmp_lit_var!=clause_lit[c][clause_lit_count[c]-1].var_num)
//        {
//            tmp_lit_var=clause_lit[c][clause_lit_count[c]-1].var_num;
//            if(gate[tmp_lit_var].dominat_attr==1 || gate[tmp_lit_var].dominat_attr==4 )
//            {
//                Dominated[Dominated_count++]=tmp_lit_var;
//                //assign1();
//            }
//        }
//    }
    for(int i=0;i<num_vars;i++)
    {
        if(gate[i].dominat_attr==1 || gate[i].dominat_attr==4)
            Dominated[Dominated_count++]=i;
    }


    Blocked_Edge.clear();
    int iteration=0;
    while(Dominated_count!=0)
    {

        dominated_gate=Dominated[Dominated_count-1];
        Dominated_count--;
        int first = gate[dominated_gate].first_clauses;
        int last = gate[dominated_gate].last_clauses;
        if(first<0 || last<0 || first>=num_clauses || last>=num_clauses || first>last)
        {
            continue;
        }
        for(int i=first;i<=last;i++)
        {
            if(clause_delete[i]==1)
            {
                continue;
            }
            if(clause_lit_count[i]==2 )
            {
                unit_clause[unit_clause_count++]=clause_lit[i][clause_lit_count[i]-1];
                unit_clause[unit_clause_count++]=clause_lit[i][clause_lit_count[i]-2];
            }
        }
    }
    int pre_unit_clause_count=0;
    while(Filtered_Node_count !=0 || unit_clause_count!=pre_unit_clause_count ||!Blocked_Edge.empty())
    {
        iteration++;
        cout<<"Iteration:"<<iteration<<endl;
        pre_unit_clause_count=unit_clause_count;
        // ���Դ���
            for(int c=0;c<num_clauses;c++)
            {
                for(int i=0;i<clause_lit_count[c];i++)
                {
                    if(clause_delete[c]!=1){
                        if(clause_lit[c][i].sense==false)
                        {
                            //cout<<"-"<<clause_lit[c][i].var_num<<" ";
                        }
                        //else cout<<clause_lit[c][i].var_num<<" ";
                    }
                }
                //cout<<endl;
            }
        while(Filtered_Node_count!=0)
        {
//            cout<<"while Filtered_Node_count"<<Filtered_Node_count<<endl;
//            cout<<"Filtered_Node_count1:"<<endl;
//            cout<<unit_clause_count<<" "<<pre_unit_clause_count<<" "<<Blocked_Edge.size()<<endl;
            filtered_gate=Filtered_Node[Filtered_Node_count-1];
            //Filtered_Node.pop_back();
            Filtered_Node_count--;
//            cout<<"Filtered_Node_count2"<<Filtered_Node_count<<endl;
//            cout<<"filtered_gate"<<filtered_gate<<endl;
            while(!gate[filtered_gate].in_vars.empty())
            {
                //cout<<gate[filtered_gate].in_vars.size()<<endl;
                blocked_edge[gate[filtered_gate].in_vars[gate[filtered_gate].in_vars.size()-1]] = filtered_gate;
                gate[filtered_gate].in_vars.pop_back();
            }
            Filtered_Edge.push_back(blocked_edge);
        }//�������Žڵ㲻Ϊ�գ����ýڵ������߶�����Ϊ���˱�


        if(unit_clause_count>0)//��Ԫ�Ӿ䴫������ܻ����blocked_Edge
        {
//            cout<<"unit_clause_count"<<endl;
            assign1();
            for(int i=0;i<=num_clauses;i++)
            {
                if(clause_delete[i]==1)
                    continue;
                else{
                    cur_gate=clause_lit[i][clause_lit_count[i]-1].var_num;
                    first_clauses=i;
                    last_clauses=i;
                    break;
                }
            }
            for(int c=0;c<num_clauses;c++)
            {
                if(clause_delete[c]==1)
                {
                    continue;
                }
                if(cur_gate==clause_lit[c][clause_lit_count[c]-1].var_num && re_traverse==false  && clause_delete[c]!=1)
                {
                    if(clause_lit_count[c]==2 && added_tmp_out==false)
                    {
                        added_tmp_out=true;
                        tmp_out=clause_lit[c][clause_lit_count[c]-2].var_num;
                    }
                    else if(clause_lit_count[c]>2)
                    {
                        count3=true;
                    }
                }
                else if(cur_gate!=clause_lit[c][clause_lit_count[c]-1].var_num && re_traverse==false && clause_delete[c]!=1)
                {
                    c=first_clauses;
                    gate[clause_lit[c][clause_lit_count[c]-1].var_num].first_clauses=first_clauses;
                    re_traverse=true;
                    cur_gate=clause_lit[c][clause_lit_count[c]-1].var_num;
                    c--;
                }
                else if(cur_gate==clause_lit[c][clause_lit_count[c]-1].var_num && re_traverse==true && clause_delete[c]!=1)
                {
                    if(count3==true && added_tmp_out==true)
                    {
                        if(clause_lit_count[c]>2)
                        {
                            int clause_var_tmp_index=clause_lit_count[c]-3;
                            while(clause_var_tmp_index>=0)
                            {
                                clause_lit[c][clause_var_tmp_index].B_Node_attr=2;
                                blocked_edge[clause_lit[c][clause_var_tmp_index].var_num] = clause_lit[c][clause_lit_count[c]-1].var_num;
                                clause_var_tmp_index--;
                            }
                        }
                    }
                }
                else if(clause_delete[c]!=1)
                {
                    if(blocked_edge.size()>0)
                    {
                        Blocked_Edge.push_back(blocked_edge);
                        //cout<<"blocked_size: "<<Blocked_Edge.size()<<endl;
                    }
                    tmp_out=0;
                    added_tmp_out=false;
                    re_traverse=false;
                    first_clauses=c;
                    gate[cur_gate].last_clauses=c-1;
                    blocked_edge.clear();
                    cur_gate=clause_lit[c][clause_lit_count[c]-1].var_num ;
                    c--;
                }
            }
        }
        int z_var=0;
        int out_gate_var=0;
        int in_gate_var=0;
        //Blocked_Edge ���ܲ���Dominated �ű���
        for(int i=0;i<Blocked_Edge.size();i++)
        {
//            cout<<"Blocked_Edge"<<endl;
            map<int,int> p = Blocked_Edge[i];
            for(map<int,int>::iterator it=p.begin();it!=p.end();it++)
            {
                //cout<<it->first<<" "<<it->second<<endl;
                z_var=it->first;
                out_gate_var=it->second;
                in_gate_var=in_gate[it->first];
                //�����Ŵ�����ߵ�����ż�����ɾ��
                int tmp_index_gate=0;
                int tmp_gate=0;
                while(var_gate_neighbor[z_var][tmp_index_gate]!=out_gate_var)
                {
                    tmp_index_gate++;//�ҵ�out_gate_var ��index
                }
                var_gate_neighbor[z_var][tmp_index_gate]=var_gate_neighbor[z_var][var_gate_neighbor_count[z_var]-1];
                var_gate_neighbor_count[z_var]--;
                int tmp_dominated;
                if(var_gate_neighbor_count[z_var]==1)
                {
                    out_gate_var=var_gate_neighbor[z_var][tmp_index_gate];
                    gate[in_gate_var].dominator=gate[out_gate_var].gate_name;
                    //Dominated[Dominated_count++]=in_gate_var;

                    if(gate[in_gate_var].dominat_attr==0  || gate[in_gate_var].dominat_attr==1)
                    {
                        gate[in_gate_var].dominat_attr=1;
                    }
                    else
                    {
                        gate[in_gate_var].dominat_attr=4;
                    }
                    if(gate[out_gate_var].dominat_attr==0 || gate[out_gate_var].dominat_attr==2)
                    {
                        gate[out_gate_var].dominat_attr=2;
                    }
                    else
                    {
                        gate[out_gate_var].dominat_attr=4;
                    }
                }
                else if(var_gate_neighbor_count[z_var]==0)
                {
                    Filtered_Node[Filtered_Node_count++]=in_gate[z_var];
                }
            }
        }
        Blocked_Edge.clear();
        Dominated_count=0;
        for(int i=0;i<num_vars;i++)
        {
            if(gate[i].dominat_attr==1 || gate[i].dominat_attr==4)
            {
                Dominated[Dominated_count++]=i;
            }
        }
        while(Dominated_count!=0)
        {
//            cout<<"Dominated_count"<<endl;
            dominated_gate=Dominated[Dominated_count-1];
            Dominated_count--;
            int first = gate[dominated_gate].first_clauses;
            int last = gate[dominated_gate].last_clauses;
            if(first<0 || last<0 || first>=num_clauses || last>=num_clauses || first>last)
            {
                continue;
            }
            for(int i=first;i<=last;i++)
            {
                if(clause_delete[i]==1)
                {
                    continue;
                }
                if(clause_lit_count[i]==2)
                {
                    unit_clause[unit_clause_count++]=clause_lit[i][clause_lit_count[i]-1];
                    unit_clause[unit_clause_count++]=clause_lit[i][clause_lit_count[i]-2];
                }
            }
        }

    }
    // ���Դ���
    while(!q.empty())
        q.pop();
    vector<int> v=get_var_clause();
    const char* file_name="remain_clauses1.txt";
    write_remaining_clauses(file_name,v);
    //return v[v.size()-1];

//    assign();
//    int cur_gate=clause_lit[0][clause_lit_count[0]-1].var_num;
//    int first_clauses=0;
//    int last_clauses=0;
////    vector<int> Blocked_edge;
//    int tmp_out=0;
//    map<int,int> blocked_edge;
//    bool added_tmp_out=false;
//    bool count3=false;
//    bool re_traverse=false;

//���´����ǵ���ʹ�ã������Ԫ����֮����Ӿ�
//    ofstream fout("remain_clauses_1.txt");
//    cout<<"WRITE:"<<endl;
//    for(int c=0;c<num_clauses-gate_num;c++)
//    {
//        if(clause_delete[c]!=1){
//            fout<<gate_num+1<<" ";
//            for(int i=0;i<clause_lit_count[c];i++)
//            {
//
//                if(clause_lit[c][i].sense==false)
//                {
//                    fout<<"-"<<clause_lit[c][i].var_num<<" ";
//                }
//                else fout<<clause_lit[c][i].var_num<<" ";
//            }
//        }
//        if(clause_delete[c]!=1)
//        {
//            fout<<"0"<<endl;
//        }
//    }
//    for(int c=num_clauses-gate_num;c<num_clauses;c++)
//    {
//        if(clause_delete[c]!=1){
//            fout<<1<<" ";
//            for(int i=0;i<clause_lit_count[c];i++)
//            {
//
//                if(clause_lit[c][i].sense==false)
//                {
//                    fout<<"-"<<clause_lit[c][i].var_num<<" ";
//                }
//                else fout<<clause_lit[c][i].var_num<<" ";
//            }
//        }
//        if(clause_delete[c]!=1)
//        {
//            fout<<"0"<<endl;
//        }
//    }

//
//    for(int c=0;c<num_clauses;c++)
//    {
//        if(clause_delete[c]==1)
//        {
//            continue;
//        }
//        if(cur_gate==clause_lit[c][clause_lit_count[c]-1].var_num && re_traverse==false  && clause_delete[c]!=1)
//        {
//            if(clause_lit_count[c]==2 && added_tmp_out==false)
//            {
//                added_tmp_out=true;
//                tmp_out=clause_lit[c][clause_lit_count[c]-2].var_num;
//
//            }
//            else if(clause_lit_count[c]>2)
//            {
//                count3=true;
//            }
//
//        }
//        else if(cur_gate!=clause_lit[c][clause_lit_count[c]-1].var_num && re_traverse==false && clause_delete[c]!=1)
//        {
//            c=first_clauses;
//            gate[clause_lit[c][clause_lit_count[c]-1].var_num].first_clauses=first_clauses;
//            re_traverse=true;
//            cur_gate=clause_lit[c][clause_lit_count[c]-1].var_num;
//            c--;
//
//        }
////        else if(cur_gate!=clause_lit[c][clause_lit_count[c]-1].var_num && re_traverse==true)
////        {
////
////        }
//        else if(cur_gate==clause_lit[c][clause_lit_count[c]-1].var_num && re_traverse==true && clause_delete[c]!=1)
//        {
//            if(count3==true && added_tmp_out==true)
//            {
//                if(clause_lit_count[c]>2)
//                {
//                    int clause_var_tmp_index=clause_lit_count[c]-3;
//                    while(clause_var_tmp_index>=0)
//                    {
//                        clause_lit[c][clause_var_tmp_index].B_Node_attr=2;
//                        blocked_edge[clause_lit[c][clause_var_tmp_index].var_num] = clause_lit[c][clause_lit_count[c]-1].var_num;
//                        //cout<<"vars:"<<clause_lit[c][clause_lit_count[c]-1].var_num<<endl;
//                        //cout<<blocked_edge[clause_lit[c][clause_var_tmp_index].var_num]<<" "<<clause_lit[c][clause_lit_count[c]-1].var_num<<endl;
//                        clause_var_tmp_index--;
//                        //blocked_edge.insert(pair<int,int>(clause_lit[c][clause_var_tmp_index].var_num,clause_lit[c][clause_lit_count[c]-1].var_num);
//                    }
//                }
//            }
//        }
//        else if(clause_delete[c]!=1)
//        {
//            if(blocked_edge.size()>0)
//            {
//                Blocked_Edge.push_back(blocked_edge);
//                cout<<"blocked_size: "<<Blocked_Edge.size()<<endl;
//            }
//
//            //tmp_out.erase(tmp_out.size()-1);
//            tmp_out=0;
//            added_tmp_out=false;
//            re_traverse=false;
//            first_clauses=c;
//            gate[cur_gate].last_clauses=c-1;
//            blocked_edge.clear();
//            cur_gate=clause_lit[c][clause_lit_count[c]-1].var_num ;
//            c--;
//        }
//
//    }
//    for(int i=1;i<6;i++)
//    {
//        cout<<gate[i].first_clauses<<" "<<gate[i].last_clauses<<endl;
//    }

//    for(int i=0;i<Blocked_Edge.size();i++){
//        map<int,int> p = Blocked_Edge[i];
//        for(map<int,int>::iterator it=p.begin();it!=p.end();it++)
//            cout<<it->first<<" "<<it->second<<endl;
//    }
    //�ж��ȳ��ڵ���blocked_edge ���Žڵ��Ƿ��Ǳ�ͳ�ε�
    //�ж�Blocked_Edge �ڵ�(Filtered_Edge)�Ƿ�������Filtered_Node



}

void Satlike::assign_F()
{
    while(unit_clause_count_saved>0)
    {
        unit_clause_count_saved--;
        //cout<<"unit:"<<unit_clause_saved[unit_clause_count_saved].var_num;
        unit_clause[unit_clause_count++]=unit_clause_saved[unit_clause_count_saved];
    }
    while(unit_clause_count>0)
    {
        unit_clause_count--;
//        cout<<unit_clause[unit_clause_count].var_num<<endl;
        int v=unit_clause[unit_clause_count].var_num;
        int sense=unit_clause[unit_clause_count].sense;
        F_value[v]=sense; //������assign1
        //cout<<"F_assigned: "<<v<<endl;
        int c,l;
        for(int i=0;i<var_lit_count[v];++i)
        {
            c=var_lit[v][i].clause_num;
            if(clause_delete[c]==1) continue;

            if(sense==var_lit[v][i].sense)
            {
                clause_delete[c]=1;
                continue;
            }
            int swap_lit_var;
            bool swap_lit_sense;
            for(int j=0;j<clause_lit_count[c];j++)
            {
                if(clause_lit[c][j].var_num==v)
                {
                    for(int ii=j+1;ii<clause_lit_count[c];ii++)
                    {
                        lit tmp = clause_lit[c][ii-1];
                        clause_lit[c][ii-1] = clause_lit[c][ii];
                        clause_lit[c][ii] = tmp;
                    }
                    clause_lit_count[c]--;
                    break;
                }
            }
            if(clause_lit_count[c]==1)
            {
                unit_clause[unit_clause_count++]=clause_lit[c][0];

            }
        }
    }

//    cout<<"after assign F:"<<endl;
//    for(int c=0;c<num_clauses;c++)
//    {
//        for(int i=0;i<clause_lit_count[c];i++)
//        {
//            if(clause_delete[c]!=1){
//                if(clause_lit[c][i].sense==false)
//                {
//                    cout<<"-"<<clause_lit[c][i].var_num<<" ";
//                }
//                else cout<<clause_lit[c][i].var_num<<" ";
//            }
//        }
//        cout<<endl;
//    }
}
void Satlike::recover_clauses_lit()
{


    //�ָ��洢ֵ
    for(int c=0;c<num_clauses;c++)
    {
        clause_delete[c]=clause_delete_saved[c];

        clause_lit_count[c]=clause_lit_count_saved[c];
        //clause_lit_saved[c]=new lit[clause_lit_count_saved[c]+1];
        for(int v=0;v<clause_lit_count_saved[c];v++)
        {
            clause_lit[c][v]=clause_lit_saved[c][v];
        }
    }
//    cout<<"�ָ�CHECK!"<<endl;
    for(int c=0;c<num_clauses;c++)
    {
        if(clause_delete[c]!=1){
            for(int i=0;i<clause_lit_count[c];i++)
            {

                if(clause_lit[c][i].sense==false)
                {
                    //cout<<"-"<<clause_lit[c][i].var_num<<" ";
                }
                //else cout<<clause_lit[c][i].var_num<<" ";
            }
            //cout<<endl;
        }

    }

}
void Satlike::assign_B()
{

//    for(int i=1;i<=num_vars;i++)
//    {
//        cout<<"i_F_value: "<<i<<"  "<<F_value[i]<<endl;
//    }
    lit tmp_lit;
    queue<lit> q_com;
    while(!unit_out_clause.empty())
    {
        tmp_lit=unit_out_clause.front();
        unit_out_clause.pop();
        int v=tmp_lit.var_num;
        int sense=tmp_lit.sense;
        assigned_B[v]=sense;
        //cout<<"unit_out_clause: "<<sense<<" "<<v<<endl;
        if(assigned_B[v]==F_value[v])
        {
            tmp_lit.var_num=v;
            tmp_lit.sense=F_value[v];
            unit_clause[unit_clause_count++]=tmp_lit;
            tmp_lit.var_num=in_gate[v];
            tmp_lit.sense=0;
            unit_clause[unit_clause_count++]=tmp_lit;
        }
        int c,l;
        for(int i=0;i<var_lit_count[v];++i)
        {
            c=var_lit[v][i].clause_num;
            if(clause_delete[c]==1) continue;

            if(sense==var_lit[v][i].sense)
            {
                clause_delete[c]=1;//B==F
                continue;
            }
            int swap_lit_var;
            bool swap_lit_sense;
            if(clause_lit_count[c]>2){
                for(int j=0;j<clause_lit_count[c];j++)
                {
                    if(clause_lit[c][j].var_num==v )
                    {
                        for(int ii=j+1;ii<clause_lit_count[c];ii++)
                        {
                            lit tmp = clause_lit[c][ii-1];
                            clause_lit[c][ii-1] = clause_lit[c][ii];
                            clause_lit[c][ii] = tmp;
                        }
                        clause_lit_count[c]--;
                        //if(clause_lit_count[c]==2 && assigned_B[clause_lit[c][0].var_num]==-1 && (F_value[v]==B1_value[v] || F_value[v]==B2_value[v]))

                        if(clause_lit_count[c]==2)
                        {
                            tmp_lit.var_num=clause_lit[c][0].var_num;
                            tmp_lit.sense=clause_lit[c][0].sense;
                            if(B1_value[tmp_lit.var_num]==-1 || tmp_lit.sense==B1_value[tmp_lit.var_num])
                            {
                                B1_value[tmp_lit.var_num]=tmp_lit.sense;
                                B1_num[tmp_lit.var_num]++;
                                q_com.push(tmp_lit);
                            }
                            else if(tmp_lit.sense!=B1_value[tmp_lit.var_num])
                            {
                                B2_value[tmp_lit.var_num]=tmp_lit.sense;
                                B2_num[tmp_lit.var_num]++;
                            }
                        }
                        break;
                    }
                }
            }
        }
        while(!q_com.empty())
        {
            tmp_lit=q_com.front();
            q_com.pop();
            if(B2_value[tmp_lit.var_num]==-1 && B1_value[tmp_lit.var_num]==F_value[tmp_lit.var_num])
            {
                unit_out_clause.push(tmp_lit);
            }
        }
    }
}

/*
void Satlike::assign_B()
{
    lit tmp_lit;
    while(!unit_out_clause.empty())
    {
        tmp_lit=unit_out_clause.front();
        unit_out_clause.pop();
        int v=tmp_lit.var_num;
        int sense=tmp_lit.sense;
        assigned_B[v]=sense;
        cout<<"unit_out_clause: "<<sense<<" "<<v<<endl;
        if(B1_value[v]==-1)
        {
            B1_value[v]=sense;//�������B1��ֵ(Bֵֻ��B1ֵ��
            if(v==16)
            {
                cout<<"v==16"<<endl;
            }
            if(B1_value[v]==F_value[v])
            {
                tmp_lit.var_num=v;
                tmp_lit.sense=F_value[v];
                unit_clause[unit_clause_count++]=tmp_lit;
                tmp_lit.var_num=in_gate[v];
                tmp_lit.sense=0;
                unit_clause[unit_clause_count++]=tmp_lit;
            }
        }
        else if(B1_value[v]!=sense)
        {
            B2_value[v]=sense;
            tmp_lit.sense=F_value[v];
            tmp_lit.var_num=v;
            unit_clause[unit_clause_count++]=tmp_lit;
            tmp_lit.var_num=in_gate[v];
            tmp_lit.sense=0;
            unit_clause[unit_clause_count++]=tmp_lit;
        }
        int c,l;
        for(int i=0;i<var_lit_count[v];++i)
        {
            c=var_lit[v][i].clause_num;
            if(clause_delete[c]==1) continue;

            if(sense==var_lit[v][i].sense)
            {
                clause_delete[c]=1;//B==F
                continue;
            }
            int swap_lit_var;
            bool swap_lit_sense;
            if(clause_lit_count[c]>2){
                for(int j=0;j<clause_lit_count[c];j++)
                {
                    if(clause_lit[c][j].var_num==v )
                    {
                        for(int ii=j+1;ii<clause_lit_count[c];ii++)
                        {
                            lit tmp = clause_lit[c][ii-1];
                            clause_lit[c][ii-1] = clause_lit[c][ii];
                            clause_lit[c][ii] = tmp;
                        }
                        clause_lit_count[c]--;
                        //if(clause_lit_count[c]==2 && assigned_B[clause_lit[c][0].var_num]==-1 && (F_value[v]==B1_value[v] || F_value[v]==B2_value[v]))

                        if(clause_lit_count[c]==2  && (F_value[v]==B1_value[v] || F_value[v]==B2_value[v]))
                        {
                            tmp_lit.var_num=clause_lit[c][0].var_num;
                            tmp_lit.sense=clause_lit[c][0].sense;
                            //tmp_lit.sense=F_value[tmp_lit.var_num];
                            unit_out_clause.push(tmp_lit);
                            cout<<"v: "<<v<<" B1_value: "<<B1_value[v]<<" B2_value: "<<B2_value[v]<<endl;
                            //cout<<tmp_lit.var_num<<endl;
                        }
                        break;
                    }
                }
            }
//    cout<<"after assign B:"<<endl;
//    for(int c=0;c<num_clauses;c++)
//    {
//        for(int i=0;i<clause_lit_count[c];i++)
//        {
//            if(clause_delete[c]!=1){
//                if(clause_lit[c][i].sense==false)
//                {
//                    cout<<"-"<<clause_lit[c][i].var_num<<" ";
//                }
//                else cout<<clause_lit[c][i].var_num<<" ";
//            }
//        }
//        cout<<endl;
//    }
        }
    }
}
*/


int Satlike::gain_F_value()
{
    for(int i=1;i<=num_vars;i++)
    {
        F_value[i]=-1;
        B1_value[i]=-1;
        B2_value[i]=-1;
        B1_num[i]=0;
        B2_num[i]=0;
        assigned_B[i]=-1;
    }
    //ofstream fout("remain_clauses.txt");
    int remaining_clauses_num=0;

    assign_F();//�ڲ������Լ����F��ֵ
    recover_clauses_lit();
    assign_B();
    recover_clauses_lit();
    int filtered_gate=0;
    int dominated_gate=0;
    int last_clauses;
    int cur_gate;
    int first_clauses;
    int tmp_out=0;
    map<int,int> blocked_edge;
    bool added_tmp_out=false;
    bool count3=false;
    bool re_traverse=false;
    int in_var=0;

    int pre_unit_clause_count=0;
    bool new_dominated=false;
    int iteration=0;
    while(iteration<1 && (!unit_out_clause.empty() || Filtered_Node_count!=0 || unit_clause_count!=pre_unit_clause_count || new_dominated==true || !Blocked_Edge.empty()))
    {

        iteration++;
        new_dominated=false;
        //cout<<"Iteration:"<<iteration<<endl;
        pre_unit_clause_count=unit_clause_count;
        //cout<<"pre_UNIT_num:"<<pre_unit_clause_count<<endl;
        //cout<<"pre_UNIT_num:"<<pre_unit_clause_count<<endl;
        //cout<<"pre_UNIT_num:"<<pre_unit_clause_count<<endl;

        while(Filtered_Node_count!=0)
        {
//            cout<<"while Filtered_Node_count"<<Filtered_Node_count<<endl;
//            cout<<"Filtered_Node_count1:"<<endl;
//            cout<<unit_clause_count<<" "<<pre_unit_clause_count<<" "<<Blocked_Edge.size()<<endl;
            filtered_gate=Filtered_Node[Filtered_Node_count-1];
            //Filtered_Node.pop_back();
            Filtered_Node_count--;
//            cout<<"Filtered_Node_count2"<<Filtered_Node_count<<endl;
//            cout<<"filtered_gate"<<filtered_gate<<endl;
            while(!gate[filtered_gate].in_vars.empty())
            {
                //cout<<gate[filtered_gate].in_vars.size()<<endl;
                blocked_edge[gate[filtered_gate].in_vars[gate[filtered_gate].in_vars.size()-1]] = filtered_gate;
                gate[filtered_gate].in_vars.pop_back();
            }
            Filtered_Edge.push_back(blocked_edge);
        }//�������Žڵ㲻Ϊ�գ����ýڵ������߶�����Ϊ���˱�


        if(unit_clause_count>0)//��Ԫ�Ӿ䴫������ܻ����blocked_Edge
        {
//            cout<<"unit_clause_count"<<endl;
            assign1();
            for(int i=0;i<=num_clauses-gate_num-out_len;i++)
            {
                if(clause_delete[i]==1)
                    continue;
                else{
                    cur_gate=clause_lit[i][clause_lit_count[i]-1].var_num;
                    first_clauses=i;
                    last_clauses=i;
                    break;
                }
            }
            for(int c=0;c<num_clauses-gate_num-out_len;c++)
            {
                if(clause_delete[c]==1)
                {
                    continue;
                }
                if(cur_gate==clause_lit[c][clause_lit_count[c]-1].var_num && re_traverse==false  && clause_delete[c]!=1)
                {
                    if(clause_lit_count[c]==2 && added_tmp_out==false)
                    {
                        added_tmp_out=true;
                        tmp_out=clause_lit[c][clause_lit_count[c]-2].var_num;
                    }
                    else if(clause_lit_count[c]>2)
                    {
                        count3=true;
                    }
                }
                else if(cur_gate!=clause_lit[c][clause_lit_count[c]-1].var_num && re_traverse==false && clause_delete[c]!=1)
                {
                    c=first_clauses;
                    gate[clause_lit[c][clause_lit_count[c]-1].var_num].first_clauses=first_clauses;
                    re_traverse=true;
                    cur_gate=clause_lit[c][clause_lit_count[c]-1].var_num;
                    c--;
                }
                else if(cur_gate==clause_lit[c][clause_lit_count[c]-1].var_num && re_traverse==true && clause_delete[c]!=1)
                {
                    if(count3==true && added_tmp_out==true)
                    {
                        if(clause_lit_count[c]>2)
                        {
                            int clause_var_tmp_index=clause_lit_count[c]-3;
                            while(clause_var_tmp_index>=0)
                            {
                                clause_lit[c][clause_var_tmp_index].B_Node_attr=2;
                                blocked_edge[clause_lit[c][clause_var_tmp_index].var_num] = clause_lit[c][clause_lit_count[c]-1].var_num;
                                //cout<<"blocked_edge:"<<clause_lit[c][clause_var_tmp_index].var_num<<clause_lit[c][clause_lit_count[c]-1].var_num<<endl;
                                clause_var_tmp_index--;
                            }
                        }
                    }
                }
                else if(clause_delete[c]!=1)
                {
                    if(blocked_edge.size()>0)
                    {
                        Blocked_Edge.push_back(blocked_edge);
                        //cout<<"blocked_size: "<<Blocked_Edge.size()<<endl;
                    }
                    tmp_out=0;
                    added_tmp_out=false;
                    re_traverse=false;
                    first_clauses=c;
                    gate[cur_gate].last_clauses=c-1;
                    blocked_edge.clear();
                    cur_gate=clause_lit[c][clause_lit_count[c]-1].var_num ;
                    c--;
                }
            }
        }
        int z_var=0;
        int out_gate_var=0;
        int in_gate_var=0;
        //Blocked_Edge ���ܲ���Dominated �ű���
        for(int i=0;i<Blocked_Edge.size();i++)
        {
//            cout<<"Blocked_Edge"<<endl;
            map<int,int> p = Blocked_Edge[i];
            for(map<int,int>::iterator it=p.begin();it!=p.end();it++)
            {
                //cout<<it->first<<" "<<it->second<<endl;
                z_var=it->first;
                out_gate_var=it->second;
                in_gate_var=in_gate[it->first];
                if(z_var<=0 || z_var>num_vars) continue;
                if(var_gate_neighbor_count[z_var]<=0) continue;
                //�����Ŵ�����ߵ�����ż�����ɾ��
                int tmp_index_gate=0;
                int tmp_gate=0;
                int neighbor_count = var_gate_neighbor_count[z_var];
                while(tmp_index_gate<neighbor_count && var_gate_neighbor[z_var][tmp_index_gate]!=out_gate_var)
                {
                    tmp_index_gate++;//�ҵ�out_gate_var ��index
                    //cout<<var_gate_neighbor[z_var][tmp_index_gate]<<"  "<<out_gate_var<<endl;
                }
                if(tmp_index_gate>=neighbor_count) continue;
                var_gate_neighbor[z_var][tmp_index_gate]=var_gate_neighbor[z_var][var_gate_neighbor_count[z_var]-1];
                var_gate_neighbor_count[z_var]--;
                int tmp_dominated;
                if(var_gate_neighbor_count[z_var]==1)//���������ֻ���뵽һ������
                {
                    out_gate_var=var_gate_neighbor[z_var][tmp_index_gate];
                    gate[in_gate_var].dominator=gate[out_gate_var].gate_name;
                    //Dominated[Dominated_count++]=in_gate_var;

                    if(gate[in_gate_var].dominat_attr==0  || gate[in_gate_var].dominat_attr==1)
                    {
                        if(gate[in_gate_var].dominat_attr==0)
                        {
                            new_dominated=true;
                        }
                        gate[in_gate_var].dominat_attr=1;

                        //�ж�in_gate_var �ű�����������ص��ű���A�������ص��ű����Ƿ�����ͬ��ͳ����B
                        //���ǵĻ�����A������Ϊ��ͳ����,ͳ������B
                        int in_var_num=gate[in_gate_var].in_vars.size();

                        while(in_var_num>0)
                        {
                            int tmp_in_var=gate[in_gate_var].in_vars[in_var_num-1];//z����
                            in_var_num--;
                            int tmp_I_var=in_gate[tmp_in_var];//z������������I
                            if(tmp_I_var==0)
                            {
                                continue;
                            }
                            int tmp_out_var=gate[tmp_I_var].out_var;//I���������Z����
                            bool tmp_flag=false;
                            int tmp_var_gate_neighbor_count=var_gate_neighbor_count[tmp_out_var];
                            int tmp_gate_var;//��I��ͨ��Z�������ӵ������ű���
                            string dominator_gate;
                            while(tmp_var_gate_neighbor_count>0 && tmp_flag==false)
                            {

                                tmp_gate_var=var_gate_neighbor[tmp_out_var][tmp_var_gate_neighbor_count-1];
                                if(tmp_gate_var==tmp_I_var)
                                {
                                    tmp_var_gate_neighbor_count--;
                                    continue;
                                }
                                if(dominator_gate.size()==0)
                                {
                                    dominator_gate=gate[tmp_gate_var].dominator;
                                }
                                else if(dominator_gate!=gate[tmp_gate_var].dominator || (gate[tmp_gate_var].dominat_attr!=1 && gate[tmp_gate_var].dominat_attr!=4 ))
                                {
                                    tmp_flag=true;
                                }
                                tmp_var_gate_neighbor_count--;
                            }

                            if(tmp_flag==false)//˵�������ű������Ǳ�dominator_gate ͳ�ε�
                            {
                                new_dominated=true;
                                gate[tmp_I_var].dominator=dominator_gate;
                                if(gate[tmp_I_var].dominat_attr==0 || gate[tmp_I_var].dominat_attr==1)
                                {
                                    gate[tmp_I_var].dominat_attr=1;
                                }
                                else
                                {
                                    gate[tmp_I_var].dominat_attr=4;
                                }

                            }
                        }

                    }
                    else
                    {
                        gate[in_gate_var].dominat_attr=4;
                    }
                    if(gate[out_gate_var].dominat_attr==0 || gate[out_gate_var].dominat_attr==2)
                    {
                        gate[out_gate_var].dominat_attr=2;
                    }
                    else
                    {
                        gate[out_gate_var].dominat_attr=4;
                    }
                }
                else if(var_gate_neighbor_count[z_var]==0)
                {
                    Filtered_Node[Filtered_Node_count++]=in_gate[z_var];
                }
            }
        }
        Blocked_Edge.clear();
        Dominated_count=0;
        for(int i=0;i<num_vars;i++)
        {
            if(gate[i].dominat_attr==1 || gate[i].dominat_attr==4)
            {
                Dominated[Dominated_count++]=i;
                //cout<<i<<"is dominated"<<endl;
            }
        }
        while(Dominated_count!=0)
        {
//            cout<<"Dominated_count"<<endl;
            dominated_gate=Dominated[Dominated_count-1];
            Dominated_count--;
            for(int i=gate[dominated_gate].first_clauses;i<=gate[dominated_gate].last_clauses;i++)
            {
                if(clause_delete[i]==1)
                {
                    continue;
                }
                if(clause_lit_count[i]==2)
                {
                    unit_clause[unit_clause_count++]=clause_lit[i][clause_lit_count[i]-1];
                    unit_clause[unit_clause_count++]=clause_lit[i][clause_lit_count[i]-2];
                }
            }
        }
//        cout<<"UNIT_num:"<<unit_clause_count<<endl;
    }

    //���Դ���
    int reduce2=0;
    for(int c=0;c<num_clauses-gate_num-out_len-in_len;c++)
    {
        if(clause_delete[c]!=1){
            for(int i=0;i<clause_lit_count[c];i++)
            {

                if(clause_lit[c][i].sense==false)
                {
                    //cout<<"-"<<clause_lit[c][i].var_num<<" ";
                    //fout<<"-"<<clause_lit[c][i].var_num<<" ";
                }
                else{
                    //cout<<clause_lit[c][i].var_num<<" ";
                    //fout<<clause_lit[c][i].var_num<<" ";
                }
            }
            reduce2++;
           // cout<<"0"<<endl;
            remaining_clauses_num++;

            //cout<<"c:"<<c<<endl;
        }
    }
    int hard_clauses_num=remaining_clauses_num;
//    cout<<"remaing_clauses: "<<remaining_clauses_num<<endl;
    for(int c=num_clauses-gate_num-out_len;c<num_clauses;c++)
    {

        if(clause_lit[c][0].sense==false)
        {
            //cout<<"-"<<clause_lit[c][0].var_num<<" 0"<<endl;
            //fout<<"-"<<clause_lit[c][0].var_num<<" 0"<<endl;
        }
        else{
            //cout<<clause_lit[c][0].var_num<<" 0"<<endl;
            //fout<<clause_lit[c][0].var_num<<" 0"<<endl;
        }
        remaining_clauses_num++;
    }
    cout<<remaining_clauses_num<<" "<<remaining_clauses_num-hard_clauses_num-out_len<<endl;
    //cout<<"C2: "<<reduce2-out_len-gate_num<<" I2: "<<iteration<<"  ";

/*
    for(int c=0;c<num_clauses-gate_num-out_len;c++)
    {
        if(clause_delete[c]!=1){
            for(int i=0;i<clause_lit_count[c];i++)
            {

                if(clause_lit[c][i].sense==false)
                {
                    fout<<"-"<<clause_lit[c][i].var_num<<" ";
                    cout<<"-"<<clause_lit[c][i].var_num<<" ";
                }
                else{
                    fout<<clause_lit[c][i].var_num<<" ";
                    cout<<clause_lit[c][i].var_num<<" ";
                }
            }
            fout<<"0"<<endl;
            cout<<"0"<<endl;
        }
    }
    for(int c=num_clauses-gate_num-out_len;c<num_clauses;c++)
    {

        if(clause_lit[c][0].sense==false)
        {
            cout<<"-"<<clause_lit[c][0].var_num<<" 0"<<endl;
            fout<<"-"<<clause_lit[c][0].var_num<<" 0"<<endl;
        }
        else{
            fout<<clause_lit[c][0].var_num<<" 0"<<endl;
            cout<<clause_lit[c][0].var_num<<" 0"<<endl;
        }
    }
    */
    st.clear();
    vector<int> v=get_var_clause();
    const char* file_name="remain_clauses2.wcnf";
    write_remaining_clauses(file_name,v);
    //return v[v.size()-1];
    return remaining_clauses_num-hard_clauses_num-out_len;
}


int Satlike::reduce_clauses1()
{
    for(int i=0;i<num_clauses;i++)
    {
        clause_delete[i]=0;
    }
    //ofstream fout("remain_clauses1.txt");

    int filtered_gate=0;
    int dominated_gate=0;
    int last_clauses;
    int cur_gate;
    int first_clauses;
    int tmp_out=0;
    map<int,int> blocked_edge;
    bool added_tmp_out=false;
    bool count3=false;
    bool re_traverse=false;
    int in_var=0;

    if(unit_clause_count>0)
    {
        assign1();
        for(int i=0;i<=num_clauses-gate_num-out_len;i++)
        {
            if(clause_delete[i]==1)
                continue;
            else{
                cur_gate=clause_lit[i][clause_lit_count[i]-1].var_num;
                first_clauses=i;
                last_clauses=i;
                break;
            }
        }
        for(int c=0;c<num_clauses-gate_num-out_len;c++)
        {
        //cout<<num_clauses-gate_num-out_len<<endl;
            if(clause_delete[c]==1)
            {
            //cout<<"delete: "<<c<<endl;
                continue;
            }
            if(cur_gate==clause_lit[c][clause_lit_count[c]-1].var_num && re_traverse==false )
            {
                if(clause_lit_count[c]==2 && added_tmp_out==false)
                {
                    added_tmp_out=true;
                    tmp_out=clause_lit[c][clause_lit_count[c]-2].var_num;
                }
                else if(clause_lit_count[c]>2)
                {
                    count3=true;
                }
            }
            else if(cur_gate!=clause_lit[c][clause_lit_count[c]-1].var_num && re_traverse==false )
            {
                //cout<<cur_gate<<"  "<<clause_lit[c][clause_lit_count[c]-1].var_num<<endl;
                c=first_clauses;
                //cout<<c<<endl;
                //cout<<clause_lit[c][clause_lit_count[c]-1].var_num<<endl;
                gate[clause_lit[c][clause_lit_count[c]-1].var_num].first_clauses=first_clauses;
                re_traverse=true;
                cur_gate=clause_lit[c][clause_lit_count[c]-1].var_num;
                c--;//��ȥfor ѭ��c++ ��Ӱ��
            }
            else if(cur_gate==clause_lit[c][clause_lit_count[c]-1].var_num && re_traverse==true && clause_delete[c]!=1)
            {
                if(count3==true && added_tmp_out==true)
                {
                    if(clause_lit_count[c]>2)
                    {
                        int clause_var_tmp_index=clause_lit_count[c]-3;
                        while(clause_var_tmp_index>=0)
                        {
                            clause_lit[c][clause_var_tmp_index].B_Node_attr=2;
                            blocked_edge[clause_lit[c][clause_var_tmp_index].var_num] = clause_lit[c][clause_lit_count[c]-1].var_num;
                            //cout<<"blocked_edge:"<<clause_lit[c][clause_var_tmp_index].var_num<<clause_lit[c][clause_lit_count[c]-1].var_num<<endl;
                            clause_var_tmp_index--;
                        }
                    }
                }
            }
            else
            {
                if(blocked_edge.size()>0)
                {
                    Blocked_Edge.push_back(blocked_edge);
                    //cout<<"blocked_size: "<<Blocked_Edge.size()<<endl;
                }
                tmp_out=0;
                added_tmp_out=false;
                re_traverse=false;
                first_clauses=c;
                gate[cur_gate].last_clauses=c-1;
                blocked_edge.clear();
                cur_gate=clause_lit[c][clause_lit_count[c]-1].var_num ;
                c--;
            }
        }
    }
    for(int i=0;i<num_vars;i++)
    {
        if(gate[i].dominat_attr==1 || gate[i].dominat_attr==4)
        {   Dominated[Dominated_count++]=i;
            //cout<<"Dominated: "<<i<<endl;
            }
    }


    //Blocked_Edge.clear();//��bug
    int iteration=0;
    while(Dominated_count!=0)
    {

        dominated_gate=Dominated[Dominated_count-1];
        Dominated_count--;
        int first = gate[dominated_gate].first_clauses;
        int last = gate[dominated_gate].last_clauses;
        if(first<0 || last<0 || first>=num_clauses || last>=num_clauses || first>last)
        {
            continue;
        }
        for(int i=first;i<=last;i++)
        {
            if(clause_delete[i]==1)
            {
                continue;
            }
            if(clause_lit_count[i]==2 )
            {
                unit_clause[unit_clause_count++]=clause_lit[i][clause_lit_count[i]-1];
                unit_clause[unit_clause_count++]=clause_lit[i][clause_lit_count[i]-2];
            }
        }
    }
    int pre_unit_clause_count=0;
    bool new_dominated=false;
    //cout<<"unit_count:"<<unit_clause_count<<endl;
    while(iteration<1 && (Filtered_Node_count!=0 || unit_clause_count!=pre_unit_clause_count || new_dominated==true || !Blocked_Edge.empty()))
    {

        iteration++;
        new_dominated=false;
        //cout<<"Iteration:"<<iteration<<endl;
        pre_unit_clause_count=unit_clause_count;
        //cout<<"pre_UNIT_num:"<<pre_unit_clause_count<<endl;

        while(Filtered_Node_count!=0)
        {
//            cout<<"while Filtered_Node_count"<<Filtered_Node_count<<endl;
//            cout<<"Filtered_Node_count1:"<<endl;
//            cout<<unit_clause_count<<" "<<pre_unit_clause_count<<" "<<Blocked_Edge.size()<<endl;
            filtered_gate=Filtered_Node[Filtered_Node_count-1];
            //Filtered_Node.pop_back();
            Filtered_Node_count--;
//            cout<<"Filtered_Node_count2"<<Filtered_Node_count<<endl;
//            cout<<"filtered_gate"<<filtered_gate<<endl;
            while(!gate[filtered_gate].in_vars.empty())
            {
                //cout<<gate[filtered_gate].in_vars.size()<<endl;
                blocked_edge[gate[filtered_gate].in_vars[gate[filtered_gate].in_vars.size()-1]] = filtered_gate;
                gate[filtered_gate].in_vars.pop_back();
            }
            Filtered_Edge.push_back(blocked_edge);
        }//�������Žڵ㲻Ϊ�գ����ýڵ������߶�����Ϊ���˱�


        if(unit_clause_count>0)//��Ԫ�Ӿ䴫������ܻ����blocked_Edge
        {
//            cout<<"unit_clause_count"<<endl;
            assign1();
            for(int i=0;i<=num_clauses-gate_num-out_len;i++)
            {
                if(clause_delete[i]==1)
                    continue;
                else{
                    cur_gate=clause_lit[i][clause_lit_count[i]-1].var_num;
                    first_clauses=i;
                    last_clauses=i;
                    break;
                }
            }
            for(int c=0;c<num_clauses-gate_num-out_len;c++)
            {
                if(clause_delete[c]==1)
                {
                    continue;
                }
                if(cur_gate==clause_lit[c][clause_lit_count[c]-1].var_num && re_traverse==false  && clause_delete[c]!=1)
                {
                    if(clause_lit_count[c]==2 && added_tmp_out==false)
                    {
                        added_tmp_out=true;
                        tmp_out=clause_lit[c][clause_lit_count[c]-2].var_num;
                    }
                    else if(clause_lit_count[c]>2)
                    {
                        count3=true;
                    }
                }
                else if(cur_gate!=clause_lit[c][clause_lit_count[c]-1].var_num && re_traverse==false && clause_delete[c]!=1)
                {
                    c=first_clauses;
                    gate[clause_lit[c][clause_lit_count[c]-1].var_num].first_clauses=first_clauses;
                    re_traverse=true;
                    cur_gate=clause_lit[c][clause_lit_count[c]-1].var_num;
                    c--;
                }
                else if(cur_gate==clause_lit[c][clause_lit_count[c]-1].var_num && re_traverse==true && clause_delete[c]!=1)
                {
                    if(count3==true && added_tmp_out==true)
                    {
                        if(clause_lit_count[c]>2)
                        {
                            int clause_var_tmp_index=clause_lit_count[c]-3;
                            while(clause_var_tmp_index>=0)
                            {
                                clause_lit[c][clause_var_tmp_index].B_Node_attr=2;
                                blocked_edge[clause_lit[c][clause_var_tmp_index].var_num] = clause_lit[c][clause_lit_count[c]-1].var_num;
                                //cout<<"blocked_edge:"<<clause_lit[c][clause_var_tmp_index].var_num<<clause_lit[c][clause_lit_count[c]-1].var_num<<endl;
                                clause_var_tmp_index--;
                            }
                        }
                    }
                }
                else if(clause_delete[c]!=1)
                {
                    if(blocked_edge.size()>0)
                    {
                        Blocked_Edge.push_back(blocked_edge);
                        //cout<<"blocked_size: "<<Blocked_Edge.size()<<endl;
                    }
                    tmp_out=0;
                    added_tmp_out=false;
                    re_traverse=false;
                    first_clauses=c;
                    gate[cur_gate].last_clauses=c-1;
                    blocked_edge.clear();
                    cur_gate=clause_lit[c][clause_lit_count[c]-1].var_num ;
                    c--;
                }
            }
        }
        int z_var=0;
        int out_gate_var=0;
        int in_gate_var=0;
        //Blocked_Edge ���ܲ���Dominated �ű���
        for(int i=0;i<Blocked_Edge.size();i++)
        {
            //cout<<"Blocked_Edge"<<endl;
            map<int,int> p = Blocked_Edge[i];
            for(map<int,int>::iterator it=p.begin();it!=p.end();it++)
            {
                //cout<<it->first<<" "<<it->second<<endl;
                z_var=it->first;
                out_gate_var=it->second;
                in_gate_var=in_gate[it->first];
                if(z_var<=0 || z_var>num_vars) continue;
                if(var_gate_neighbor_count[z_var]<=0) continue;
                //�����Ŵ�����ߵ�����ż�����ɾ��
                int tmp_index_gate=0;
                int tmp_gate=0;
                int neighbor_count = var_gate_neighbor_count[z_var];
                while(tmp_index_gate<neighbor_count && var_gate_neighbor[z_var][tmp_index_gate]!=out_gate_var)
                {
                    tmp_index_gate++;//�ҵ�out_gate_var ��index
                }
                if(tmp_index_gate>=neighbor_count) continue;
                var_gate_neighbor[z_var][tmp_index_gate]=var_gate_neighbor[z_var][var_gate_neighbor_count[z_var]-1];
                var_gate_neighbor_count[z_var]--;
                int tmp_dominated;
                if(var_gate_neighbor_count[z_var]==1)//���������ֻ���뵽һ������
                {
                    out_gate_var=var_gate_neighbor[z_var][tmp_index_gate];
                    gate[in_gate_var].dominator=gate[out_gate_var].gate_name;
                    //Dominated[Dominated_count++]=in_gate_var;

                    if(gate[in_gate_var].dominat_attr==0  || gate[in_gate_var].dominat_attr==1)
                    {
                        if(gate[in_gate_var].dominat_attr==0)
                        {
                            new_dominated=true;
                        }
                        gate[in_gate_var].dominat_attr=1;

                        //�ж�in_gate_var �ű�����������ص��ű���A�������ص��ű����Ƿ�����ͬ��ͳ����B
                        //���ǵĻ�����A������Ϊ��ͳ����,ͳ������B
                        int in_var_num=gate[in_gate_var].in_vars.size();

                        while(in_var_num>0)
                        {
                            int tmp_in_var=gate[in_gate_var].in_vars[in_var_num-1];//z����
                            in_var_num--;
                            int tmp_I_var=in_gate[tmp_in_var];//z������������I
                            if(tmp_I_var==0)
                            {
                                continue;
                            }
                            int tmp_out_var=gate[tmp_I_var].out_var;//I���������Z����
                            bool tmp_flag=false;
                            int tmp_var_gate_neighbor_count=var_gate_neighbor_count[tmp_out_var];
                            int tmp_gate_var;//��I��ͨ��Z�������ӵ������ű���
                            string dominator_gate;
                            while(tmp_var_gate_neighbor_count>0 && tmp_flag==false)
                            {

                                tmp_gate_var=var_gate_neighbor[tmp_out_var][tmp_var_gate_neighbor_count-1];
                                if(tmp_gate_var==tmp_I_var)
                                {
                                    tmp_var_gate_neighbor_count--;
                                    continue;
                                }
                                if(dominator_gate.size()==0)
                                {
                                    dominator_gate=gate[tmp_gate_var].dominator;
                                }
                                else if(dominator_gate!=gate[tmp_gate_var].dominator || (gate[tmp_gate_var].dominat_attr!=1 && gate[tmp_gate_var].dominat_attr!=4 ))
                                {
                                    tmp_flag=true;
                                }
                                tmp_var_gate_neighbor_count--;
                            }

                            if(tmp_flag==false)//˵�������ű������Ǳ�dominator_gate ͳ�ε�
                            {
                                new_dominated=true;
                                gate[tmp_I_var].dominator=dominator_gate;
                                if(gate[tmp_I_var].dominat_attr==0 || gate[tmp_I_var].dominat_attr==1)
                                {
                                    gate[tmp_I_var].dominat_attr=1;
                                }
                                else
                                {
                                    gate[tmp_I_var].dominat_attr=4;
                                }
                            }
                        }

                    }
                    else
                    {
                        gate[in_gate_var].dominat_attr=4;
                    }
                    if(gate[out_gate_var].dominat_attr==0 || gate[out_gate_var].dominat_attr==2)
                    {
                        gate[out_gate_var].dominat_attr=2;
                    }
                    else
                    {
                        gate[out_gate_var].dominat_attr=4;
                    }
                }
                else if(var_gate_neighbor_count[z_var]==0)
                {
                    Filtered_Node[Filtered_Node_count++]=in_gate[z_var];
                }
            }
        }
        Blocked_Edge.clear();
        Dominated_count=0;
        for(int i=0;i<num_vars;i++)
        {
            if(gate[i].dominat_attr==1 || gate[i].dominat_attr==4)
            {
                Dominated[Dominated_count++]=i;
                //cout<<i<<"is dominated"<<endl;
            }
        }
        while(Dominated_count!=0)
        {
//            cout<<"Dominated_count"<<endl;
            dominated_gate=Dominated[Dominated_count-1];
            Dominated_count--;
            int first = gate[dominated_gate].first_clauses;
            int last = gate[dominated_gate].last_clauses;
            if(first<0 || last<0 || first>=num_clauses || last>=num_clauses || first>last)
            {
                continue;
            }
            for(int i=first;i<=last;i++)
            {
                if(clause_delete[i]==1)
                {
                    continue;
                }
                if(clause_lit_count[i]==2)
                {
                    unit_clause[unit_clause_count++]=clause_lit[i][clause_lit_count[i]-1];
                    unit_clause[unit_clause_count++]=clause_lit[i][clause_lit_count[i]-2];
                }
            }
        }
//        cout<<"UNIT_num:"<<unit_clause_count<<endl;
    }

     //���Դ���
//    while(!q.empty())
//        q.pop();
    st.clear();
    vector<int> v=get_var_clause();
    const char* file_name="remain_clauses1.wcnf";
    write_remaining_clauses(file_name,v);
    return v[v.size()-1];
}


vector<int> Satlike::get_var_clause_initial()
{
    int reduce1=0;
    for(int c=0;c<num_clauses-gate_num-out_len-in_len;c++)
    {
        if(clause_delete[c]!=1){
            for(int i=0;i<clause_lit_count[c];i++)
            {
                if(clause_lit[c][i].sense==false)
                {
                    //cout<<"-"<<clause_lit[c][i].var_num<<" ";
                    get_num(clause_lit[c][i].var_num);
                    //fout<<"-"<<clause_lit[c][i].var_num<<" ";
                }
                else
                {
                    //cout<<clause_lit[c][i].var_num<<" ";
                    get_num(clause_lit[c][i].var_num);
                    //fout<<clause_lit[c][i].var_num<<" ";
                }

            }
            reduce1++;
            //cout<<endl;
            //fout<<endl;
        }
    }
    for(int c=num_clauses-gate_num-out_len-in_len;c<num_clauses;c++)
    {
        if(clause_lit[c][0].sense==false)
        {
            //cout<<"-"<<clause_lit[c][0].var_num<<" 0"<<endl;
            get_num(clause_lit[c][0].var_num);
            //fout<<"-"<<clause_lit[c][0].var_num<<" 0"<<endl;
        }
        else{
            //cout<<clause_lit[c][0].var_num<<" 0"<<endl;
            get_num(clause_lit[c][0].var_num);
            //fout<<clause_lit[c][0].var_num<<" 0"<<endl;
        }
    }
//    cout<<" st.size: "<<st.size()<<endl;
    int* num_v_tmp=new int[st.size()+1];
    for(int i=1;i<=st.size();i++)
    {
        num_v_tmp[i]=-1;
    }
    for(int c=0;c<num_clauses;c++)
    {
        if(clause_delete[c]!=1){
            for(int i=0;i<clause_lit_count[c];i++)
            {
                num_v_tmp[clause_lit[c][i].var_num]=1;
            }
        }
    }
    vector<int> get_result;
    //num_v_tmp+1,num_v_tmp+num_v+1);
    for(int i=1;i<=st.size();i++)
        get_result.push_back(num_v_tmp[i]);
    int tmp=reduce1+gate_num+out_len+in_len;
    get_result.push_back(tmp);
    get_result.push_back(st.size());
    return get_result;

}
vector<int> Satlike::get_var_clause()
{
    st.clear();
    int reduce1=0;
    int* num_v_tmp=new int[num_v+1];
//    cout<<"num_v: "<<num_v<<endl;
    for(int i=1;i<=num_v;i++)
    {
        num_v_tmp[i]=-1;
    }
    for(int c=0;c<num_clauses-gate_num-out_len-in_len;c++)
    {
        if(clause_delete[c]!=1){
            for(int i=0;i<clause_lit_count[c];i++)
            {
                if(clause_lit[c][i].sense==false)
                {
                    //cout<<"-"<<clause_lit[c][i].var_num<<" ";
                    get_num(clause_lit[c][i].var_num);
                    //fout<<"-"<<clause_lit[c][i].var_num<<" ";
                }
                else
                {
                    //cout<<clause_lit[c][i].var_num<<" ";
                    get_num(clause_lit[c][i].var_num);
                    //fout<<clause_lit[c][i].var_num<<" ";
                }

            }
            reduce1++;
            //cout<<endl;
            //fout<<endl;
        }
    }
//    cout<<"num_c:"<<num_clauses<<" : "<<num_clauses-gate_num-out_len-in_len<<endl;
    for(int c=num_clauses-gate_num-out_len-in_len;c<num_clauses;c++)
    {
        num_v_tmp[clause_lit[c][0].var_num]=1;
        if(clause_lit[c][0].sense==false)
        {
            //cout<<"-"<<clause_lit[c][0].var_num<<" 0"<<endl;
            get_num(clause_lit[c][0].var_num);
            //fout<<"-"<<clause_lit[c][0].var_num<<" 0"<<endl;
        }
        else{
            //cout<<clause_lit[c][0].var_num<<" 0"<<endl;
            get_num(clause_lit[c][0].var_num);
            //fout<<clause_lit[c][0].var_num<<" 0"<<endl;
        }
    }

    for(int c=0;c<num_clauses;c++)
    {
        if(clause_delete[c]!=1){
            for(int i=0;i<clause_lit_count[c];i++)
            {
                num_v_tmp[abs(clause_lit[c][i].var_num)]=1;
                //cout<<clause_lit[c][i].var_num<<endl;
            }
        }
    }
    vector<int> get_result;
    for(int i=1;i<=num_v;i++)
        get_result.push_back(num_v_tmp[i]);
    int tmp=reduce1+gate_num+out_len+in_len;
    get_result.push_back(tmp);
    get_result.push_back(st.size());
    cout<<num_v<<" set size"<<st.size()<<endl;
//    for(set<int>::iterator it=st.begin();it!=st.end();it++)
//        cout<<*it<<endl;
    return get_result;

}
void Satlike::get_num(int num)
{
    num = abs(num);
    st.insert(abs(num));
//    int s = q.size();
//    cout<<"sssssssssssss"<<s<<endl;
//    int i=0;
//    for(; i<s; ++i)
//    {
//        int a = q.front();
//        if (a == num)
//        {
//            return;
//        }
//        q.pop();
//        q.push(a);
//    }
//    if(i==s)
//    {
//        q.push(num);
//    }
}
void Satlike::write_remaining_clauses(const char* infile,vector<int> v)
{
    map<int,int> ma;
    ofstream fout(infile);
    int num_v=v[v.size()-1];
    int num_c=v[v.size()-2];
    fout<<"p wcnf "<<v[v.size()-1]<<" "<<v[v.size()-2]<<" "<<gate_num+1<<endl;
    cout<<"v.size()"<<v.size()<<endl;
    int cnt_empty = 0;
    for(int i=0;i<num_v;i++)
        if(v[i]==-1)
            cnt_empty++;
    cout<<"cnt_empty "<<cnt_empty<<endl;
    ma.clear();
    for(int c=0;c<num_clauses-gate_num-out_len-in_len;c++)
    {
        if(clause_delete[c]!=1){
            fout<<gate_num+1<<" ";
            for(int i=0;i<clause_lit_count[c];i++)
            {
                if(clause_lit[c][i].sense==false)
                {
                //ma.find(12)!=ma.end()//cunzai
                    //if(clause_lit[c][i].var_num>num_v && ma.find(clause_lit[c][i].var_num)==ma.end() )
                    if(clause_lit[c][i].var_num>num_v)
                    {
                        if(ma.find(clause_lit[c][i].var_num)==ma.end())
                        {
                            bool flag = false;
                            for(int j=0;j<v.size()-2;j++)
                            {
                                if(v[j]==-1)
                                {
                                    ma[clause_lit[c][i].var_num]=j+1;
                                    v[j]=1;
                                    fout<<"-"<<ma[clause_lit[c][i].var_num]<<" ";
                                    flag = true;
                                    break;
                                }

                            }
                            if(!flag)
                                fout<<"-"<<clause_lit[c][i].var_num<<" ";
                        }
                         else if(ma.find(clause_lit[c][i].var_num)!=ma.end())
                         {
                            fout<<"-"<<ma[clause_lit[c][i].var_num]<<" ";
                         }
                    }
                    //cout<<"-"<<clause_lit[c][i].var_num<<" ";
                    else{
                        fout<<"-"<<clause_lit[c][i].var_num<<" ";
                    }
                }
                else
                {
                    if(clause_lit[c][i].var_num>num_v)
                    {
                        if( ma.find(clause_lit[c][i].var_num)==ma.end())
                        {
                            bool flag = false;
                            for(int j=0;j<v.size()-2;j++)
                            {
                                if(v[j]==-1)
                                {
                                    ma[clause_lit[c][i].var_num]=j+1;
                                    v[j]=1;
                                    fout<<ma[clause_lit[c][i].var_num]<<" ";
                                    flag = true;
                                    break;
                                }
                            }
                            if(!flag)
                                fout<<"-"<<clause_lit[c][i].var_num<<" ";
                        }
                        else if(ma.find(clause_lit[c][i].var_num)!=ma.end())
                        {
                            fout<<ma[clause_lit[c][i].var_num]<<" ";
                        }
                    }
                    //cout<<"-"<<clause_lit[c][i].var_num<<" ";
                    else{
                        fout<<clause_lit[c][i].var_num<<" ";
                    }
                }

            }
            //cout<<endl;
            fout<<" 0"<<endl;
        }
    }
    for(int c=num_clauses-gate_num-out_len-in_len;c<num_clauses-gate_num;c++)
    {
        if(clause_lit[c][0].sense==false)
        {
            //cout<<"-"<<clause_lit[c][0].var_num<<" 0"<<endl;
            fout<<gate_num+1<<" -"<<clause_lit[c][0].var_num<<" 0"<<endl;
        }
        else{
            //cout<<clause_lit[c][0].var_num<<" 0"<<endl;
            fout<<gate_num+1<<" "<<clause_lit[c][0].var_num<<" 0"<<endl;
        }
    }
    for(int c=num_clauses-gate_num;c<num_clauses;c++)
    {
        if(clause_lit[c][0].sense==false)
        {
            //cout<<"-"<<clause_lit[c][0].var_num<<" 0"<<endl;
            fout<<1<<" -"<<clause_lit[c][0].var_num<<" 0"<<endl;
        }
        else{
            //cout<<clause_lit[c][0].var_num<<" 0"<<endl;
            fout<<1<<" "<<clause_lit[c][0].var_num<<" 0"<<endl;
        }
    }
}

/*
void Satlike::reduce_clauses2()
{
    //cout<<"Flitering end."<<endl;
    //��ʼ��
//    for(int i=1;i<=num_vars;i++)
//    {
//        confict[i]=-1;
//        confict_0[i]=0;
//        confict_1[i]=0;
//    }
    while(!unit_out_clause.empty())
    {
        //unit_out_clause_count--;
        //cout<<"unit_out_clause: "<<unit_out_clause[unit_out_clause_count].var_num<<"   "<<unit_out_clause[unit_out_clause_count].sense<<endl;
        int v=unit_out_clause[unit_out_clause_count].var_num;
        int sense=unit_out_clause[unit_out_clause_count].sense;
        int c,l;
        for(int i=0;i<var_lit_count[v];++i)
        {
            c=var_lit[v][i].clause_num;
            if(clause_delete[c]==1) continue;

            if(sense==var_lit[v][i].sense)
            {
                clause_delete[c]=1;//B==F
                continue;
            }
            int swap_lit_var;
            bool swap_lit_sense;
            for(int j=0;j<clause_lit_count[c];j++)
            {
                if(clause_lit[c][j].var_num==v && clause_lit_count[c]>2)
                {
                    for(int ii=j+1;ii<clause_lit_count[c];ii++)
                    {
                        lit tmp = clause_lit[c][ii-1];
                        clause_lit[c][ii-1] = clause_lit[c][ii];
                        clause_lit[c][ii] = tmp;
                    }
                    clause_lit_count[c]--;
                    break;
                }
                else if(clause_lit[c][j].var_num==v && clause_lit_count[c]==2)
                {
                    if(clause_lit[c][j].sense==0)
                    {
                        confict_0[v]++;
                    }
                    else{
                        confict_1[v]++;
                    }
//                    cout<<c<<" :: "<<clause_lit[c][j].var_num<<endl;
//                    confict[v]=1;//��־�ڴ������ʱ�Ѿ��ҵ������ĳ�ͻ
                }
            }
            if(clause_lit_count[c]==2 && clause_lit[c][0].var_num!=v)
            {
                unit_out_clause[unit_out_clause_count++]=clause_lit[c][0];
            }
        }
    }
    for(int v=1;v<=num_vars;v++)
    {
        if((confict_0[v]!=0 && confict_1[v]!=0))
        {
//            if(confict[v]==1)
//            {
//                confict[in_gate[v]]=2;
//            }

            //cout<<in_gate[v]<<" "<<confict_0[v]<<" "<<confict_1[v]<<endl;
            for(int i=gate[in_gate[v]].first_clauses;i<=gate[in_gate[v]].last_clauses;i++)
            {
                if(clause_delete[i]==1)
                {
                    continue;
                }
                if(clause_lit_count[i]==2)
                {
                    unit_clause[unit_clause_count++]=clause_lit[i][1];
                    unit_clause[unit_clause_count++]=clause_lit[i][0];

                }
            }
            //unit_clause[unit_clause_count++]=i;
        }
    }
//    for(int v=1;v<=num_vars;v++)
//    {
//        if(confict[v]==2)
//        {
//            diag_len++;
//            cout<<"Diag: "<<v<<" ";//DIAG
//        }
//    }
    recover_clauses_lit();
    int filtered_gate=0;
    int dominated_gate=0;
    int last_clauses;
    int cur_gate;
    int first_clauses;
    int tmp_out=0;
    map<int,int> blocked_edge;
    bool added_tmp_out=false;
    bool count3=false;
    bool re_traverse=false;
    int in_var=0;

    int pre_unit_clause_count=0;
    bool new_dominated=false;
    int iteration=0;
    //cout<<"unit_count:"<<unit_clause_count<<endl;
    while(unit_out_clause_count>0 ||Filtered_Node_count !=0 || unit_clause_count!=pre_unit_clause_count || new_dominated==true ||!Blocked_Edge.empty())
    {

        iteration++;
        new_dominated=false;
        //cout<<"Iteration:"<<iteration<<endl;
        pre_unit_clause_count=unit_clause_count;
        //cout<<"pre_UNIT_num:"<<pre_unit_clause_count<<endl;

        while(Filtered_Node_count!=0)
        {
//            cout<<"while Filtered_Node_count"<<Filtered_Node_count<<endl;
//            cout<<"Filtered_Node_count1:"<<endl;
//            cout<<unit_clause_count<<" "<<pre_unit_clause_count<<" "<<Blocked_Edge.size()<<endl;
            filtered_gate=Filtered_Node[Filtered_Node_count-1];
            //Filtered_Node.pop_back();
            Filtered_Node_count--;
//            cout<<"Filtered_Node_count2"<<Filtered_Node_count<<endl;
//            cout<<"filtered_gate"<<filtered_gate<<endl;
            while(!gate[filtered_gate].in_vars.empty())
            {
                //cout<<gate[filtered_gate].in_vars.size()<<endl;
                blocked_edge[gate[filtered_gate].in_vars[gate[filtered_gate].in_vars.size()-1]] = filtered_gate;
                gate[filtered_gate].in_vars.pop_back();
            }
            Filtered_Edge.push_back(blocked_edge);
        }//�������Žڵ㲻Ϊ�գ����ýڵ������߶�����Ϊ���˱�


        if(unit_clause_count>0)//��Ԫ�Ӿ䴫������ܻ����blocked_Edge
        {
//            cout<<"unit_clause_count"<<endl;
            assign1();
            for(int i=0;i<=num_clauses;i++)
            {
                if(clause_delete[i]==1)
                    continue;
                else{
                    cur_gate=clause_lit[i][clause_lit_count[i]-1].var_num;
                    first_clauses=i;
                    last_clauses=i;
                    break;
                }
            }
            for(int c=0;c<num_clauses;c++)
            {
                if(clause_delete[c]==1)
                {
                    continue;
                }
                if(cur_gate==clause_lit[c][clause_lit_count[c]-1].var_num && re_traverse==false  && clause_delete[c]!=1)
                {
                    if(clause_lit_count[c]==2 && added_tmp_out==false)
                    {
                        added_tmp_out=true;
                        tmp_out=clause_lit[c][clause_lit_count[c]-2].var_num;
                    }
                    else if(clause_lit_count[c]>2)
                    {
                        count3=true;
                    }
                }
                else if(cur_gate!=clause_lit[c][clause_lit_count[c]-1].var_num && re_traverse==false && clause_delete[c]!=1)
                {
                    c=first_clauses;
                    gate[clause_lit[c][clause_lit_count[c]-1].var_num].first_clauses=first_clauses;
                    re_traverse=true;
                    cur_gate=clause_lit[c][clause_lit_count[c]-1].var_num;
                    c--;
                }
                else if(cur_gate==clause_lit[c][clause_lit_count[c]-1].var_num && re_traverse==true && clause_delete[c]!=1)
                {
                    if(count3==true && added_tmp_out==true)
                    {
                        if(clause_lit_count[c]>2)
                        {
                            int clause_var_tmp_index=clause_lit_count[c]-3;
                            while(clause_var_tmp_index>=0)
                            {
                                clause_lit[c][clause_var_tmp_index].B_Node_attr=2;
                                blocked_edge[clause_lit[c][clause_var_tmp_index].var_num] = clause_lit[c][clause_lit_count[c]-1].var_num;
                                //cout<<"blocked_edge:"<<clause_lit[c][clause_var_tmp_index].var_num<<clause_lit[c][clause_lit_count[c]-1].var_num<<endl;
                                clause_var_tmp_index--;
                            }
                        }
                    }
                }
                else if(clause_delete[c]!=1)
                {
                    if(blocked_edge.size()>0)
                    {
                        Blocked_Edge.push_back(blocked_edge);
                        //cout<<"blocked_size: "<<Blocked_Edge.size()<<endl;
                    }
                    tmp_out=0;
                    added_tmp_out=false;
                    re_traverse=false;
                    first_clauses=c;
                    gate[cur_gate].last_clauses=c-1;
                    blocked_edge.clear();
                    cur_gate=clause_lit[c][clause_lit_count[c]-1].var_num ;
                    c--;
                }
            }
        }
        int z_var=0;
        int out_gate_var=0;
        int in_gate_var=0;
        //Blocked_Edge ���ܲ���Dominated �ű���
        for(int i=0;i<Blocked_Edge.size();i++)
        {
//            cout<<"Blocked_Edge"<<endl;
            map<int,int> p = Blocked_Edge[i];
            for(map<int,int>::iterator it=p.begin();it!=p.end();it++)
            {
                //cout<<it->first<<" "<<it->second<<endl;
                z_var=it->first;
                out_gate_var=it->second;
                in_gate_var=in_gate[it->first];
                //�����Ŵ�����ߵ�����ż�����ɾ��
                int tmp_index_gate=0;
                int tmp_gate=0;
                while(var_gate_neighbor[z_var][tmp_index_gate]!=out_gate_var)
                {
                    tmp_index_gate++;//�ҵ�out_gate_var ��index
                }
                var_gate_neighbor[z_var][tmp_index_gate]=var_gate_neighbor[z_var][var_gate_neighbor_count[z_var]-1];
                var_gate_neighbor_count[z_var]--;
                int tmp_dominated;
                if(var_gate_neighbor_count[z_var]==1)//���������ֻ���뵽һ������
                {
                    out_gate_var=var_gate_neighbor[z_var][tmp_index_gate];
                    gate[in_gate_var].dominator=gate[out_gate_var].gate_name;
                    //Dominated[Dominated_count++]=in_gate_var;

                    if(gate[in_gate_var].dominat_attr==0  || gate[in_gate_var].dominat_attr==1)
                    {
                        if(gate[in_gate_var].dominat_attr==0)
                        {
                            new_dominated=true;
                        }
                        gate[in_gate_var].dominat_attr=1;

                        //�ж�in_gate_var �ű�����������ص��ű���A�������ص��ű����Ƿ�����ͬ��ͳ����B
                        //���ǵĻ�����A������Ϊ��ͳ����,ͳ������B
                        int in_var_num=gate[in_gate_var].in_vars.size();

                        while(in_var_num>0)
                        {
                            int tmp_in_var=gate[in_gate_var].in_vars[in_var_num-1];//z����
                            in_var_num--;
                            int tmp_I_var=in_gate[tmp_in_var];//z������������I
                            if(tmp_I_var==0)
                            {
                                continue;
                            }
                            int tmp_out_var=gate[tmp_I_var].out_var;//I���������Z����
                            bool tmp_flag=false;
                            int tmp_var_gate_neighbor_count=var_gate_neighbor_count[tmp_out_var];
                            int tmp_gate_var;//��I��ͨ��Z�������ӵ������ű���
                            string dominator_gate;
                            while(tmp_var_gate_neighbor_count>0 && tmp_flag==false)
                            {

                                tmp_gate_var=var_gate_neighbor[tmp_out_var][tmp_var_gate_neighbor_count-1];
                                if(tmp_gate_var==tmp_I_var)
                                {
                                    tmp_var_gate_neighbor_count--;
                                    continue;
                                }
                                if(dominator_gate.size()==0)
                                {
                                    dominator_gate=gate[tmp_gate_var].dominator;
                                }
                                else if(dominator_gate!=gate[tmp_gate_var].dominator || (gate[tmp_gate_var].dominat_attr!=1 && gate[tmp_gate_var].dominat_attr!=4 ))
                                {
                                    tmp_flag=true;
                                }
                                tmp_var_gate_neighbor_count--;
                            }

                            if(tmp_flag==false)//˵�������ű������Ǳ�dominator_gate ͳ�ε�
                            {
                                new_dominated=true;
                                gate[tmp_I_var].dominator=dominator_gate;
                                if(gate[tmp_I_var].dominat_attr==0 || gate[tmp_I_var].dominat_attr==1)
                                {
                                    gate[tmp_I_var].dominat_attr=1;
                                }
                                else
                                {
                                    gate[tmp_I_var].dominat_attr=4;
                                }

                            }
                        }

                    }
                    else
                    {
                        gate[in_gate_var].dominat_attr=4;
                    }
                    if(gate[out_gate_var].dominat_attr==0 || gate[out_gate_var].dominat_attr==2)
                    {
                        gate[out_gate_var].dominat_attr=2;
                    }
                    else
                    {
                        gate[out_gate_var].dominat_attr=4;
                    }
                }
                else if(var_gate_neighbor_count[z_var]==0)
                {
                    Filtered_Node[Filtered_Node_count++]=in_gate[z_var];
                }
            }
        }
        Blocked_Edge.clear();
        Dominated_count=0;
        for(int i=0;i<num_vars;i++)
        {
            if(gate[i].dominat_attr==1 || gate[i].dominat_attr==4)
            {
                Dominated[Dominated_count++]=i;
                //cout<<i<<"is dominated"<<endl;
            }
        }
        while(Dominated_count!=0)
        {
//            cout<<"Dominated_count"<<endl;
            dominated_gate=Dominated[Dominated_count-1];
            Dominated_count--;
            for(int i=gate[dominated_gate].first_clauses;i<=gate[dominated_gate].last_clauses;i++)
            {
                if(clause_delete[i]==1)
                {
                    continue;
                }
                if(clause_lit_count[i]==2)
                {
                    unit_clause[unit_clause_count++]=clause_lit[i][clause_lit_count[i]-1];
                    unit_clause[unit_clause_count++]=clause_lit[i][clause_lit_count[i]-2];
                }
            }
        }
//        cout<<"UNIT_num:"<<unit_clause_count<<endl;
    }

    //���Դ���
            int reduce2=0;
            int reduce=0;
            for(int c=0;c<num_clauses;c++)
            {
                if(clause_delete[c]!=1){
                    for(int i=0;i<clause_lit_count[c];i++)
                    {

                        if(clause_lit[c][i].sense==false)
                        {
                            //cout<<"-"<<clause_lit[c][i].var_num<<" ";
                        }
                        //else cout<<clause_lit[c][i].var_num<<" ";
                    }
                    //cout<<endl;
                    reduce2++;
                    //cout<<"c:"<<c<<endl;
                }
                else{reduce++;}


            }
            cout<<"C2: "<<reduce2-gate_num-out_len<<" I2: "<<iteration<<" ";
           // cout<<reduce<<endl;

//    while(confict_count!=0)
//    {
//
//        int confict_var=confict[confict_count-1];
//        confict_count--;
//        for(int i=gate[dominated_gate].first_clauses;i<=gate[dominated_gate].last_clauses;i++)
//        {
//            if(clause_delete[i]==1)
//            {
//                continue;
//            }
//            if(clause_lit_count[i]==2 )
//            {
//                unit_clause[unit_clause_count++]=clause_lit[i][clause_lit_count[i]-1];
//                unit_clause[unit_clause_count++]=clause_lit[i][clause_lit_count[i]-2];
//            }
//        }
//    }


}

*/

void Satlike::dominated_relation(char*filename)
{
//    freopen(filename,"r",stdin);
    ifstream fin(filename);
    int flag=0;
    string str;
    int gate_num=0;
    while(fin>>str)
    {
        if(str=="Gates")
        {
            flag=1;
            fin>>str;
            fin>>str;////���˵�'['����
        }
        else if(str=="].")
        {
            flag=0;
        }
        else if(str=="Cones")
        {
            flag=2;
            fin>>str; //���˵�'['
            fin>>str;
        }
        if(flag==1)
        {
            str=split_syspp(str);
            vector<string> res = split(str.c_str(),",");
            //cout<<res[0]<<" "<<res[res.size()-1]<<m[res[0]]<<endl;
//            for(int i=0;i<res.size();i++)
//            {
//                cout<<res[i]<<" ";            }
            gate[m[res[0]]].gate_name=res[0];
            gate[m[res[0]]].gate_attr=res[res.size()-2];
            gate[m[res[0]]].out_var=m[res[1]];
            int in_vars_num=0;
            string in_tmp;
            for(int i=2;i<res.size()-2;i++)
            {
                in_tmp=split(res[i]);
                //cout<<in_tmp;
                //gate[m[res[0]]].in_vars.insert(pair<int,int>(m[in_tmp],in_vars_num++));
                gate[m[res[0]]].in_vars.push_back(m[in_tmp]);
            }

            if(res[res.size()-1]=="free")
            {
                gate[m[res[0]]].dominat_attr=0;//û������ͳ�ι�ϵ�ı���
            }
            else if(res[res.size()-1]=="dominated")
            {
                gate[m[res[0]]].dominat_attr=1;//�ñ����Ǳ�ͳ�εģ�
            }
            else if(res[res.size()-1]=="dominator")
            {
                gate[m[res[0]]].dominat_attr=2;//�ñ�����ͳ����
            }
        }

        if(flag==2)
        {
            str=split_syspp(str);
            vector<string> res = split(str.c_str(),",");
            string dominated=split(res[res.size()-1]);

            //cout<<res[0]<<" "<<dominated<<endl;
            gate[m[dominated]].dominator=res[0];
        }
//        if(str=="Sections")
//        {
//            flag=true;
//            cin>>str; //���˵�'['
//            cin>>str;
//        }
//        if(str=="].")
//        {
//            flag=false;
//        }
//        if(flag)
//        {
//            str=split_syyspp(str);
//            vector<string> res = split(str.c_str(),",");
//            cout<<res[0]<<" "<<res[res.size()-1]<<endl;
//        }

    }


}
void Satlike::input(){
    string str;
//    freopen("c17.sys","r",stdin);
    freopen("systems/c17.sys","r",stdin);
    bool flag = false;
    while(cin>>str){
        if(str[0]=='[' && str[1]=='['){
            flag = true;
        }
        if(flag){
//            cout<<str<<endl;
//            string pre_str;
//            pre_str = str;
//            cout<<pre_str<<endl;
            str = split(str);
//            cout<<str<<endl;
            vector<string> res = split(str.c_str(),",");
//            for(auto str:res)
//                cout<<str<<" ";
//            cout<<endl;
            //�ӵ�2����ʼ������ʽ������
            int idx = -1;
//            for(auto str1:res){
            int output_idx = -1;
            for(int i=2;i<res.size();i++){
                string str1 = res[i];
//                cout<<str1<<" ";
                if(mp.find(str1) != mp.end()){   //exist
                    idx = mp[str1];
                }
                else{    //not exist
                    idx = cur_idx;
                    mp.insert(pair<string,int>(str1,cur_idx++));
                }
                if(i==2){   //output
                    output_idx = idx;
                    mp_gate[idx] = str;
                }
                else{    //input
                    graph[output_idx].push_back(idx);
                }
            }
//            cout<< endl;
        }
    }
}

vector<string> Satlike::getRes(string str){
//    cout<<"============================================"<<endl;

    map<int,bool> mp_output;
    cout<<"query variable : "<<str<<endl;
    vector <string> gate;
    int res = 0;
    int idx = mp[str];   //��idx�ڵ㿪ʼ�����в�α���
    queue<int> q;
    q.push(idx);
    while(!q.empty()){
        int cur = q.front();
        q.pop();
        if(mp_gate.find(cur) != mp_gate.end()){
            if(mp_output.find(cur) == mp_output.end()){
//                cout<<res<<" "<<mp_gate[cur]<<endl;
                vector<string> gate_str = split(mp_gate[cur].c_str(),",");
                gate.push_back(gate_str[1]);
                mp_output[cur] = true;
                res++;
            }
        }

        vector<int> neighbors = graph[cur];
        for(int i=0;i<neighbors.size();i++){
            int nb = neighbors[i];
            q.push(nb);
        }
//        for(int nb:neighbors){
//            q.push(nb);
//        }
    }
//    cout<<"total gate count "<<res<<endl;
//    cout<<"============================================"<<endl;
    return gate;

}

//void Satlike::add_weight(string file_in,string file_out){
////    ifstream fin(file_in);
////    ofstream fout(file_out);
//
//}

void Satlike::input1(char* infile_name,char* obsfile,char* outfile_name,int in_len,int out_len,int gate_num,int obs_index){
    //char* outfile_name="./out_c17_CNF.txt";
    ofstream fout(outfile_name);
    string str;
    int var_index=1;
    num_clauses=0;
    //cout<<in_len<<" "<out_len<<" "<<gate_num<<endl;
//    cout<<in_len<<"  "<<out_len<<"  "<<gate_num<<endl;
//    freopen("systems/c432.sys","r",stdin);
    ifstream fin(infile_name);
    bool flag = false;
    while(fin>>str){
        if(str[0]=='[' && str[1]=='['){
            flag = true;
        }
        if(flag){
//            cout<<str<<endl;
//            string pre_str;
//            pre_str = str;
//            cout<<pre_str<<endl;
            str = split(str);
//            cout<<str<<endl;
            vector<string> res = split(str.c_str(),",");
            if(m[res[2]]==0)
            {
           //     cout<<res[0]<<endl;
            }
            if(res[0]=="inverter")
            {
                fout<<"-"<<m[res[3]]<<" "<<"-"<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<m[res[3]]<<" "<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                num_clauses+=2;
            }
            else if(res[0]=="buffer")
            {
                fout<<" "<<m[res[3]]<<" "<<"-"<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<"-"<<m[res[3]]<<" "<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                num_clauses+=2;
            }
            else if(res[0]=="nor2")
            {
                fout<<m[res[4]]<<" "<<m[res[3]]<<" "<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<"-"<<m[res[4]]<<" "<<"-"<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<"-"<<m[res[3]]<<" "<<"-"<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                num_clauses+=3;
            }
            else if(res[0]=="nor3")
            {
                fout<<m[res[5]]<<" "<<m[res[4]]<<" "<<m[res[3]]<<" "<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<"-"<<m[res[4]]<<" "<<"-"<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<"-"<<m[res[3]]<<" "<<"-"<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<"-"<<m[res[5]]<<" "<<"-"<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                num_clauses+=4;
            }
            else if(res[0]=="nor4")
            {
                fout<<m[res[6]]<<" "<<m[res[5]]<<" "<<m[res[4]]<<" "<<m[res[3]]<<" "<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<"-"<<m[res[4]]<<" "<<"-"<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<"-"<<m[res[3]]<<" "<<"-"<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<"-"<<m[res[6]]<<" "<<"-"<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<"-"<<m[res[5]]<<" "<<"-"<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                num_clauses+=5;
            }
            else if(res[0]=="nor5")
            {
                fout<<m[res[7]]<<" "<<m[res[6]]<<" "<<m[res[5]]<<" "<<m[res[4]]<<" "<<m[res[3]]<<" "<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<"-"<<m[res[4]]<<" "<<"-"<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<"-"<<m[res[3]]<<" "<<"-"<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<"-"<<m[res[7]]<<" "<<"-"<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<"-"<<m[res[6]]<<" "<<"-"<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<"-"<<m[res[5]]<<" "<<"-"<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                num_clauses+=6;
            }

            else if(res[0]=="or2")
            {
                fout<<m[res[4]]<<" "<<m[res[3]]<<" "<<"-"<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<"-"<<m[res[4]]<<" "<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<"-"<<m[res[3]]<<" "<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                num_clauses+=3;
            }
            else if(res[0]=="or3")
            {
                fout<<m[res[5]]<<" "<<m[res[4]]<<" "<<m[res[3]]<<" "<<"-"<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<"-"<<m[res[4]]<<" "<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<"-"<<m[res[3]]<<" "<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<"-"<<m[res[5]]<<" "<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                num_clauses+=4;
            }
            else if(res[0]=="or4")
            {
                fout<<m[res[6]]<<" "<<m[res[5]]<<" "<<m[res[4]]<<" "<<m[res[3]]<<" "<<"-"<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<"-"<<m[res[6]]<<" "<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<"-"<<m[res[5]]<<" "<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<"-"<<m[res[4]]<<" "<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<"-"<<m[res[3]]<<" "<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                num_clauses+=5;
            }
            else if(res[0]=="xor2")
            {
                fout<<m[res[4]]<<" "<<m[res[3]]<<" "<<"-"<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<"-"<<m[res[4]]<<" "<<"-"<<m[res[3]]<<" "<<"-"<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<"-"<<m[res[4]]<<" "<<m[res[3]]<<" "<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<m[res[4]]<<" "<<"-"<<m[res[3]]<<" "<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                num_clauses+=4;
            }
            else if(res[0]=="nand2")
            {
                fout<<"-"<<m[res[4]]<<" "<<"-"<<m[res[3]]<<" "<<"-"<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<m[res[4]]<<" "<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<m[res[3]]<<" "<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                num_clauses+=3;
            }
            else if(res[0]=="nand3")
            {
                fout<<"-"<<m[res[5]]<<" "<<"-"<<m[res[3]]<<" "<<"-"<<m[res[4]]<<" "<<"-"<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<m[res[5]]<<" "<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<m[res[3]]<<" "<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<m[res[4]]<<" "<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                num_clauses+=4;
            }
            else if(res[0]=="nand4")
            {
                fout<<"-"<<m[res[6]]<<" "<<"-"<<m[res[3]]<<" "<<"-"<<m[res[4]]<<" "<<"-"<<m[res[5]]<<" "<<"-"<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<m[res[6]]<<" "<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<m[res[3]]<<" "<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<m[res[4]]<<" "<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<m[res[5]]<<" "<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                num_clauses+=5;
            }
            else if(res[0]=="nand5")
            {
                fout<<"-"<<m[res[7]]<<" "<<"-"<<m[res[6]]<<" "<<"-"<<m[res[3]]<<" "<<"-"<<m[res[4]]<<" "<<"-"<<m[res[5]]<<" "<<"-"<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<m[res[7]]<<" "<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<m[res[6]]<<" "<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<m[res[3]]<<" "<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<m[res[4]]<<" "<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<m[res[5]]<<" "<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                num_clauses+=6;
            }
            else if(res[0]=="and2")
            {
                fout<<"-"<<m[res[4]]<<" "<<"-"<<m[res[3]]<<" "<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<m[res[4]]<<" "<<"-"<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<m[res[3]]<<" "<<"-"<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                num_clauses+=3;
            }
            else if(res[0]=="and3")
            {
                fout<<"-"<<m[res[5]]<<" "<<"-"<<m[res[4]]<<" "<<"-"<<m[res[3]]<<" "<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<m[res[5]]<<" "<<"-"<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<m[res[4]]<<" "<<"-"<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<m[res[3]]<<" "<<"-"<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                num_clauses+=4;
            }
            else if(res[0]=="and4")
            {
                fout<<"-"<<m[res[6]]<<" "<<"-"<<m[res[5]]<<" "<<"-"<<m[res[4]]<<" "<<"-"<<m[res[3]]<<" "<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<m[res[6]]<<" "<<"-"<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<m[res[5]]<<" "<<"-"<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<m[res[4]]<<" "<<"-"<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<m[res[3]]<<" "<<"-"<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                num_clauses+=5;
            }
            else if(res[0]=="and5")
            {
                fout<<"-"<<m[res[7]]<<" "<<"-"<<m[res[6]]<<" "<<"-"<<m[res[5]]<<" "<<"-"<<m[res[4]]<<" "<<"-"<<m[res[3]]<<" "<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<m[res[7]]<<" "<<"-"<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<m[res[6]]<<" "<<"-"<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<m[res[5]]<<" "<<"-"<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<m[res[4]]<<" "<<"-"<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<m[res[3]]<<" "<<"-"<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                num_clauses+=6;
            }
            else if(res[0]=="and8")
            {
                fout<<"-"<<m[res[3]]<<" "<<"-"<<m[res[4]]<<" "<<"-"<<m[res[5]]<<" "<<"-"<<m[res[6]]<<" "<<"-"<<m[res[7]]<<" "<<"-"<<m[res[8]]
                <<" "<<"-"<<m[res[9]]<<" "<<"-"<<m[res[10]]<<" "<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<m[res[6]]<<" "<<"-"<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<m[res[3]]<<" "<<"-"<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<m[res[4]]<<" "<<"-"<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<m[res[5]]<<" "<<"-"<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<m[res[7]]<<" "<<"-"<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<m[res[8]]<<" "<<"-"<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<m[res[9]]<<" "<<"-"<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<m[res[10]]<<" "<<"-"<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                num_clauses+=9;
            }
            else if(res[0]=="and9")
            {
                fout<<"-"<<m[res[3]]<<" "<<"-"<<m[res[4]]<<" "<<"-"<<m[res[5]]<<" "<<"-"<<m[res[6]]<<" "<<"-"<<m[res[7]]<<" "<<"-"<<m[res[8]]
                <<" "<<"-"<<m[res[9]]<<" "<<"-"<<m[res[10]]<<" "<<"-"<<m[res[11]]<<" "<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<m[res[6]]<<" "<<"-"<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<m[res[3]]<<" "<<"-"<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<m[res[4]]<<" "<<"-"<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<m[res[5]]<<" "<<"-"<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<m[res[7]]<<" "<<"-"<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<m[res[8]]<<" "<<"-"<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<m[res[9]]<<" "<<"-"<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<m[res[10]]<<" "<<"-"<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                fout<<m[res[11]]<<" "<<"-"<<m[res[2]]<<" "<<m[res[1]]<<" 0"<<endl;
                num_clauses+=10;
            }
        }
    }
    freopen(obsfile,"r",stdin);
   // string s="i";
    //fout<<"7 0"<<endl<<"8 0"<<endl<<"9 0"<<endl<<"-10 0"<<endl<<"11 0"<<endl;
    string obs_str;
    bool obs_flag = false;
    bool obs_Flag= false;
    while(cin>>obs_str){

        obs_str=split(obs_str);

        vector<string> res = split(obs_str.c_str(),",");
        if(atoi(res[1].c_str())==obs_index){
            obs_flag = true;
        }//

        if(obs_flag){
            if(res.size()>2)
            {
//                cout<<res[2]<<endl;
                res[2]=split(res[2]);                cout<<res[2]<<endl;
                //cout<<in_len<<" "<out_len<<endl;
                for(int i=2;i<=in_len+out_len+1;i++)
                {
                    num_clauses++;
                    const char* c=res[i].c_str();
                    if(c[0]=='-')
                    {
                        fout<<"-"<<m[res[i].substr(1,res[i].size()-1)]<<" 0"<<endl;
                    }
                    else
                    {
                        fout<<m[res[i]]<<" 0"<<endl;
                    }
                }
                break;
            }
            else{
                while(cin>>obs_str)
                {
                    //cout<<obs_str<<endl;
                    obs_str=split(obs_str);
                    //cout<<obs_str<<endl;
                    vector<string> res = split(obs_str.c_str(),",");
                    //cout<<res[1].c_str()<<endl;
                    if(atoi(res[1].c_str())==(obs_index+1)){

                        //cout<<"FALG"<<endl;
                        obs_Flag=true;
                        break;
                    }
                    else
                    {
                        for(int i=0;i<res.size();i++)
                        {
                            num_clauses++;
                            const char* c=res[i].c_str();
                            if(c[0]=='-')
                            {
                                fout<<"-"<<m[res[i].substr(1,res[i].size()-1)]<<" 0"<<endl;
                            }
                            else
                            {
                                fout<<m[res[i]]<<" 0"<<endl;
                            }
                        }
                    }
                }
            }
            if(obs_Flag==true)
            {break;}
        }
    }



//
//    for(int i=1;i<=in_len;i++){
//        stringstream ss;
//        ss<<i;
//        string tmp;
//        ss>>tmp;
//        s=s+tmp;
//        //cout<<s<<endl;
//        if(random(2)==0){
//            fout<<m[s]<<" 0"<<endl;
//            num_clauses+=1;
//        }
//        else
//        {
//            fout<<"-"<<m[s]<<" 0"<<endl;
//            num_clauses+=1;
//        }
//        s="i";
//    }
//    s="o";
//    for(int i=1;i<=out_len;i++){
//        stringstream ss;
//        ss<<i;
//        string tmp;
//        ss>>tmp;
//        s=s+tmp;
//        if(random(2)==0){
//            fout<<m[s]<<" 0"<<endl;
//        }
//        else
//        {
//            fout<<"-"<<m[s]<<" 0"<<endl;
//        }
//        s="o";
//    }

    ifstream fin1(infile_name);
    flag = false;
    while(fin1>>str){
        if(str[0]=='[' && str[1]=='['){
            flag = true;
        }
        if(flag){
            str = split(str);
            vector<string> res = split(str.c_str(),",");
            fout<<"-"<<m[res[1]]<<" 0"<<endl;
            num_clauses++;

        }
    }




//    for(int i=1;i<=gate_num;i++){
//        fout<<"-"<<i<<" 0"<<endl;
//        num_clauses++;
//    }
    num_vars=num_clauses;//?????

}

void Satlike::build_instance(char *filename)
{
	int     cur_lit;
	int     i,v,c;
    //int     temp_lit[MAX_VARS];

	ifstream infile(filename);
    if(!infile)
    {
		cout<<"c the input filename "<<filename<<" is invalid, please input the correct filename."<<endl;
		exit(-1);
	}

    /*** build problem data structures of the instance ***/

    allocate_memory();

	for (c = 0; c < num_clauses; c++)
	{
		clause_lit_count[c] = 0;
        clause_lit[c]=NULL;
	}
	for (v=1; v<=num_vars; ++v)
	{
		var_lit_count[v] = 0;
		var_lit[v]=NULL;
		var_neighbor[v]=NULL;
	}

    num_hclauses=num_sclauses=0;
	unit_clause_count=0;
	//Now, read the clauses, one at a time.
	int lit_redundent,clause_redundent;

	for (c = 0; c < num_clauses-gate_num-out_len; )
	{
		clause_redundent=0;
        clause_lit_count[c] = 0;

		infile>>cur_lit;
		while (cur_lit != 0) {

			temp_lit[clause_lit_count[c]] = cur_lit;
            clause_lit_count[c]++;
			infile>>cur_lit;
		}
		clause_lit[c] = new lit[clause_lit_count[c]+1];
		for(i=0; i<clause_lit_count[c]; ++i)
        {
            clause_lit[c][i].clause_num = c;
            clause_lit[c][i].var_num = abs(temp_lit[i]);

            if (temp_lit[i] > 0) clause_lit[c][i].sense = 1;
            else clause_lit[c][i].sense = 0;

            var_lit_count[clause_lit[c][i].var_num]++;
        }
        clause_lit[c][i].var_num=0;
        clause_lit[c][i].clause_num = -1;
		if(clause_lit_count[c]==1)
        {
            unit_clause[unit_clause_count++]=clause_lit[c][0];
            unit_clause_saved[unit_clause_count_saved++]=clause_lit[c][0];
        }
        c++;

	}
	for(c=num_clauses-gate_num-out_len;c<num_clauses-gate_num;c++)
    {
        infile>>cur_lit;
        //cout<<cur_lit<<endl;
        clause_lit[c] = new lit[2];
        clause_lit[c][0].clause_num = c;
        clause_lit[c][0].var_num = abs(cur_lit);
        if(cur_lit<0){
            clause_lit[c][0].sense = 0;
        }
        else
        {
            clause_lit[c][0].sense = 1;
        }
        //unit_out_clause[unit_out_clause_count++]=clause_lit[c][0];
        unit_out_clause.push(clause_lit[c][0]);

        var_lit_count[clause_lit[c][0].var_num]++;
        infile>>cur_lit;//���˵�0
    }
    //cout<<num_clauses<<" "<<gate_num<<" "<<endl;
	for(c=num_clauses-gate_num;c<num_clauses;c++)
    {
        infile>>cur_lit;
        //cout<<cur_lit<<endl;
        clause_lit[c] = new lit[2];
        clause_lit[c][0].clause_num = c;
        clause_lit[c][0].var_num = abs(cur_lit);
        clause_lit[c][0].sense = 0;
        unit_clause_saved[unit_clause_count_saved++]=clause_lit[c][0];

        var_lit_count[clause_lit[c][0].var_num]++;
        infile>>cur_lit;//���˵�0
    }
	infile.close();
	//creat var literal arrays
	for (v=1; v<=num_vars; ++v)
	{
		var_lit[v] = new lit[var_lit_count[v]+1];
		var_lit_count[v] = 0;	//reset to 0, for build up the array
	}
	//scan all clauses to build up var literal arrays
	//cout<<"num_clauses"<<num_clauses<<endl;
	for (c = 0; c < num_clauses; ++c)
	{
		for(i=0; i<clause_lit_count[c]; ++i)
		{
			v = clause_lit[c][i].var_num;
			//cout<<v<<" "<<var_lit_count[v]<<endl;
			var_lit[v][var_lit_count[v]++] = clause_lit[c][i];
		}
	}
    for (v=1; v<=num_vars; ++v)
		var_lit[v][var_lit_count[v]].clause_num=-1;


	build_neighbor_relation();

}

void Satlike::init(vector<int>& init_solution)
{
	int 		v,c;
	int			i,j;
    hard_unsat_nb=0;
    soft_unsat_weight=0;
    local_soln_feasible=0;
	local_opt_unsat_weight=top_clause_weight+1;

	if(best_soln_feasible==0)
	{
		for (c = 0; c<num_clauses; c++)
    	{
        already_in_soft_large_weight_stack[c]=0;
		clause_selected_count[c]=0;

		if (org_clause_weight[c]==top_clause_weight)
            clause_weight[c] = 1;
        else
		{
			clause_weight[c] = 0;
		}
    	}
	}
	else
	{
    //Initialize clause information
	for (c = 0; c<num_clauses; c++)
    {
        already_in_soft_large_weight_stack[c]=0;
		clause_selected_count[c]=0;

		if (org_clause_weight[c]==top_clause_weight)
            clause_weight[c] = 1;
        else
		{
			clause_weight[c] = org_clause_weight[c]*weight_multiplier;;
			if(clause_weight[c]>1 && already_in_soft_large_weight_stack[c]==0)
			{
				already_in_soft_large_weight_stack[c]=1;
				soft_large_weight_clauses[soft_large_weight_clauses_count++] = c;
			}
		}
    }
    }

    //init solution
    if(init_solution.size()==0)
    {
        for (v = 1; v <= num_vars; v++)
        {
            cur_soln[v]=0;
		    time_stamp[v] = 0;
		    unsat_app_count[v] = 0;
	    }
    }
	else
    {
        for (v = 1; v <= num_vars; v++)
        {
            cur_soln[v]=init_solution[v];
		    time_stamp[v] = 0;
		    unsat_app_count[v] = 0;
	    }
    }

	//init stacks
	hardunsat_stack_fill_pointer = 0;
    softunsat_stack_fill_pointer = 0;
	unsatvar_stack_fill_pointer = 0;
	large_weight_clauses_count=0;
    soft_large_weight_clauses_count=0;

	/* figure out sat_count, sat_var and init unsat_stack */
	for (c=0; c<num_clauses; ++c)
	{
		sat_count[c] = 0;
		for(j=0; j<clause_lit_count[c]; ++j)
		{
			if (cur_soln[clause_lit[c][j].var_num] == clause_lit[c][j].sense)
			{
				sat_count[c]++;
				sat_var[c] = clause_lit[c][j].var_num;
			}
		}
		if (sat_count[c] == 0)
        {
            unsat(c);
        }
	}

	/*figure out score*/
	for (v=1; v<=num_vars; v++)
	{
		score[v]=0;
		for(i=0; i<var_lit_count[v]; ++i)
		{
			c = var_lit[v][i].clause_num;
            if (sat_count[c]==0) score[v]+=clause_weight[c];
            else if (sat_count[c]==1 && var_lit[v][i].sense==cur_soln[v]) score[v]-=clause_weight[c];
		}
	}

	//init goodvars stack
	goodvar_stack_fill_pointer = 0;
	for (v=1; v<=num_vars; v++)
	{
		if(score[v]>0)
		{
			already_in_goodvar_stack[v] = goodvar_stack_fill_pointer;
			mypush(v,goodvar_stack);
		}
		else
			already_in_goodvar_stack[v] = -1;
	}
}

int Satlike::pick_var()
{
	int     i,v;
	int     best_var;

	if(goodvar_stack_fill_pointer>0 )
	{
		if( (rand()%MY_RAND_MAX_INT)*BASIC_SCALE< rdprob )
            return goodvar_stack[rand()%goodvar_stack_fill_pointer];

        if (goodvar_stack_fill_pointer < hd_count_threshold)
        {
		    best_var = goodvar_stack[0];
		    for (i=1; i<goodvar_stack_fill_pointer; ++i)
		    {
		    	v = goodvar_stack[i];
		    	if (score[v]>score[best_var]) best_var=v;
		        else if (score[v]==score[best_var])
		        {
		            if(time_stamp[v]<time_stamp[best_var]) best_var=v;
		        }
		    }
		    return best_var;
        }
        else
		{
		    best_var = goodvar_stack[rand()%goodvar_stack_fill_pointer];
			for (i=1; i<hd_count_threshold; ++i)
		    {
		        v = goodvar_stack[rand()%goodvar_stack_fill_pointer];
		        if (score[v]>score[best_var]) best_var=v;
		        else if (score[v]==score[best_var])
		        {
		        	if(time_stamp[v]<time_stamp[best_var]) best_var=v;
		        }
		    }
		    return best_var;
        }
	}

    update_clause_weights();

    int sel_c;
    lit* p;

    if (hardunsat_stack_fill_pointer>0)
    {
    	sel_c = hardunsat_stack[rand()%hardunsat_stack_fill_pointer];
    }
    else
    {
    	sel_c= softunsat_stack[rand()%softunsat_stack_fill_pointer];

		long long best_selected_num;
		int best_soft_clause_count;
    	int c;
    	best_soft_clause[0]=softunsat_stack[0];
    	best_selected_num=clause_selected_count[softunsat_stack[0]];
    	best_soft_clause_count=1;

    	for(int j=1;j<softunsat_stack_fill_pointer;++j)
    	{
    		c=softunsat_stack[j];
    		if(clause_selected_count[c]<best_selected_num)
    		{
    			best_soft_clause[0]=c;
    			best_selected_num=clause_selected_count[c];
    			best_soft_clause_count=1;
    		}
    		else if(clause_selected_count[c]==best_selected_num)
    		{
    			best_soft_clause[best_soft_clause_count]=c;
    			best_soft_clause_count++;
    		}
    	}
    	sel_c=best_soft_clause[rand()%best_soft_clause_count];
    	clause_selected_count[sel_c]++;
    }
    if( (rand()%MY_RAND_MAX_INT)*BASIC_SCALE< rwprob )
        return clause_lit[sel_c][rand()%clause_lit_count[sel_c]].var_num;

    best_var = clause_lit[sel_c][0].var_num;
    p=clause_lit[sel_c];
	for(p++; (v=p->var_num)!=0; p++)
    {
        if (score[v]>score[best_var]) best_var=v;
		else if (score[v]==score[best_var])
		{
			if(time_stamp[v]<time_stamp[best_var]) best_var=v;
		}
    }
    return best_var;
}

void Satlike::update_goodvarstack1(int flipvar)
{
	int v;
	//remove the vars no longer goodvar in goodvar stack
	for(int index=goodvar_stack_fill_pointer-1; index>=0; index--)
	{
		v = goodvar_stack[index];
		if(score[v]<=0)
		{
			goodvar_stack[index] = mypop(goodvar_stack);
			already_in_goodvar_stack[v] = -1;
		}
	}

	//add goodvar
	for(int i=0; i<var_neighbor_count[flipvar]; ++i)
	{
		v = var_neighbor[flipvar][i];
		if( score[v] > 0)
		{
            if(already_in_goodvar_stack[v] == -1)
            {
            	already_in_goodvar_stack[v] = goodvar_stack_fill_pointer;
                mypush(v,goodvar_stack);
            }
		}
	}
}
void Satlike::update_goodvarstack2(int flipvar)
{
	if(score[flipvar]>0 && already_in_goodvar_stack[flipvar]==-1)
	{
		already_in_goodvar_stack[flipvar]=goodvar_stack_fill_pointer;
		mypush(flipvar,goodvar_stack);
	}
	else if(score[flipvar]<=0 && already_in_goodvar_stack[flipvar]!=-1)
	{
		int index=already_in_goodvar_stack[flipvar];
		int last_v=mypop(goodvar_stack);
		goodvar_stack[index]=last_v;
		already_in_goodvar_stack[last_v]=index;
		already_in_goodvar_stack[flipvar]=-1;
	}
	int i,v;
	for(i=0; i<var_neighbor_count[flipvar]; ++i)
	{
		v = var_neighbor[flipvar][i];
		if( score[v] > 0)
		{
            if(already_in_goodvar_stack[v] == -1)
            {
            	already_in_goodvar_stack[v] = goodvar_stack_fill_pointer;
                mypush(v,goodvar_stack);
            }
		}
		else if(already_in_goodvar_stack[v]!=-1)
		{
			int index=already_in_goodvar_stack[v];
			int last_v=mypop(goodvar_stack);
			goodvar_stack[index]=last_v;
			already_in_goodvar_stack[last_v]=index;
			already_in_goodvar_stack[v]=-1;
		}
	}
}

void Satlike::flip(int flipvar)
{
	int i,v,c;
	int index;
	lit* clause_c;

    int	org_flipvar_score = score[flipvar];
	cur_soln[flipvar] = 1 - cur_soln[flipvar];

    for(i=0; i<var_lit_count[flipvar]; ++i)
	{
		c = var_lit[flipvar][i].clause_num;
		clause_c = clause_lit[c];

        if(cur_soln[flipvar] == var_lit[flipvar][i].sense)
        {
            ++sat_count[c];
            if (sat_count[c] == 2) //sat_count from 1 to 2
            {
                score[sat_var[c]] += clause_weight[c];
            }
            else if (sat_count[c] == 1) // sat_count from 0 to 1
            {
                sat_var[c] = flipvar;//record the only true lit's var
                for(lit* p=clause_c; (v=p->var_num)!=0; p++)
                {
                	score[v] -= clause_weight[c];
                }
                sat(c);
            }
        }
        else // cur_soln[flipvar] != cur_lit.sense
        {
            --sat_count[c];
            if (sat_count[c] == 1) //sat_count from 2 to 1
            {
                for(lit* p=clause_c; (v=p->var_num)!=0; p++)
                {
                     if(p->sense == cur_soln[v] )
                    {
                        score[v] -= clause_weight[c];
                        sat_var[c] = v;
                        break;
                    }
                }
            }
            else if (sat_count[c] == 0) //sat_count from 1 to 0
            {
                for(lit* p=clause_c; (v=p->var_num)!=0; p++)
                {
					score[v] += clause_weight[c];
				}
                unsat(c);
            }//end else if
        }//end else
	}

	//update information of flipvar
	score[flipvar] = -org_flipvar_score;
	update_goodvarstack1(flipvar);
}

void Satlike::local_search(vector<int>& init_solution)
{
//	settings();
//	max_flips = 200000000;
//	for(tries=1;tries<max_tries;++tries)
//	{
//		init(init_solution);
//		for (step = 1; step<max_flips; ++step)
//		{
//			if (hard_unsat_nb==0 && (soft_unsat_weight<opt_unsat_weight || best_soln_feasible==0) )
//        	{
//		    	if (soft_unsat_weight<top_clause_weight)
//		    	{
//		        	best_soln_feasible = 1;
//		        	opt_unsat_weight = soft_unsat_weight;
//					//opt_time = get_runtime();
//		        	for(int v=1; v<=num_vars; ++v) best_soln[v] = cur_soln[v];
//		    	}
//        	}
//			if (step%1000==0)
//        	{
//        		//double elapse_time=get_runtime();
//            	//if(elapse_time>=cutoff_time) return;
//				else if(opt_unsat_weight==0) return;
//        	}
//			int flipvar = pick_var();
//			flip(flipvar);
//			time_stamp[flipvar] = step;
//		}
//	}
}

void Satlike::print_best_solution()
{
	if (best_soln_feasible==0) return;

     printf("v");
     for (int i=1; i<=num_vars; i++) {
		printf(" ");
		if(best_soln[i]==0) printf("-");
		printf("%d", i);
     }
     printf("\n");
}

void Satlike::local_search_with_decimation(vector<int>& init_solution, char* inputfile)
{
//	settings();
//
//	Decimation deci(var_lit,var_lit_count,clause_lit,org_clause_weight,top_clause_weight);
//	deci.make_space(num_clauses,num_vars);
//
//	for(int i=1;i<=num_vars;++i)
//	{
//		local_opt_soln[i]=rand()%2;
//	}
//
//	for(tries=1;tries<max_tries;++tries)
//	{
//		deci.init(local_opt_soln,best_soln,unit_clause,unit_clause_count,clause_lit_count);
//
//		deci.unit_prosess();
//
//		init_solution.resize(num_vars+1);
//		for(int i=1;i<=num_vars;++i)
//		{
//			init_solution[i]=deci.fix[i];
//		}
//
//		init(init_solution);
//		for (step = 1; step<max_flips; ++step)
//		{
//			if (hard_unsat_nb==0 && (soft_unsat_weight<opt_unsat_weight || best_soln_feasible==0) )
//        	{
//        		if(best_soln_feasible==0)
//				{
//					best_soln_feasible=1;
//					opt_unsat_weight = soft_unsat_weight;
//					cout<<"o "<<opt_unsat_weight<<endl;
//					//opt_time = get_runtime();
//		        	for(int v=1; v<=num_vars; ++v) best_soln[v] = cur_soln[v];
//		        	break;
//				}
//		    	if (soft_unsat_weight<top_clause_weight)
//		    	{
//
//		        	opt_unsat_weight = soft_unsat_weight;
//		        	cout<<"o "<<opt_unsat_weight<<endl;
//
//					opt_time = get_runtime();
//		        	for(int v=1; v<=num_vars; ++v) best_soln[v] = cur_soln[v];
//
//		        	if(print1==1 || print2==1)
//		        	{
//		        		print_best_solution();
//		        	}
//
//		        	if(opt_unsat_weight==0)
//		        	{
//		        		print_best_solution();
//		        		return;
//		        	}
//		    	}
//        	}
//
//        	if(hard_unsat_nb==0 && (soft_unsat_weight<local_opt_unsat_weight || local_soln_feasible==0))
//			{
//				if(soft_unsat_weight<top_clause_weight)
//				{
//					local_soln_feasible=1;
//					local_opt_unsat_weight=soft_unsat_weight;
//					max_flips=step+max_non_improve_flip;
//					for(int v=1;v<=num_vars;++v) local_opt_soln[v]=cur_soln[v];
//				}
//			}
//
//
//			if (step%1000==0)
//        	{
//        		double elapse_time=get_runtime();
//
//        		if(print1==0 && elapse_time>print_time1 && elapse_time<60)
//        		{
//        			print1=1;
//        			if(best_soln_feasible==1)
//        				print_best_solution();
//        		}
//
//        		if(elapse_time > 60) print1=0;
//
//        		if(print2==0 && elapse_time>print_time2)
//        		{
//        			print2=1;
//        			if(best_soln_feasible==1)
//        				print_best_solution();
//        		}
//
//            	if(elapse_time>=cutoff_time) {deci.free_memory();return;}
//        	}
//
//			int flipvar = pick_var();
//			flip(flipvar);
//			time_stamp[flipvar] = step;
//		}
//	}
}

bool Satlike::verify_sol()
{
	int c,j,flag;
    long long verify_unsat_weight=0;

	for (c = 0; c<num_clauses; ++c)
	{
        flag = 0;
        for(j=0; j<clause_lit_count[c]; ++j)
            if (best_soln[clause_lit[c][j].var_num] == clause_lit[c][j].sense) {flag = 1; break;}

        if(flag ==0)
        {
            if(org_clause_weight[c]==top_clause_weight)//verify hard clauses
            {
                //output the clause unsatisfied by the solution
                cout<<"c Error: hard clause "<<c<<" is not satisfied"<<endl;

                cout<<"c ";
                for(j=0; j<clause_lit_count[c]; ++j)
                {
                    if(clause_lit[c][j].sense==0)cout<<"-";
                    cout<<clause_lit[c][j].var_num<<" ";
                }
                cout<<endl;
                cout<<"c ";
                for(j=0; j<clause_lit_count[c]; ++j)
                    cout<<best_soln[clause_lit[c][j].var_num]<<" ";
                cout<<endl;
                return 0;
            }
            else
            {
                verify_unsat_weight+=org_clause_weight[c];
            }
        }
    }

    if(verify_unsat_weight==opt_unsat_weight)  return 1;
    else{
    	cout<<"c Error: find opt="<<opt_unsat_weight<<", but verified opt="<<verify_unsat_weight<<endl;
    }
    return 0;
}

void Satlike::simple_print()
{
	if(best_soln_feasible==1)
	{
		if(verify_sol()==1)
			cout<<opt_unsat_weight<<'\t'<<opt_time<<endl;
		else
			cout<<"solution is wrong "<<endl;
	}
	else
		cout<<-1<<'\t'<<-1<<endl;
}

void Satlike::increase_weights()
{
	int i,c,v;
    for(i=0; i < hardunsat_stack_fill_pointer; ++i)
    {
        c=hardunsat_stack[i];
		clause_weight[c]+=h_inc;

        if(clause_weight[c] == (h_inc+1))
			large_weight_clauses[large_weight_clauses_count++] = c;

        for(lit* p=clause_lit[c]; (v=p->var_num)!=0; p++)
		{
			score[v]+=h_inc;
            if (score[v]>0 && already_in_goodvar_stack[v]==-1)
            {
                already_in_goodvar_stack[v] = goodvar_stack_fill_pointer;
                mypush(v,goodvar_stack);
            }
		}
    }
    if(best_soln_feasible!=0)
    {
	for(i=0; i < softunsat_stack_fill_pointer; ++i)
    {
        c=softunsat_stack[i];
        if(clause_weight[c]>softclause_weight_threshold) continue;
		else clause_weight[c]++;

        if(clause_weight[c]>1 && already_in_soft_large_weight_stack[c]==0)
        {
            already_in_soft_large_weight_stack[c]=1;
			soft_large_weight_clauses[soft_large_weight_clauses_count++] = c;
        }
        for(lit* p=clause_lit[c]; (v=p->var_num)!=0; p++)
		{
			score[v]++;
            if (score[v]>0 && already_in_goodvar_stack[v]==-1)
            {
				already_in_goodvar_stack[v] = goodvar_stack_fill_pointer;
                mypush(v,goodvar_stack);
            }
		}
    }
    }
}

void Satlike::smooth_weights()
{
	int i, clause, v;

	for(i=0; i<large_weight_clauses_count; i++)
	{
		clause = large_weight_clauses[i];
		if(sat_count[clause]>0)
		{
			clause_weight[clause]-=h_inc;

			if(clause_weight[clause]==1)
			{
				large_weight_clauses[i] = large_weight_clauses[--large_weight_clauses_count];
				i--;
			}
			if(sat_count[clause] == 1)
			{
				v = sat_var[clause];
				score[v]+=h_inc;
                if (score[v]>0 && already_in_goodvar_stack[v]==-1)
                {
					already_in_goodvar_stack[v] = goodvar_stack_fill_pointer;
                    mypush(v,goodvar_stack);
                }
			}
		}
	}
	if(best_soln_feasible!=0)
	{
	for(i=0; i<soft_large_weight_clauses_count; i++)
	{
		clause = soft_large_weight_clauses[i];
		if(sat_count[clause]>0)
		{
			clause_weight[clause]--;
			if(clause_weight[clause]==1 && already_in_soft_large_weight_stack[clause]==1)
			{
                already_in_soft_large_weight_stack[clause]=0;
				soft_large_weight_clauses[i] = soft_large_weight_clauses[--soft_large_weight_clauses_count];
				i--;
			}
			if(sat_count[clause] == 1)
			{
				v = sat_var[clause];
				score[v]++;
                if (score[v]>0 && already_in_goodvar_stack[v]==-1)
                {
					already_in_goodvar_stack[v] = goodvar_stack_fill_pointer;
                    mypush(v,goodvar_stack);
                }
			}
		}
	}
	}
}

void Satlike::update_clause_weights()
{
	if( ((rand()%MY_RAND_MAX_INT)*BASIC_SCALE)<smooth_probability
       && large_weight_clauses_count>large_clause_count_threshold  )
	{
		smooth_weights();
	}
	else
	{
		increase_weights();
	}
}

inline void Satlike::unsat(int clause)
{
    if(org_clause_weight[clause]==top_clause_weight)
    {
        index_in_hardunsat_stack[clause] = hardunsat_stack_fill_pointer;
        mypush(clause,hardunsat_stack);
        hard_unsat_nb++;
    }
    else
    {
        index_in_softunsat_stack[clause] = softunsat_stack_fill_pointer;
        mypush(clause,softunsat_stack);
        soft_unsat_weight += org_clause_weight[clause];
    }
}

inline void Satlike::sat(int clause)
{
	int index,last_unsat_clause;

    if (org_clause_weight[clause]==top_clause_weight)
    {
        last_unsat_clause = mypop(hardunsat_stack);
        index = index_in_hardunsat_stack[clause];
        hardunsat_stack[index] = last_unsat_clause;
        index_in_hardunsat_stack[last_unsat_clause] = index;

        hard_unsat_nb--;
    }
    else
    {
        last_unsat_clause = mypop(softunsat_stack);
        index = index_in_softunsat_stack[clause];
        softunsat_stack[index] = last_unsat_clause;
        index_in_softunsat_stack[last_unsat_clause] = index;

        soft_unsat_weight -= org_clause_weight[clause];
    }
}

#endif
