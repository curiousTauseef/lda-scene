# Usage ./inference.sh imageFile modelsDirectory dictionary.yml

img="$1"
models="$2"
dictionary="$3"
label="unknown"
imgName=${img##*/}
imgNameNoExt=${imgName%.*}
mkdir "tmp_inference" 2>/dev/null
./codeword-gen/codeword-gen "$img" $label "$dictionary" --yaml > /dev/null
mv "$label"_"$imgNameNoExt".yml "tmp_inference/"

for i in `ls $models  | cut -d'.' -f1 | uniq`
do
   echo -ne "$i\t" && ./utils/matrixMul "$models/$i.alpha.yml" "$models/$i.beta.yml" "tmp_inference/"$label"_$imgNameNoExt.yml" | tr -d '[' | tr -d ']' 
done | sort -g -k2

