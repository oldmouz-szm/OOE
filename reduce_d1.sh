#bin/bash
#g++ -o ./main ./pms.cpp

count3=1
while [ $count3 -le 999 ]
do
  ./main ./systems/b19_C.sys systems/b19_C.txt ./systems/b19_C.syspp ./iscas85/b19_C.obs $count3 >> resducedb19_C.txt
cp remain_clauses.txt systems/b19_C/b19_C_$count3.wcnf
cp remain_clauses1.txt systems/b19_C_1/b19_C_$count3.wcnf
cp remain_clauses2.txt systems/b19_C_2/b19_C_$count3.wcnf
  count3=$((count3+1)) 
done
count3=1
while [ $count3 -le 997 ]
do
  ./main ./systems/b20_C.sys systems/b20_C.txt ./systems/b20_C.syspp ./iscas85/b20_C.obs $count3 >> resducedb20_C.txt
cp remain_clauses.txt systems/b20_C/b20_C_$count3.wcnf
cp remain_clauses1.txt systems/b20_C_1/b20_C_$count3.wcnf
cp remain_clauses2.txt systems/b20_C_2/b20_C_$count3.wcnf
  count3=$((count3+1)) 
done
count3=1
while [ $count3 -le 992 ]
do
  ./main ./systems/b21_C.sys systems/b21_C.txt ./systems/b21_C.syspp ./iscas85/b21_C.obs $count3 >> resducedb21_C.txt
cp remain_clauses.txt systems/b21_C/b21_C_$count3.wcnf
cp remain_clauses1.txt systems/b21_C_1/b21_C_$count3.wcnf
cp remain_clauses2.txt systems/b21_C_2/b21_C_$count3.wcnf
  count3=$((count3+1)) 
done
count3=1
while [ $count3 -le 1000 ]
do
  ./main ./systems/b22_C.sys systems/b22_C.txt ./systems/b22_C.syspp ./iscas85/b22_C.obs $count3 >> resducedb22_C.txt
cp remain_clauses.txt systems/b22_C/b22_C_$count3.wcnf
cp remain_clauses1.txt systems/b22_C_1/b22_C_$count3.wcnf
cp remain_clauses2.txt systems/b22_C_2/b22_C_$count3.wcnf
  count3=$((count3+1)) 
done

