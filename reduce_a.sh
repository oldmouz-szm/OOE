#bin/bash
#g++ -o ./main ./pms.cpp


count3=1
while [ $count3 -le 1000 ]
do
  ./main ./systems/b22_C.sys systems/b22_C.wcnf ./systems/b22_C.syspp ./iscas85/b22_C.obs $count3 >> resducedb22_C.wcnf
/home/dell/下载/RC2/rc_solver/lib/python2.7/site-packages/pysat/examples/rc2.py -c b remain_clauses.wcnf >> run_result22_1.wcnf
/home/dell/下载/RC2/rc_solver/lib/python2.7/site-packages/pysat/examples/rc2.py -c b remain_clauses1.wcnf >> run_result22_2.wcnf
/home/dell/下载/RC2/rc_solver/lib/python2.7/site-packages/pysat/examples/rc2.py -c b remain_clauses2.wcnf >> run_result22_3.wcnf
echo b22_$count3  
count3=$((count3+1)) 
done


count3=1
while [ $count3 -le 999 ]
do
  ./main ./systems/b19_C.sys systems/b19_C.wcnf ./systems/b19_C.syspp ./iscas85/b19_C.obs $count3 >> resducedb19_C.wcnf
#cp remain_clauses.wcnf systems/b19_C/b19_C_$count3.wcnf
#cp remain_clauses1.wcnf systems/b19_C_1/b19_C_$count3.wcnf
#cp remain_clauses2.wcnf systems/b19_C_2/b19_C_$count3.wcnf
/home/dell/下载/RC2/rc_solver/lib/python2.7/site-packages/pysat/examples/rc2.py -c b remain_clauses.wcnf >> run_result19_1.wcnf
/home/dell/下载/RC2/rc_solver/lib/python2.7/site-packages/pysat/examples/rc2.py -c b remain_clauses1.wcnf >> run_result19_2.wcnf
/home/dell/下载/RC2/rc_solver/lib/python2.7/site-packages/pysat/examples/rc2.py -c b remain_clauses2.wcnf >> run_result19_3.wcnf
echo $count3 
count3=$((count3+1)) 
done

count3=1
while [ $count3 -le 919 ]
do
  ./main ./systems/b14_C.sys systems/b14_C.wcnf ./systems/b14_C.syspp ./iscas85/b14_C.obs $count3 >> resducedb14_C.wcnf
/home/dell/下载/RC2/rc_solver/lib/python2.7/site-packages/pysat/examples/rc2.py -c b remain_clauses.wcnf >> run_result14_1.wcnf
/home/dell/下载/RC2/rc_solver/lib/python2.7/site-packages/pysat/examples/rc2.py -c b remain_clauses1.wcnf >> run_result14_2.wcnf
/home/dell/下载/RC2/rc_solver/lib/python2.7/site-packages/pysat/examples/rc2.py -c b remain_clauses2.wcnf >> run_result14_3.wcnf
  count3=$((count3+1))  
done
count3=1
while [ $count3 -le 1000 ]
do
  ./main ./systems/b18_C.sys systems/b18_C.wcnf ./systems/b18_C.syspp ./iscas85/b18_C.obs $count3 >> resducedb18_C.wcnf
/home/dell/下载/RC2/rc_solver/lib/python2.7/site-packages/pysat/examples/rc2.py -c b remain_clauses.wcnf >> run_result18_1.wcnf
/home/dell/下载/RC2/rc_solver/lib/python2.7/site-packages/pysat/examples/rc2.py -c b remain_clauses1.wcnf >> run_result18_2.wcnf
/home/dell/下载/RC2/rc_solver/lib/python2.7/site-packages/pysat/examples/rc2.py -c b remain_clauses2.wcnf >> run_result18_3.wcnf
  count3=$((count3+1)) 
done
count3=1
while [ $count3 -le 997 ]
do
  ./main ./systems/b20_C.sys systems/b20_C.wcnf ./systems/b20_C.syspp ./iscas85/b20_C.obs $count3 >> resducedb20_C.wcnf
/home/dell/下载/RC2/rc_solver/lib/python2.7/site-packages/pysat/examples/rc2.py -c b remain_clauses.wcnf >> run_result20_1.wcnf
/home/dell/下载/RC2/rc_solver/lib/python2.7/site-packages/pysat/examples/rc2.py -c b remain_clauses1.wcnf >> run_result20_2.wcnf
/home/dell/下载/RC2/rc_solver/lib/python2.7/site-packages/pysat/examples/rc2.py -c b remain_clauses2.wcnf >> run_result20_3.wcnf
  count3=$((count3+1)) 
  echo $count3
done



count3=1
while [ $count3 -le 992 ]
do
  ./main ./systems/b21_C.sys systems/b21_C.wcnf ./systems/b21_C.syspp ./iscas85/b21_C.obs $count3 >> resducedb21_C.wcnf
/home/dell/下载/RC2/rc_solver/lib/python2.7/site-packages/pysat/examples/rc2.py -c b remain_clauses.wcnf >> run_result21_1.wcnf
/home/dell/下载/RC2/rc_solver/lib/python2.7/site-packages/pysat/examples/rc2.py -c b remain_clauses1.wcnf >> run_result21_2.wcnf
/home/dell/下载/RC2/rc_solver/lib/python2.7/site-packages/pysat/examples/rc2.py -c b remain_clauses2.wcnf >> run_result21_3.wcnf
  count3=$((count3+1)) 
done
