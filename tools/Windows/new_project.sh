#!/bin/bash

if [[ $# -ne 2 ]]
then
  echo "Usage: cd <repo_root> && .tools/Windows/new_project.sh ProjectName path/to/project/root/folder"
  exit 1
fi

TEMPLATES_ROOT='tools/Windows/templates'
GUID=`/c/Program\ Files\ \(x86\)/Windows\ Kits/10/bin/x64/uuidgen.exe`
PROJECT_NAME=$1
LOCATION=$2

while IFS= read -r -d '' FILE; do
  SOURCE=$TEMPLATES_ROOT/$FILE
  DESTINATION=$LOCATION/`echo $FILE | sed -e "s/template/$PROJECT_NAME/g"`
  mkdir -p `dirname $DESTINATION`
  cp $SOURCE $DESTINATION
  sed -i -e s/%%PROJECT_NAME%%/$PROJECT_NAME/g $DESTINATION
  sed -i -e s/%%GUID%%/$GUID/g $DESTINATION
done < <(cd $TEMPLATES_ROOT; find * -type f -print0)

