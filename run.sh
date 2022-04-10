make
fld=$1
n=$2
p=$3

mkdir -p output
for ((i=1 ; i<=$n ; i++))
do
	timeout 60 ./client-phase$p ./$fld/client$i-config.txt ./$fld/files/client$i/ > ./output/ans_$i\_$p.txt &
done
