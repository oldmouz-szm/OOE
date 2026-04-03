#bin/bash
count3=1
while [ $count3 -le 999 ]
do
  ./gen_b ./systems/b19_C.sys systems/b19_C.wcnf ./systems/b19_C.syspp ./iscas85/b19_C.obs $count3 >> resducedb19_C.wcnf
cp remain_clauses.wcnf systems/b19_C/b19_C_$count3.wcnf
echo $count3 
count3=$((count3+1)) 
done
