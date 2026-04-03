#bin/bash
#g++ -o ./main ./pms.cpp
count3=1
while [ $count3 -le 919 ]
do
  ./main ./systems/b14_C.sys systems/b14_C.txt ./systems/b14_C.syspp ./iscas85/b14_C.obs $count3 >> resducedb14_C.txt
cp remain_clauses.txt systems/b14_C/b14_C_$count3.wcnf
cp remain_clauses1.txt systems/b14_C_1/b14_C_$count3.wcnf
cp remain_clauses2.txt systems/b14_C_2/b14_C_$count3.wcnf
  count3=$((count3+1)) 
done
count3=1
while [ $count3 -le 995 ]
do
  ./main ./systems/b15_C.sys systems/b15_C.txt ./systems/b15_C.syspp ./iscas85/b15_C.obs $count3 >> resducedb15_C.txt
cp remain_clauses.txt systems/b15_C/b15_C_$count3.wcnf
cp remain_clauses1.txt systems/b15_C_1/b15_C_$count3.wcnf
cp remain_clauses2.txt systems/b15_C_2/b15_C_$count3.wcnf
  count3=$((count3+1)) 
done
count3=1
while [ $count3 -le 1000 ]
do
  ./main ./systems/b17_C.sys systems/b17_C.txt ./systems/b17_C.syspp ./iscas85/b17_C.obs $count3 >> resducedb17_C.txt
cp remain_clauses.txt systems/b17_C/b17_C_$count3.wcnf
cp remain_clauses1.txt systems/b17_C_1/b17_C_$count3.wcnf
cp remain_clauses2.txt systems/b17_C_2/b17_C_$count3.wcnf
  count3=$((count3+1)) 
done
count3=1
while [ $count3 -le 1000 ]
do
  ./main ./systems/b18_C.sys systems/b18_C.txt ./systems/b18_C.syspp ./iscas85/b18_C.obs $count3 >> resducedb18_C.txt
cp remain_clauses.txt systems/b18_C/b18_C_$count3.wcnf
cp remain_clauses1.txt systems/b18_C_1/b18_C_$count3.wcnf
cp remain_clauses2.txt systems/b18_C_2/b18_C_$count3.wcnf
  count3=$((count3+1)) 
done



