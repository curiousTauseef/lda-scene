# Usage: ./genOpenCVMat.sh inputFile  (inputFile is the alpha or beta matrix file)
# The output will be placed in the same location

inFile="$1"
rows=`cat "$inFile" | wc -l`
cols=`cat "$inFile" | head -1 | wc -w`
data=`cat trainedModels/kitchen.alpha | while read a ; do echo $a ; done | tr ' ' ','`

cat "./scripts/opencvMatrix-template.yml" |  sed "s/ROWS/$rows/g" |  sed "s/COLS/$cols/g" | sed "s/DATA/$data/g" > "$inFile".yml
