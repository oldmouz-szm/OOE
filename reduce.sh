#bin/bash

g++ -o ./main ./pms.cpp
 echo $1
 count=1
rm -f resduced.txt
rm -f resducedc432.txt
rm -f resducedc499.txt
rm -f resducedc880.txt
rm -f resducedc1355.txt
rm -f resducedc1908.txt
rm -f resducedc2670.txt
rm -f resducedc3540.txt
rm -f resducedc5315.txt
rm -f resducedc6288.txt
rm -f resducedc7552.txt

rm -f resduced74181.txt
rm -f resduced74182.txt
rm -f resduced74183.txt
while [ $count -le 63 ]
do
  ./main ./systems/c17.sys systems/c17.txt ./systems/c17.syspp ./iscas85/c17_iscas85.obs $count >> resduced.txt
  cp remain_clauses.txt  systems/c17/c17_$count.wcnf
  cp remain_clauses1.txt systems/c17_1/c17_$count.wcnf
  cp remain_clauses2.txt systems/c17_2/c17_$count.wcnf
  count=$((count+1)) 
done

count2=1
while [ $count2 -le 301 ]
do
  ./main ./systems/c432.sys systems/c432.txt ./systems/c432.syspp ./iscas85/c432_iscas85.obs $count2 >> resducedc432.txt
  cp remain_clauses.txt systems/c432/c432_$count2.wcnf
  cp remain_clauses1.txt systems/c432_1/c432_$count2.wcnf
  cp remain_clauses2.txt systems/c432_2/c432_$count2.wcnf
  count2=$((count2+1))
done

count3=1
while [ $count3 -le 835 ]
do
  ./main ./systems/c499.sys systems/c499.txt ./systems/c499.syspp ./iscas85/c499_iscas85.obs $count3 >> resducedc499.txt
  cp remain_clauses.txt systems/c499/c499_$count3.wcnf
  cp remain_clauses1.txt systems/c499_1/c499_$count3.wcnf
  cp remain_clauses2.txt systems/c499_2/c499_$count3.wcnf
  count3=$((count3+1)) 
done

count3=1
while [ $count3 -le 1182 ]
do
  ./main ./systems/c880.sys systems/c880.txt ./systems/c880.syspp ./iscas85/c880_iscas85.obs $count3 >> resducedc880.txt
  cp remain_clauses.txt systems/c880/c880_$count3.wcnf
  cp remain_clauses1.txt systems/c880_1/c880_$count3.wcnf
  cp remain_clauses2.txt systems/c880_2/c880_$count3.wcnf
  #echo $count3
  count3=$((count3+1)) 
done

count3=1
while [ $count3 -le 836 ]
do
  ./main ./systems/c1355.sys systems/c1355.txt ./systems/c1355.syspp ./iscas85/c1355_iscas85.obs $count3 >> resducedc1355.txt
  cp remain_clauses.txt systems/c1355/c1355_$count3.wcnf
  cp remain_clauses1.txt systems/c1355_1/c1355_$count3.wcnf
  cp remain_clauses2.txt systems/c1355_2/c1355_$count3.wcnf
  count3=$((count3+1)) 
done

count3=1
while [ $count3 -le 846 ]
do
  ./main ./systems/c1908.sys systems/c1908.txt ./systems/c1908.syspp ./iscas85/c1908_iscas85.obs $count3 >> resducedc1908.txt
  cp remain_clauses.txt systems/c1908/c1908_$count3.wcnf
  cp remain_clauses1.txt systems/c1908_1/c1908_$count3.wcnf
  cp remain_clauses2.txt systems/c1908_2/c1908_$count3.wcnf
  count3=$((count3+1)) 
done

count3=1
while [ $count3 -le 1162 ]
do
  ./main ./systems/c2670.sys systems/c2670.txt ./systems/c2670.syspp ./iscas85/c2670_iscas85.obs $count3 >> resducedc2670.txt
cp remain_clauses.txt systems/c2670/c2670_$count3.wcnf
cp remain_clauses1.txt systems/c2670_1/c2670_$count3.wcnf
cp remain_clauses2.txt systems/c2670_2/c2670_$count3.wcnf
  count3=$((count3+1)) 
done

count3=1
while [ $count3 -le 756 ]
do
  ./main ./systems/c3540.sys systems/c3540.txt ./systems/c3540.syspp ./iscas85/c3540_iscas85.obs $count3 >> resducedc3540.txt
cp remain_clauses.txt systems/c3540/c3540_$count3.wcnf
cp remain_clauses1.txt systems/c3540_1/c3540_$count3.wcnf
cp remain_clauses2.txt systems/c3540_2/c3540_$count3.wcnf
  count3=$((count3+1)) 
done

count3=1
while [ $count3 -le 2038 ]
do
  ./main ./systems/c5315.sys systems/c5315.txt ./systems/c5315.syspp ./iscas85/c5315_iscas85.obs $count3 >> resducedc5315.txt
cp remain_clauses.txt systems/c5315/c5315_$count3.wcnf
cp remain_clauses1.txt systems/c5315_1/c5315_$count3.wcnf
cp remain_clauses2.txt systems/c5315_2/c5315_$count3.wcnf
  count3=$((count3+1)) 
done

count3=1
while [ $count3 -le 1557 ]
do
  ./main ./systems/c7552.sys systems/c7552.txt ./systems/c7552.syspp ./iscas85/c7552_iscas85.obs $count3 >> resducedc7552.txt
cp remain_clauses.txt systems/c7552/c7552_$count3.wcnf
cp remain_clauses1.txt systems/c7552_1/c7552_$count3.wcnf
cp remain_clauses2.txt systems/c7552_2/c7552_$count3.wcnf
  count3=$((count3+1)) 
done

count3=1
while [ $count3 -le 404 ]
do
  ./main ./systems/c6288.sys systems/c6288.txt ./systems/c6288.syspp ./iscas85/c6288_iscas85.obs $count3 >> resducedc6288.txt
cp remain_clauses.txt systems/c6288/c6288_$count3.wcnf
cp remain_clauses1.txt systems/c6288_1/c6288_$count3.wcnf
cp remain_clauses2.txt systems/c6288_2/c6288_$count3.wcnf
  count3=$((count3+1)) 
done

count3=1
while [ $count3 -le 350 ]
do
  ./main ./systems/74181.sys systems/74181.txt ./systems/74181.syspp ./iscas85/74181_iscas85.obs $count3 >> resduced74181.txt
cp remain_clauses.txt systems/74181/74181_$count3.wcnf
cp remain_clauses1.txt systems/74181_1/74181_$count3.wcnf
cp remain_clauses2.txt systems/74181_2/74181_$count3.wcnf
  count3=$((count3+1)) 

count3=1
while [ $count3 -le 250 ]
do
  ./main ./systems/74182.sys systems/74182.txt ./systems/74182.syspp ./iscas85/74182_iscas85.obs $count3 >> resduced74182.txt
cp remain_clauses.txt systems/74182/74182_$count3.wcnf
cp remain_clauses1.txt systems/74182_1/74182_$count3.wcnf
cp remain_clauses2.txt systems/74182_2/74182_$count3.wcnf
  count3=$((count3+1)) 

count3=1
while [ $count3 -le 202 ]
do
  ./main ./systems/74183.sys systems/74183.txt ./systems/74183.syspp ./iscas85/74183_iscas85.obs $count3 >> resduced74183.txt
cp remain_clauses.txt systems/74183/c74183_$count3.wcnf
cp remain_clauses1.txt systems/74183_1/c74183_$count3.wcnf
cp remain_clauses2.txt systems/74183_2/c74183_$count3.wcnf
  count3=$((count3+1)) 
done
#./distupls $file 300 50000 >>distupls_result50000.txt
#./distupls $file 300 1000000　>>distupls_result1.txt
#./distupls $file 300 2000000　>>distupls_result2.txt
#./126 $file $file+flipcom.cnf 60 50000　>$file+126ressult60.cnf
#./126 $file $file+flipcom.cnf 300 50000　>$file+126ressult60.cnf


