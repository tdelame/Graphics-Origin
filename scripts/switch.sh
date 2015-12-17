function switch {
  name=$1
  if [[ $# -eq 0 ]]
  then
    name="release"    
  fi
  case $name in
    "release")
        ;;
    "debug")
        ;;
    *)
     echo "build configuration $name not handled";
     return 1;;
  esac

  mkdir -p $projectdir/bin
  echo "BUILD_CONF = $name" > bin/build_conf.mk
  here=$(pwd)
  cd $projectdir
  make strap -j4
  rm -f $projectdir/bin/products $projectdir/bin/tutorial $projectdir/bin/lib
  ln -s $projectdir/bin/$name                 $projectdir/bin/products
  ln -s $projectdir/bin/products/tutorial $projectdir/bin/tutorial
  ln -s $projectdir/bin/products/lib          $projectdir/bin/lib
  cd $here
}
