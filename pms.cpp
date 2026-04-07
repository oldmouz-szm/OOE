#include "basis_pms.h"
#include "pms.h"
#include <stdlib.h>


Satlike s;


int main(int argc, char* argv[])
{
	//start_timing();

    if (argc < 6)
    {
        cout << "Usage: " << argv[0] << " <sys> <cnf_in> <dom_file> <obs_file> <obs_index> [mode]" << endl;
        cout << "  mode: be | doe | ooe | all (default: be)" << endl;
        return -1;
    }

    string mode = "be";
    if (argc >= 7)
    {
        mode = argv[6];
    }


    //ofstream fout("out1.txt");
	//fout<<argv[1]<<"  ";

	//vector<int> init_solution;

	//s.check_lit();

	cout<<argv[1]<<endl;
    //for(int obs_index=1;obs_index<=63;obs_index++)
    //{
        //int obs_index=15;
        //cout<<argv[1]<<endl;
        s.seq(argv[1]);
        //s.seq_dominate(argv[1],argv[2]);

        cout<<s.in_len<<" "<<s.out_len<<" "<<s.gate_num<<endl;
        s.input1(argv[1],argv[4],argv[2],s.in_len,s.out_len,s.gate_num,atoi(argv[5]));
    	cout<<"OBS_index:  "<<atoi(argv[5])<<"  ";
        s.build_instance(argv[2]);
        vector<int> initial_v_c=s.get_var_clause_initial();
        s.num_v=initial_v_c[initial_v_c.size()-1];
        s.num_c=initial_v_c[initial_v_c.size()-2];
        //char* file_name="";
        //fout<<"p wcnf "<<s.num_v<<" "<<s.num_v<<" "<<s.gate_num+1<<endl;
        ifstream in(argv[2]); //源文件读
        ofstream out( "remain_clauses.wcnf" ); //目标文件写
        if (!in){
            cout <<"open source file error!"<<endl;
            return -1;
        }
        out<<"p wcnf "<<s.num_v<<" "<<s.num_c<<" "<<s.gate_num+1<<endl;
        char str[1024];
        in.getline(str,sizeof(str),'\n'); //读一行
        int tmp_clauses_num=s.num_c-s.gate_num;
        while(tmp_clauses_num>0 && in.getline(str,sizeof(str), '\n')) //文件未结束，循环
        {
            tmp_clauses_num--;
            out<<s.gate_num+1<<" "<< str <<endl; //输出到文件
        }
        tmp_clauses_num=s.gate_num;
        while(tmp_clauses_num>0 && in.getline(str,sizeof(str), '\n')) //文件未结束，循环
        {
            tmp_clauses_num--;
            out<<1<<" "<< str <<endl; //输出到文件
        }
        in.close();
        out.close();
        //cout<<num_v<<" "<<num_c<<endl;
    	//cout<<"build finish"<<endl;
        if (mode == "doe" || mode == "ooe" || mode == "all")
        {
            s.dominated_relation(argv[3]);
            s.reduce_clauses1();
        }

        // OOE is built on top of the DOE preprocessing path in this implementation.
        if (mode == "ooe" || mode == "all")
        {
            s.save();
            s.gain_F_value();
        }


    return 0;
}
