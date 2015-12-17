function __start(){
  echo -e "(+starting $project mode+)"
  export projectdir=$(echo $PWD | sed s/"\/scripts/"/)
  if [ "$(echo $projectdir | grep " ")" ]
  then
    echo -e "ERROR: there is space in root filepath"
    echo -e "Aborting..."
    exit 2
  fi
  
  export started="true"
  export previous_ps1=$PS1
  export previous_path=$PATH
  export previous_ld_path=$LD_LIBRARY_PATH
  export exports="projectdir started previous_ps1 previous_path previous_ld_path"
  export PS1="\n\[\e[30;1m\]\[\033(0\]l\[\033(B\](\[\e[34;1m\]\u\[\e[30;1m\])\[\033(0\]q\[\033(B\](\[\e[34;1m\]\t\[\e[30;1m\])\[\033(0\]q\[\033(B\](\[\e[34;1m\]$(git branch 2>/dev/null | grep '^*'|colrm 1 2)\[\e[30;1m\])\[\033(0\]q\[\033(B\]>\[\e[30;1m\]\n\[\033(0\]mq\[\033(B\](\[\[\e[32;1m\]\w\[\e[30;1m\])\[\033(0\]q\[\033(B\](\[\e[32;1m\]11 files, 3.7Mb\[\e[30;1m\])\[\033(0\]q\[\033(B\]> \[\e[0m\]"
  export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$projectdir/bin/lib/:/usr/local/lib:/usr/local/lib/OpenMesh
  export PATH=$PATH:$projectdir/bin/apps
  export OMP_NUM_THREADS=$(cat /proc/cpuinfo | grep processor | wc -l )
  echo -e "(+$project mode started+)"
}

