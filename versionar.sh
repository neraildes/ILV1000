#!/bin/bash 
clear
git branch
echo '--------------------------------------------'
echo 'Iniciando processo de alteracao de versao...'
echo 'CheckList:'
echo '1-Crie o Branch de Versao e acesse-o'
echo '2-Rode o Script versionar.sh'
echo '3-Compile o codigo fonte'
echo '4-Atualize o ChangeLog'
echo '5-Adicione as Modificacoes'
echo '6-Commit as alteracoes'
echo '--------------------------------------------'

#check=$(git diff --name-only '<rev>'^! | xargs git log '<rev>'^ -1 --)

#check=$(git diff --name-only $branch^! | xargs git log $branch^ -1 --)

# check if the checkout was to checkout a branch
#if [ '$3' != '1' ]
#    then echo 'git checkout did not checkout a branch - quitting'
#fi

# get current branch name
branch=$(git rev-parse --abbrev-ref HEAD)
info=$(git log --pretty=format:"%an - %aD" -1)
#check=$(git diff --name-only $branch^! | xargs git log $branch^ -1 --)

# run maven versions plugin to set new version
#mvn versions:set -DgenerateBackupPoms=false -DnewVersion=$version

echo "// arquivo criado pelo script versionar.sh" > versao.h
echo "// $info" >> versao.h
echo "#ifndef VERSAO_H"   >> versao.h
echo "#define	VERSAO_H" >> versao.h
echo "#define FVERSION \"ATUAL $branch\"" >> versao.h
echo "#endif"              >> versao.h


echo "Alterado o arquivo para versao \"$branch\"."