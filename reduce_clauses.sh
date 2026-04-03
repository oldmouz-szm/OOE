#bin/bash

directory="systems"
#rm $1/file_bb3_wy 
 echo $1
 count=1
ls "$directory" | more |
while read file
do
 if [[ ${file:0-4} == ".sys" ]]
  then
   file1=${file%%.*}
   

   ./main $file $file1.txt　$file1.syspp >> resduced_$file1.txt




  count=$((count+1))

   


 fi 
done


#./distupls $file 300 50000 >>distupls_result50000.txt
#./distupls $file 300 1000000　>>distupls_result1.txt
#./distupls $file 300 2000000　>>distupls_result2.txt
#./126 $file $file+flipcom.cnf 60 50000　>$file+126ressult60.cnf
#./126 $file $file+flipcom.cnf 300 50000　>$file+126ressult60.cnf


