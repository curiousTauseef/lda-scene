# Usage: ./chooseTrainSet categoriesDirectory trainDirecctory outDirectory

catDir="$1" #consists one folder per category, each consisting images.
trainDir="$2" #directory which contains training set, to exclude from test set.
outDir="$3" #output directory in which testing sets will be copied each in the same name directory as input 
#TODO make soft links insted 

for i in `find $catDir/* -maxdepth 1 -type d`
do
    category="${i##*/}"
    mkdir "$outDir/$category" 2>/dev/null
done

find "$trainDir" | rev | cut -d'/' -f1,2 | rev > ./tmp.nkh
find "$catDir" > ./tmp2.nkh
grep -v -f ./tmp.nkh ./tmp2.nkh > tmp3.nkh
while read a 
do
    cat=`echo $a | rev | cut -d'/' -f2 | rev`
    cp -v $a "$outDir/$cat/" 2>/dev/null
done < <(cat tmp3.nkh)
rm tmp.nkh tmp2.nkh tmp3.nkh
