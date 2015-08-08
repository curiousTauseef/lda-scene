# Usage: ./genOpenCVMat.sh inputFile  (inputFile is the alpha or beta matrix file)
# The output will be placed in the same location

inFile="$1"
rows=`cat "$inFile" | wc -l`
cols=`cat "$inFile" | head -1 | wc -w`
data="`cat "$inFile" | while read a ; do echo $a ; done | tr ' ' ','`"
tag=${inFile##*/}
struct=`cat "./scripts/opencvMatrix-template.yml"` 
(echo -n "$struct" |  sed "s/ROWS/$rows/g" |  sed "s/COLS/$cols/g" | sed "s/TAG/$tag/g" && (echo -n "$data" | sed ':a;N;$!ba;s/\n/,\n       /g' ) && echo " ]") > "$inFile".yml
