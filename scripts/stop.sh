function stop ()
{
  echo -e "(+stopping $project mode+)"
  export PS1=$previous_ps1
  export PATH=$previous_path
  export LD_LIBRARY_PATH=$previous_ld_path
  for var in $(echo $exports)
  do
    unset $var
  done  
  unset exports

  for var in $(echo $aliases)
  do
    unalias $var
  done
  unset aliases

  echo -e "(+$project mode stopped+)"
  unset project
  unset __start
  unset load
  unset stop
}

