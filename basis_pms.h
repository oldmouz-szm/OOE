#ifndef _BASIS_PMS_H_
#define _BASIS_PMS_H_


#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <queue>
#include <set>
#include <stdio.h>
#include <stdlib.h>

#include <cstdio>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <map>
#include <queue>
#include <cstring>
#include <sstream>
#include <algorithm>


#define random(x) (rand()%x)
//#include <times.h> //these two h files are for timing in linux
//#include <unistd.h>

using namespace std;

#define mypop(stack) stack[--stack ## _fill_pointer]
#define mypush(item, stack) stack[stack ## _fill_pointer++] = item

const float       MY_RAND_MAX_FLOAT = 10000000.0;
const int   	  MY_RAND_MAX_INT =   10000000;
const float 	  BASIC_SCALE = 0.0000001; //1.0f/MY_RAND_MAX_FLOAT;

// Define a data structure for a literal.
struct lit
{
    int             clause_num;		//clause num, begin with 0
    int             var_num;		//variable num, begin with 1
    bool             sense;			//is 1 for true literals, 0 for false literals.
    int B_Node_attr;//��Ϊ1��blocked Node�ڵ�,2Ϊblocked Edge
    int filtered_attr;//��Ϊ1 ��filtered_node,2Ϊfiltered_edge
    int lit_attr;//1=gate;2=sysin,3=sysout;4=internal_var
    int min_clauses;
    int max_clauses;//������������ʱû��
};

struct Node{
    string name;
    int idx;
};

struct Gate
{
    string gate_name;
    string gate_attr;
    int influ_out_num;//��ʱδ����
    int dominat_attr; //0:û��ͳ�ι�ϵ 1����ͳ�� 2��ͳ����
    string dominator;//�������Ǳ�ͳ�ε��ţ��ñ�������ͳ����
    int section_num;
    vector<int> in_vars;
    int out_var;
    int first_clauses;
    int last_clauses;


};

class map_value_finder
{
public:
       map_value_finder(const int &cmp_string):m_s_cmp_string(cmp_string){}
       bool operator ()(const map<string, int>::value_type &pair)
       {
            return pair.second == m_s_cmp_string;
       }
private:
        const int &m_s_cmp_string;
};
//static struct tms start_time;
//static double get_runtime()
//{
//    struct tms stop;
//    times(&stop);
//    return (double)(stop.tms_utime-start_time.tms_utime+stop.tms_stime-start_time.tms_stime)/sysconf(_SC_CLK_TCK);
//}
//static void start_timing()
//{
//    times(&start_time);
//}

class Satlike
{
	public:
	int weight_multiplier;
	/***********non-algorithmic information ****************/
	int problem_weighted;
	int partial; //1 if the instance has hard clauses, and 0 otherwise.

	int max_clause_length;
	int min_clause_length;

	//size of the instance
	int     num_vars;		//var index from 1 to num_vars
	int     num_clauses;		//clause index from 0 to num_clauses-1
	int     num_hclauses;
	int     num_sclauses;

	//steps and time
	int tries;
	int max_tries;
	unsigned int max_flips;
	unsigned int max_non_improve_flip;
	unsigned int step;

	int print_time;
	int print_time1;
	int print_time2;
	int print1;
	int print2;
	int cutoff_time;
	int cutoff_time1;
	int cutoff_time2;
	int prioup_time;
	double  opt_time;

	/**********end non-algorithmic information*****************/
	/* literal arrays */
	lit**	var_lit;				//var_lit[i][j] means the j'th literal of var i.
	int*	var_lit_count;			//amount of literals of each var
	lit**	clause_lit;			//clause_lit[i][j] means the j'th literal of clause i.
	int*	clause_lit_count; 			// amount of literals in each clause
    int* clause_delete;
    lit** clause_lit_saved;
    int* clause_lit_count_saved;
    int* clause_delete_saved;
	Gate* gate;

	/* Information about the variables. */
	long long*	score;
	long long*	time_stamp;
	int**	var_neighbor;
	int*	var_neighbor_count;
	int*	neighbor_flag;
	int*	temp_neighbor;

	int* var_gate_neighbor_count;
	int** var_gate_neighbor;
	int* gate_neighbor_flag;
	int* temp_gate_neighbor;
	int* in_gate;

	/* Information about the clauses */
	long long	top_clause_weight;
	long long*	org_clause_weight;
	long long	total_soft_weight;
	long long*	clause_weight;
	int*	sat_count;
	int*	sat_var;
	long long*	clause_selected_count;
	int*	best_soft_clause;

	//original unit clause stack
	lit* unit_clause;
	lit* unit_clause_saved;
	//lit* unit_out_clause;
	queue<lit> unit_out_clause;

	int unit_clause_count=0;
	int unit_clause_count_saved=0;
	//int unit_out_clause_count;

	lit* unit_gate;

	//unsat clauses stack
	int*	hardunsat_stack;		//store the unsat clause number
	int*	index_in_hardunsat_stack;//which position is a clause in the unsat_stack
	int 	hardunsat_stack_fill_pointer;

	int*	softunsat_stack;		//store the unsat clause number
	int*	index_in_softunsat_stack;//which position is a clause in the unsat_stack
	int 	softunsat_stack_fill_pointer;

	//variables in unsat clauses
	int*	unsatvar_stack;
	int		unsatvar_stack_fill_pointer;
	int*	index_in_unsatvar_stack;
	int*	unsat_app_count;		//a varible appears in how many unsat clauses

	//good decreasing variables (dscore>0 and confchange=1)
	int*	goodvar_stack;
	int		goodvar_stack_fill_pointer;
	int*	already_in_goodvar_stack;

	/* Information about solution */
	int*    cur_soln;	//the current solution, with 1's for True variables, and 0's for False variables
	int*    best_soln;
	int*	local_opt_soln;
	int     best_soln_feasible; //when find a feasible solution, this is marked as 1.
	int		local_soln_feasible;
	int     hard_unsat_nb;
	long long    soft_unsat_weight;
	long long    opt_unsat_weight;
	long long	 local_opt_unsat_weight;

	//clause weighting
	int*   large_weight_clauses;
	int    large_weight_clauses_count;
	int    large_clause_count_threshold;

	int*   soft_large_weight_clauses;
	int*   already_in_soft_large_weight_stack;
	int    soft_large_weight_clauses_count;
	int    soft_large_clause_count_threshold;

	//tem data structure used in algorithm
	int* best_array;
	int  best_count;
	int* temp_lit;

	//parameters used in algorithm
	float rwprob;
	float rdprob;
	float  smooth_probability;
	int hd_count_threshold;
	int h_inc;
	int softclause_weight_threshold;

	//function used in algorithm
	void build_neighbor_relation();
	void allocate_memory();

	bool verify_sol();
	void increase_weights();
	void smooth_weights();
	void update_clause_weights();
	void unsat(int clause);
	void sat(int clause);
	void init(vector<int>& init_solution);
	void flip(int flipvar);
	void update_goodvarstack1(int flipvar);
	void update_goodvarstack2(int flipvar);
	int pick_var();
	void settings();



	int in_len=0;
    int out_len=0;
    int gate_num=0;
    int diag_len=0;
    map<string,int> m;
    map<string,int> mp;  //from name to idx
    map<int,string> mp_gate;  //idx to gate string
    map<int,vector<int> > graph;  //�����洢����ͼ
    int cur_idx = 1;   //graph idx starts with 1

    int* assigned_B;
    int* confict_1; //����
    int* confict_0; //����
    int confict_count=0;
    int* B1_value;
    int* B1_num;
    int* B2_value;
    int* B2_num;
    int* F_value;
    //int*  z_count=new int[5000];
    vector<int> z_count;
    vector<string> dominator_str;
    vector< map<int,int> > Blocked_Edge;
    //vector< int > Dominated;
    //vector< int > Filtered_Node;
    int* Filtered_Node;
    int* Conflict_Node;
    int Filtered_Node_count=0;
    int* Dominated;
    int Dominated_count=0;
    vector<map<int,int> > Filtered_Edge;
    queue<int> q;
    set<int> st;
    int num_c;
    int num_v;
	Satlike();
	void save();
	void build_instance(char *filename);
	void check_lit();
	void input1(char *infile,char* obs_file,char*outfile,int in_len,int out_len,int gata_num,int obs_index);
	vector<string> getRes(string str);
	void input();
	void dominated_relation(char* filename);
	void seq(char* filename);
	void seq_dominate(char* filename,char* outfile);
	void reduce_clauses();//ͨ��ͳ�ι�ϵ�����Ӿ�
	int reduce_clauses1();
	void get_num(int num);
	vector<int> get_var_clause();
	vector<int> get_var_clause_initial();
	void write_remaining_clauses(const char* infile,vector<int> v);
	//void gain_B_value();
	int gain_F_value();
	void recover_clauses_lit();
	void reduce_clauses2();
	void assign1();
	void assign_F();
	void assign_B();
	void unit_propagation();
	string findBegin(string str);
	string split_syspp(string str);
	string split(string str);
	vector<string> split(const char *s, const char *delim);
	void local_search(vector<int>& init_solution);
	void local_search_with_decimation(vector<int>& init_solution, char* inputfile);
	void simple_print();
	void print_best_solution();
	void free_memory();
};


#endif

