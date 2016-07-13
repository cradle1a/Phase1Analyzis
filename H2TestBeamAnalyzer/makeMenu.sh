#!/bin/bash

if [[ "$#" != "1" ]] ; then
  echo "Usage: ./makeHtml.sh <input directory>"
  exit 1
fi

inputDir=$1

if [[ ! -d $inputDir ]] ; then
  echo "ERROR: input directory does not exist"
  exit 1
fi

cd $inputDir
runName=$(basename $(pwd))


function make_index () {
  echo "<h1>DQM</h1>"
  echo "<p>"
  echo "Run name: $runName<br>"
  echo "Plots generation time: $(date)<br>"
  echo "<hr>"
  
  echo "<h3>Plots</h3>"
  echo "<ul>"
  while read link ; do
    echo "<li><a href=\"$link.html\">$link</a></li>"
  done
  echo "</ul>"

}

function make_subpage () {
  local title=$1
  
  echo "<h1>$title</h1>"
  echo "<hr>"
  
  while read f ; do
    echo "<a href=\"$f\"><img src=\"${f%.*}.gif\" width=\"30%\"></a>"
  done
}

echo "Generating html menu..."

# plots file name format is plot_name--index.pdf

# extract list of plots:
plotsList=$(find . -type f -name "*.pdf" | cut -d "/" -f 2 | sed 's,--.*,,' | sort -u)

# generate index
echo "$plotsList" | make_index > index.html

# generate subpages
for i in $plotsList ; do
  #echo "==> $i"
  find . -type f -name "$i--*.pdf" | cut -d "/" -f 2 | sort | make_subpage $i > $i.html
done

