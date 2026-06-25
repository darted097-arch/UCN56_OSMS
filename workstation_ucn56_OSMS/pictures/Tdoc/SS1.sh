for file in *.Tdoc
do
sed 's/MBLee/CORENET/' $file > $file.tmp
rm $file
mv $file.tmp $file
done
