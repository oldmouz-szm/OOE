#include "basis_pms.h"
#include "pms.h"
#include <stdlib.h>


Satlike s;


int main(int argc, char* argv[])
{
    if(argc < 6)
    {
        cout << "Usage: " << argv[0] << " <sys> <tmp_cnf> <syspp> <obs> <obs_index>" << endl;
        return 1;
    }

	//start_timing();


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
        while( !in.eof() && tmp_clauses_num>0 ) //文件未结束，循环
        {
            char str[1024];
            in.getline(str,sizeof(str),'\n'); //读一行
            tmp_clauses_num--;
            out<<s.gate_num+1<<" "<< str <<endl; //输出到文件
        }
        tmp_clauses_num=s.gate_num;
        while( !in.eof() && tmp_clauses_num>0) //文件未结束，循环
        {
            char str[1024];
            in.getline(str,sizeof(str),'\n'); //读一行
            tmp_clauses_num--;
            out<<1<<" "<< str <<endl; //输出到文件
        }
        //啵啵早上吃的啥？吃的啥？吃的神资源的米饭你咋知道
        //因为我刚才好像问过你了 才想起来你吃了啥
        //我吃的月饼和苹果  你bun能吃太多甜的 好的，月饼已经没了，之后就不吃甜的了 我等会就去吃午饭饭饭
        //去哪吃  去楼下吃，砂刀削面 你咋不自己做吃  没买菜菜  nonono 因为你不想做  哈哈啊啊啊啊
        //继续代码吧加油 ⛽️⛽️⛽️⛽️⛽️⛽️⛽️这是啥 是油？对， 加油的图标 哼唧 爱啵啵 ❤️❤️❤️❤️❤️哈哈哈哈哈拜拜 拜拜
        in.close();
        out.close();
        //cout<<num_v<<" "<<num_c<<endl;
    	//cout<<"build finish"<<endl;
        s.dominated_relation(argv[3]);
//    	cout<<"dominated relation finish"<<endl;
//    	cout<<argv[2]<<endl;
//    	cout<<argv[3]<<endl;
//    	cout<<argv[4]<<endl;
        //s.reduce_clauses();
        int r1=s.reduce_clauses1();
        s.save();
        int r2=s.gain_F_value();
    		cout << " DOE_reduction=" << r1 << " OOE_reduction=" << r2;


    return 0;
}
