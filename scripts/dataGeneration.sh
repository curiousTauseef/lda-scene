# Usage: ./dataGeneration categoriesDirectory outDirectory dictionary.yml

catDir="$1" #consists one folder per category, each consisting images.
outDir="$2" #output directory in which training data files will be placed, same name as the category
dictionary=$3 #codebook yml file 
codeWordGen="./codeword-gen/codeword-gen"

for i in `find $catDir/* -maxdepth 1 -type d`
do
    category="${i##*/}"
    echo generating BOW data files for "$category" ... 
    ls "$i" | while read a; do $codeWordGen "$i/$a" "$category" "$dictionary" --no-yaml; done > "$outDir/$category.data"
done
