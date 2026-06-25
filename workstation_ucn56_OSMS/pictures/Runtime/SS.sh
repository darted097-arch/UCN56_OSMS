for file in PicYGN*.Tdoc
#for file in *.Tdoc
do
sed 's/updateMode          = 2;/updateMode          = 1;/' $file > $file.tmp
rm $file
mv $file.tmp $file
done
